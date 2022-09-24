/*********
  Modified from the examples of the Arduino LoRa library
  More resources: https://randomnerdtutorials.com
*********/

#include <SPI.h>
#include <LoRa.h>

//define the pins used by the transceiver module
#define ss 5
#define rst 14
#define dio0 2

#define LED_PIN 21
#define BUTTON_PIN 13 // GIOP21 pin connected to button
#define SERIAL_SPEED 115200
const String commandWord = String("b139970d-74b4-4d58-a9c0-4654e128907b");

int counter = 0;
long loraLastTime = 0;

// Variables will change:
int lastState = LOW;  // the previous state from the input pin
int currentState;     // the current reading from the input pin

void setupLora() {
  pinMode (LED_PIN, OUTPUT);
  
  Serial.println("LoRa Sender");

  //setup LoRa transceiver module
  LoRa.setPins(ss, rst, dio0);
  
  //replace the LoRa.begin(---E-) argument with your location's frequency 
  //433E6 for Asia
  //866E6 for Europe
  //915E6 for North America
  while (!LoRa.begin(915E6)) {
    Serial.println(".");
    delay(500);
  }
   // Change sync word (0xF3) to match the receiver
  // The sync word assures you don't get LoRa messages from other LoRa transceivers
  // ranges from 0-0xFF
  LoRa.setSyncWord(0xF3);

  loraLastTime = millis();
  Serial.println("LoRa Initializing OK!");
}

void setupButton() {
  // initialize the pushbutton pin as an pull-up input
  // the pull-up input pin will be HIGH when the switch is open and LOW when the switch is closed.
  pinMode(BUTTON_PIN, INPUT_PULLUP);
}

void sendLoraMsg() {
    digitalWrite (LED_PIN, HIGH);  
    Serial.print("Sending packet: ");
    Serial.println(counter);

    //Send LoRa packet to receiver
    LoRa.beginPacket();
    LoRa.print(commandWord);
    // LoRa.print(counter);
    LoRa.endPacket();
    delay(500);
    digitalWrite (LED_PIN, LOW);  

    counter++;    
}

void handleLora(long interval) {
  long currTime = millis();
  if ((currTime - loraLastTime) >= interval) {
    sendLoraMsg();
    loraLastTime = currTime;
  }
}

void handleButton() {
  Serial.println("###handleButton");
  
  // read the state of the switch/button:
  currentState = digitalRead(BUTTON_PIN);
  Serial.print("###handleButton - currentState");
  Serial.println(currentState);
  
  if (lastState == HIGH && currentState == LOW) {
    Serial.println("The button is pressed");
    sendLoraMsg();
  } else if (lastState == LOW && currentState == HIGH) {
    Serial.println("The button is released");
  }

  // save the the last state
  lastState = currentState;
}

void setup() {
  //initialize Serial Monitor
  Serial.begin(SERIAL_SPEED);
  while (!Serial);

  setupLora();
  setupButton();
}

void loop() {
  // handleLora(10000);
  handleButton();

  delay(100);
}
