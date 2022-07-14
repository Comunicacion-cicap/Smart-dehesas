#!/usr/bin/env python
# -*- coding: utf-8 -*-

HOST = "localhost"
PORT = 4223
UID = "Cvv" # Change XYZ to the UID of your RS232 Bricklet

import sys
import paho.mqtt.client as mqtt
import time, random, threading
import multiprocessing as mp

from tinkerforge.ip_connection import IPConnection
from tinkerforge.bricklet_rs232 import BrickletRS232

# client, user and device details
serverUrl   = "mqtt.cumulocity.com"
clientId    = "0edabed5-0c3f-4ec4-9971-a26b2b4e868a"#"linux-0000000024647748"#
device_name = "RS232 MQTT"
tenant      = "t588701203"
username    = "miguel.villagra@babel.es"
password    = "2020Covap"

ipcon = IPConnection() # Create IP connection
rs232 = BrickletRS232(UID, ipcon) # Create device object
# BrickletRS232.set_configuration(5,4,1,8,0,0)
ipcon.connect(HOST, PORT) # Connect to brickd
#rs232.get_configuration()
mensaje=rs232.read()
print ("Mensaje: ", str(mensaje), " ", rs232.get_identity())
ipcon.disconnect()