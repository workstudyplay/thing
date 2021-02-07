import React from 'react'

const SOC = ({ chipName = ''}) => {
    const height = 120
    const width = 80
    const fill = 'silver'
    return (
        <div style={{
            width: width,
            height: height,
            background: fill,
            border: '1px solid black',
            position: 'absolute',
            bottom: 0,
            left:'15%',
        }}>
            <div style={{
                textAlign: 'center',
                background: fill,
                position: 'absolute',
                bottom: 10,
                width: '100%',
                margin: 'auto',
            }}>
                {chipName}
            </div>
        </div>
    )
}

export const ESP8266 = ({data = {} }) => {
    const { id = '', name = '', address = '', chipName = '' } = data
    const height = 280
    const width = 120
    const fill = 'rgb(200,200,80)'
    return (
        <div style={{
            width: width,
            height: height,
            background: fill,
            border: '1px solid black',
            position: 'relative',
            fontFamily: 'arial',
            display: 'inline-block',
            margin: 10,
        }}>
            <div style={{
                position: 'absolute',
                top: 10,
                left: 10,
                right: 10,
                height: 100,
                background: 'black',
                color: 'white',
                textAlign: 'center',
                lineHeight: 1.5,
            }}>
                <strong>{name}</strong><br />
                <strong>{address}</strong><br />
            </div>
            <SOC chipName={chipName} />
        </div>
    )
}


