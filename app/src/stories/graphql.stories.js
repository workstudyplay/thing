// import React from 'react'
// import { storiesOf } from '@storybook/react'
// import { SubscriptionClient, ApolloClient, ApolloProvider, InMemoryCache, gql, useQuery } from '@apollo/client'

// // import { WebSocketLink } from "@apollo/client/link/ws";
// // // import { WebSocketLink } from "@apollo/client/subscription/ws";
// // import { SubscriptionClient } from "subscriptions-transport-ws";

// import { split, HttpLink } from '@apollo/client';
// import { getMainDefinition } from '@apollo/client/utilities';
// import { WebSocketLink } from '@apollo/client/link/ws';

// import GraphiQL from 'graphiql';
// import { connection, mutate,  } from 'thunder-react';
// import './graphiql.css';

// const subscribe = () => {
//   return {}
// }
// function graphQLFetcher({query, variables}) {
//   if (query.startsWith("mutation")) {
//     return mutate({
//       query,
//       variables,
//     });
//  } else if( query.indexOf("IntrospectionQuery") >-1 ) {
//    alert('sss')
//   return 
//   }

// console.log(query)
//   return {
//     subscribe(subscriber) {
//       console.log(subscriber);
//       // const next = subscriber.next || subscriber;
//       // const {error, complete} = subscriber;

//       // const subscription = connection.subscribe({
//       //   query: query,
//       //   variables: variables,
//       //   observer: ({state, valid, error, value}) => {
//       //     if (valid) {
//       //       next({data: value});
//       //     } else {
//       //       next({state, error});
//       //     }
//       //   }
//       // });

//       // return {
//       //   unsubscribe() {
//       //     return subscription.close();
//       //   }
//       // };
//     }
//   };
// }

// var visualStories = storiesOf('Lab | GraphQL')

// visualStories.add('Thunder!', () =>
//   <div>
//     <GraphiQL fetcher={graphQLFetcher} />
//   </div>

//  )

// // const GRAPHQL_ENDPOINT = "ws://localhost:3131/graphql";
// const httpLink = new HttpLink({
//   uri: 'http://localhost:3030/graphql'
// });

// const wsLink = new WebSocketLink({
//   uri: `ws://localhost:3030/graphql`,
//   options: {
//     reconnect: true,
//     connectionParams: {
//       authToken: "abc123",
//     },
//   },
// });

// const link = split(
//   ({ query }) => {
//     const definition = getMainDefinition(query);
//     return (
//       definition.kind === 'OperationDefinition' &&
//       definition.operation === 'subscription'
//     );
//   },
//   wsLink,
//   httpLink,
// );

// const client = new ApolloClient({
//   cache: new InMemoryCache(),
//   link: wsLink,
// });

// visualStories.add('WS', () =>
//   <div>
//     <h1>asdasd</h1>
//     <ApolloProvider client={client}>
//       <TokenStatus />
//     </ApolloProvider>
//   </div>

//  )
// const tokensQuery = gql`
// query time {
//   time
// }
// `

// const Loading = () => <div>
//   Loading...
// </div>

// const TokenStatus = () => {

//   const { loading, error, data } = useQuery(tokensQuery, {
//     variables: {
//     }
//   })
//   if (loading) {
//     return <Loading />
//   }

//   console.log(data)
//   // try {
//   //     setLibraryData && setLibraryData(data.user.library.projects)
//   // } catch (err) { }
//   if (error) {
//     console.log(error)
//     return <p>Error :(</p>
//   }
//   if (data && data.tokens && data.tokens.length) {
//     return <div>
//       {data.tokens.map(row =>
//         <div>
//           {row.forgeEmail}
//         </div>
//       )}
//     </div>
//   }
//   return <div>
//     ...
//       </div>
// }
