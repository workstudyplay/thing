import React, { useState, useEffect, useRef } from 'react'
import zeropad from 'zeropad'

function useInterval(callback, delay) {
    const savedCallback = useRef();

    // Remember the latest function.
    useEffect(() => {
        savedCallback.current = callback;
    }, [callback]);

    // Set up the interval.
    useEffect(() => {
        function tick() {
            savedCallback.current();
        }
        if (delay !== null) {
            let id = setInterval(tick, delay);
            return () => clearInterval(id);
        }
    }, [delay]);
}

export const Clock = ({
    height = 200,
    fill = "#555",
    //hour = 0, minute = 0, second = 0
    }) => {

    const [hourAngle, setHourAngle] = useState(0);
    const [minuteAngle, setMinuteAngle] = useState(0);
    const [secondAngle, setSecondAngle] = useState(0);

    const [ms, setMicroseconds] = useState(new Date().getSeconds());
    const [seconds, setSeconds] = useState(new Date().getSeconds());
    const [minutes, setMinutes] = useState(new Date().getMinutes());
    const [hours, setHours] = useState(new Date().getHours()>12?new Date().getHours()%12:new Date().getHours());

    const movePerTick = 360 / 60

    useInterval(() => {
        if (ms === 999) {
            setMicroseconds(0)
        } else {
            setMicroseconds( ms+1 )
        }   
    }, 1);

    useInterval(() => {
        
        if (seconds===59){
            if (minutes===59) {
                if (hours === 11 ) {
                    setHours( 0 )
                } else {
                    setHours( hours +1 )
                }
                setMinutes( 0 )
            } else {
                setMinutes( minutes + 1)
            }
            setSeconds(0)
        } else {
            setSeconds(seconds+1)
        }         
        setSecondAngle( (360 / 60 ) * seconds -180 ) 
        setMinuteAngle( movePerTick/60 * (minutes *60 + seconds) -180  )
        setHourAngle( (360/12 *hours) + (360/60/60 * minutes) -180  )
    }, 1000);

    const outerBorder = 8
    const outerBorderWidth = height + outerBorder*2

    return (
        <div style={{
            width: height,
            height: height,
            borderRadius: height / 2,
            border: '20px solid tan',
            background: fill,
            boxSizing: 'border-box',
            position: 'relative',
        }}>

        <div style={{
            width: outerBorderWidth,
            height: outerBorderWidth,
            borderRadius: outerBorderWidth,
            border: `${outerBorder}px solid black`,
            background: 'transparent',
            boxSizing: 'border-box',
            position: 'absolute',
            top: (outerBorder+20) * -1,
            left: (outerBorder+20) * -1,
        }} />

            <div key="hour-hand" style={{
                position: 'absolute',
                left: '50%',
                top: '50%',
                width: 5,
                height: height * .26,
                background: 'black',
                transform: `rotate(${hourAngle}deg)`,
                transformOrigin: "top",
            }} />

            <div key="minute-hand" style={{
                position: 'absolute',
                left: '50%',
                top: '50%',
                width: 5,
                height: height * .46,
                background: 'black',
                transform: `rotate(${minuteAngle}deg)`,
                transformOrigin: "top",
            }} />

            <div key="second-hand" style={{
                position: 'absolute',
                left: '50%',
                top: '50%',
                width: 3,
                height: height * .46,
                background: 'red',
                transform: `rotate(${secondAngle}deg)`,
                transformOrigin: "top",
            }} />
            
            <div key="time readout" style={{
                position: 'absolute',
                left: 0,
                right: 0,
                bottom: -60,
                height: 23,
                background: 'black',
                color: 'lime',
                fontWeight: 'bold',
                fontFamily: 'courier',
                fontSize: 20,
                textAlign: 'center',
                lineHeight: '1.2em',
            }}>
                {zeropad(hours)}:{zeropad(minutes)}:{zeropad(seconds)}:{zeropad(ms,4)}
            </div>
        </div>

    )
}