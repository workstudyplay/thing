package server

import (
	"fmt"

	"github.com/rs/cors"
)

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
