import serial
import time
ser = serial.Serial('COM8', baudrate = 9600, timeout = 1)
x = 0
msg = 'this is a message.'
msg += '\n'
if ser.isOpen():
    ser.close()
ser.open()
while x<2:
	#ser.write(b'HelloPythonCOMport\n')
	print(msg)
	ser.write(msg.encode())
	time.sleep(1)
	x+=1
nameOfFile = "test.txt"
modeOfFile = "w+"
file = open(nameOfFile, "r")

#--Command satellite to open a file
time.sleep(1)
msg = ''
msg += 'gsToBIRDcommand'
msg += 'nameOfFile'
msg += nameOfFile
msg += '\n'
print(msg)
ser.write(msg.encode())
#--wait for ok signal to send next command
feedback = ser.readline()
while len(feedback) <= 2:
	feedback = ser.readline()
feedback2 = feedback[0:22] #This is still a byte obj
feedback3 = feedback2.decode() #Feedback3 is now a string
print(feedback3)
while feedback3 != 'Procceed with the next':
	feedback = ser.readline()
	feedback2 = feedback[0:22] #This is still a byte obj
	feedback3 = feedback2.decode() #Feedback3 is now a string
	print(feedback3)
#--now send mode command
time.sleep(2)
#absence of delay sometimes 
#sends two line at same time
#need to investigate later
msg = ''
msg += 'gsToBIRDcommand'
msg += 'modeOfFile'
msg += 'w+'
msg += '\n'
print(msg)
ser.write(msg.encode())
#--wait for ok signal to send next command
feedback = ser.readline()
while len(feedback) <= 2:
	feedback = ser.readline()
feedback2 = feedback[0:22] #This is still a byte obj
feedback3 = feedback2.decode() #Feedback3 is now a string
print(feedback3)
while feedback3 != 'Procceed with the next':
	feedback = ser.readline()
	feedback2 = feedback[0:22] #This is still a byte obj
	feedback3 = feedback2.decode() #Feedback3 is now a string
	print(feedback3)
time.sleep(1)
#absence of delay sometimes 
#sends two line at same time
#need to investigate later

#--Send the file
x = 0
while x < 20:
	msg = 'data'
	msg += file.readline()
	msg += '\n'
	print(msg)
	ser.write(msg.encode())
	
	time.sleep(.1)
	#absence of delay sometimes 
	#sends two line at same time
	#need to investigate later
	feedback = ser.readline()
	while len(feedback) <= 2:
		feedback = ser.readline()
	feedback2 = feedback[0:22] #This is still a byte obj
	feedback3 = feedback2.decode() #Feedback3 is now a string
	print(feedback3)
	while feedback3 != 'Procceed with the next':
		feedback = ser.readline()
		feedback2 = feedback[0:22] #This is still a byte obj
		feedback3 = feedback2.decode() #Feedback3 is now a string
		print(feedback3)
	x+=1

#Tell satellite to close the file
time.sleep(1)
msg = ''
msg += 'gsToBIRDcommand'
msg += 'endOfFile'
msg += '\n'
print(msg)
ser.write(msg.encode())	
#--GS work is done. Close and exit
file.close()
ser.close()