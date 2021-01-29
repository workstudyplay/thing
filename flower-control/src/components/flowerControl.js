import React from 'react';
import { SketchPicker } from 'react-color';
import { makeStyles } from '@material-ui/core/styles';
import Typography from '@material-ui/core/Typography';
import Slider from '@material-ui/core/Slider';
import Button from '@material-ui/core/Button';
import ButtonBase from '@material-ui/core/Button';
import IconButton from '@material-ui/core/IconButton';
import { colors } from '@material-ui/core';
import Select from '@material-ui/core/Select';


// fetch(uri, {
//     method: 'POST',
//     headers: {
//         'Content-Type': 'application/json',
//     },
//     options.body
// })
//     .then(response => {
//         return response.json()
//     })
//     .then(responseAsJson => {
//         this.setState({ loading: false, data: responseAsJson.data })
//     })
// )

function hexToRgb(hex) {
    var result = /^#?([a-f\d]{2})([a-f\d]{2})([a-f\d]{2})$/i.exec(hex);
    return result ? {
      r: parseInt(result[1], 16),
      g: parseInt(result[2], 16),
      b: parseInt(result[3], 16)
    } : null;
  }

var flowerState = { 
    r: 0,
    g: 0,
    b: 0,
    pos: 0,
    brightness: 0,
}

const setFlowerProperties = ( props ) => {
    flowerState = props;
  }
  
const host = "http://10.5.1.162"
alert( host )
const sendFlowerProperties = () => {

  alert('s')
  const props = flowerState;
  var uri = host + "/light?r=" + props.r + "&g=" + props.g + "&b=" + props.b;
  uri += "&pos=" + props.pos
  console.log( props)
  if (props.brightness && parseInt(props.brightness) ) {
      uri += "&brightness=" + props.brightness
  }
  fetch(uri, {
      method: 'GET',
  })
      .then(response => {
          return response.json()
      })
      .then(responseAsJson => {
          this.setState({ loading: false, data: responseAsJson.data })
      })
  
}

const sendWipe = () => {
  const props = flowerState;
  
  var uri = host + "/wipe?a=wipe" //?r=" + props.r + "&g=" + props.g + "&b=" + props.b;
 // uri += "&pos=" + props.pos
  console.log( props)
  if (props.brightness && parseInt(props.brightness) ) {
      uri += "&brightness=" + props.brightness
  }
  fetch(uri, {
      method: 'GET',
  })
      .then(response => {
          return response.json()
      })
      .then(responseAsJson => {
          this.setState({ loading: false, data: responseAsJson.data })
      })
  
}



class Component extends React.Component {
  state = {
    background: '#fff',
  };

  sendSequence = (props) => {    
    const delay = 100
    const {
      currentStepIndex = 0,
      loop = true,
      sequence = [
        {
          url: "/light?r=255&g=0&b=0&brightness=60",
          delay: delay,
        },
        {
          url: "/light?r=255&g=26&b=1&brightness=60",
          delay: delay,
        },
        {
          url: "/light?r=203&g=187&b=1&brightness=60",
          delay: delay,
        },
        {
          url: "/light?r=0&g=255&b=0&brightness=60",
          delay: delay,
        },
        {
          url: "/light?r=0&g=0&b=255&brightness=60",
          delay: delay,
        },
        {
          url: "/light?r=144&g=19&b=254&brightness=60",
          delay: delay,
        },       
      ],
      sequences = [
        {
          url: "/light?r=255&g=255&b=255&brightness=20",
          delay: 400,
        },
       
        {
          url: "/light?r=255&g=146&b=0&brightness=80",
          delay: 400,
        },
            
      ],
    } = props
  
    let step = sequence[currentStepIndex]
    let nextProps = false
   
    if ( currentStepIndex + 1 == sequence.length ) {
      if (  loop ) {        
       
        nextProps = {
          host: props.host,        
          currentStepIndex: 0,
          sequence: sequence,
        }
      }
        // nextProps = {
        //   host: props.host,        
        //   currentStepIndex: 0,
        //   sequence: sequence,
        // }
        

    } else if ( currentStepIndex +1 < sequence.length ) {
      nextProps = {
        host: host,        
        currentStepIndex: currentStepIndex + 1,
        sequence: sequence,
      }
    } else {
      return
    }
  
    
    const url = host + step.url 

    fetch(url , {
      method: 'GET',
    })
    .then(response => {
      return response.json()
    })
    .then(responseAsJson => {
      //this.setState({ loading: false, data: responseAsJson.data })
      //setTimeout( this.sendSquence( props ), 2000 )
      alert("done")
    }).finally( response => {
      if ( nextProps != false ) {
        setTimeout( () => {
          this.sendSequence( nextProps )

        }, step.delay )
      }
      console.log( nextProps )
    })
  }

  sendLight = ( props = {
    r: 255,
    g: 0,
    b: 0,
  } ) => {

    var uri = host + "/light?r=" + props.r + "&g=" + props.g + "&b=" + props.b;
    console.log( props)
    if (props.brightness && parseInt(props.brightness) ) {
        uri += "&brightness=" + props.brightness
    }
    fetch(uri, {
        method: 'GET',
    })
    .then(response => {
        return response.json()
    })
    .then(responseAsJson => {
        this.setState({ loading: false, data: responseAsJson.data })
    })
    
  }

  handleHostChange = (event, newValue) => {

    console.log( newValue)
    this.setState({ host: newValue });
  }
  handleSequenceChange = (event, newValue) => {

    console.log( newValue )
  }

