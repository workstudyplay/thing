import React from 'react'

const UI = {
   title: 'UI',
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

const width = 10

const Square = ({ rowId, colId, color = 'white', borderColor = 'black', borderWidth = .75 }) =>
   <div
      onClick={() => {

         console.log('rowId: ' + rowId + ' colId: ' + colId)
      }}
      style={{
         display: 'inline-block',
         width: width,
         height: width,
         background: color,
         border: `${borderWidth}px solid ${borderColor}`,
         margin: .5,
      }}></div>

const Vcc = ({ rowId, colId }) => <Square colId={colId} rowId={rowId} color="red" />
const Gnd = ({ rowId, colId }) => <Square colId={colId} rowId={rowId} color="gray" />

const BoardRow = ({ rowId }) =>
   <div style={{
      background: 'rgba(200, 100,10, 0.8)',
      width: 280,
      textAlign: 'center',
      lineHeight: '10px',
      padding: 0,
      margin: 0,
   }}>
      <Vcc colId={"A"} rowId={rowId} />
      <Gnd colId={"B"} rowId={rowId} />
      <Square colId={"C"} rowId={rowId} color="tan" />
      <Square colId={"D"} rowId={rowId} color="tan" />
      <Square colId={"E"} rowId={rowId} color="tan" />
      <Square colId={"F"} rowId={rowId} color="tan" />
      <Square colId={"G"} rowId={rowId} color="tan" />
      <Square colId={"H"} rowId={rowId} color="tan" />
      <Square colId={"I"} rowId={rowId} color="tan" />
      <Square colId={"J"} rowId={rowId} color="tan" />
      <Square colId={"K"} rowId={rowId} color="tan" />
      <Square colId={"L"} rowId={rowId} color="tan" />
      <Vcc colId={"M"} rowId={rowId} />
      <Gnd colId={"N"} rowId={rowId} />
   </div>

export const Breadboard = ({ rowCount = 20, width = 100, height = 8 }) => {
   const rows = []
   for (let n = 0; n < rowCount; n++) {
      rows.push(n)
   }
   return rows.map(entry => <BoardRow />)
}

export const Hamburger = ({ width = 25 }) =>
   <div class="circle" style={{
      background: '#456BD9',
      borderRadius: '50%',
      color: '#fff',
      height: width,
      position: 'relative',
      width: width,
   }}>
      <svg class="circle-icon" style={{
         fill: 'currentColor',
         height: '50%',
         left: '50%',
         position: 'absolute',
         stroke: 'currentColor',
         strokeWidth: 0,
         top: '50%',
         transform: 'translate(-50%, -50%)',
         width: '50%',
      }} viewBox="0 0 24 24" width="24" height="24">
         <line x1="2" x2="22" y1="5" y2="5" stroke-width="3" stroke-linecap="round" />
         <line x1="2" x2="22" y1="12" y2="12" stroke-width="3" stroke-linecap="round" />
         <line x1="2" x2="22" y1="19" y2="19" stroke-width="3" stroke-linecap="round" />
      </svg>
   </div>
Hamburger.args = {
   width: 25,
}


export default UI