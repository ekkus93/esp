/*********
  Modified from the examples of the Arduino LoRa library
  More resources: https://randomnerdtutorials.com
*********/

#include <SPI.h>
#include <LoRa.h>
#include <ESP32Servo.h>

//define the pins used by the transceiver module
#define ss 5
#define rst 14
#define dio0 2

#define LED_PIN 21
#define SERVO_PIN 13

#define SERIAL_SPEED 115200

Servo myservo;  // create servo object to control a servo

const String commandWord = String("b139970d-74b4-4d58-a9c0-4654e128907b");

int pos = 0;    // variable to store the servo position

void setupLora() {
  pinMode (LED_PIN, OUTPUT);
  
  Serial.println("LoRa Receiver");

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
  Serial.println("LoRa Initializing OK!");
}

void setupServo() {
  // Allow allocation of all timers
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);
  myservo.setPeriodHertz(50);    // standard 50 hz servo
  myservo.attach(SERVO_PIN, 500, 2400); 
  // using default min/max of 1000us and 2000us
  // different servos may require different min/max settings
  // for an accurate 0 to 180 sweep
}

void setup() {
  //initialize Serial Monitor
  Serial.begin(SERIAL_SPEED);
  while (!Serial);

  setupServo();
  setupLora();
}

void handleServo() {
  for (pos = 2; pos <= 88; pos += 1) { 
    // in steps of 1 degree
    myservo.write(pos);    // tell servo to go to position in variable 'pos'
    delay(15);             // waits 15ms for the servo to reach the position
  }
  for (pos = 88; pos >= 2; pos -= 1) {
    // goes from 180 degrees to 0 degrees
    myservo.write(pos);    // tell servo to go to position in variable 'pos'
    delay(15);             // waits 15ms for the servo to reach the position
  }
}

void loop() {
  // try to parse packet
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    // received a packet
    Serial.print("Received packet '");
    
    // read packet
    while (LoRa.available()) {
      String LoRaData = LoRa.readString();
      Serial.print(LoRaData); 
      if (commandWord.compareTo(LoRaData) == 0) {
        digitalWrite (LED_PIN, HIGH); 
        handleServo();
        digitalWrite (LED_PIN, LOW);
      }
    }

    // print RSSI of packet
    Serial.print("' with RSSI ");
    Serial.println(LoRa.packetRssi()); 
    delay(500);
  }
  
  
}
