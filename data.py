import Adafruit_BBIO.UART as UART
import serial
 
UART.setup("UART1")
 
ser = serial.Serial(port = "/dev/ttyO1", baudrate=9600)
ser.close()

data = [0x86,0xA2,0x40,0x40,0x40,0x40,0x60,0xAE,0x64,0x8C,0xA6,0x40,0x40,0x68,0xA4,0x8A,0x98,0x82,0xB2,0x40,0x61,0x03,0xF0,0x54,0x65,0x73,0x74]
ser.open()
if ser.isOpen():
	print "Serial is open!"
    	ser.write("Hello World!")
	print "now data"
	x = 0
	while x < 27:
    		print data[x]
		y = str(data[x])
		ser.write(y)
		#ser.write(data[x])
		x = x + 1
ser.close()
