import React from 'react'

const SOC = () => {
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
                ESP8266
            </div>
        </div>
    )
}

export const ESP8266 = ({ id }) => {
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
        }}>
            <SOC />
        </div>
    )
}


