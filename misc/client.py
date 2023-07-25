import socket
import time

client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
client.connect(('localhost', 2020))  # Connect to the server
print("Connected to the server.")

while True:
    try:
        file = open('C:/Users/alesh/Pictures/cvtest/frame.png', 'rb')
        image_data = file.read(2048)

        while image_data:
            client.send(image_data)
            image_data = file.read(2048)

        file.close()
        
        print("Image sent successfully.")
    except ConnectionRefusedError:
        print("Connection to the server refused. Retrying in 5 seconds...")
        time.sleep(5)  # Wait for 5 seconds before retrying
    except FileNotFoundError:
        print("File 'image.jpg' not found. Please check the file path.")
        break  # Exit the loop if the file is not found
    except Exception as e:
        print("An error occurred:", str(e))
        break  # Exit the loop if any other error occurs
