import socket
import cv2
import numpy as np
import struct
import threading
import keyboard
import time


# client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
# HOST = "127.0.0.1"
# PORT = 11111
# client_socket.connect((HOST, PORT))

class cvStreamClient:
    def __init__(self, HOST) -> None:
        self.data = b""
        self.payload_size = struct.calcsize("Q")
        self.HOST = HOST
        self.PORT_CV = 3333

        self.clientCV = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.clientCV.connect((HOST, self.PORT_CV))
    
    def videoStream(self) -> None:
        while True:
            while len(self.data) < self.payload_size:
                packet = self.clientCV.recv(4 * 1024)
                if not packet:
                    break
                self.data += packet
            packed_msg_size = self.data[:self.payload_size]
            self.data = self.data[self.payload_size:]
            msg_size = struct.unpack("Q", packed_msg_size)[0]

            while len(self.data) < msg_size:
                self.data += self.clientCV.recv(4 * 1024)

            frame_data = self.data[:msg_size]
            self.data = self.data[msg_size:]

            nparr = np.frombuffer(frame_data, np.uint8)
            frame = cv2.imdecode(nparr, cv2.IMREAD_COLOR)
            # print("getting frames...")

            cv2.putText(frame, str(distance_rec.dist), (100, 100), cv2.FONT_HERSHEY_SIMPLEX, 3, (255, 255, 255), 1)

            cv2.imshow("OUTPUT", frame)
            cv2.waitKey(1)

        self.clientCV.close()
        cv2.destroyAllWindows()


# def video_stream():
#     data = b""
#     payload_size = struct.calcsize("Q")

#     while True:
#         while len(data) < payload_size:
#             packet = client_socket.recv(4 * 1024)
#             if not packet:
#                 break
#             data += packet

#         packed_msg_size = data[:payload_size]
#         data = data[payload_size:]
#         msg_size = struct.unpack("Q", packed_msg_size)[0]

#         while len(data) < msg_size:
#             data += client_socket.recv(4 * 1024)

#         frame_data = data[:msg_size]
#         data = data[msg_size:]

#         nparr = np.frombuffer(frame_data, np.uint8)
#         frame = cv2.imdecode(nparr, cv2.IMREAD_COLOR)
#         # print("getting frames...")

#         cv2.imshow("OUTPUT", frame)
#         cv2.waitKey(1)
        
#         #key = cv2.waitKey(2) & 0xFF
#         #if key == ord("q"):
#         #    break

#     client_socket.close()
#     cv2.destroyAllWindows()


# class camSend:
#     def __init__(self, HOST) -> None:
#         self.interval = 1.0

#         self.comm = ""
#         self.HOST = HOST
#         self.PORT_C = 1111

#         self.clientCS = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
#         self.clientCS.connect((HOST, self.PORT_C))

#     def sendComm(self) -> None:
#         while True:
#             start_time = time.time()

#             if keyboard.is_pressed("up"):
#                 self.comm = "zoomin"
#             elif keyboard.is_pressed("down"):
#                 self.comm = "zoomout"
#             elif keyboard.is_pressed("left"):
#                 self.comm = "focusout"
#             elif keyboard.is_pressed("right"):
#                 self.comm = "focusin"
#             else:
#                 self.comm = ""

#             if self.comm:
#                print(self.comm)
#             self.clientCS.sendall(self.comm.encode("utf-8"))
            
#             elapsed_time = time.time() - start_time
#             if elapsed_time < self.interval:
#                 time.sleep(self.interval - elapsed_time)

class camSend:
    def __init__(self, HOST) -> None:
        self.interval = 1
        self.is_running = False
        self.timer = None

        self.comm = ""
        self.HOST = HOST
        self.PORT_C = 1111

        self.clientCS = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.clientCS.connect((HOST, self.PORT_C))

    def sendComm(self) -> None:
        if not self.is_running:
            self.is_running = True

            if keyboard.is_pressed("up"):
                self.comm = "zoomin"
            elif keyboard.is_pressed("down"):
                self.comm = "zoomout"
            elif keyboard.is_pressed("left"):
                self.comm = "focusout"
            elif keyboard.is_pressed("right"):
                self.comm = "focusin"
            else:
                self.comm = ""

            if self.comm:
               print(self.comm)
               self.clientCS.sendall(self.comm.encode("utf-8"))

            self.is_running = False

        self.timer = threading.Timer(self.interval, self.sendComm)
        self.timer.start()

    def start_timer(self):
        self.timer = threading.Timer(self.interval, self.sendComm)
        self.timer.start()

    def stop_timer(self):
        if self.timer:
            self.timer.cancel()

class rangeClient:
    def __init__(self, HOST) -> None:
        self.dist = 0.0
        self.HOST = HOST
        self.PORT_RC = 2222

    def getDist(self) -> None:
        self.serverRF = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.serverRF.bind((self.HOST, self.PORT_RS))
        self.serverRF.listen()

        print("Rangefinder is waiting for a client to connect...")
        self.clientRF, self.client_address = self.serverRF.accept()
        print("Rangefinder is connected to client:", self.client_address, self.clientRF)
        
        while True:
            data = self.clientRF.recv(4)
            self.dist = struct.unpack('f', data)[0]
            print(f'Range: {self.dist::.1f} metres.' )
            time.sleep(0.25)


if __name__ == "__main__":
    HOST = "192.168.2.12"

    # cams_stream = cvStreamClient(HOST)
    ipcam_comm = camSend(HOST)
    distance_rec = rangeClient(HOST)

    # video_thread = threading.Thread(target=cams_stream.videoStream)
    comm_thread = threading.Thread(target=ipcam_comm.sendComm)
    range_thread = threading.Thread(target=distance_rec.getDist)

    # video_thread.start()
    comm_thread.start()
    range_thread.start()

    # video_thread.join()
    comm_thread.join()
    range_thread.join()
