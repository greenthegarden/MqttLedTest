#!/usr/bin/env python

import paho.mqtt.client as mqtt
#import paho.mqtt.publish as publish

import os, urlparse
import time

hostname = "localhost"

qos = 2
topics = []
topics.append(("relayduino/status/#", 0))
topics.append(("relayduino/request/#", 0))
topics.append(("relayduino/control/#", 0))

# MQTT Callbacks

def on_connect(client, userdata, flags, rc) :
	mqttc.subscribe(topics)

def on_disconnect(client, userdata, rc) :
	print("Disconnect returned:")
	print("client: {0}".format(str(client)))
	print("userdata: {0}".format(str(userdata)))
	print("result: {0}".format(str(rc)))

def on_message(client, userdata, message) :
#    print(time.clock())
	print(message.topic+" "+str(message.payload))
#    print("message received: topic is {0} with payload {1}".format(message.topic, message.payload))

def on_publish(client, userdata, mid) :
    print("mid: {0}".format(str(mid)))

def on_subscribe(client, userdata, mid, granted_qos) :
    print("Subscribed: "+str(mid)+" "+str(granted_qos))

def on_log(client, userdata, level, buf) :
    print("{0}".format(buf))

mqttc               = mqtt.Client()
mqttc.on_connect    = on_connect
mqttc.on_message    = on_message
#mqttc.on_publish    = on_publish
mqttc.on_subscribe  = on_subscribe
mqttc.on_disconnect = on_disconnect
# Uncomment to enable debug messages
#mqttc.on_log       = on_log

# Parse CLOUDMQTT_URL (or fallback to localhost)
url_str = os.environ.get('CLOUDMQTT_URL', 'mqtt://192.168.1.30:1883')
url = urlparse.urlparse(url_str)

# Connect
#mqttc.username_pw_set(url.username, url.password)
mqttc.connect(url.hostname, url.port)
mqttc.loop_start()

command_types_valid_inputs = ['t', 'm']
led_control_valid_inputs   = ['0', '1']

print("Enter 't' for uptime or 'm' for free memory: ")

# Loop continuously
while True :
	try :
		command_type = raw_input()
		if command_type in command_types_valid_inputs :
			if command_type is 't' :
				mqttc.publish("relayduino/request/uptime");
			elif command_type is 'm' :
				mqttc.publish("relayduino/request/memory");
			elif command_type is 'm' :
				led_control = raw_input("Enter 0/1 to switch off/on led: ")
				if led_control in led_control_valid_inputs :
					mqttc.publish("relayduino/control/led", led_control);
			else :
				print("Should not be here!!")
		else :
			print("Invalid command type entered")

	except KeyboardInterrupt :      #Triggered by pressing Ctrl+C
		running = False       #Stop thread1
		# Disconnect mqtt client
		mqttc.loop_stop()
		mqttc.disconnect()
		print("Bye")
		break         #Exit
