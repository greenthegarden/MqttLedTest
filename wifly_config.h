/* 
 * RN-XV WiFly Module - Wire Antenna
 * MAC: 00:06:66:50:71:6f
 * IP: 192.168.1.52
 *  
 * RN-XV WiFly Module – SMA
 * MAC: 00:06:66:71:68:d5
 * IP: 192.168.1.51
 */

/* 
 * WiFly configuration
 * reboot	
 * $$$	
 * factory RESET	
 * 	
 * set wlan join 0    // Stop device connecting while we setup
 * 
 * set ip dhcp 3
 * set wlan ssid xxx
 * set wlan phrase xxx
 * set wlan join 1
 * 
 * set time address 203.0.178.191
 * set time zone 0
 * set time enable 1
 * 
 * save
 * reboot
 */


#ifndef MQTTWIFLYRELAYDUINO_WIFLY_CONFIG_H_
#define MQTTWIFLYRELAYDUINO_WIFLY_CONFIG_H_


#include <SoftwareSerial.h>

const byte WIFLY_SERIAL_RX        = 11;
const byte WIFLY_SERIAL_TX        = 12;

SoftwareSerial wifly_serial(WIFLY_SERIAL_RX, WIFLY_SERIAL_TX);


#include <SPI.h>
#include <WiFly.h>

WiFlyClient    wifly_client;


boolean wifly_connected           = false;
const int AFTER_ERROR_DELAY       = 1000;


#endif  /* MQTTWIFLYRELAYDUINO_WIFLY_CONFIG_H_ */
