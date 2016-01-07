#ifndef MQTTLEDTEST_ETHERNET_CONFIG_H_
#define MQTTLEDTEST_ETHERNET_CONFIG_H_


// Update these with values suitable for your network.
byte mac[]    = {  0xDE, 0xED, 0xBA, 0xFE, 0xFE, 0xED };
IPAddress ip(192, 168, 1, 90);

EthernetClient ethernet_client;


#endif

