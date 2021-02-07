package server

import (
	"context"
	"database/sql"
	"errors"
	"fmt"
	"net/http"
	"net/http/httputil"
	"sync"
	"time"

	"github.com/rs/cors"
	"github.com/rs/zerolog/log"
	"github.com/samsarahq/thunder/graphql"
	"github.com/samsarahq/thunder/graphql/graphiql"
	"github.com/samsarahq/thunder/graphql/introspection"
	"github.com/samsarahq/thunder/graphql/schemabuilder"
	"github.com/samsarahq/thunder/reactive"

	_ "github.com/lib/pq"

	"github.com/workstudyplay/thing/internal/bonjour"
	"github.com/workstudyplay/thing/internal/configuration"
)

// server is our graphql server.
type server struct {
	posts  []Post
	boards  []Board
	
	tokens []TokenStatus

	db *sql.DB
}

func NewDatabaseConnection(postgresURI string) (*sql.DB, error) {
	instance, err := sql.Open("postgres", postgresURI)
	if err != nil {
		// log.Fatal(err)
		return nil, err
	}
	err = instance.Ping()
	if err != nil {
		// log.Error("Error connecting to database")
		return instance, err
	}
	// TODO: handle connection pooling for postgres better
	instance.SetMaxOpenConns(20)
	instance.SetMaxIdleConns(20)
	instance.SetConnMaxLifetime(5 * time.Minute)

	// log.Info("connected postgres database")
	return instance, nil
}

// registerQuery registers the root query type.
func (s *server) registerQuery(schema *schemabuilder.Schema) {
	obj := schema.Query()

	obj.FieldFunc("posts", func() []Post {
		return s.posts
	})
	obj.FieldFunc("boards", func() []Board {
		return s.boards
	})

	obj.FieldFunc("tokens", func() []TokenStatus {
		return s.tokens
	})

	// time returns the current time.
	obj.FieldFunc("time", func(ctx context.Context) string {
		// Invalidate the result of this resolver after 10 seconds.
		reactive.InvalidateAfter(ctx, 1*time.Second)
		// Return the current time. Will be re-executed automatically.

		nowString := time.Now().UTC().String()

		log.Info().Msg("Serving time")
		return nowString
	})
}

// registerMutation registers the root mutation type.
func (s *server) registerMutation(schema *schemabuilder.Schema) {
	obj := schema.Mutation()
	obj.FieldFunc("echo", func(args struct{ Message string }) string {
		return args.Message
	})
}

// registerPost registers the post type.
func (s *server) registerPost(schema *schemabuilder.Schema) {
	obj := schema.Object("Post", Post{})
	obj.FieldFunc("age", func(ctx context.Context, p *Post) string {
		reactive.InvalidateAfter(ctx, 5*time.Second)
		return time.Since(p.CreatedAt).String()
	})
}

// func (s *server) registerTokenStatus(schema *schemabuilder.Schema, db *sql.DB) {
// 	obj := schema.Object("TokenStatus", TokenStatus{})
// 	obj.FieldFunc("tokens", func(ctx context.Context, p *post) []TokenStatus {
// 		return tokens
// 	})
// }

// schema builds the graphql schema.
func (s *server) schema(db *sql.DB) *graphql.Schema {
	s.db = db

	builder := schemabuilder.NewSchema()
	s.registerQuery(builder)
	s.registerMutation(builder)
	s.registerPost(builder)

	go s.DataWatcher()

	//s.registerTokenStatus(builder, db)
	return builder.MustBuild()
}

func (s *server) DataWatcher() {
	s. pollTables()
	// setup interval for checking the status of tokens, every 5 minutes is an arbitrary decision, we may want to revisit
	for range time.Tick(time.Second * 15) {
		s. pollTables()
	}
}

func (s *server) pollTables() {
	s.pollPosts()
	s.pollBoards()
}

func (s *server) pollPosts() {
	log.Info().Msg("poll posts table")
	var items = make([]Post, 0)
	query := `
	SELECT 
		id,
		title,
		body,
		author
	FROM 
		posts
	`
	records, queryError := s.db.Query(query)
	if queryError != nil {
		panic(queryError)
	}
	defer records.Close()
	for records.Next() {
		row := Post{}
		err := records.Scan(
			&row.ID,
			&row.Title,
			&row.Body,
			&row.Author,
		)
		if err == nil {
			items = append(items, row)
		} else {
			log.Error().Msg(err.Error())
		}
	}
	recordsErr := records.Err()
	if recordsErr != nil {
		log.Error().Msg(recordsErr.Error())
	}
	s.posts = items
}


