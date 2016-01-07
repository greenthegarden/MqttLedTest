// Ethernet libraries
#include <SPI.h>
#include <Ethernet.h>

#include "config.h"

#include <MemoryFree.h>

#include <PubSubClient.h>


// callback function definition required here as client needs to be defined before
// including relay.h
void callback(char* topic, uint8_t* payload, unsigned int length);


PubSubClient   mqtt_client(mqtt_server_addr, MQTT_PORT, callback, ethernet_client);


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


void mqtt_connect()
{
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

boolean mqtt_reconnect() {
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
  }
  return mqtt_client.connected();
}


/*--------------------------------------------------------------------------------------
  setup()
  Called by the Arduino framework once, before the main loop begins
  --------------------------------------------------------------------------------------*/
void setup()
{
  Serial.begin(BAUD_RATE);

  // Configure Ethernet
  Ethernet.begin(mac, ip);
  delay(1500);
  
  pinMode(LED_PIN, OUTPUT);
  setLedOff();
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
#if 0
  // alternative based on code in relayr
  if (mqtt_client.connected()) {
    mqtt_client.loop();
  } else {
    //if connection lost, try to reconnect
    mqtt_connect();
  }
#endif
  
  if (!mqtt_client.connected()) {
    long now = millis();
    if (now - lastReconnectAttempt > 5000) {
      lastReconnectAttempt = now;
      // Attempt to reconnect
      if (mqtt_reconnect()) {
        lastReconnectAttempt = 0;
      }
    }
  } else {
    // Client connected
    mqtt_client.loop();
  }
  
}

