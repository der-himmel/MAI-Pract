import socket
import cv2
import numpy as np
import struct
import threading
import keyboard
import time


client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
HOST = "127.0.0.1"
PORT = 11111
client_socket.connect((HOST, PORT))


def video_stream():
    data = b""
    payload_size = struct.calcsize("Q")

    while True:
        while len(data) < payload_size:
            packet = client_socket.recv(4 * 1024)
            if not packet:
                break
            data += packet

        packed_msg_size = data[:payload_size]
        data = data[payload_size:]
        msg_size = struct.unpack("Q", packed_msg_size)[0]

        while len(data) < msg_size:
            data += client_socket.recv(4 * 1024)

        frame_data = data[:msg_size]
        data = data[msg_size:]

        nparr = np.frombuffer(frame_data, np.uint8)
        frame = cv2.imdecode(nparr, cv2.IMREAD_COLOR)
        # print("getting frames...")

        cv2.imshow("OUTPUT", frame)
        cv2.waitKey(1)
        
        #key = cv2.waitKey(2) & 0xFF
        #if key == ord("q"):
        #    break

    client_socket.close()
    cv2.destroyAllWindows()


def comm_thread():
    comm = ""

    while True:
        if keyboard.is_pressed("up"):
            comm = "zoomin"
            print(comm)
        elif keyboard.is_pressed("down"):
            comm = "zoomout"
            print(comm)
        elif keyboard.is_pressed("left"):
            comm = "focusin"
            print(comm)
        elif keyboard.is_pressed("right"):
            comm = "focusout"
            print(comm)
        else:
            comm = ""

        client_socket.sendall(comm.encode("utf-8"))
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
    video_thread = threading.Thread(target=video_stream)
    comm_thread = threading.Thread(target=comm_thread)

    video_thread.start()
    comm_thread.start()

    video_thread.join()
    comm_thread.join()
