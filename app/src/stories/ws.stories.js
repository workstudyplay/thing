import { useState, useEffect } from 'react'
import { createClient } from 'graphql-ws';

import { a } from './parse'
import { Client } from 'graphql-ws'
import { Post } from '../posts'

const WSClient = () => {
  const [count, setCount] = useState(1)
  const [posts, setPosts] = useState([])
  const [time, setTime] = useState('00:00:00')

  useEffect(() => {
    const ws = new WebSocket("ws://localhost:3030/graphql?pat")
    ws.onmessage = function (e) {
      const data = JSON.parse(e.data)
      let message
      
      console.log( typeof(data.message), data.message )
      if (typeof (data.message.posts) == "object") {
        try {
          console.log('data.message.posts', data.message.posts)
          // setPosts(data.message.posts)
          
          const v = a( posts, data.message.posts )
          console.log("UPDATED:", v)
    
          console.log( v)
          const newPosts = []
          for( var x in v ){
            const row = v[x]
            console.log( x, row)
            if ( row.id !== undefined ){
              newPosts.push(row)
            }
          }

          setPosts(newPosts)
          // console.log( typeof(data.message.posts), data.message.posts)
          // data.message.posts.map( entry => {
          //   console.log("Udate post", entry )
          // })
        } catch (err) {
          console.error( err)
        }
      } else if (typeof (data.message[0].posts) == "object") {
        console.log('data.message[0]posts', data.message[0].posts)
        setPosts(data.message[0].posts)
      }
    };
    ws.onclose = function (e) {
      console.log("Closed");
    }
    ws.onopen = function (e) {
      console.log("Sending query:")
      // const query = `{ posts { title author age body } }`
      // const variables = `{}`
      ws.send(`{"id":"${count}","type":"subscribe","message":{"query":"{ posts {  id    title    author    age    body  }}","variables":{}}}`)
    };

    return () => {
      console.log("unsubscribe ...")
      ws.send(`{"id":"${count}","type":"unsubscribe"}`)
    }
  }, [])



  return <Listing posts={posts} time={time} />
}
const Listing = ({ time = '??:??:??', posts = [] }) =>
  <div>
    {posts.length>0 ? (posts.map(entry =>
      <Post data={entry} />

    )) : <div>No posts
      </div>}
  </div>

const Story = {
  title: "WS",
}


export default Story

export {
  WSClient,
}