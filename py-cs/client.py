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
                data += packet
            packed_msg_size = self.data[:self.payload_size]
            self.data = data[self.payload_size:]
            msg_size = struct.unpack("Q", packed_msg_size)[0]

            while len(data) < msg_size:
                data += self.clientCV.recv(4 * 1024)

            frame_data = data[:msg_size]
            data = data[msg_size:]

            nparr = np.frombuffer(frame_data, np.uint8)
            frame = cv2.imdecode(nparr, cv2.IMREAD_COLOR)
            # print("getting frames...")

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


class camSend:
    def __init__(self, HOST) -> None:
        self.comm = ""
        self.HOST = HOST
        self.PORT_C = 1111

        self.clientCS = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.clientCS.connect((HOST, self.PORT_C))

    def sendComm(self) -> None:

        while True:
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
            time.sleep(0.5)

    # while True:
    #     data = conn.recv(1024).decode("utf-8")
    #     if data == "ZoomIn":
    #         print("Command ZoomIn received!")
    #         # r=requests.post(https://admin:Admin123@172.16.0.5:8080/ptzctrl.cgi?-step=0&-act=zoomin&-speed=1)

    #     if data == "ZoomOut":
    #         print("Command ZoomOut received!")
    #         # r=requests.post(https://admin:Admin123@172.16.0.5:8080/ptzctrl.cgi?-step=0&-act=zoomout&-speed=1)

    #     if data == "FocusIn":
    #         print("Command FocusIn received!")
    #         # r=requests.post(https://admin:Admin123@172.16.0.5:8080/ptzctrl.cgi?-step=0&-act=focusin&-speed=1)

    #     if data == "FocusOut":
    #         print("Command FocusOut received!")
    #         # r=requests.post(https://admin:Admin123@172.16.0.5:8080/ptzctrl.cgi?-step=0&-act=focusout&-speed=1)


if __name__ == "__main__":
    HOST = "127.0.0.1"

    cams_stream = cvStreamClient(HOST)
    ipcam_comm = camSend(HOST)
    # distance_rec = 

    video_thread = threading.Thread(target=cams_stream.videoStream)
    comm_thread = threading.Thread(target=ipcam_comm.sendComm)

    video_thread.start()
    comm_thread.start()

    video_thread.join()
    comm_thread.join()
