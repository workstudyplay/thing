package server

import (
	"fmt"
	"net/http"
	"net/http/httputil"
)

func WSAuth(next http.Handler) http.Handler {
	return http.HandlerFunc(func(w http.ResponseWriter, r *http.Request) {
		ctx := r.Context()

		secWebsocketProtocol := ""
		fmt.Print("Headers:\n")
		for name, headers := range r.Header {
			//fmt.Printf("%s: %s\n", name, headers[0])
			if name == "Sec-Websocket-Protocol" {
				secWebsocketProtocol = headers[0]
			}
		}

		fmt.Printf("[WEBSOCKET] Sec-Websocket-Protocol: %s\n", secWebsocketProtocol)
		d, _ := httputil.DumpRequest(r, true)
		fmt.Printf("%s\n", d)

		r = r.WithContext(ctx)

		//w.Write([]byte("ok"))

		next.ServeHTTP(w, r)
		//w.Header().Set("Sec-WebSocket-Protocol", secWebsocketProtocol)
		w.Header().Set("Sec-WebSocket-Protocol", secWebsocketProtocol)
	})
}

// func websocketServer(schema *graphql.Schema, serviceInstance *autodesk.AutodeskService) {

// 	graphQLHandler := WSAuth(serviceInstance, graphql.Handler(schema))

// 	http.Handle("/graphql", graphQLHandler)

// 	publicHTML := "resources/html/graphql-ws"
// 	staticFiles := http.FileServer(http.Dir(publicHTML))

// 	http.Handle("/", http.StripPrefix("/", staticFiles))
// 	http.ListenAndServe(":3131", nil)
// }