  handleBrightnessSliderChange = (event, newValue) => {
    this.setState({ brightness: newValue });
    flowerState.brightness = newValue;
    console.log("position change: " + newValue);
    };
    handleChangeComplete = (color) => {
      this.setState({ background: color.hex });
      console.log( color )   
      flowerState.r = color.rgb.r;
      flowerState.g = color.rgb.g;
      flowerState.b = color.rgb.b;
      };
    render() {
    return (
      <div style={{
          width: '90%',
          margin: '4%',
          marginTop: 10,
      }}>

      <SketchPicker
          width={'96%'}
          style={{
            margin:'auto',
          }}
          color={ this.state.background }
          onChangeComplete={ this.handleChangeComplete }
      />

      <div  style={{
          width: '90%',
          margin: '4%',
          marginTop: 20,
      }}>
        {/* <IconButton onClick={sendFlowerProperties} color="secondary">
          Set Color
        </IconButton> */}
        <Button style={{width:"100%", marginBottom: 5}} variant="contained" size="large" color="primary" onClick={() => { alert('s')}}>sssSet Star Color</Button>
        <Button style={{width:"100%", marginBottom: 5}} variant="contained" size="large" color="primary" onClick={sendWipe}>Wipe</Button>
      </div>

      <div  style={{
          width: '90%',
          margin: '4%',
          marginTop: 20,
      }}>

        <Select
          size="large"
          // native
          value={0}
          onChange={(v) => { this.handleSequenceChange( v ) } }
          inputProps={{
            name: 'age',
            id: 'filled-age-native-simple',
          }}
        >
          <option value="0">Sequence 1</option>
          <option value="1">Sequence 2</option>
        </Select>

        <select >
        </select>
        <Button style={{width:"100%", marginBottom: 5}} variant="contained" size="large" color="primary" onClick={this.sendSequence}>Send Sequence</Button>
        

        <select onChange={(v) => { this.handleHostChange( ) } }>
          <option value="10.5.1.161">Sparky</option>
          <option value="10.5.1.162">Boop</option>
        </select>
        

        
        <div style={{
            color: '#3f51b5',
            fontSize: 12,
            fontWeight: 'bold',
            textAlign:'center',
            background: 'white',
            borderRadius: 4,   
            marginBottom: 5,
          }}>
          <strong>Brightness</strong>
           <Button style={{width:"100%", marginBottom: 5}} variant="contained" size="large" color="transparent">
            <Slider
              defaultValue={this.state.brightness}
              onChange={this.handleBrightnessSliderChange}
              getAriaValueText={valuetext}
              aria-labelledby="brightness"
              valueLabelDisplay="off"
              step={5}
              // marks
              min={0}
              max={100}
              /> 
          </Button>
        </div>1

        <div style={{
          background: 'white',
          borderRadius: 4,          
        }}>
        { [
          {
            color: 'red',
            rgb: {
              r: 255,
              g: 0,
              b: 0,
              a: 1,
            },    
          },
          {
            color: 'orange',            
            rgb: {
              r: 255,
              g: 66,
              b: 6,
              a: 1,
            },    
          },
          {
            color: 'yellow',            
            rgb: {
              r: 255,
              g: 167,
              b: 2,
              a: 1,
            },    
          },
          {
            color: 'green',            
            rgb: {
              r: 0,
              g: 255,
              b: 0,
              a: 1,
            },    
          },
          {
            color: 'blue',            
            rgb: {
              r: 0,
              g: 0,
              b: 255,
              a: 1,
            },    
          },
          {
            color: 'purple',            
            rgb: {
              r: 176,
              g: 3,
              b: 255,
              a: 1,
            },    
          },
          {
            color: 'gold',            
            rgb: {
              r: 245,
              g: 145,
              b: 0,
              a: 1,
            },    
          },
        ].map( entry => <ButtonBase
          focusRipple
          key={'mine'}
          onClick={ () => {
            this.sendLight( entry.rgb )
          }}
          style={{
            width: 90,
            height: 90,
            background: `rgba(${entry.rgb.r},${entry.rgb.g},${entry.rgb.b},${entry.rgb.a})`,
            margin: 5,
          }}
        />
        )}
        </div>

        </div>
      </div>
    );
  }
}


const useStyles = makeStyles({
  root: {

    margin: '4%',
    marginTop: 10,
  },
});

function valuetext(value) {
  return `${value}°C`;
}

const handleSliderChange = (event, newValue) => {
    flowerState.pos = newValue;
    console.log("position change: " + newValue);
};


const openFlower = () => {
    
}
var flowerStepNum = 0;
var flowerSteps = [
    { r: 0, g: 0, b: 0, pos: 0 , brigtness: 0 },
    { r: 0, g: 0, b: 0, pos: 10 , brigtness: 0 },
    { r: 0, g: 0, b: 0, pos: 20 , brigtness: 0 },
    { r: 200, g: 0, b: 0, pos: 30 , brigtness: 0 },
    { r: 200, g: 0, b: 0, pos: 40 , brigtness: 0 },
    { r: 200, g: 0, b: 0, pos: 50 , brigtness: 0 },
    { r: 255, g: 0, b: 0, pos: 60 , brigtness: 100 },    
];

const stepFlower = () => {
    setTimeout( () => {
        
    })
}

const closeFlower = () => {

}


function DiscreteSlider() {
  const classes = useStyles();

  return (
    <div className={classes.root}>
      {/* <Typography id="discrete-slider" gutterBottom>
        Temperature
      </Typography>
      <Slider
        defaultValue={30}
        onChange={handleSliderChange}
        getAriaValueText={valuetext}
        aria-labelledby="discrete-slider"
        valueLabelDisplay="auto"
        step={3}
        marks
        min={0}
        max={60}
      /> */}


      {/* <div>
        <Button variant="outlined" onClick={openFlower}>Open</Button>
        <Button variant="outlined" onClick={closeFlower}>Close</Button>
      </div> */}

    </div>
  );
}



export default Component
