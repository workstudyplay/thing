import React, { useState, useEffect } from 'react'
import { Servo, ServoController } from '../workstudyplay-parts'

export {
   Servo,
   ServoController,
}
Servo.args = {
   id: 0,
   angle: 0,
}
ServoController.args = {
   id: 1,
   channels: [
      {
         id: 2,         
      }
   ]
}
export const ServoApp = () => {
   const [controllers] = useState([
      {
         id: 30,
      },
      {
         id: 31,
      },
      {
         id: 32,
      },
   ]);
   const [servos] = useState([
      {
         id: 1,
         enabled: false,
         value: 0,
      },
      {
         id: 2,
         enabled: false,
         value: 0,
      },
      {
         id: 3,
         enabled: false,
         value: 0,
      },
      {
         id: 4,
         enabled: false,
         value: 0,
      },
      {
         id: 5,
         enabled: false,
         value: 0,
      },
      {
         id: 6,
         enabled: false,
         value: 0,
      },
      // {
      //    id: 7,
      //    enabled: false,
      //    value: 0,
      // },
      // {
      //    id: 8,
      //    enabled: false,
      //    value: 0,
      // },
      // {
      //    id: 9,
      //    enabled: false,
      //    value: 0,
      // },
      // {
      //    id: 10,
      //    enabled: false,
      //    value: 0,
      // },
      // {
      //    id: 11,
      //    enabled: false,
      //    value: 0,
      // },
      // {
      //    id: 12,
      //    enabled: false,
      //    value: 0,
      // },
      // {
      //    id: 13,
      //    enabled: false,
      //    value: 0,
      // },
      // {
      //    id: 14,
      //    enabled: false,
      //    value: 0,
      // },
      // {
      //    id: 15,
      //    enabled: false,
      //    value: 0,
      // },
      // {
      //    id: 16,
      //    enabled: false,
      //    value: 0,
      // },
   ]);

   const [channels, setChannelState] = useState([
      ...servos,
      ...controllers,     
   ]);

   const [channelData, setChannelData] = useState({});

   useEffect(() => {
      // Update the document title using the browser API
      console.log("channels changed:", channels)

   }, [channels, servos, controllers]);

   const handleChange = (msg) => {
      const val = msg.value
      const changes = {}
      let numChanges = 0
      if (msg.channels.length > 0) {
         msg.channels.map(ch => {
            changes[ch] = val
            return numChanges++
         })
      }
      
      const channelStateData = {}
      const newState = []

      // newState[id] = msg.value
      channels.map(entry => {
         const { id } = entry
         if (changes[id] !== undefined) {
            console.log('settingChannel', entry)
            channelStateData[id] = val
            newState.push({
               ...entry,
               value: val,
            })
         } else {
            channelStateData[id] = entry.value
            newState.push({
               ...entry,
            })
         }
         return newState
      })
      if (numChanges > 0) {
         setChannelData(channelStateData)
         setChannelState(newState)
      }
      return
   }
   return (
      <div>
         { servos.map(entry => <Servo
            key={`servo-${entry.id}`}
            id={entry.id}
            angle={channelData[entry.id]}
         />)}
         { controllers.map(entry => <ServoController
            id={entry.id}
            channels={channels}
            angle={channelData[entry.id]}
            onChange={msg => handleChange(msg)}
         />)}
      </div>
   )
}

export const Servos = ({
   servoCount = 4,
   pixelCount = 16,
   ringWidth = 60,
   handleMessage = (msg) => {
      console.log(msg)
   },
}) => {
   const items = []
   for (let n = 0; n < servoCount; n++) {
      items.push({
         id: n,
         onMessage: handleMessage,
      })
   }
   return (
      <div>
         { items.map(entry =>
            <div style={{ textAlign: 'center', display: 'inline-block', margin: 10, }}>
               <Servo id={entry.id} angle={Math.floor(Math.random() * 180)} />
            </div>
         )}
      </div>
   )
}
Servos.args = {
   servoCount: 16,
   onChange: () => {
      return (
         { action: "changed" }
      )
   }
}

const ServoStory = {
   title: 'Servos',
   parameters: {
      actions: {
         handles: ['onMessage', 'click .btn'],
      },
   },
   argTypes: {
      // label: { control: 'text' },
      // borderWidth: { control: { type: 'number', min: 0, max: 10 } },
   },
}
export default ServoStory