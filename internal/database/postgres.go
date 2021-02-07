package database

import (
	"database/sql"
	"errors"
	"fmt"
	"reflect"
	"strings"
	"time"

	_ "github.com/lib/pq"
	log "github.com/sirupsen/logrus"
)

var db *sql.DB
var logQueriesEnabled = false
var requiredTables []DataMapping

func Init(required []DataMapping, db *sql.DB) {
	requiredTables = required
	ValidateRequirements(required, db)
	CreateViews(db)
}

func NewConnection(postgresURI string) (*sql.DB, error) {
	instance, err := sql.Open("postgres", postgresURI)
	if err != nil {
		log.Fatal(err)
		return nil, err
	}
	err = instance.Ping()
	if err != nil {
		log.Error("Error connecting to database")
		return instance, err
	}
	instance.SetMaxOpenConns(20)
	instance.SetMaxIdleConns(20)
	instance.SetConnMaxLifetime(5 * time.Minute)

	log.Info("connected postgres database")
	return instance, nil
}

func GetVersion(db *sql.DB) (string, error) {
	if db == nil {
		return "", errors.New("No database connection")
	}
	query := "SELECT version();"
	versionString := ""
	row := db.QueryRow(query)

	switch err := row.Scan(&versionString); err {
	case sql.ErrNoRows:
		// fmt.Println("No rows were returned!")
	case nil:
		log.Info(versionString)
	default:
		log.Error("Error getting version of Postgres")
	}
	return versionString, nil
}

type Table struct {
	SchemaName string
	TableName  string
	TableOwner string
	TableSpace string

	HasIndexes  bool
	HasRules    bool
	HasTriggers bool
	RowSecurity bool
}

type Column struct {
	ColumnName     string `pg:"columnname"`
	DataType       string `pg:"datatype"`
	DataTypeSymbol string `pg:"-"`
	IsPrimaryKey   bool   `pg:"-"`
}
type DataMapping struct {
	TableName    string
	RecordStruct interface{}
}

func ValidateRequirements(required []DataMapping, db *sql.DB) error {
	verbose := false

	tableMap := make(map[string]*Table)
	if db == nil {
		log.Error("No db connection")
		return errors.New("No db connection")
	}

	tables, _ := GetTables(db)
	tableSummary := "Current tables:\n"
	for _, table := range tables {
		tableSummary += "- " + table.TableName + "\n"
		tableMap[table.TableName] = table
	}
	if verbose {
		log.Info(tableSummary)
	}
	for _, mapping := range required {
		if verbose {
			log.Infof("Checking table [%s]", mapping.TableName)
		}
		if _, ok := tableMap[mapping.TableName]; !ok {
			query := "CREATE TABLE " + mapping.TableName + " ();"
			if verbose {
				log.Infof("Missing table: %s %s", mapping.TableName, query)
			}
			records, queryError := db.Query(query)
			if queryError != nil {
				if verbose {
					log.Info(queryError.Error())
				}
			}
			defer records.Close()
		}
		CheckStructure(mapping.TableName, mapping.RecordStruct, db)
		// var msg = ""
		// for _, line := range logItems {
		// 	msg += line
		// }
		// log.Info(msg)
	}
	return nil
}
func CheckStructure(tableName string, rowStruct interface{}, db *sql.DB) ([]string, error) {
	// keep track of log items in a slice
	summaryItems := make([]string, 0)
	if db == nil {
		log.Error("No db connection")
		return summaryItems, errors.New("No db connection")
	}

	columns, getColumnsError := GetTableColumns(tableName, db)
	if columns == nil || getColumnsError != nil {
		txt := "Error: Could not get columns for table " + tableName
		summaryItems = append(summaryItems, txt)
		return summaryItems, errors.New(txt)
	}

	tagName := "pg"
	t := reflect.TypeOf(rowStruct)
	var structMap = make(map[string]*Column)

	for i := 0; i < t.NumField(); i++ {
		// Get the field, returns https://golang.org/pkg/reflect/#StructField
		field := t.Field(i)
		tag := field.Tag.Get(tagName)
		//fmt.Printf("%d. %v (%v), tag: '%v'\n", i+1, field.Name, field.Type.Name(), tag)

		var k = ""
		var d = ""
		var t = ""
		var pk = false
		s := strings.Split(tag, ",")
		if len(s) == 1 {
			k = tag
			// log.Info(k)
		} else {
			k = s[0]

			switch s[1] {
			case "type:character varying":
				t = "VARCHAR"
				d = "character varying"
				break
			case "type:boolean":
				t = "BOOLEAN"
				d = "boolean"
				break
			case "type:integer":
				t = "INTEGER"
				d = "integer"
				break
			case "type:jsonb":
				t = "JSONB"
				d = "jsonb"
				break
			case "type:timestamp without time zone":
				t = "TIMESTAMP"
				d = "timestamp without time zone"
				break
			case "type:timestamp with time zone":
				t = "TIMESTAMPTZ"
				d = "timestamp with time zone"
				break
			case "type:text":
				t = "TEXT"
				d = "text"
				break
			default:
			}
			if len(s) >= 3 {
				switch s[2] {
				case "pk":
					pk = true
				}
			}
		}
		if len(k) > 0 && k != "-" {
			//log.Info("Adding field to map [" + k + "]")
			structMap[k] = &Column{
				ColumnName:     k,
				DataType:       d,
				DataTypeSymbol: t,
				IsPrimaryKey:   pk,
			}
		} else {
			// skip
			// log.Info("NOT adding field to map " + field.Name)
		}
	}
	var columnsMap = make(map[string]*Column)

	//log.Info("Comparing table [" + tableName + "] schema to the struct")
	// Check from the database against the struct
	for _, column := range columns {
		columnsMap[column.ColumnName] = column
		if _, columnExistsInStruct := structMap[column.ColumnName]; !columnExistsInStruct {
			// The field should likely be deleted from the database, since it is not in the model
			msg := "ALTER TABLE " + tableName + " DROP COLUMN " + column.ColumnName + "; currently exists in database but not in struct"
			summaryItems = append(summaryItems, msg)
		}
	}
	summaryItems = append(summaryItems, "Checking table schema ["+tableName+"]")
	var numGood = 0
	// check from the struct to the database
	for columnName, column := range structMap {
		// log.Info(">>> " + columnName + " " + column.DataType)
		if val, ok := columnsMap[columnName]; ok {
			// check the type
			if val.DataType != column.DataType {
				msg := ("Column " + columnName + " has wrong data type, currently: [" + val.DataType + "] should be: " + column.DataType + "]")
				summaryItems = append(summaryItems, msg)
			} else {
				numGood++
				if column.IsPrimaryKey {
					query := "ALTER TABLE " + tableName + " ADD PRIMARY KEY (" + columnName + ");"
					msg := fmt.Sprintf("COLUMN SHOULD BE PRIMARY KEY %s\n", query)
					summaryItems = append(summaryItems, msg)
				}
			}
		} else {
			query := "ALTER TABLE " + tableName + " ADD COLUMN " + column.ColumnName + " " + column.DataTypeSymbol + " NOT NULL"
			if column.DataTypeSymbol == "VARCHAR" || column.DataTypeSymbol == "TEXT" {
				query += " DEFAULT ''"
			}
			if column.IsPrimaryKey {
				query += " PRIMARY KEY;"
			} else {
				query += ";"
			}
			msg := fmt.Sprintf("Column missing from Table %s %s ", tableName, query)
			summaryItems = append(summaryItems, msg)
			// log.Info("SQL: " + query)
			_, queryError := db.Exec(query)
			if queryError != nil {
				summaryItems = append(summaryItems, queryError.Error())
			}
		}
	}
	if numGood == len(structMap) {
		msg := " - Struct looks ok"
		summaryItems = append(summaryItems, msg)
	}

	return summaryItems, nil
}

