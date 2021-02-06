import React from 'react'

export const Servo = ({
    id,
    angle = 90,

    max = 180,
    min = 0,

    width = 80,
    height = 180,

    pinWidth = 40,
}) => {
    let errorCondition = -1
    let targetAngle = 0

    if (angle < min) {
        targetAngle = min
        errorCondition = 1
    } else if (angle > max) {
        targetAngle = max
        errorCondition = 1
    } else {
        targetAngle = angle
    }
    return (
        <div style={{
            width: width,
            height: height,
            position: 'relative',
            background: 'rgba(0,0,0,.9)',
            borderRadius: 7,
            margin: 10,
            marginTop: 10,
            display: 'inline-block',
        }}>
            <div style={{
                position: 'absolute',
                top: 10,
                left: 20,
                right: 20,

                width: '2.0em',
                height: '1.0em',
                borderRadius: 3,
                background: 'rgba(255,255,0,.6)',
                border: '1px solid rgba(0,0,0,.4)',

                textAlign: 'center',
                fontWeight: 'bold',
                fontSize: '0.9em',
                lineHeight: '1.0em',
            }}>{id}</div>

            <div style={{
                position: 'absolute',
                bottom: 10,
                left: 20,
                right: 20,

                width: 'auto',
                height: 25,
                borderRadius: 5,
                background: (errorCondition === -1 ? 'rgba(255,255,255,1)' : 'rgba(255,200,200,1)'),

                textAlign: 'center',
                fontWeight: 'bold',
                fontSize: '1.4em',
                lineHeight: '1.2em',
            }}>{angle}</div>

            <div style={{
                position: 'absolute',
                top: height / 2 - (pinWidth / 2),
                left: width / 2 - (pinWidth / 2),

                width: pinWidth,
                height: pinWidth,
                borderRadius: 20,
                background: 'rgba(255,255,255,1)',
            }}>
                <div style={{
                    position: 'relative'
                }}>
                    <div style={{
                        position: 'absolute',
                        top: 20,
                        left: 20 - 2.5,
                        width: 5,
                        height: 120,
                        background: 'red',
                        transform: `rotate(${targetAngle}deg)`,
                        transformOrigin: "top",
                    }}></div>
                </div>
            </div>
        </div>
    )
}
