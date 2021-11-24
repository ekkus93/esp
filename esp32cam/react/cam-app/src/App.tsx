import React from 'react';
import './App.css';
import { Button } from 'react-bootstrap';
import axios from 'axios';
import 'bootstrap/dist/css/bootstrap.css';

function App(): JSX.Element {
  const onClick_FrontLED = (): void => {
    axios.get('/api/whiteled/toggle');
  };

  return (
    <div className="App">
      <Button variant="primary" size="lg" onClick={onClick_FrontLED}>Front LED</Button>
    </div>
  );
}

export default App;
