import React, { useState } from 'react';
import './App.css';
import { Button, Col, Container, Row } from 'react-bootstrap';
import axios from 'axios';
import 'bootstrap/dist/css/bootstrap.css';

function App(): JSX.Element {
  const [imgUrl, setImgUrl] = useState<string>("");

  const onClick_FrontLED = (): void => {
    axios.get('/api/whiteled/toggle');
  };

  const onClick_TakePic = (): void => {
    axios.get('/api/img/take')
      .then(function (response) {
        console.log("status: " + response.data.status);

        console.log(response);
        if (response.status !== 200)
        {
          throw "take pic failed";
        }
        
        setImgUrl(response.data.imgPath);
      })
      .catch(function (error) {
        // handle error
        console.log(error);
      });
  }

  return (
    <Container className="App">
      <Row>
        <Col>
          {
            (imgUrl === "")
            ? <div className="imgBox"></div>
            : <div className="imgBox"><img className="imgBox" src={imgUrl}/></div>
          }
        </Col>
      </Row>
      <Row>
        <Col className="col-3">
          <Button variant="primary" size="lg" onClick={onClick_FrontLED}>Front LED</Button>
        </Col>
        <Col className="col-3">
          <Button variant="secondary" size="lg" onClick={onClick_TakePic}>Take Pic</Button>
        </Col>
      </Row>
    </Container>

  );
}

export default App;
