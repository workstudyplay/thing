package article

import "time"

type Article struct {
	ID        int       `graphql:"id" pg:"id,type:integer,pk"`
	Title     string    `pg:"title,type:character varying"`
	Author    string    `pg:"author,type:character varying"`
	Body      string    `pg:"body,type:text"`
	CreatedAt time.Time `graphql:"createdAt" json:"timestamp" pg:"created_timestamp,type:timestamp with time zone"`
}
