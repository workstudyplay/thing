import { useState, useEffect } from 'react'
import { ApolloLink, HttpLink, Operation, FetchResult, Observable, SubscriptionClient, ApolloClient, ApolloProvider, InMemoryCache, gql, useQuery } from '@apollo/client';

import { print, GraphQLError } from 'graphql';
import { createClient, ClientOptions, Client } from 'graphql-ws';


import { Post } from '../posts'

// import { ThunderProvider, Connection, Mutate,  } from 'thunder-react';

// import { getClient } from './apollo'


// const client = getClient()

// class WebSocketLink extends ApolloLink {
//     client = false

//     constructor(options) {
//         super();
//         this.client = createClient(options);
//     }

//     request(operation) {
//         return new Observable((sink) => {
//             return this.client.subscribe(
//                 { ...operation, query: print(operation.query) },
//                 {
//                     next: sink.next.bind(sink),
//                     complete: sink.complete.bind(sink),
//                     error: (err) => {
//                         if (err instanceof Error) {
//                             sink.error(err);
//                         } else if (err instanceof CloseEvent) {
//                             sink.error(
//                                 new Error(
//                                     `Socket closed with event ${err.code}` + err.reason
//                                         ? `: ${err.reason}` // reason will be available on clean closes
//                                         : '',
//                                 ),
//                             );
//                         } else {

//                             console.error("error")
//                             //   sink.error(
//                             //     new Error(
//                             //       (err as GraphQLError[])
//                             //         .map(({ message }) => message)
//                             //         .join(', '),
//                             //     ),
//                             //   );
//                         }
//                     },
//                 },
//             );
//         });
//     }
// }


// class WebConnect extends WebSocket {
//     constructor(wsuri, proto) {
//         // Ignore passing the proto value
//         super( wsuri )
//     }

//     onopen(event) {
//         console.log("Connection open event:");
//         console.log(event);
//         super.onopen(event)
//     }

//     // onclose(event) {
//     //     console.log("Connection closed event:");
//     //     console.log(event);
//     // }

//     // onerror(event) {
//     //     console.log("Connection error event:");
//     //     console.log(event);
//     // }

//     // onmessage(event) {
//     //     console.log("Message event:");
//     //     console.log(event);
//     // }
// }

// export const Polling = () => {
//     const [apolloClient, setApolloClient] = useState( false )
//     const [pollingEnabled, setPollingEnabled] = useState( true )
    
//     useEffect( () => {
//         // const client = getClient()
//         // setApolloClient( client )
//         return () => {
//             // unmount / cleanup
//         }  
//     },[])

//     if (apolloClient === false ) {
//         return <div>Loading...</div>
//     }
//     return (
//         <ApolloProvider client={client}>
//             <div>
//                 <h1>zxcasd</h1>
//             </div>
//         </ApolloProvider>
//     )
// }


export const Http = () => {
    const [client, setClient] = useState(undefined)
    useEffect(() => {
        // setup 
        // const link = new HttpLink({
        //     url: 'http://localhost:3030/graphql?dork=yes',
        //     connectionParams: () => {
        //         const session = false
        //         if (!session) {
        //             return {};
        //         }
        //         return {
        //             Authorization: `Bearer ${session.token}`,
        //         };
        //     },
        //     //webSocketImpl: WebConnect,
        // });
        const client = new ApolloClient({
            cache: new InMemoryCache(),
            uri: 'http://localhost:3030/graphql-http?dork=yes',
        });
        setClient(client)
        return () => {
            console.log("UnMount: Cleaning up ApolloClient")
            // cleanup
        }
    }, [])
    return (
        <div>
            { client !== undefined ?
                <ApolloProvider client={client}>
                    <SomeData />
                </ApolloProvider>
                :
                <div>
                    Connecting...
            </div>
            }
        </div>
    )
}

const ErrorDisplay = (error) => <div style={{
    color: 'red',
    border: '1px solid red',
    background: '#fdd',
}}>Error: {error} </div>

// export const Websocket = () => {
//     const [client, setClient] = useState(undefined)
//     useEffect(() => {
//         // setup 
//         console.log("Setting up websocket")
//         const link = new WebSocketLink({
//             url: 'ws://localhost:3030/graphql?dork=yes',
//             connectionParams: () => {
//                 const session = false
//                 if (!session) {
//                     return {};
//                 }
//                 return {
//                     Authorization: `Bearer ${session.token}`,
//                 };
//             },
//             //webSocketImpl: WebConnect,
//         });

//         const client = new ApolloClient({
//             cache: new InMemoryCache(),
//             link: link,
//         });

//         setClient(client)
//         return () => {
//             console.log("UnMount: Cleaning up websocket")

//             // cleanup
//         }
//     }, [])
//     return (
//         <div>
//             <h1>Apollo boundary</h1>
//             { client !== undefined ?
//                 <ApolloProvider client={client}>
//                     <h1>Hello apollo</h1>
//                     <SomeData />
//                 </ApolloProvider>
//                 :
//                 <div>
//                     Connecting...
//             </div>
//             }
//         </div>
//     )
// }


const SomeData = () => {
    const q = gql`
        query {
            items: articles {
                id
                title
                author
                body
            }
        }
    `
    const { loading, error, data } = useQuery(q, {
        pollInterval: 5 * 1000,
        variables: {
        }
    })
    if (error) {
        return <p>Error :(</p>
    }
    if (loading) {
        return <Loading />
    }
    if  (data.items.length == 0 ) {
        return <h2>No articles available</h2>
    }
    return <div>{ data.items.map( entry => <Post data={entry} /> ) }</div>
}


const Loading = () => <div>Loading...</div>
const ApolloUse = {
    title: "Apollo",
}
export default ApolloUse
