#!/usr/bin/env python
# -*- coding: utf-8 -*-

HOST = "localhost"
PORT = 4223
UID = "Cvv" # Change XYZ to the UID of your RS232 Bricklet
UIDEM = "Qjt" # Change XYZ to the UID of your RS232 Bricklet

import sys
import paho.mqtt.client as mqtt
import time, random, threading
import multiprocessing as mp

from tinkerforge.ip_connection import IPConnection
from tinkerforge.bricklet_rs232 import BrickletRS232
from tinkerforge.bricklet_energy_monitor import BrickletEnergyMonitor

# client, user and device details
serverUrl   = "mqtt.cumulocity.com"
clientId    = "0edabed5-0c3f-4ec4-9971-a26b2b4e868a"#"linux-0000000024647748"#
device_name = "RS232 MQTT"
tenant      = "t588701203"
username    = "miguel.villagra@babel.es"
password    = "2020Covap"


# Convert string to char array with length 60, as needed by write
def string_to_char_list(message):
    chars = list(message)
    chars.extend(['\0']*(60 - len(message)))
    return chars, len(message)

if __name__ == "__main__":
    ipcon = IPConnection() # Create IP connection
    rs232 = BrickletRS232(UID, ipcon) # Create device object
    em = BrickletEnergyMonitor(UIDEM, ipcon) # Create device object
    ipcon.connect(HOST, PORT) # Connect to brickd
    # Don't use device before ipcon is connected
    #rs232.set_configuration(5,0,1,8,0,0)
    # Ensure read callback is disabled, otherwise the read call will return empty
    rs232.disable_read_callback()
    # connect the client to Cumulocity IoT and register a device
    client = mqtt.Client(clientId)
    client.username_pw_set(tenant + "/" + username, password)
    client.connect(serverUrl)
    count = 0
    rs232.write(*string_to_char_list('S02 \r\n'))
     
    while count < 12:
        rs232.write(*string_to_char_list('MSV? \r\n'))
        time.sleep(1)
        buffer=rs232.read()
        medida=''
        for x in buffer[0]:
            if (x == "-" or x == "0" or x == "1" or x == "" or x == "1" or x == "2" or x == "3" or x == "4" or x == "5" or x == "6" or x == "7" or x == "8" or x == "9"):
                medida=medida+str(x)
        #rs232 = str(random.randint(5, 95))
        payload="200,c8y_RS232MeasurementTest,Peso,"+medida
        client.publish("s/us", payload)

        # Get current energy data
        voltage, current, energy, real_power, apparent_power, reactive_power, power_factor, \
        frequency = em.get_energy_data()
        payload="200,c8y_EnergyMeasurement,Current,"+str((current/100.0)+0.03)
        client.publish("s/us", payload)
        payload="200,c8y_EnergyMeasurement,Voltage,"+str(voltage/100.0)
        client.publish("s/us", payload)
        payload="200,c8y_EnergyMeasurement,Apparent Power,"+str(-apparent_power)
        client.publish("s/us", payload)
        payload="200,c8y_EnergyMeasurement,Real Power,"+str(-real_power)
        client.publish("s/us", payload)
        payload="200,c8y_EnergyMeasurement,Energy,"+str(energy/100.0)
        client.publish("s/us", payload)

        count += 1
        time.sleep(4)
    
    ipcon.disconnect()