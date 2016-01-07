#ifndef MQTTLEDTEST_MQTT_CONFIG_H_
#define MQTTLEDTEST_MQTT_CONFIG_H_


// MQTT parameters
byte mqtt_server_addr[]           = { 192, 168, 1, 55 };    // Pi
//byte mqtt_server_addr[]           = { 192, 168, 1, 30 };    // Airology
char mqtt_client_id[]             = "robustness";
const int MQTT_PORT               = 1883;
#define MQTT_MAX_PACKET_SIZE        168
#define MQTT_KEEPALIVE              300


const char WIFLY_STATUS[]      PROGMEM = "relayduino/status/wifly";
const char UPTIME_STATUS[]     PROGMEM = "relayduino/status/uptime";
const char MEMORY_STATUS[]     PROGMEM = "relayduino/status/memory";

PGM_P const STATUS_TOPICS[]    PROGMEM = {WIFLY_STATUS,     // idx = 0
                                          UPTIME_STATUS,    // idx = 1
                                          MEMORY_STATUS,    // idx = 2
                                          };


const char UPTIME_REQUEST[]    PROGMEM = "relayduino/request/uptime";
const char MEMORY_REQUEST[]    PROGMEM = "relayduino/request/memory";

const char LED_CONTROL[]       PROGMEM = "relayduino/control/led";

PGM_P const CONTROL_TOPICS[]   PROGMEM = {UPTIME_REQUEST,    // idx = 0
                                          MEMORY_REQUEST,    // idx = 1
                                          LED_CONTROL,       // idx = 2
                                          };


#endif  /* MQTTWIFLYRELAYDUINO_MQTT_CONFIG_H_ */

