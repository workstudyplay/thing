import React, { useState, useEffect } from 'react'
import { Slider } from '@material-ui/core';

export const ServoController = ({
    id,
    angle = 90,

    width = 80,
    height = 220,
    pinWidth = 40,

    channels = [],
    onChange = (evt) => {
        console.log(evt)
    }
}) => {
    const [activeChannels, setActiveChannels] = useState([]);
    let errorCondition = -1

    const [currentValue, setCurrentValue] = useState(angle)
    
    useEffect( () => {
        console.log( currentValue )
        onChange({
            id: id,
            channels: activeChannels,
            value: currentValue,
        })
    }, [
        currentValue
    ])

    if (activeChannels.length > 0) {
        if (currentValue !== angle ) {
            setCurrentValue( angle )
        }

    }

    return (
        <div style={{
            width: width,
            height: height,
            position: 'relative',
            background: 'rgba(0,0,0,.4)',
            borderRadius: 7,
            margin: 10,
            marginTop: 10,
            display: 'inline-block',
            fontSize: '0.8em',
        }}>
            <div style={{
                height: 25,
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
            </div>
            { channels.map((entry) => ( entry.id===id? '' : 
                <div style={{
                    float: 'left',
                    width: '50%',
                }}>
                    <input type="checkbox" onChange={() => {
                        if (activeChannels.indexOf(entry.id) !== -1) {
                            console.log("disable channel", entry.id)
                            const c = [
                                ...activeChannels.filter(e => e !== entry.id)
                            ]
                            console.log(c)
                            setActiveChannels(c)
                        } else {
                            console.log("enable channel", entry.id)
                            const c = [
                                ...activeChannels,
                                entry.id,
                            ]
                            console.log(c)
                            setActiveChannels(c)
                        }
                    }} name="ch[]" value="1" /> {entry.id}
                </div>
            ))}
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
            }}>{currentValue}</div>

            <div style={{
                position: 'absolute',
                bottom: -50,

                width: '100%',
                height: pinWidth,
                borderRadius: 20,
                background: 'rgba(255,255,255,.5)',
            }}>
                <Slider
                    defaultValue={0}
                    onChange={(event, newValue) => {
                        if (activeChannels.length > 0) {
                            onChange({
                                id: id,
                                channels: activeChannels,
                                value: newValue,
                            })
                        }
                    }}
                    //getAriaValueText={"slider"}
                    aria-labelledby="claw"
                    valueLabelDisplay="off"
                    step={5}
                    min={0}
                    max={180}
                />
            </div>
        </div>
    )
}
