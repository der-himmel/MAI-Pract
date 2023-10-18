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
        self.serverRF = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.serverRF.bind((HOST, self.PORT_RS))

        self.ser = serial.Serial(
            port="/dev/ttyS0",
            baudrate=9600,
            parity=serial.PARITY_NONE,
            stopbits=serial.STOPBITS_ONE,
            bytesize=serial.EIGHTBITS,
            timeout=0.25,
        )
        self.packet = bytearray(b"\xae\xa7\x04\x00\x05\x09\xbc\xbe")

    def rangeConnect(self):
        while True:
            try:
                self.serverRF.listen()

                print("Rangefinder is waiting for a client to connect...")
                self.clientRF, self.client_address = self.serverRF.accept()
                print("Rangefinder is connected to client:", self.client_address)

                self.measureDist()
            except OSError as OSE:
                print(f"catch RF: Cannot assign requested address: {OSE}")
                try:
                    self.clientRF.close()
                    time.sleep(1)
                except AttributeError as AE:
                    print(f"catch: No client RF socket found {AE}")
                    time.sleep(1)
                    continue

    def measureDist(self) -> None:
        while True:
            self.ser.write(self.packet)
            rec = self.ser.readline()

            try:
                bytelist = bytes(rec)
                dist = bytelist[8] * 0.1

            except IndexError as err:
                print("Rangefinder reading error...", err)
                continue
            
            try:
                packed_data = struct.pack("f", dist)
                self.clientRF.sendall(packed_data)
            except ConnectionResetError as CER:
                print(f"catch: {CER}")
                self.rangeConnect()
            except BrokenPipeError as BPE:
                print(f"catch: {BPE}")
                self.rangeConnect()


class camReceive:
    def __init__(self, HOST) -> None:
        self.cam_ip = "172.16.0.5"
        self.cam_port = "8554"
        self.username = "admin"
        self.password = "Admin123"

        self.HOST = HOST
        self.PORT_C = 1111
        self.serverCR = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.serverCR.bind((HOST, self.PORT_C))

    def controlServerConnect(self):
        while True:
            try:
                self.serverCR.listen()

                print("Control is waiting for a client to connect...")
                self.clientCR, self.client_address = self.serverCR.accept()
                print("Camera control is connected to client:", self.client_address)

                self.reqProcessing()

            except OSError as OSE:
                print(f"catch CONTR: Cannot assign requested address: {OSE}")
                try:
                    self.clientCR.close()
                    time.sleep(1)
                except AttributeError as AE:
                    print(f"catch: No client CR socket found {AE}")
                    time.sleep(1)
                    continue
                    
    def reqProcessing(self) -> None:
        while True:
            try:
                data = self.clientCR.recv(1024)
            except ConnectionResetError as CER:
                print(f"catch: {CER}")
                self.controlServerConnect()
            except BrokenPipeError as BPE:
                print(f"catch: {BPE}")
                self.controlServerConnect()

            if len(data) > 0:
                received_data = data.decode("utf-8")
                # rec_uri = f"http://{self.username}:{self.password}@{self.cam_ip}:{self.cam_port}/ptzctrl.cgi?-step=0&-act={received_data}&-speed=1"
                rec_uri = f"http://{self.cam_ip}/ptzctrl.cgi?-step=0&-act={received_data}&-speed=50"
                stop_uri = f"http://{self.cam_ip}/ptzctrl.cgi?-step=0&-act=stop&-speed=50"

                print(received_data, ":", rec_uri)
                response = requests.get(rec_uri, auth=(self.username, self.password))
                print("Response status: ", response.status_code)
                time.sleep(0.25)
                requests.get(stop_uri, auth=(self.username, self.password))


class cvStreamServer:
    def __init__(self, HOST) -> None:
        self.therm = cv2.VideoCapture(0)
        self.ipcam = cv2.VideoCapture("rtsp://admin:Admin123@172.16.0.5:554/12")

        self.therm.set(cv2.CAP_PROP_FPS, 25)
        self.ipcam.set(cv2.CAP_PROP_FPS, 25)
        # cv2.waitKey(200)

        self.HOST = HOST
        self.PORT_CV = 3333
        self.serverCV = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.serverCV.bind((HOST, self.PORT_CV))

    def cvServerConnect(self):
        while True:
            try:
                self.serverCV.listen()

                print("Stream is waiting for a client to connect...")
                self.clientCV, self.client_address = self.serverCV.accept()
                print("Camera stream is connected to client:", self.client_address)

                self.videoStream()
                
            except OSError as OSE:
                print(f"catch CV: Cannot assign requested address: {OSE}")
                try:
                    self.clientCV.close()
                    time.sleep(1)
                except AttributeError as AE:
                    print(f"catch: No client CV socket found {AE}")
                    time.sleep(1)
                    continue

    def videoStream(self) -> None:
        while True:
            try:
                ret1, tframe = self.therm.read()
                ret2, cframe = self.ipcam.read()
            except:
                print("Error while reading frames...")
                continue

            if ret1 and ret2:
                try:
                    self.editFrame(tframe, cframe)
                except:
                    print("Error while editing frame...")
                    continue

            frame_data = cv2.imencode(".jpg", self.resframe)[1].tobytes()
            msg_size = struct.pack("Q", len(frame_data))
            try:
                self.clientCV.sendall(msg_size + frame_data)
            except:
                print(f"CLIENTCV.SENDALL() catched some error ")
                self.cvServerConnect()

            cv2.imshow("SOURCE", self.resframe)
            cv2.waitKey(1)

        self.therm.release()
        self.ipcam.release()
        self.client_socket.close()
        cv2.destroyAllWindows()

    def editFrame(self, tframe, cframe):
        tframe = cv2.rotate(tframe, cv2.ROTATE_90_CLOCKWISE)
        tframe = cv2.rotate(tframe, cv2.ROTATE_90_CLOCKWISE)

        tframe = cv2.resize(tframe,
            (int(cframe.shape[0] / tframe.shape[0] * tframe.shape[1]),
            int(cframe.shape[0])))

        self.resframe = cv2.hconcat([tframe, cframe])

        return self.resframe


if __name__ == "__main__":
    HOST = "192.168.2.13"

    range_finder = rangeFinder(HOST)
    webcam = cvStreamServer(HOST)
    cam_control = camReceive(HOST)

    range_thread = threading.Thread(target=range_finder.rangeConnect)
    cam_thread = threading.Thread(target=webcam.cvServerConnect)
    contr_thread = threading.Thread(target=cam_control.controlServerConnect)

    range_thread.start()
    cam_thread.start()
    contr_thread.start()

    range_thread.join()
    contr_thread.join()
    cam_thread.join()
