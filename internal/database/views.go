package database

import (
	"database/sql"
	"errors"
	"fmt"

	log "github.com/sirupsen/logrus"
)

var views = map[string]string{}

func CreateViews(db *sql.DB) {
	//DeleteViews()
	for name, sqlStatement := range views {
		CreateView(name, sqlStatement, db)
	}
}

func DeleteViews(db *sql.DB) {
	for name := range views {
		DeleteView(name, db)
	}
}

func CreateView(name string, sqlStatement string, db *sql.DB) error {
	if db == nil {
		return errors.New("No database connection")
	}
	_, err := db.Exec(fmt.Sprintf("CREATE VIEW %s AS %s", name, sqlStatement))
	if err != nil {
		//log.Info("SQL Error: " + err.Error())
		return err
	}
	return nil
}

func DeleteView(name string, db *sql.DB) error {
	if db == nil {
		return errors.New("No database connection")
	}
	sql := fmt.Sprintf("DROP VIEW IF EXISTS %s CASCADE;", name)
	log.Debug(sql)
	_, err := db.Exec(sql)
	if err != nil {
		log.Info("SQL Error: " + err.Error())
		return err
	}
	return nil
}
