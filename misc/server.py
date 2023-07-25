import socket
import cv2
import numpy as np

server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server.bind(('localhost', 2020))
server.listen()

print("Waiting for a client to connect...")
client_socket, client_address = server.accept()
print("Connected to client:", client_address)


while True:
    file = open('C:/Users/alesh/Pictures/cvtest/frameREC.png', "wb")
    image_chunk = client_socket.recv(2048)

    while image_chunk:
        file.write(image_chunk)
        image_chunk = client_socket.recv(2048)

    # Read the received image using OpenCV
    received_image = cv2.imread('C:/Users/alesh/Pictures/cvtest/frameREC.png')

    # Check if the image was read successfully
    if received_image is not None:
        # Display the received image
        print("Image received successfully")
        cv2.imshow('Received Image', received_image)
        cv2.waitKey(1)
        # cv2.destroyAllWindows()
    else:
        print("Failed to read the received image.")
