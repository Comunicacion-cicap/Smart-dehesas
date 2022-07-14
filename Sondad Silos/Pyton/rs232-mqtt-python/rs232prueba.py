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


if __name__ == "__main__":
    ipcon = IPConnection() # Create IP connection
    rs232 = BrickletRS232(UID, ipcon) # Create device object
    em = BrickletEnergyMonitor(UIDEM, ipcon) # Create device object
    ipcon.connect(HOST, PORT) # Connect to brickd
    # Don't use device before ipcon is connected
    
    # Get current energy data
    voltage, current, energy, real_power, apparent_power, reactive_power, power_factor, \
      frequency = em.get_energy_data()

    #print("Voltage: " + str(voltage/100.0) + " V")
    #print("Current: " + str(current/100.0) + " A")
    #print("Energy: " + str(energy/100.0) + " Wh")
    #print("Real Power: " + str(real_power/100.0) + " h")
    #print("Apparent Power: " + str(apparent_power/100.0) + " VA")
    #print("Reactive Power: " + str(reactive_power/100.0) + " var")
    #print("Power Factor: " + str(power_factor/1000.0))
    #print("Frequency: " + str(frequency/100.0) + " Hz")

    # connect the client to Cumulocity IoT and register a device
    #clientem = mqtt.Client("linux-0000000024647748")
    #clientem.username_pw_set(tenant + "/" + username, password)
    #clientem.connect(serverUrl)
    
    
    # connect the client to Cumulocity IoT and register a device
    client = mqtt.Client(clientId)
    client.username_pw_set(tenant + "/" + username, password)
    client.connect(serverUrl)
    
    count = 0
    while count < 5:
        # Get current energy data
        voltage, current, energy, real_power, apparent_power, reactive_power, power_factor, \
        frequency = em.get_energy_data()
        #buffer=rs232.read()
        #print (buffer)
        medida=''
        #for x in buffer[0]:
            #if 'X' in x:
             #   print (x)#print (bytearray.fromhex(x).decode())
         #   if (x == "-" or x == "0" or x == "1" or x == "" or x == "1" or x == "2" or x == "3" or x == "4" or x == "5" or x == "6" or x == "7" or x == "8" or x == "9"):
                #print (x)                
          #      medida=medida+str(x)
        # Check if global buffer contains a complete response
        print ("PESO ",medida)
        rs232 = str(random.randint(5, 95))
        payload="200,c8y_RS232Measurement,Peso,"+rs232
        client.publish("s/us", payload)
        payload="200,c8y_EnergyMeasurement,Current,"+str(current/100.0)
        client.publish("s/us", payload)
        count += 1
        time.sleep(2)
        #print ("Medida: " + medida)
    
    ipcon.disconnect()