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

        # self.clientCV = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        # self.clientCV.connect((self.HOST, self.PORT_CV))

    def cvStreamConnect(self):
        while True:
            try:
                self.clientCV = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                self.clientCV.connect((self.HOST, self.PORT_CV))
                print("CAMERA connected!")
                self.videoStream()
            except:
                print("Couldn't connect to the CAMERA server... Reconnecting in 5 sec")
                # self.clientCV.close()
                time.sleep(1)

    def videoStream(self) -> None:
        # self.clientCV = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        # self.clientCV.connect((self.HOST, self.PORT_CV))

        while True:
            while len(self.data) < self.payload_size:
                try:
                    packet = self.clientCV.recv(4 * 1024)
                    if not packet:
                        break
                        # print("Error receiving packet...")
                        # continue
                    self.data += packet

                    packed_msg_size = self.data[: self.payload_size]
                    self.data = self.data[self.payload_size :]
                    msg_size = struct.unpack("Q", packed_msg_size)[0]
                except:
                    print("Couldn't acquire data...")
                    continue

            while len(self.data) < msg_size:
                try:
                    self.data += self.clientCV.recv(4 * 1024)
                except:
                    print("Some data was lost...")
                    continue
            frame_data = self.data[:msg_size]
            self.data = self.data[msg_size:]

            try:
                nparr = np.frombuffer(frame_data, np.uint8)
                frame = cv2.imdecode(nparr, cv2.IMREAD_COLOR)
                # print("getting frames...")
            except:
                print("Couldn't decode image...")
                continue

            # frame = cv2.resize(
            #     frame,
            #     (int(frame.shape[1] * 2), int(frame.shape[0] * 2)),
            #     cv2.INTER_LINEAR,
            # )

            if frame is not None:
                try:
                    self.drawHUD(frame)
                    cv2.imshow("OUTPUT", frame)
                    cv2.waitKey(1)
                except:
                    print("Error while drawing HUD...")
                    # continue
                    self.cvStreamClient()

            # cv2.imshow("OUTPUT", frame)
            # cv2.waitKey(1)

        self.clientCV.close()
        cv2.destroyAllWindows()

    def drawHUD(self, frame):
        # heightT, widthT = frame.shape[0] -
        # center_xT, center_yT = width // 2, height // 2

        # cv2.line(
        #     image,
        #     (center_xT - 20, center_yT),
        #     (center_xT + 20, center_yT),
        #     color,
        #     thickness,
        # )
        # cv2.line(
        #     image,
        #     (center_xT, center_yT - 20),
        #     (center_xT, center_yT + 20),
        #     color,
        #     thickness,
        # )

        dist_string = f"RANGE: {distance_rec.dist:.1f}M"

        (text_width, text_height), _ = cv2.getTextSize(
            dist_string, cv2.FONT_HERSHEY_SIMPLEX, 1, 5
        )
        # try:
        #     rect_x = int(frame.shape[1]) - 2 * text_width
        #     rect_y = int(frame.shape[0]) - text_height - 15

        #     cv2.rectangle(
        #         frame,
        #         (rect_x, rect_y),
        #         (rect_x + text_width, rect_y + text_height),
        #         (0, 0, 0, 127),
        #         cv2.FILLED,
        #     )

        #     cv2.putText(
        #         frame,
        #         dist_string,
        #         (rect_x, rect_y + text_height),
        #         cv2.FONT_HERSHEY_SIMPLEX,
        #         1,
        #         (255, 255, 255),
        #         2,
        #     )
        # except:
        #     print("Error while editing received frames...")
        #     return frame

        rect_x = int(frame.shape[1]) - 2 * text_width
        rect_y = int(frame.shape[0]) - text_height - 15

        cv2.rectangle(
            frame,
            (rect_x, rect_y),
            (rect_x + text_width, rect_y + text_height),
            (0, 0, 0, 127),
            cv2.FILLED,
        )

        cv2.putText(
            frame,
            dist_string,
            (rect_x, rect_y + text_height),
            cv2.FONT_HERSHEY_SIMPLEX,
            1,
            (255, 255, 255),
            2,
        )

        return frame


class camSend:
    def __init__(self, HOST) -> None:
        self.interval = 1.0

        self.comm = ""
        self.HOST = HOST
        self.PORT_C = 1111

        # self.clientCS = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        # self.clientCS.connect((self.HOST, self.PORT_C))

    def camSendConnect(self):
        while True:
            try:
                self.clientCS = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                self.clientCS.connect((self.HOST, self.PORT_C))
                print("CONTROL connected!")
                self.sendComm()
            except:
                print("Couldn't connect to the CONTROL server... Reconnecting in 5 sec")
                # self.clientCS.close()
                time.sleep(1)

    def sendComm(self) -> None:
        # self.clientCS = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        # self.clientCS.connect((self.HOST, self.PORT_C))

        while True:
            start_time = time.time()

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

            try:
                self.clientCS.sendall(self.comm.encode("utf-8"))
            except:
                print("Couldn't send control command...")
                self.camSendConnect()
                # continue

            elapsed_time = time.time() - start_time
            if elapsed_time < self.interval:
                time.sleep(self.interval - elapsed_time)


class rangeClient:
    def __init__(self, HOST) -> None:
        self.dist = 0.0
        self.HOST = HOST
        self.PORT_RC = 2222

        # self.clientRF = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        # self.clientRF.connect((self.HOST, self.PORT_RC))

    def rangeClientConnect(self):
        while True:
            try:
                self.clientRF = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                self.clientRF.connect((self.HOST, self.PORT_RC))
                print("RANGEFINDER connected!")
                self.getDist()
            except:
                print(
                    "Couldn't connect to the RANGEFINDER server... Reconnecting in 5 sec"
                )
                # self.clientRF.close()
                time.sleep(1)

    def getDist(self) -> None:
        # self.clientRF = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        # self.clientRF.connect((self.HOST, self.PORT_RC))

        while True:
            try:
                data = self.clientRF.recv(4)
                self.dist = struct.unpack("f", data)[0]
                print(f"Range: {self.dist:.1f} metres.")
                time.sleep(0.25)
            except:
                print("Error fetching rangefinder data.")
                # continue
                self.rangeClientConnect()


if __name__ == "__main__":
    HOST = "192.168.2.13"

    cams_stream = cvStreamClient(HOST)
    ipcam_comm = camSend(HOST)
    distance_rec = rangeClient(HOST)

    # video_thread = threading.Thread(target=cams_stream.videoStream)
    # comm_thread = threading.Thread(target=ipcam_comm.sendComm)
    # range_thread = threading.Thread(target=distance_rec.getDist)

    range_thread = threading.Thread(target=distance_rec.rangeClientConnect)
    video_thread = threading.Thread(target=cams_stream.cvStreamConnect)
    comm_thread = threading.Thread(target=ipcam_comm.camSendConnect)

    range_thread.start()
    video_thread.start()
    comm_thread.start()

    range_thread.join()
    video_thread.join()
    comm_thread.join()