func (s *server) pollBoards() {
	log.Info().Msg("poll boards table")
	var items = make([]Board, 0)
	query := `
	SELECT 
		id,
		name,
		address,
		owner,
		chip_name
	FROM 
		boards
	`
	records, queryError := s.db.Query(query)
	if queryError != nil {
		panic(queryError)
	}
	defer records.Close()
	for records.Next() {
		row := Board{}
		err := records.Scan(
			&row.ID,
			&row.Name,
			&row.Address,
			&row.Owner,
			&row.ChipName,			
		)
		if err == nil {
			items = append(items, row)
		} else {
			log.Error().Msg(err.Error())
		}
	}
	recordsErr := records.Err()
	if recordsErr != nil {
		log.Error().Msg(recordsErr.Error())
	}
	s.boards = items
}

func connectDatabase(postgresURI string) (*sql.DB, error) {
	db, err := NewDatabaseConnection(postgresURI)
	if err != nil {
		log.Error().Msg("Database connection error: " + err.Error())
		return nil, errors.New("Database not connected")
	}
	return db, nil
}

type TokenStatus struct {
	IrisEmail          string
	IrisUserID         string
	ForgeEmail         string
	ForgeUserID        string
	ExpireDateEST      string
	MinutesTillExpired int
}



func configureCORS(publicURI string, port int) *cors.Cors {
	allowed := []string{
		"http://localhost:3030",
		"http://localhost:3000",
		"http://localhost:3032",
		publicURI,
		fmt.Sprintf("%s:%d", publicURI, port),
	}

	c := cors.New(cors.Options{
		AllowedOrigins:   allowed,
		AllowCredentials: true,
		AllowedHeaders: []string{
			"Authorization",
			"Content-Type",
		},
		AllowedMethods: []string{
			"OPTIONS",
			"POST",
		},

		// Enable Debugging for testing, consider disabling in production
		Debug: false,
	})
	return c
}

func New(config configuration.Config) {

	db, err := connectDatabase(config.PostgresURI)
	if err != nil {
		panic(err)
	}
	CreateOrUpdateDatabaseTables(db)

	go bonjour.Start()

	var wg sync.WaitGroup

	// Instantiate a server, build a server, and serve the schema on port 3030.
	server := &server{
		posts: []Post{},
		boards: []Board{},		
	}

	corsConfig := configureCORS(config.PublicURI, config.HTTPPort)


	schema := server.schema(db)
	introspection.AddIntrospectionToSchema(schema)

	// Expose schema and graphiql.
	http.Handle("/graphql", WSAuth(graphql.Handler(schema)))

	graphQLHandler := corsConfig.Handler(graphql.HTTPHandler(schema))
	http.Handle("/graphql-http",graphQLHandler)

	http.Handle("/graphiql/", http.StripPrefix("/graphiql/", graphiql.Handler()))

	wg.Add(1)
	publicHTML := "resources/html"
	staticFiles := http.FileServer(http.Dir(publicHTML))

	http.Handle("/", http.StripPrefix("/", staticFiles))
	http.ListenAndServe(":3030", nil)

}

func WSAuth(next http.Handler) http.Handler {
	return http.HandlerFunc(func(w http.ResponseWriter, r *http.Request) {
		ctx := r.Context()

		secWebsocketProtocol := ""
		fmt.Print("Headers:\n")
		for name, headers := range r.Header {
			//fmt.Printf("%s: %s\n", name, headers[0])
			if name == "Sec-Websocket-Protocol" {
				secWebsocketProtocol = headers[0]
			}
		}

		fmt.Printf("[WEBSOCKET] Sec-Websocket-Protocol: %s\n", secWebsocketProtocol)
		d, _ := httputil.DumpRequest(r, true)
		fmt.Printf("%s\n", d)

		r = r.WithContext(ctx)

		//w.Write([]byte("ok"))

		next.ServeHTTP(w, r)
		//w.Header().Set("Sec-WebSocket-Protocol", secWebsocketProtocol)
		w.Header().Set("Sec-WebSocket-Protocol", secWebsocketProtocol)
	})
}

// func websocketServer(schema *graphql.Schema, serviceInstance *autodesk.AutodeskService) {

// 	graphQLHandler := WSAuth(serviceInstance, graphql.Handler(schema))

// 	http.Handle("/graphql", graphQLHandler)

// 	publicHTML := "resources/html/graphql-ws"
// 	staticFiles := http.FileServer(http.Dir(publicHTML))

// 	http.Handle("/", http.StripPrefix("/", staticFiles))
// 	http.ListenAndServe(":3131", nil)
// }
