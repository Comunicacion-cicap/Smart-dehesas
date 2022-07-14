#!/usr/bin/env python
# -*- coding: utf-8 -*-

HOST = "localhost"
PORT = 4223
UID = "Cvv" # Change XYZ to the UID of your RS232 Bricklet

import time
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

if __name__ == "__main__":
    ipcon = IPConnection() # Create IP connection
    rs232 = BrickletRS232(UID, ipcon) # Create device object

    ipcon.connect(HOST, PORT) # Connect to brickd
    # Don't use device before ipcon is connected
    
    # Ensure read callback is disabled, otherwise the read call will return empty
    rs232.disable_read_callback()

    # Send request
    rs232.write(*string_to_char_list('S02\r\n'))
    rs232.write(*string_to_char_list('MSV?\r\n'))

    # Wait for response
    time.sleep(1)

    # Read response
    message = char_list_to_string(*rs232.read())

    print('Message: ' + repr(message))
    
    ipcon.disconnect()