#import every thing necessary
import serial
import time
import os
import Adafruit_BBIO.GPIO as GPIO

#define pins
LED="P9_12"
GPIO.setup(LED, GPIO.OUT)
GPIO.output(LED, GPIO.LOW) #to enter bootloader mode

#open Serial
ser = serial.Serial()
ser.port= "/dev/ttyO4"
ser.baudrate = 9600
ser.bytesize = serial.EIGHTBITS
ser.parity = serial.PARITY_NONE
ser.stopbits = serial.STOPBITS_ONE
ser.timeout = 5
ser.xonxoff= True
ser.rtscts = False
ser.dsrdtr = False
ser.writeTimeout = 5

#prepare files
nameOfFile = "appV2.hex"
modeOfFile = "r"

with open(nameOfFile, modeOfFile) as file:
    lineList = file.readlines()
print("list loading done.\n\r")
print("Number of lines: ")
print(len(lineList))



try:
        ser.open()
except Exception as e:
        print("error open Serial Port: " +str(e))
        GPIO.output(LED, GPIO.HIGH) #Normally needs to be high
        file.close()
	GPIO.cleanup()
        exit()

#do the work
if ser.isOpen():
	print("\n\rSerial is Open\n\r")
	for x in range(len(lineList)):
		print(lineList[x])
    		ser.write(lineList[x])
		print("Feed, ")
		print(ser.readline())
		print("\n")
else:
	print("Serial is not open")

#analyze feedback
for y in range(5):
	feedback = ser.readline()
	if (feedback.find(b'\x06') != -1):
		print("Success feedback")

#cleanup
GPIO.output(LED, GPIO.HIGH) #Normally needs to be high
ser.close()
file.close()
GPIO.cleanup()
print("end")
