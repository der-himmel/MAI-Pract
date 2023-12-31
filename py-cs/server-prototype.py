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
        self.PORT_RS = 2222

        # self.serverRF = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        # self.serverRF.bind((self.HOST, self.PORT_RS))
        # self.serverRF.listen()

        # print("Rangefinder is waiting for a client to connect...")
        # self.client_socket, self.client_address = self.serverRF.accept()
        # print("Rangefinder is connected to client:", self.client_address, self.client_socket)

        self.ser = serial.Serial(
            port="/dev/ttyS0",
            baudrate=9600,
            parity=serial.PARITY_NONE,
            stopbits=serial.STOPBITS_ONE,
            bytesize=serial.EIGHTBITS,
            timeout=0.25,
        )
        self.packet = bytearray(b"\xae\xa7\x04\x00\x05\x09\xbc\xbe")

    # def rangeConnect(self):
    #     while True:
    #         try:
    #             self.serverRF = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    #             self.serverRF.bind((HOST, self.PORT_RS))
    #             self.serverRF.listen()

    #             print("Rangefinder is waiting for a client to connect...")
    #             self.client_socket, self.client_address = self.serverRF.accept()
    #             print("Rangefinder is connected to client:", self.client_address)

    #             self.measureDist()
    #         except:
    #             print("Couldn't connect to RANGEFINDER client. Reconnecting...")
    #             time.sleep(1)

    def rangeConnect(self):
        while True:
            self.serverRF = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self.serverRF.bind((HOST, self.PORT_RS))
            self.serverRF.listen()

            print("Rangefinder is waiting for a client to connect...")
            self.clientRF, self.client_address = self.serverRF.accept()
            print("Rangefinder is connected to client:", self.clientRF)

            self.measureDist()


    def measureDist(self) -> None:
        # self.serverRF = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        # self.serverRF.bind((self.HOST, self.PORT_RS))
        # self.serverRF.listen()

        # print("Rangefinder is waiting for a client to connect...")
        # self.clientRF, self.client_address = self.serverRF.accept()
        # print("Rangefinder is connected to client:", self.client_address, self.clientRF)

        while True:
            self.ser.write(self.packet)
            rec = self.ser.readline()

            try:
                bytelist = bytes(rec)
                dist = bytelist[8] * 0.1
                print(f"{dist:.1f} meters")

            except IndexError as err:
                print("Rangefinder reading error...", err)
                continue
            
            try:
                packed_data = struct.pack("f", dist)
                self.clientRF.sendall(packed_data)
            except ConnectionResetError as CER:
                print(f"catched error: {CER}")
                self.rangeConnect()
            except BrokenPipeError as BPE:
                print(f"catched error: {BPE}")
                self.rangeConnect()


class camReceive:
    def __init__(self, HOST) -> None:
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

    # def controlServerConnect(self):
    #     while True:
    #         try:
    #             self.serverCR = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    #             self.serverCR.bind((HOST, self.PORT_C))
    #             self.serverCR.listen()

    #             print("Control is waiting for a client to connect...")
    #             self.clientCR, self.client_address = self.serverCR.accept()
    #             print("Camera control is connected to client:", self.clientCR)

    #             self.reqProcessing()
    #         except:
    #             print("Couldn't connect to CONTROL client. Reconnecting...")
    #             time.sleep(1)

    def controlServerConnect(self):
        while True:
            self.serverCR = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self.serverCR.bind((HOST, self.PORT_C))
            self.serverCR.listen()

            print("Control is waiting for a client to connect...")
            self.clientCR, self.client_address = self.serverCR.accept()
            print("Camera control is connected to client:", self.clientCR)

            self.reqProcessing()

    def reqProcessing(self) -> None:
        # self.serverCR = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        # self.serverCR.bind((self.HOST, self.PORT_C))
        # self.serverCR.listen()

        # print("Controls are waiting for a client to connect...")
        # self.clientCR, self.client_address = self.serverCR.accept()
        # print("Controls are connected to client:", self.client_address, self.clientCR)

        while True:
            try:
                data = self.clientCR.recv(1024)
            except:
                print("Error while fetching control commands...")
                continue

            if len(data) > 0:
                received_data = data.decode("utf-8")
                # rec_uri = f"http://{self.username}:{self.password}@{self.cam_ip}:{self.cam_port}/ptzctrl.cgi?-step=0&-act={received_data}&-speed=1"
                rec_uri = f"http://{self.cam_ip}/ptzctrl.cgi?-step=0&-act={received_data}&-speed=1"

                print(received_data, ":", rec_uri)
                response = requests.get(rec_uri, auth=(self.username, self.password))

                print("Response status: ", response.status_code)

            # time.sleep(1)


