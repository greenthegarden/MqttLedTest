#ifndef MQTTWIFLYRELAYDUINO_CONFIG_H_
#define MQTTWIFLYRELAYDUINO_CONFIG_H_


#define DEBUG                       true

#if DEBUG
#include "Debug.h"
#endif


// Macros
#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))


// Serial parameters
const int BAUD_RATE               = 9600;


const byte LED_PIN                = 13;

byte ledIsOn()
{
  if (digitalRead(LED_PIN))
    return 1;
  return 0;
}

byte ledIsOff()
{
  if (!digitalRead(LED_PIN))
    return 1;
  return 0;
}

void setLedOn()
{
  digitalWrite(LED_PIN, HIGH);
}

void setLedOff()
{
  digitalWrite(LED_PIN, LOW);
}


// Watchdog configuration
#include "Watchdog_config.h"


// Network configuration
#include "networkConfig.h"


// WiFly configuration
#include "wifly_config.h"


// MQTT configuration
#include "mqtt_config.h"


const char COMMAND_SEPARATOR      = ':';

const byte BUFFER_SIZE            = 32;
char char_buffer[BUFFER_SIZE];
char prog_buffer[BUFFER_SIZE];
char message[BUFFER_SIZE];


#endif  /* MQTTWIFLYRELAYDUINO_CONFIG_H_ */

