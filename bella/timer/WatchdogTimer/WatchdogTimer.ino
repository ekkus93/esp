#include "esp_system.h"

#define BUTTON_PIN 13 // GIOP21 pin connected to button
#define SERIAL_SPEED 115200

const int button = BUTTON_PIN;         //gpio to use to trigger delay
const int wdtTimeout = 3000;  //time in ms to trigger the watchdog
hw_timer_t *timer = NULL;

long loopTime = 0;

void ARDUINO_ISR_ATTR resetModule() {
  ets_printf("reboot\n");
  esp_restart();
}

void setup() {
  Serial.begin(SERIAL_SPEED);
  Serial.println();
  Serial.println("running setup");

  /*
  pinMode(button, INPUT_PULLUP);                    //init control pin
  timer = timerBegin(0, 80, true);                  //timer 0, div 80
  timerAttachInterrupt(timer, &resetModule, true);  //attach callback
  timerAlarmWrite(timer, wdtTimeout * 1000, false); //set time in us
  timerAlarmEnable(timer);                          //enable interrupt

  timerWrite(timer, 0); //reset timer (feed watchdog)
  */
  loopTime = millis();
}

void loop() {
  int currentState;     // the current reading from the input pin
  
  Serial.println("running main loop");

  //while button is pressed, delay up to 3 seconds to trigger the timerr

  /*
  currentState = digitalRead(button);
  Serial.print("currentState: ");
  Serial.println(currentState);
  while (currentState == HIGH) {
    Serial.println("button pressed");
    delay(500);
  }
  Serial.println("outside of while loop");
  */
  delay(1000); //simulate work
  loopTime = millis() - loopTime;
  
  Serial.print("loop time is = ");
  Serial.println(loopTime); //should be under 3000
}
