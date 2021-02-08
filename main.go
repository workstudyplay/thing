package main

import (
	"fmt"
	"sync"

	"github.com/workstudyplay/thing/internal/configuration"
	"github.com/workstudyplay/thing/internal/server"
)

var config configuration.Config

func main() {

	config = configuration.LoadConfig()

	fmt.Printf("REDIS_URI: %s\n", config.RedisURI)
	fmt.Printf("POSTGRES_URI: %s\n", config.PostgresURI)
	fmt.Printf("SERVICE_PORT: %d\n", config.HTTPPort)

	var wg sync.WaitGroup

	server.New(config)

	wg.Add(1)

	wg.Wait()

}
