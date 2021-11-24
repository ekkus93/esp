#include "Arduino.h"
#include "led.h"

# define WHITE_LED 4
# define RED_LED 33

bool whiteLed = false;
bool redLed = false;

void ledSetup()
{
    pinMode(WHITE_LED, OUTPUT);
    whiteLedOff();

    pinMode(RED_LED, OUTPUT);
    redLedOff();
}

bool getWhiteLed()
{
    return whiteLed;
}

void whiteLedOn()
{
    whiteLed = true;
    digitalWrite(WHITE_LED, HIGH);
}

void whiteLedOff()
{
    whiteLed = false;
    digitalWrite(WHITE_LED, LOW);
}

void whiteLedToggle()
{
    if (whiteLed)
    {
        whiteLedOff();
    } else {
        whiteLedOn();
    }
}

void redLedOn()
{
    redLed = true;
    digitalWrite(RED_LED, LOW);
}

void redLedOff()
{
    redLed = false;
    digitalWrite(RED_LED, HIGH);
}

void redLedToggle()
{
    if (redLed)
    {
        redLedOff();
    } else {
        redLedOn();
    }
}

bool getRedLed()
{
    return whiteLed;
}