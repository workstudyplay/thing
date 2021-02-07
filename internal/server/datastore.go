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
	CreatedAt time.Time `graphql:"createdAt" json:"timestamp" pg:"created_timestamp,type:timestamp with time zone"`
}

type Board struct {
	ID        int       `graphql:"id" pg:"id,type:integer,pk"`
	Name     string    `pg:"name,type:character varying"`
	Owner    string    `pg:"owner,type:character varying"`
	Address      string    `pg:"address,type:character varying"`
	ChipName      string    `graphql:"chipName" pg:"chip_name,type:character varying"`
	CreatedAt time.Time `graphql:"createdTimestamp" json:"timestamp" pg:"timestamp,type:timestamp with time zone"`
}

func GetRequiredTables() []database.DataMapping {
	required := []database.DataMapping{
		{
			TableName:    "posts",
			RecordStruct: Post{},
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
