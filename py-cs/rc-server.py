import socket
import cv2
import numpy as np
import struct
import serial
import requests
import threading
import time

class rangeFinder:
    def __init__(self, HOST) -> None:
        self.HOST = HOST
        self.PORT_R = 2222

        self.serverRF = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.serverRF.bind((self.HOST, self.PORT_R))
        self.serverRF.listen()

        print("Rangefinder is waiting for a client to connect...")
        self.client_socket, self.client_address = self.serverRF.accept()
        print("Rangefinder is connected to client:", self.client_address, self.client_socket)

        self.ser = serial.Serial(
            port='/dev/ttyS0',
            baudrate=9600,
            parity=serial.PARITY_NONE,
            stopbits=serial.STOPBITS_ONE,
            bytesize=serial.EIGHTBITS,
            timeout=0.25
        )
        self.packet = bytearray(b'\xae\xa7\x04\x00\x05\x09\xbc\xbe')

    def measureDist(self) -> None:
        # PORT_R = 11112

        # self.server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        # self.server.bind((HOST, PORT_R))
        # self.server.listen()

        # print("Rangefinder is waiting for a client to connect...")
        # self.client_socket, self.client_address = self.server.accept()
        # print("Rangefinder is connected to client:", self.client_address, self.client_socket)

        while True:
            self.ser.write(self.packet)
            rec = self.ser.readline()

            # bytelist = bytes(rec)
            dist = bytes(rec)[8] * 0.1
            print(dist, " meters")

            packed_data = struct.pack('f', dist)
            self.serverRF.sendall(packed_data)

class camReceive:
    def __init__(self, HOST) -> None:
        # self.client_socket = client_socket
        self.cam_ip = "172.16.0.5"
        self.cam_port = "8554"
        self.username = "admin"
        self.password = "Admin123"

        self.HOST = HOST
        self.PORT_C = 1111

        # self.serverCR = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        # self.serverCR.bind((self.HOST, self.PORT_C))
        # self.serverCR.listen()

        # print("Controls are waiting for a client to connect...")
        # self.client_socket, self.client_address = self.serverCR.accept()
        # print("Controls are connected to client:", self.client_address, self.client_socket)

    def reqProcessing(self) -> None:
        self.serverCR = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.serverCR.bind((self.HOST, self.PORT_C))
        self.serverCR.listen()

        print("Controls are waiting for a client to connect...")
        self.client_socket, self.client_address = self.serverCR.accept()
        print("Controls are connected to client:", self.client_address, self.client_socket)

        while True:
            data = self.client_socket.recv(1024)

            if len(data) > 0:
                received_data = data.decode("utf-8")
                rec_uri = f"http://{self.username}:{self.password}@{self.cam_ip}:{self.cam_port}/ptzctrl.cgi?-step=0&-act={received_data}&-speed=1"

                print(received_data, ':', rec_uri)
                # requests.get(rec_uri, auth=(self.username, self.password))

            time.sleep(1)


class cvStreamServer:
    def __init__(self, HOST) -> None:
        self.cap = cv2.VideoCapture(4)

        self.HOST = HOST
        self.PORT_CV = 3333

        # self.serverCV = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        # self.serverCV.bind((HOST, self.PORT_CV))
        # self.serverCV.listen()

        # print("Stream is waiting for a client to connect...")
        # self.client_socket, self.client_address = self.serverCV.accept()
        # print("Camera stream is connected to client:", self.client_address)

    def videoStream(self) -> None:
        self.serverCV = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.serverCV.bind((HOST, self.PORT_CV))
        self.serverCV.listen()

        print("Stream is waiting for a client to connect...")
        self.client_socket, self.client_address = self.serverCV.accept()
        print("Camera stream is connected to client:", self.client_address)

        while True:
            ret, frame = self.cap.read()
            frame_data = cv2.imencode('.jpg', frame)[1].tobytes()

            msg_size = struct.pack("Q", len(frame_data))
            self.client_socket.sendall(msg_size + frame_data)

            cv2.imshow("SOURCE", frame)
            if cv2.waitKey(1) & 0xFF == ord('q'):
                break

        self.cap.release()
        self.client_socket.close()
        cv2.destroyAllWindows()


if __name__ == "__main__":
    # HOST = '192.168.2.13'
    HOST = "127.0.0.1"
    # PORT = 11111

    # server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    # server.bind((HOST, PORT))
    # server.listen()

    # print("Waiting for a client to connect...")
    # client_socket, client_address = server.accept()
    # print("Connected to client:", client_address)

    # range_finder = rangeFinder(HOST)
    webcam = cvStreamServer(HOST)
    cam_control = camReceive(HOST)

    # range_thread = threading.Thread(target=range_finder.measureDist)
    cam_thread = threading.Thread(target=webcam.videoStream)
    contr_thread = threading.Thread(target=cam_control.reqProcessing)

    # range_thread.start()
    cam_thread.start()
    contr_thread.start()

    # range_thread.join()
    contr_thread.join()
    cam_thread.join()
