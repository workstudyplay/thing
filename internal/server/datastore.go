package server

import (
	"database/sql"
	"errors"
	"time"

	"github.com/rs/zerolog/log"
	"github.com/workstudyplay/thing/internal/database"
	"github.com/workstudyplay/thing/internal/elements/article"
)

type Board struct {
	ID        int       `graphql:"id" pg:"id,type:integer,pk"`
	Name      string    `pg:"name,type:character varying"`
	Owner     string    `pg:"owner,type:character varying"`
	Address   string    `pg:"address,type:character varying"`
	ChipName  string    `graphql:"chipName" pg:"chip_name,type:character varying"`
	CreatedAt time.Time `graphql:"createdTimestamp" json:"timestamp" pg:"timestamp,type:timestamp with time zone"`
}

func connectDatabase(postgresURI string) (*sql.DB, error) {
	db, err := NewDatabaseConnection(postgresURI)
	if err != nil {
		log.Error().Msg("Database connection error: " + err.Error())
		return nil, errors.New("Database not connected")
	}
	return db, nil
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

func GetRequiredTables() []database.DataMapping {
	required := []database.DataMapping{
		{
			TableName:    "articles",
			RecordStruct: article.Article{},
		},
		{
			TableName:    "boards",
			RecordStruct: Board{},
		},
	}
	return required
}

func CreateOrUpdateDatabaseTables(db *sql.DB) error {
	required := GetRequiredTables()
	database.Init(required, db)

	return nil
}
