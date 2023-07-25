import socket
import cv2
import numpy as np

server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server.bind(('192.168.2.13', 2020))
server.listen()

while True:
    print("Waiting for a client to connect...")
    client_socket, client_address = server.accept()
    print("Connected to client:", client_address)

    file = open('server_image.jpg', "wb")
    image_chunk = client_socket.recv(2048)

    while image_chunk:
        file.write(image_chunk)
        image_chunk = client_socket.recv(2048)

    file.close()
    client_socket.close()

    # Read the received image using OpenCV
    received_image = cv2.imread('server_image.jpg')

    # Check if the image was read successfully
    if received_image is not None:
        # Display the received image
        cv2.imshow('Received Image', received_image)
        cv2.waitKey(0)
        cv2.destroyAllWindows()
    else:
        print("Failed to read the received image.")
