import socket
import cv2
import numpy as np
import struct
import serial
import requests
import threading


HOST = '192.168.2.13'
PORT = 11111

server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server.bind((HOST, PORT))
server.listen()

print("Waiting for a client to connect...")
client_socket, client_address = server.accept()
print("Connected to client:", client_address)


def rangefinder():
	ser = serial.Serial(
        port='/dev/ttyS0',
        baudrate = 9600,
        parity=serial.PARITY_NONE,
        stopbits=serial.STOPBITS_ONE,
        bytesize=serial.EIGHTBITS,
        timeout=0.25
        )

	packet = bytearray(b'\xae\xa7\x04\x00\x05\x09\xbc\xbe')
	
	while True:
		ser.write(packet)	
		rec = ser.readline()
	
		bytelist = bytes(rec)
		dist = bytelist[8] * 0.1
		print(dist, " meters")
		
		server.sendall(dist)

def cam_control():
	cam_ip = "172.16.0.5"
	cam_port = "8554"
	username = "admin"
	password = "Admin123"
	
	while True:
		data = client_socket.recv(1024)
	
		if len(data) > 0:
			received_data = data.decode("utf-8")
			rec_uri = f"http://{username}:{password}@{cam_ip}:{cam_port}/ptzctrl.cgi?-step=0&-act={received_data}&-speed=1"
			
			print(received_data)
			requests.get(rec_uri, auth=(username, password))
			
		time.sleep(0.25)
			
        
if __name__ == "__main__":
    #range_thread = threading.Thread(target=rangefinder)
    contr_thread = threading.Thread(target=cam_control)

    #range_thread.start()
    contr_thread.start()

    #range_thread.join()
    contr_thread.join()
