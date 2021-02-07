import { useState, useEffect } from 'react'
import { ApolloLink, HttpLink, Operation, FetchResult, Observable, SubscriptionClient, ApolloClient, ApolloProvider, InMemoryCache, gql, useQuery } from '@apollo/client';


import { ESP8266 } from '../workstudyplay-parts/esp8266'

const ESP8266Story = {
    title: "ESP8266",
}


export const Boards = () => {
    const [client, setClient] = useState(undefined)
    useEffect(() => {
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
                    <RegisteredBoards />
                </ApolloProvider>
                :
                <div>
                    Connecting...
            </div>
            }
        </div>
    )
}


const RegisteredBoards = () => {
    const q = gql`
        query {
            items: boards {
                id
                name
                address
                chipName
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
        return <h2>No posts available</h2>
    }
    return <div>{ data.items.map( entry => <ESP8266 data={entry} /> ) }</div>
}


const Loading = () => <strong>Loading...</strong>

export {
    ESP8266,
}
ESP8266.args ={

}

export default ESP8266Story