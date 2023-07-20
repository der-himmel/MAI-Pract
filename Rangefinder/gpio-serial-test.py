import time
import serial


ser = serial.Serial(
        port='/dev/ttyS0', #Replace ttyS0 with ttyAM0 for Pi1,Pi2,Pi0
        baudrate = 9600,
        parity=serial.PARITY_NONE,
        stopbits=serial.STOPBITS_ONE,
        bytesize=serial.EIGHTBITS,
        timeout=1
)

# ser = serial.Serial('/dev/ttyS0',
#         baudrate=9600,
#         parity=serial.PARITY_NONE,
#         stopbits=serial.STOPBITS_ONE)

packet = bytearray()
packet.append(0xAE)
packet.append(0xA7)
packet.append(0x04)
packet.append(0x00)
packet.append(0x05)
packet.append(0x09)
packet.append(0xBC)
packet.append(0xBE)

# ser.write(packet)

# data = ser.read()
# print(bytes)

while 1:
        ser.write(packet)  #transmit
        print("sent:     ",  packet, '\n')
        rec = ser.readline()
        print("received: ", rec, '\n')  #receive
        time.sleep(1)
