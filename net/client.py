import socket
import cv2
import numpy as np
import struct

client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
HOST = '127.0.0.1'
PORT = 11111
client_socket.connect((HOST, PORT))

data = b""
payload_size = struct.calcsize("Q")

while True:
    while len(data) < payload_size:
        packet = client_socket.recv(4*1024)
        if not packet: break
        data += packet

    packed_msg_size = data[:payload_size]
    data = data[payload_size:]
    msg_size = struct.unpack("Q", packed_msg_size)[0]

    while len(data) < msg_size:
        data += client_socket.recv(4*1024)

    frame_data = data[:msg_size]
    data = data[msg_size:]

    # Convert the received data back to a numpy array (frame)
    nparr = np.frombuffer(frame_data, np.uint8)

    # Decode the frame
    frame = cv2.imdecode(nparr, cv2.IMREAD_COLOR)
    print("getting frames...")

    cv2.imshow("OUTPUT", frame)
    key = cv2.waitKey(1) & 0xFF
    if key == ord('q'):
        break

client_socket.close()
cv2.destroyAllWindows()