class cvStreamServer:
    def __init__(self, HOST) -> None:
        self.therm = cv2.VideoCapture(0)
        self.ipcam = cv2.VideoCapture("rtsp://admin:Admin123@172.16.0.5:554/12")

        self.therm.set(cv2.CAP_PROP_FPS, 27)
        self.ipcam.set(cv2.CAP_PROP_FPS, 27)
        cv2.waitKey(200)

        self.HOST = HOST
        self.PORT_CV = 3333

        # self.serverCV = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        # self.serverCV.bind((HOST, self.PORT_CV))
        # self.serverCV.listen()

        # print("Stream is waiting for a client to connect...")
        # self.client_socket, self.client_address = self.serverCV.accept()
        # print("Camera stream is connected to client:", self.client_address)

    # def cvServerConnect(self):
    #     while True:
    #         try:
    #             self.serverCV = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    #             self.serverCV.bind((HOST, self.PORT_CV))
    #             self.serverCV.listen()

    #             print("Stream is waiting for a client to connect...")
    #             self.client_socket, self.client_address = self.serverCV.accept()
    #             print("Camera stream is connected to client:", self.client_address)

    #             self.videoStream()
    #         except:
    #             print("Couldn't connect to CAMERA client. Reconnecting...")
    #             time.sleep(1)

    def cvServerConnect(self):
        while True:
            self.serverCV = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self.serverCV.bind((HOST, self.PORT_CV))
            self.serverCV.listen()

            print("Stream is waiting for a client to connect...")
            self.client_socket, self.client_address = self.serverCV.accept()
            print("Camera stream is connected to client:", self.client_address)

            self.videoStream()

    def videoStream(self) -> None:
        # self.serverCV = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        # self.serverCV.bind((HOST, self.PORT_CV))
        # self.serverCV.listen()

        # print("Stream is waiting for a client to connect...")
        # self.client_socket, self.client_address = self.serverCV.accept()
        # print("Camera stream is connected to client:", self.client_address)

        while True:
            try:
                ret1, tframe = self.therm.read()
                ret2, cframe = self.ipcam.read()
            except:
                print("Error while reading frames...")
                continue

            try:
                tframe = cv2.rotate(tframe, cv2.ROTATE_90_CLOCKWISE)
                tframe = cv2.rotate(tframe, cv2.ROTATE_90_CLOCKWISE)

                tframe = cv2.resize(tframe, (int(cframe.shape[0] / tframe.shape[0] * tframe.shape[1]), int(cframe.shape[0])))
                #tframe = cv2.resize(tframe, (470, 352))
                # color = cv2.applyColorMap(tframe, cv2.COLORMAP_JET)
                res = cv2.hconcat([tframe, cframe])
            except:
                print("Error while editing frames...")
                continue

            frame_data = cv2.imencode(".jpg", res)[1].tobytes()
            msg_size = struct.pack("Q", len(frame_data))
            try:
                self.client_socket.sendall(msg_size + frame_data)
            except ConnectionResetError as CER:
                print(f"catched error: {CER}")
                self.cvServerConnect()
            except BrokenPipeError as BPE:
                print(f"catched error: {BPE}")
                self.cvServerConnect()

            cv2.imshow("SOURCE", res)
            if cv2.waitKey(1) & 0xFF == ord("q"):
                break

        self.therm.release()
        self.ipcam.release()
        self.client_socket.close()
        cv2.destroyAllWindows()


if __name__ == "__main__":
    HOST = "192.168.2.13"
    # HOST = "127.0.0.1"

    # server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    # server.bind((HOST, PORT))
    # server.listen()

    # print("Waiting for a client to connect...")
    # client_socket, client_address = server.accept()
    # print("Connected to client:", client_address)

    range_finder = rangeFinder(HOST)
    webcam = cvStreamServer(HOST)
    cam_control = camReceive(HOST)

    # range_thread = threading.Thread(target=range_finder.measureDist)
    # cam_thread = threading.Thread(target=webcam.videoStream)
    # contr_thread = threading.Thread(target=cam_control.reqProcessing)

    range_thread = threading.Thread(target=range_finder.rangeConnect)
    cam_thread = threading.Thread(target=webcam.cvServerConnect)
    contr_thread = threading.Thread(target=cam_control.controlServerConnect)

    range_thread.start()
    cam_thread.start()
    contr_thread.start()

    range_thread.join()
    contr_thread.join()
    cam_thread.join()
