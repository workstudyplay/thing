// import { split } from 'apollo-link';
import { HttpLink, setContext, split, Operation, FetchResult, Observable, SubscriptionClient, ApolloClient, ApolloProvider, InMemoryCache, gql, useQuery } from '@apollo/client';

// import { HttpLink } from 'apollo-link-http';
import { getMainDefinition } from 'apollo-utilities';
// import { SubscriptionClient } from 'subscriptions-transport-ws';
// import { WebSocketLink } from 'apollo-link-ws';

// import { setContext } from 'apollo-link-context';
// import { ApolloClient, ApolloProvider, InMemoryCache, gql, useQuery } from '@apollo/client'

const GRAPHQL_HTTP_URI = 'http://localhost:3030/api/graphql?pat'
const GRAPHQL_WS_URI = `ws://localhost:3030/graphql`


const httpLink = new HttpLink({
    uri: GRAPHQL_HTTP_URI,
});

const getStoredToken = () => "NO-TOKEN"

const authLink = setContext((_, { headers }) => {
    // get the authentication token from local storage if it exists
    const token = getStoredToken()
    // return the headers to the context so httpLink can read them
    console.log("Adding auth details")
    return {
        headers: {
            ...headers,
            authorization: token ? `Bearer ${token}` : "",
        }
    }
})

const getLink = () => split(
    // split based on operation type
    ({ query }) => {
        const definition = getMainDefinition(query);

        console.log(`definition: ${definition}`)
        return (
            definition.kind === 'OperationDefinition' &&
            definition.operation === 'subscription'
        );
    },
    authLink.concat(httpLink),
);


export const getClient = () => {
    return new ApolloClient({
        cache: new InMemoryCache(),
        link: authLink.concat(httpLink),
    })
}
