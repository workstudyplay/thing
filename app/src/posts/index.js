import { useState, useEffect } from 'react'
import { ApolloClient, ApolloProvider, InMemoryCache, gql, useQuery } from '@apollo/client';

export const Posts = () => {
    const [client, setClient] = useState(undefined)
    useEffect(() => {
        const client = new ApolloClient({
            cache: new InMemoryCache(),
            uri: 'http://localhost:3030/graphql-http?dork=yes',
        });
        setClient(client)
        return () => {
            console.log("UnMount: Cleaning up Articles ApolloClient")
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
    if (data.items.length == 0) {
        return <span />
    }
    return <div>{data.items.map(entry => <Post data={entry} />)}</div>
}


const Loading = () => <div>Loading...</div>





export const Post = ({ data }) => {
    const { id, title = '', author = '', body = '' } = data
    return (
        <div key={`post-${id}`}
            style={{
                background: 'rgba(0,0,0,.2)',
                border: '4px solid rgba(0,0,0,.4)',
                marginBottom: 10,
            }}
        >
            <h1>[{id}]{title}</h1>
            <h2>{author}</h2>
            <div>
                {body}
            </div>
        </div>
    )
}
