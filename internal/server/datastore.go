package server

import (
	"database/sql"
	"time"

	"github.com/workstudyplay/thing/internal/database"
)

type Post struct {
	ID        int       `graphql:"id" pg:"id,type:integer,pk"`
	Title     string    `pg:"title,type:character varying"`
	Author    string    `pg:"author,type:character varying"`
	Body      string    `pg:"body,type:text"`
	CreatedAt time.Time `graphql:"timestamp" json:"timestamp" pg:"timestamp,type:timestamp with time zone"`
}

func GetRequiredTables() []database.DataMapping {
	required := []database.DataMapping{
		{
			TableName:    "posts",
			RecordStruct: Post{},
		},
	}
	return required
}

func CreateOrUpdateDatabaseTables(db *sql.DB) error {
	required := GetRequiredTables()
	database.Init(required, db)

	return nil
}
