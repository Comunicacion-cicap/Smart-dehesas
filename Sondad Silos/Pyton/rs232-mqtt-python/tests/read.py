#!/usr/bin/env python
# -*- coding: utf-8 -*-



# For this example connect the RX1 and TX pin to receive the send message

HOST = "localhost"
PORT = 4223
UID = "Cvv" # Change XYZ to the UID of your RS232 Bricklet

import sys

from tinkerforge.ip_connection import IPConnection
from tinkerforge.bricklet_rs232 import BrickletRS232

buffer = '' # Búfer global para recopilar todos los datos entrantes

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
def cb_read(message, length):
    s = char_list_to_string(message, length)
    # Append new data to global buffer
    global buffer
    buffer += s
   
    # Check if global buffer contains a complete response
    i = buffer.find('\r\n')

    if i >= 0:
        # Get response (without \r\n) from buffer
        response = buffer[:i]

        # Remove response (with \r\n) from buffer
        buffer = buffer[i + 2:]

        # Print response
        print('Response: ' + response)

    #print('Message (Length: ' + str(length) + '): "' + s + '"')

if __name__ == "__main__":
    ipcon = IPConnection() # Create IP connection
    rs232 = BrickletRS232(UID, ipcon) # Create device object
    
    ipcon.connect(HOST, PORT) # Connect to brickd
    # Don't use device before ipcon is connected
    rs232.set_configuration(5,0,1,8,0,0)
    # Register read callback to function cb_read
    rs232.register_callback(rs232.CALLBACK_READ, cb_read)

    # Enable read callback
    rs232.enable_read_callback()
    mensaje=rs232.read()
    # Write "test" string
    rs232.write(*string_to_char_list(str(mensaje)))
    #mensaje=rs232.read()
    #if mensaje.length==0:
     #   print ("Sin datos")
    #else:
     #   print ("Datos registrados")
    
    #print ("Mensaje: ", str(mensaje))
	

    input("Press key to exit\n") # Use raw_input() in Python 2
    print ("Lista de argumentos: ", sys.argv)

    ipcon.disconnect()