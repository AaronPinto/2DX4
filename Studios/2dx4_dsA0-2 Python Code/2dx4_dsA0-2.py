import serial

#Modify the following line with your own serial port details
#   Currently set COM3 as serial port at 115.2kbps 8N1
#   Refer to PySerial API for other options.  One option to consider is
#   the "timeout" - allowing the program to proceed if after a defined
#   timeout period.  The default = 0, which means wait forever.

s = serial.Serial("COM3", 115200)

print("Opening: " + s.name)

s.write(b'1')           #This program will send a '1' or 0x31 

for i in range(10):
    x = s.read()        # read one byte
    c = x.decode()      # convert byte type to str
    print(c)
    
print("Closing: " + s.name)
s.close()
