package server

import (
	"context"
	"database/sql"
	"fmt"
	"net/http"
	"time"

	"github.com/rs/zerolog/log"
	"github.com/samsarahq/thunder/graphql"
	"github.com/samsarahq/thunder/graphql/graphiql"
	"github.com/samsarahq/thunder/graphql/introspection"
	"github.com/samsarahq/thunder/graphql/schemabuilder"
	"github.com/samsarahq/thunder/reactive"

	_ "github.com/lib/pq"

	"github.com/workstudyplay/thing/internal/bonjour"
	"github.com/workstudyplay/thing/internal/configuration"
	"github.com/workstudyplay/thing/internal/elements/article"
)

// server is our graphql server.
type server struct {
	db *sql.DB

	articles []article.Article
	boards   []Board
}

// registerQuery registers the root query type.
func (s *server) registerQuery(schema *schemabuilder.Schema) {
	obj := schema.Query()

	obj.FieldFunc("boards", func() []Board {
		return s.boards
	})

	// time returns the current time.
	obj.FieldFunc("time", func(ctx context.Context) string {
		reactive.InvalidateAfter(ctx, 10*time.Second)
		nowString := time.Now().UTC().String()
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

// schema builds the graphql schema.
func (s *server) schema(db *sql.DB) *graphql.Schema {
	s.db = db

	builder := schemabuilder.NewSchema()
	s.registerQuery(builder)
	s.registerMutation(builder)
	s.registerArticles(builder)

	//s.registerTokenStatus(builder, db)
	return builder.MustBuild()
}

func (s *server) DataWatcher() {
	s.pollTables()
	// setup interval for checking the status of tokens, every 5 minutes is an arbitrary decision, we may want to revisit
	for range time.Tick(time.Second * 15) {
		s.pollTables()
	}
}

func (s *server) pollTables() {
	s.pollPosts()
	s.pollBoards()
}

func (s *server) pollPosts() {
	log.Info().Msg("poll articles table")
	var items = make([]article.Article, 0)
	query := `
	SELECT 
		id,
		title,
		body,
		author
	FROM 
		articles
	`
	records, queryError := s.db.Query(query)
	if queryError != nil {
		panic(queryError)
	}
	defer records.Close()
	for records.Next() {
		row := article.Article{}
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
	s.articles = items
}

func New(config configuration.Config) {
	server := &server{
		articles: []article.Article{},
		boards:   []Board{},
	}
	db, err := connectDatabase(config.PostgresURI)
	if err != nil {
		log.Error().Msg(err.Error())
	} else {
		CreateOrUpdateDatabaseTables(db)
		go server.DataWatcher()
	}
	schema := server.schema(db)
	introspection.AddIntrospectionToSchema(schema)

	go bonjour.Start()

	corsConfig := configureCORS(config.PublicURI, config.HTTPPort)

	// graphQL http endpint
	graphQLHandler := corsConfig.Handler(graphql.HTTPHandler(schema))
	http.Handle("/graphql-http", graphQLHandler)

	// graphQL WS endpoint
	http.Handle("/graphql", WSAuth(graphql.Handler(schema)))

	// graphiQL app via WS
	http.Handle("/graphiql/", http.StripPrefix("/graphiql/", graphiql.Handler()))

	publicHTML := "resources/html"
	staticFiles := http.FileServer(http.Dir(publicHTML))

	http.Handle("/", http.StripPrefix("/", staticFiles))
	http.ListenAndServe(fmt.Sprintf(":%d", config.HTTPPort), nil)
}
