import requests
import json
import time
import sys

from tinkerforge.ip_connection import IPConnection
from tinkerforge.bricklet_rs232 import BrickletRS232

HOST = "localhost"
PORT = 4223
UID = "Cvv" # Change XYZ to the UID of your RS232 Bricklet

url = "https://covap.cumulocity.com/measurement/measurements"

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
    print(s)
       

if __name__ == "__main__":

    

    ipcon = IPConnection() # Create IP connection
    rs232 = BrickletRS232(UID, ipcon) # Create device object
    
    ipcon.connect(HOST, PORT) # Connect to brickd
    # Don't use device before ipcon is connected

    # Register read callback to function cb_read
    #rs232.register_callback(rs232.CALLBACK_READ, cb_read)
    
    # Enable read callback
    rs232.enable_read_callback()
    

    # Write "test" string
    #rs232.write(*string_to_char_list(sys.argv[1]))
    res= str(sys.argv[1])
    #resp=rs232.read()
    while True:
        msj=rs232.read() 
        print(msj)
        time.sleep(10)
    	
    payload = json.dumps({
          "c8y_RS232Measurement": {
            "Peso": {
              "value": int (res),
              "unit": "Kg"
            }
          },
          "time": "2021-05-13T17:03:14.000+02:00",
          "source": {
            "id": "82339"
          },
          "type": "c8y_RS232Measurement"
        })
    headers = {
      'Authorization': 'Basic bWlndWVsLnZpbGxhZ3JhQGJhYmVsLmVzOjIwMjBDb3ZhcA==',
      'Content-Type': 'application/json',
      'Accept': 'application/vnd.com.nsn.cumulocity.measurement+json'
    }
    response = requests.post(url, headers=headers, data=payload)
    	
    
    ipcon.disconnect()

    