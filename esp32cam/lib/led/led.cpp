#include "Arduino.h"
#include "led.h"

# define WHITE_LED 4
# define RED_LED 33

void ledSetup()
{
    pinMode(WHITE_LED, OUTPUT);
    whiteLedOff();

    pinMode(RED_LED, OUTPUT);
    redLedOff();
}

void whiteLedOn()
{
    digitalWrite(WHITE_LED, HIGH);
}

void whiteLedOff()
{
    digitalWrite(WHITE_LED, LOW);
}

void redLedOn()
{
    digitalWrite(RED_LED, LOW);
}

void redLedOff()
{
    digitalWrite(RED_LED, HIGH);
}

