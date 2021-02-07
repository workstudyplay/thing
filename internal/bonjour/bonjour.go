package bonjour

import (
	"log"
	"os"
	"os/signal"
	"sync"
	"time"

	"github.com/oleksandr/bonjour"
)

func Start() {
	log.Println("startung up")
	hostname, _ := os.Hostname()
	// Run registration (blocking call)
	s, err := bonjour.Register(hostname, "_workstudyplay-server._tcp", "", 9999, []string{"version=0.0.0", "app=workstudyplay-server"}, nil)
	if err != nil {
		log.Fatalln(err.Error())
	}
	log.Println("Registered service")

	// Ctrl+C handling
	handler := make(chan os.Signal, 1)
	signal.Notify(handler, os.Interrupt)
	for sig := range handler {
		if sig == os.Interrupt {
			s.Shutdown()
			time.Sleep(1e9)
			break
		}
	}
	Search()
}

func Search() {

	resolver, err := bonjour.NewResolver(nil)
	if err != nil {
		log.Println("Failed to initialize resolver:", err.Error())
		os.Exit(1)
	}

	results := make(chan *bonjour.ServiceEntry)

	go func(results chan *bonjour.ServiceEntry, exitCh chan<- bool) {
		for e := range results {
			log.Printf("%s /t%s", e.AddrIPv4, e.Instance)
			for _, v := range e.Text {
				log.Printf(" - %s", v)
			}

			exitCh <- true
			// time.Sleep(1e9)
			// os.Exit(0)
		}
	}(results, resolver.Exit)
}

func StartHost() {
	hostname, _ := os.Hostname()
	s, err := bonjour.Register(hostname, "_workstudyplay-server._tcp", "", 9999, []string{"version=0.0.1", "app=workstudyplay-server"}, nil)
	if err != nil {
		log.Fatalln(err.Error())
	}

	// Ctrl+C handling
	handler := make(chan os.Signal, 1)
	signal.Notify(handler, os.Interrupt)
	for sig := range handler {
		if sig == os.Interrupt {
			s.Shutdown()
			time.Sleep(1e9)
			break
		}
	}

	resolver, err := bonjour.NewResolver(nil)
	if err != nil {
		log.Println("Failed to initialize resolver:", err.Error())
		os.Exit(1)
	}

	results := make(chan *bonjour.ServiceEntry)

	go func(results chan *bonjour.ServiceEntry, exitCh chan<- bool) {
		for e := range results {
			log.Printf("%s /t%s", e.AddrIPv4, e.Instance)
			for _, v := range e.Text {
				log.Printf(" - %s", v)
			}

			exitCh <- true
			// time.Sleep(1e9)
			// os.Exit(0)
		}
	}(results, resolver.Exit)

	// err = resolver.Browse("_irisvr._tcp", "local.", results)
	// if err != nil {
	// 	log.Println("Failed to browse:", err.Error())
	// }

	var wg sync.WaitGroup
	wg.Add(1)

	wg.Wait()
}
