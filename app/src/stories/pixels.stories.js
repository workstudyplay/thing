import React from 'react'

import  { Servo } from '../workstudyplay-parts'

const PixelStory = {
   title: 'Pixels',
   parameters: {
      actions: {
        handles: ['onMessage', 'click .btn'],
      },
   },
   argTypes: {
      // label: { control: 'text' },
      // borderWidth: { control: { type: 'number', min: 0, max: 10 } },
   },
};
export default PixelStory

const Pixel = (props) => {
   const {
      index,
      ringId,
      deg = 0,
      r = 100,
      width = 20,
      color = '#492',

      onMessage = (msg) => { },
   } = props
   const DEG_TO_RAD = 0.0174532925
   // convert the degrees to radians
   const top = r * Math.cos(deg * DEG_TO_RAD)
   const left = r * Math.sin(deg * DEG_TO_RAD)

   return (
      <button onClick={() => onMessage({ ...props, event: "click", ringId: ringId, pixelId: index })} style={{
         position: 'absolute',
         top,
         left,
         width,
         height: width,
         borderRadius: (width / 2),
         background: color,
         border: '2px solid rgba(0,0,0,.4)',
      }}>
      </button>
   )
}

export const Rings = ({
   ringCount = 1,
   pixelCount = 16,
   ringWidth = 60,
   handleMessage = (msg) => {
      console.log(msg)
   },
}) => {

   const rings = []
   for (let n = 0; n < ringCount; n++) {
      rings.push({
         id: n,
         onMessage: handleMessage,
      })
   }
   return (
      <div>
         { rings.map(ring =>
            <div style={{ textAlign: 'center', display: 'inline-block', margin: 10, }}>
               <Bulb height={ringWidth} color={randomRGBAColor()} />
               <NeopixelRing {...ring} radius={ringWidth} pixelCount={pixelCount} onMessage={handleMessage} />
               <Servo id={ring.id} angle={Math.floor(Math.random() * 180)} />
            </div>
         )}
      </div>
   )
}
Rings.args = {
   ringCount: 2,
   pixelCount: 16,
   ringWidth: 120,
}
const randomHexColor = () => "#" + Math.floor(Math.random() * 16777215).toString(16)
const randomRGBAColor = () => "rgba(" + randomValString(255) + ", " + randomValString(255) + ", " + randomValString(255) + ", 1.0)"

const randomValString = (max) => Math.floor(Math.random() * max).toString(10)

export const NeopixelRing = ({
   radius = 150,
   background = '#a9a9a9',
   borderColor = '#000',
   borderWidth = 1,
   pixelCount = 16,
   pixelRadius = 35,
   padding = 2,
   centerDotWidth = 220,

   id,
   onMessage = (msg) => { },
}) => {

   const pixels = []
   for (let n = 0; n < pixelCount; n++) {
      pixels.push({
         index: n,
         ringId: id,
         r: radius - (pixelRadius / 2) - padding,
         width: pixelRadius,
         deg: ((n / pixelCount) * 360),
         color: randomHexColor(),
         onMessage,
      })
   }
   // create circles, position them around the item
   return (
      <div style={{
         background,
         borderColor,
         borderWidth,
         height: (radius * 2),
         width: (radius * 2),
         borderRadius: radius,
         position: 'relative',
      }}>
         {/* <div style={{
            position: 'absolute',
            top: radius - (centerDotWidth / 2),
            left: radius - (centerDotWidth / 2),
            background: '#fff',
            height: centerDotWidth,
            width: centerDotWidth,
            borderRadius: (centerDotWidth / 2),
         }} /> */}
         <div style={{
            position: 'absolute',
            top: radius - (pixelRadius / 2),
            left: radius - (pixelRadius / 2),
            background: ')',
            height: 5,
            width: 5,
            borderRadius: 2.5,

         }}>
            {pixels.map(pixel => <Pixel {...pixel} />)}
         </div>
      </div>
   )
}
NeopixelRing.args = {
   pixelCount: 16,
   radius: 150,
   pixelRadius: 35,
   padding: 2,
   centerDotWidth: 220,
}

export function LightenDarkenColor(col, amt) {
   var num = parseInt(col, 16);
   var r = (num >> 16) + amt;
   var b = ((num >> 8) & 0x00FF) + amt;
   var g = (num & 0x0000FF) + amt;
   var newColor = g | (b << 8) | (r << 16);
   return newColor.toString(16);
}

export const Bulbs = ({ num = 200 }) => {
   const bulbs = []
   for (let n = 0; n < num; n++) {
      bulbs.push({
         index: n,
         height: 60,
         color: randomRGBAColor(),
      })
   }
   return bulbs.map(bulb => <Bulb {...bulb} />)
}
Bulbs.args = {
   num: 200,
}

