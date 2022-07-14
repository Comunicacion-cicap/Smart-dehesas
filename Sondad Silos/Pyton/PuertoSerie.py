#!/usr/bin/python3
import serial
import time
 
# Arduino official doc:https://www.arduino.cc/en/serial/begin
# by default Arduino set 8 bit frame, parity none and 1 stop bit
 
puerto   = serial.Serial(port = '/dev/ttyUSB0',
                         baudrate = 9600,
                         bytesize = serial.EIGHTBITS,
                         parity   = serial.PARITY_NONE,
                         stopbits = serial.STOPBITS_ONE)


puerto_Arduino   = serial.Serial(port = '/dev/ttyACM0',
                         baudrate = 115200,
                         timeout=.1)

while(1):

    try:

        Lectura= puerto_Arduino.readline()
        
        if Lectura == 'S01':   

            puerto.write('S01 \r\n'.encode())
            time.sleep(1)
            count = 0
            while count < 12:
                puerto.write('MSV? \r\n'.encode())
                time.sleep(1)
                buffer=puerto.readline()
                medida=''
            
            #puerto.close()

            puerto_Arduino.write(buffer.encode())
            #puerto_Arduino.close()

        if Lectura == 'S02':  

            puerto.write('S02 \r\n'.encode())
            time.sleep(1)
            count = 0
            while count < 12:
                puerto.write('MSV? \r\n'.encode())
                time.sleep(1)
                buffer=puerto.readline()
                medida=''
            
           # puerto.close()

            #puerto_Arduino.write(buffer.encode())
            puerto_Arduino.write(buffer)
            #puerto_Arduino.close()

    



    except serial.SerialException:
        print('Port is not available') 
 
    except serial.portNotOpenError:
        print('Attempting to use a port that is not open')
        print('End of script') 



puerto.close()
puerto_Arduino.close()
