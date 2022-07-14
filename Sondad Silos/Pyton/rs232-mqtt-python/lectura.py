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



# Convert string to char array with length 60, as needed by write
def string_to_char_list(message):
    chars = list(message)
    chars.extend(['\0']*(60 - len(message)))
    return chars, len(message)


# Assume that the message consists of ASCII characters and
# convert it from an array of chars to a string
def char_list_to_string(message, length):
    return ''.join(message[:length])


# Callback function for read callback

   
if __name__ == "__main__":
    ipcon = IPConnection() # Create IP connection
    rs232 = BrickletRS232(UID, ipcon) # Create device object
    ipcon.connect(HOST, PORT) # Connect to brickd
    # Don't use device before ipcon is connected
    #rs232.set_configuration(5,0,1,8,0,0)
    # Ensure read callback is disabled, otherwise the read call will return empty
    rs232.disable_read_callback()

    # connect the client to Cumulocity IoT and register a device
    count = 0
    cr= '\r'
    nl= '\n'
    #rs232.enable_read_callback()
    rs232.write(*string_to_char_list('S02 \r\n'))
    time.sleep(1)
    rs232.write(*string_to_char_list('MSV? \r\n'))
    #rs232.write(*string_to_char_list('\r'))
    #rs232.write(*string_to_char_list('\n'))
    time.sleep(1)
    buffer = rs232.read()
    medida=''
    for x in buffer[0]:
        if (x == "-" or x == "0" or x == "1" or x == "" or x == "1" or x == "2" or x == "3" or x == "4" or x == "5" or x == "6" or x == "7" or x == "8" or x == "9"):
            medida=medida+str(x)
            print('X='+str(x))
    print (buffer)
    print (medida)
    
    ipcon.disconnect()