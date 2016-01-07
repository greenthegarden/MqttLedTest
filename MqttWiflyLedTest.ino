// WiFly libraries
#include <SPI.h>
#include <WiFly.h>
#include <SoftwareSerial.h>


#include "config.h"

#include <MemoryFree.h>


//#if DEBUG
//void debug(const __FlashStringHelper * console_text)
//{
//  Serial.println(console_text);
//}
//#endif


#include <PubSubClient.h>


// callback function definition required here as client needs to be defined before
// including relay.h
void callback(char* topic, uint8_t* payload, unsigned int length);


PubSubClient   mqtt_client(mqtt_server_addr, MQTT_PORT, callback, wifly_client);


void publish_connected()
{
  prog_buffer[0] = '\0';
  strcpy_P(prog_buffer, (char*)pgm_read_word(&(STATUS_TOPICS[0])));
  mqtt_client.publish(prog_buffer, "connected");
}

void publish_uptime()
{
  prog_buffer[0] = '\0';
  strcpy_P(prog_buffer, (char*)pgm_read_word(&(STATUS_TOPICS[1])));
  char_buffer[0] = '\0';
  ltoa(millis(), char_buffer, 10);
  mqtt_client.publish(prog_buffer, char_buffer);

}
void publish_memory()
{
  prog_buffer[0] = '\0';
  strcpy_P(prog_buffer, (char*)pgm_read_word(&(STATUS_TOPICS[2])));
  char_buffer[0] = '\0';
  itoa(freeMemory(), char_buffer, 10);
  mqtt_client.publish(prog_buffer, char_buffer);
}

void callback(char* topic, uint8_t* payload, unsigned int payload_length)
{
  // handle message arrived
  /* topic = part of the variable header:has topic name of the topic where the publish received
   	  NOTE: variable header does not contain the 2 bytes with the
   	        publish msg ID
   	  payload = pointer to the first item of the buffer array that
   	            contains the message tha was published
   	           EXAMPLE of payload: lights,1
   	  length = the length of the payload, until which index of payload
  */

DEBUG_LOG(1, "Payload length is");
DEBUG_LOG(1, payload_length);

  // Copy the payload to the new buffer
  char* message = (char*)malloc((sizeof(char) * payload_length) + 1); // get the size of the bytes and store in memory
  memcpy(message, payload, payload_length * sizeof(char));        // copy the memory
  message[payload_length * sizeof(char)] = '\0';                  // add terminating character

  DEBUG_LOG(1, "Message with topic");
  DEBUG_LOG(1, topic);
  DEBUG_LOG(1, "arrived with payload");
  DEBUG_LOG(1, message);

  byte topic_idx = 0;
  // find if topic is matched
  for (byte i = 0; i < ARRAY_SIZE(CONTROL_TOPICS); i++) {
    prog_buffer[0] = '\0';
    strcpy_P(prog_buffer, (PGM_P)pgm_read_word(&(CONTROL_TOPICS[i])));
    if (strcmp(topic, prog_buffer) == 0) {
      topic_idx = i;
      break;
    }
  }
  DEBUG_LOG(1, "Control topic index");
//  DEBUG_LOG(1, topic_idx);

  if (topic_idx == 0) {  // topic is UPTIME_REQUEST
    publish_uptime();
    DEBUG_LOG(1, "UPTIME_REQUEST topic arrived");
//    DEBUG_LOG(1, (char) millis());
  } else if (topic_idx == 1) {  // topic is MEMORY_REQUEST
    publish_memory();
    DEBUG_LOG(1, "MEMORY_REQUEST topic arrived");
//    DEBUG_LOG(1, (char) freeMemory());
  } else if (topic_idx == 2) {  // LED_CONTROL
    byte integer = atoi(message);    // parse to int (will return 0 if not a valid int)
    if (integer == 1 && ledIsOff()) {
      setLedOn();
    } else if (integer == 0 && ledIsOn()) { 
      setLedOff();
    }
  }

  // free memory assigned to message
  free(message);
}

void wifly_connect()
{
  DEBUG_LOG(1, "initialising wifly");

  WiFly.begin();
  delay(5000);  // allow time to WiFly to initialise

  DEBUG_LOG(1, "joining network");

//  if (!WiFly.join(MY_SSID, MY_PASSPHRASE, mode)) {
  if (!WiFly.join(MY_SSID)) {   // needs to be fixed to allow a passphrase if secure
    wifly_connected = false;
    DEBUG_LOG(1, "  failed");
    delay(AFTER_ERROR_DELAY);
  } else {
    wifly_connected = true;
    DEBUG_LOG(1, "  connected");
  }
}

void mqtt_connect()
{
  if (!wifly_connected)
    wifly_connect();

  if (wifly_connected) {
    // MQTT client setup
    //    mqttClient.disconnect();
    DEBUG_LOG(1, "connecting to broker");
    if (mqtt_client.connect(mqtt_client_id)) {
      DEBUG_LOG(1, "  connected");
      publish_connected();
#if USE_FREEMEM
      publish_memory();
#endif
      // subscribe to topics
      mqtt_client.subscribe("relayduino/request/#");
      mqtt_client.subscribe("relayduino/control/#");
    } else {
      DEBUG_LOG(1, "  failed");
      delay(AFTER_ERROR_DELAY);
    }
  }
}

void reset_connection()
{
  if (mqtt_client.connected())
    mqtt_client.disconnect();
  wifly_connect();
  mqtt_connect();
}

/*--------------------------------------------------------------------------------------
  setup()
  Called by the Arduino framework once, before the main loop begins
  --------------------------------------------------------------------------------------*/
void setup()
{
  // Configure WiFly
  Serial.begin(BAUD_RATE);

  wifly_serial.begin(BAUD_RATE);
  WiFly.setUart(&wifly_serial);

  wifly_connect();

#if USE_HARDWARE_WATCHDOG
  ResetWatchdog1();
#endif

//#if DEBUG
  //Serial.println(WiFly.ip());
  //  Serial.println(WiFly.getMAC());
//#endif

  pinMode(LED_PIN, OUTPUT);
}


/*--------------------------------------------------------------------------------------
  loop()
  Arduino main loop
  --------------------------------------------------------------------------------------*/
void loop()
{
  // require a client.loop in order to receive subscriptions
  //  mqttClient.loop();

  //  if (!mqtt_client.loop()) {
  //    mqtt_connect();
  //  }

  // alternative based on code in relayr
  if (mqtt_client.connected()) {
    mqtt_client.loop();
  } else {
    //if connection lost, try to reconnect
    mqtt_connect();
  }

#if USE_HARDWARE_WATCHDOG
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= watchdog_interval) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;

    ResetWatchdog1();
  }
#endif
}