export const Bulb = ({
   height = 250,
   color = 'rgba(240,240, 0,1.0)',
}) => {

   //   const bulbColor = 'rgba(10,10,10,0.2)'
   // const bulbColor = (color.length==0?'':color)

   const filamentColor = color
   const bulbColor = color
   //'rgba(240,240, 0,0.8)'
   return (
      <span style={{

      }}>

         <svg version="1.1" id="Layer_1" x="0px" y="0px" style={{
            height: height,
         }} viewBox="0 0 318.319 318.319" _style={{ enableBackground: 'new 0 0 318.319 318.319' }}>
            <g>
               <path style={{ fill: bulbColor }} d="M203.962,249.168c0-48.734,61.31-62.097,61.31-143.056C265.272,47.507,217.766,0,159.16,0
		C100.552,0,53.047,47.507,53.047,106.112c0,80.959,61.31,94.322,61.31,143.056H203.962z"/>
               <path style={{ fill: filamentColor }} d="M176.138,253.353c-2.311,0-4.185-1.874-4.185-4.184c0-31.724,11.316-48.158,22.259-64.052
		c9.104-13.222,17.762-25.798,19.679-47.205l-11.816,9.565c-1.699,1.376-4.171,1.211-5.673-0.378l-16.145-17.077l-15.064,16.979
		c-0.794,0.895-1.935,1.407-3.13,1.407c-1.196,0-2.336-0.513-3.13-1.407l-15.064-16.979l-16.145,17.077
		c-1.501,1.589-3.973,1.753-5.672,0.378l-11.817-9.565c1.917,21.407,10.575,33.982,19.679,47.205
		c10.943,15.894,22.259,32.328,22.259,64.052c0,2.311-1.873,4.184-4.184,4.184s-4.184-1.874-4.184-4.184
		c0-29.122-10.096-43.784-20.783-59.307c-10.594-15.384-21.547-31.292-21.547-61.036c0-1.612,0.927-3.082,2.382-3.776
		c1.459-0.694,3.181-0.489,4.435,0.524l16.014,12.961l16.622-17.581c0.806-0.853,1.948-1.339,3.106-1.309
		c1.172,0.018,2.285,0.529,3.063,1.406l14.966,16.869l14.966-16.869c0.778-0.877,1.891-1.388,3.063-1.406
		c1.225-0.019,2.3,0.457,3.105,1.309l16.622,17.581l16.014-12.961c1.255-1.014,2.978-1.217,4.435-0.524
		c1.455,0.694,2.382,2.164,2.382,3.776c0,29.744-10.953,45.652-21.546,61.036c-10.688,15.523-20.783,30.185-20.783,59.307
		C180.322,251.479,178.449,253.353,176.138,253.353z"/>
               <path style={{ fill: '#5C6670' }} d="M108.498,267.656c0,27.981,22.681,50.663,50.662,50.663s50.662-22.682,50.662-50.663H108.498z" />
               <path style={{ fill: '#333E48' }} d="M218.973,263.315c0-7.782-6.365-14.147-14.147-14.147h-91.335c-7.779,0-14.147,6.365-14.147,14.147
		v19.65c0,7.783,6.368,14.149,14.147,14.149h91.335c7.782,0,14.147-6.366,14.147-14.149V263.315z"/>
            </g>

         </svg>
      </span>

   )
}
Bulb.argTypes = {
   color: { control: 'color' },
}
Bulb.args = {
   color: 'rgba(240,240, 0,0.2)',
}

export const Led = ({ 
   width = 80,
   color = 'rgba(255,0,0,0.7)',
}) => {
   const fill = color
   const topHeight = 80
   const topWidth = 60
   const ridgeHeight = 25

   const wireFill = 'rgba(150,150,150, 1)'
   const anodeHeight = 120
   const cathodeHeight = anodeHeight - 15
   
   const height = 200
   return (
      <div style={{
         width: width,
         height: height,
         // background: 'tan',
         position: 'relative',
         display: 'inline-block',
         margin: 10,
      }}>
         <div key="anode" style={{
            position: 'absolute',
            top: topHeight-20,
            left: '30%',
            background: wireFill,
            border: '1px solid rgba(0,0,0,0.8)',
            height: anodeHeight,
            width: 3,
         }} />

         <div key="cathode" style={{
            position: 'absolute',
            top: topHeight-20,
            right: '30%',
            background: wireFill,
            border: '1px solid rgba(0,0,0,0.6)',
            borderRadius: 1,
            height: cathodeHeight,
            width: 3,
         }} />
  

         <div style={{
            opacity: 0.9,
            margin: 'auto',
            background: fill,
            border: '1px solid rgba(0,0,0,0.8)',
            width: '75%',           
            height: topHeight + ridgeHeight,
            borderRadius: topWidth/2,
            clipPath: 'inset(0 0 26px 0)',
         }} />

         <div style={{
            opacity: 0.9,
            position: 'absolute',
            top: topHeight,
            left: 0,
            right: 0,
            background: fill,
            border: '1px solid rgba(0,0,0,0.8)',
            height: ridgeHeight,
            borderRadius: 4,
         }} />


      </div>
   )
}
Led.argTypes = {
   color: { control: 'color' },
}
Led.args = {
   color: 'rgba(0,255,0,0.7)'
}


export const Leds = ({ num = 20, width = 80 }) => {
   const items = []
   for (let n = 0; n < num; n++) {
      items.push({
         index: n,
         width: width,
         color: randomRGBAColor(),
      })
   }
   return items.map(entry => <Led {...entry} />)
}
Leds.args = {
   num: 200,
   width: 80,
}