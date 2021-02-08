import React from "react";
import {
  BrowserRouter as Router,
  Switch,
  Route,
  Link,
  useHistory,
  useLocation,
  useParams
} from "react-router-dom";


import Helmet from 'react-helmet'
import  { Posts } from './posts'
import { Scene } from './models'
import Jet from './jet'

export default function ModalGalleryExample() {
  return (
    <Router>
      <ModalSwitch />
    </Router>
  );
}

function ModalSwitch() {
  let location = useLocation();
  let background = location.state && location.state.background;

  return (
    <div>
      <Helmet>
        <title>Our Stories of Migfration</title>
      </Helmet>
      <Switch location={background || location}>
        <Route exact path="/" children={<Home />} />
        <Route path="/stories" children={<Gallery />} />
        <Route path="/story/:id" children={<Modal />} />
      </Switch>

      {/* Show the modal when a background page is set */}
      {background && <Route path="/story/:id" children={<Modal />} />}
    </div>
  );
}

const stories = [
  { id: 5, title: "Story 5", color: "red", position:[-3.5, -1.3, 1.3] }, 
  { id: 6, title: "Story 6", color: "orange", position:[-2.1, -1.2, 0] },
  { id: 7, title: "Story 7", color: "green", position: [0, -1.0, 0] },
  { id: 8, title: "Story 8", color: "blue", position: [2.1, -1.2, 0] },
  { id: 9, title: "Story 9", color: "purple", position: [3.5, -1.3, 1.3] },
  

  { id: 0, title: "Story 0", color: "red", position:   [-3.5, 1.3, 1.3] }, 
  { id: 1, title: "Story 1", color: "orange", position: [-2.1, 1.7, 0] },
  { id: 2, title: "Story 2", color: "green", position:  [0, 1.7, 0] },
  { id: 3, title: "Story 3", color: "blue", position:   [2.1, 1.7, 0] },
  { id: 4, title: "Story 4", color: "purple", position: [3.5, 1.3, 1.3] },
  

];

function Thumbnail({ color }) {
  return (
    <div
      style={{
        width: 50,
        height: 50,
        background: color
      }}
    />
  );
}

function Image({ color }) {
  return (
    <div
      style={{
        width: "100%",
        height: 400,
        background: color
      }}
    />
  );
}

function Home() {
  return (
    <div>
      <Intro />
      <Featured stories={[
        {
          ...stories[1],
        },
        {
          ...stories[3],
        }
      ]} />
      <Navigation />
    </div>
  );
}


const Navigation = () =>
  <div>
    <h2>Navigation</h2>
    <ul>
    <li>
        <Link to="/stories">Browse stories</Link>
      </li>
      <li>
        <Link to="/">Home</Link>
      </li>
    </ul>
  </div>

const Intro = () => {
  return (
    <div>
      <h2>Introduction</h2>
      <p>About the project...</p>
    </div>
  )
}

const Featured = ({ stories = [] }) => {
  return (
    <div>
      { stories.length === 0 ?
        <div>
          <h1>No featured stories currently</h1>
        </div>
        :
        <div>
          <h2>Featured Stories</h2>
          <ul>
            {stories.map(entry =>
              <li>
                <Link to={`story/${entry.id}`}>{entry.title}</Link>
              </li>
            )}
          </ul>
        </div>
      }
    </div>
  )
}
const Header = () => <h1 style={{
  fontFamily: 'courier',
  textAlign: 'center',
  margin: 10,
  padding: 0,

 }}>Our Stories of Migration</h1>

function Gallery() {
  let location = useLocation();
  const history = useHistory();

  return (
    <div>
      {/* <Header /> */}
        {/* <Navigation /> */}
      <Scene stories={stories} onSelect={ story => {
        
        history.push('/story/' + story.storyId );
        //alert('loading story : ' + story.storyId )
      }}/>
      {/* <ul>
      {stories.map(i => (
        <li>

        <Link
        key={i.id}
        to={{
          pathname: `/story/${i.id}`,
          // This is the trick! This link sets
          // the `background` in location state.
          state: { background: location }
        }}
        >
          <p>{i.title}</p>
        </Link>
        </li>
      ))}
      </ul> */}
      <Posts />
    </div>
  );
}

function ImageView() {
  let { id } = useParams();
  let image = stories[parseInt(id, 10)];

  if (!image) return <div>Image not found</div>;

  return (
    <div>
      <h1>{image.title}</h1>
      <Image color={image.color} />
    </div>
  );
}

function Modal() {
  let history = useHistory();
  let { id } = useParams();
  let image = stories[parseInt(id, 10)];

  if (!image) return null;

  let back = e => {
    e.stopPropagation();
    history.goBack();
  };

  return (
    <div
      //onClick={back}
      style={{
        position: "absolute",
        top: 0,
        left: 0,
        bottom: 0,
        right: 0,
        background: "rgba(0, 0, 0, 0.15)"
      }}
    >
      <div
        className="modal"
        style={{
          position: "absolute",
          background: "#fff",
          top: 25,
          left: "10%",
          right: "10%",
          padding: 15,
          border: "2px solid #444"
        }}
      >
        
        <button style={{
          top: 0,
          right: 0,
          position: 'absolute',
        }} type="button" onClick={back}>
          X
        </button>
        <h1>{image.title}</h1>
        <Jet />
        {/* <Image color={image.color} /> */}
        
      </div>
    </div>
  );
}