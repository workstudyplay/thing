package server

import "github.com/rs/zerolog/log"


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
