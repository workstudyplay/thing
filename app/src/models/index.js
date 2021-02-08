import React, { useEffect, useRef, useState } from 'react'
import { Canvas, useFrame } from 'react-three-fiber'
import * as THREE from 'three'

import Roboto from '../fonts/Roboto_Bold.json';

function GroundPlane() {
    return (
        <mesh receiveShadow rotation={[5, 0, 0]} position={[0, -1, 0]}>
            <planeBufferGeometry attach="geometry" args={[500, 500]} />
            <meshStandardMaterial attach="material" color="white" />
        </mesh>
    );
}

function BackDrop() {
    return (
        <mesh receiveShadow position={[0, -1, -5]}>
            <planeBufferGeometry attach="geometry" args={[500, 500]} />
            <meshStandardMaterial attach="material" color="white" />
        </mesh>
    );
}

export const Scene = ({ 
    height = 637,
    width = '100%',
    stories = [],
    onSelect = () => {}
}) => {
    const [hoverItem, setHoverItem] = useState(undefined)
    const handleMouseOver = (item) => {
        setHoverItem( item )
    }
    const handleMouseOut = (item) => {
        setHoverItem(undefined)
    }
    const handleClick = (item) => {
        onSelect( item )
    }

    useEffect( () => {
        if(hoverItem!==undefined){
            console.log("hovered", hoverItem)
        }
    }, [hoverItem])

    return (
        <div style={{
            minWidth: 600,
            minHeight: 400,
            // maxWidth: 800,
            width: width,
            height: height,
            margin: 'auto',

            border: '1px solid rgba(0,0,0,0.1)',
            boxSizing: 'border-box',
        }}>
            <Canvas style={{
                width: '100%',
                height: height,
                cursor: (hoverItem !==undefined?'pointer':'default'),
            }}>
                <ambientLight />

                {/* <GroundPlane /> */}
                {/* <BackDrop /> */}

                <pointLight position={[10, 10, 10]} />
                { stories.map( s => <Box 
                    storyId={s.id} 
                    name={s.title} 
                    color={s.color} 
                    position={s.position} 
                    onSelect={i => handleClick(i)} 
                    onMouseover={(e) => handleMouseOver(e)} 
                    onMouseout={(e) => handleMouseOut(e)}
                /> )}

                {/* <Box storyId="1" name="Hat" color="red" position={[-3.5, -0.1, 0.6]} onSelect={i => handleClick(i)} onMouseover={(e) => handleMouseOver(e)} onMouseout={(e) => handleMouseOut(e)} />
                <Box storyId="2" name="Shoe" color="green" position={[-1.3, 0, 0]}onSelect={i => handleClick(i)} onMouseover={(e) => handleMouseOver(e)} onMouseout={(e) => handleMouseOut(e)} />
                <Box storyId="3" name="Sculpture" color="blue" position={[1.3, 0, 0]} onSelect={i => handleClick(i)} onMouseover={(e) => handleMouseOver(e)} onMouseout={(e) => handleMouseOut(e)} />
                <Box storyId="4" name="Jacket" color="yellow" position={[3.5, -0.1, 0.6]}onSelect={i => handleClick(i)} onMouseover={(e) => handleMouseOver(e)} onMouseout={(e) => handleMouseOut(e)} /> */}
            </Canvas>
        </div>
    )
}

function Box(props) {
    // This reference will give us direct access to the mesh
    const mesh = useRef()

    // Set up state for the hovered and active state
    const [hovered, setHover] = useState(false)
    const [active, setActive] = useState(false)

    // Rotate mesh every frame, this is outside of React without overhead
    useFrame(() => {
        if (hovered ) {
            mesh.current.rotation.y += 0.01
            //mesh.current.rotation.x = 0.2
        
        } else {
            mesh.current.rotation.x = 0
        }
    })

    const font = new THREE.FontLoader().parse(Roboto);

    const textOptions = {
        font,
        size: 0.2,
        height: 1,
        depth: 0.5,
    };
    
    const text = props.name
    return (
        <group
            {...props}
            style={{
                cusror: 'pointer',
            }}>
            <mesh
                ref={mesh}
                position={[0,0,0]}
                scale={[1, 1, 1]}
                onClick={(event) => {
                    setActive(!active)
                    props.onSelect && props.onSelect( props )
                }}
                onPointerOver={(event) => {
                    setHover(true)
                    props.onMouseover && props.onMouseover( props)
                }}
                onPointerOut={(event) => {
                    setHover(false)
                    props.onMouseout && props.onMouseout( props )
                }}>
                    <boxBufferGeometry args={[1, 1, 1]} />
                    <meshStandardMaterial color={props.color} />            
            </mesh>
            <mesh
                position={[-0.25,-1.2,0]}
                rotation={[0,props.position[0]<=0?0.2:-0.2,0]}
                scale={
                    [1,1,0.005]
            }>
                <textGeometry attach='geometry' args={[text, textOptions]} />
                <meshStandardMaterial color={'black'} attach='material' />
            </mesh>
    </group>

    )
}