func GetTables(db *sql.DB) ([]*Table, error) {
	var rows = make([]*Table, 0)

	query := `SELECT
	schemaname,
	tablename,
	tableowner,
	hasindexes,
	hasrules,
	hastriggers,
	rowsecurity
FROM
	pg_catalog.pg_tables
WHERE
	schemaname != 'pg_catalog'
AND schemaname != 'information_schema';`

	if db == nil {
		return rows, errors.New("No db connection")
	}
	// log.Debug("SQL: " + query)
	records, queryError := db.Query(query)
	if queryError != nil {
		log.Error(queryError.Error())
		return nil, queryError
	}
	defer records.Close()

	for records.Next() {
		var row = &Table{}

		err := records.Scan(
			&row.SchemaName,
			&row.TableName,
			&row.TableOwner,
			&row.HasIndexes,
			&row.HasRules,
			&row.HasTriggers,
			&row.RowSecurity,
		)
		if err != nil {
			log.Error(err.Error())
		}
		rows = append(rows, row)
	}
	err := records.Err()
	if err != nil {
		log.Fatal(err)
	}
	// log.Info(fmt.Sprintf("We have %d tables", len(rows)))
	return rows, nil
}

func FlushAll(db *sql.DB) error {

	// drop all views
	DeleteViews(db)

	// drop all tables
	DeleteTables(db)

	// Re-create
	Init(requiredTables, db)

	return nil
}

func DeleteTables(db *sql.DB) error {
	if db == nil {
		return errors.New("No database connection")
	}

	tables, _ := GetTables(db)

	for _, row := range tables {
		log.Info("DELETING TABLE " + row.TableName)
		sql := "DROP TABLE IF EXISTS " + row.TableName + " CASCADE;"
		log.Debug(sql)
		_, err := db.Exec(sql)
		if err != nil {
			log.Errorf("SQL Error: %v", err)
		}
	}
	return nil
}

func GetTableColumns(tableName string, db *sql.DB) ([]*Column, error) {
	var rows = make([]*Column, 0)
	if db == nil {
		return rows, errors.New("No database connection")
	}

	query := `SELECT
	column_name,
	data_type
FROM
	information_schema.COLUMNS
WHERE
	TABLE_NAME = $1;`
	// log.Info("SQL: " + query)
	records, queryError := db.Query(query, tableName)
	if queryError != nil {
		log.Info(queryError.Error())
		return nil, queryError
	}
	defer records.Close()

	for records.Next() {
		var row = Column{}
		err := records.Scan(&row.ColumnName, &row.DataType)
		if err != nil {
			log.Fatal(err)
		}
		rows = append(rows, &row)
	}
	err := records.Err()
	if err != nil {
		log.Fatal(err)
	}
	return rows, nil
}
