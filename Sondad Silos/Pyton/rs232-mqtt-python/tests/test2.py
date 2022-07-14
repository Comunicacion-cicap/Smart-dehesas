#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import paho.mqtt.client as mqtt
import time, random, threading
import multiprocessing as mp

# client, user and device details
serverUrl   = "mqtt.cumulocity.com"
clientId    = "0edabed5-0c3f-4ec4-9971-a26b2b4e868a"#"linux-0000000024647748"#
device_name = "RS232 MQTT"
tenant      = "t588701203"
username    = "miguel.villagra@babel.es"
password    = "2020Covap"

# connect the client to Cumulocity IoT and register a device
client = mqtt.Client(clientId)
client.username_pw_set(tenant + "/" + username, password)
client.connect(serverUrl)
print ("Conectado")
rs232 = str(random.randint(5, 95))
payload="200,c8y_RS232MeasurementTest,Peso,"+str(rs232)
client.publish("s/us", payload)
print ("publish realizado: "+payload)