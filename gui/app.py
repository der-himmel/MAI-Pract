import cv2
import tkinter as tk
import threading
from PIL import Image, ImageTk
import client

root = tk.Tk()
root.title("OpenCV Video Stream with Tkinter")
W, H = 1280, 480
root.geometry(f"{W}x{H}")
root.resizable(width=False, height=False)


black_image = Image.new('RGB', (W, H), color='black')
black_photo = ImageTk.PhotoImage(black_image)
black_label = tk.Label(root)
black_label.pack()
black_label.config(image=black_photo)
black_label.image = black_photo

therm_label = tk.Label(root)
therm_label.place(relx=0.25, rely=0.5, anchor=tk.CENTER)

ipcam_label = tk.Label(root)
ipcam_label.place(relx=0.75, rely=0.5, anchor=tk.CENTER)

range_label = tk.Label(
            text= '',
            font=("Arial", 14),
            foreground="#ffffff",
            bg="black"
            )
range_label.place(relx=0.65, rely=0.5, anchor=tk.CENTER)

def gui():
    while True:
        init = cams_stream.frame_queue.get()
        frame = drawHUD(init)

        dist_string = f"RANGE: {distance_rec.dist:.1f}M"
        range_label.config(text=dist_string)

        image = Image.fromarray(frame)
        photo = ImageTk.PhotoImage(image=image)
        therm_label.config(image=photo)
        ipcam_label.config(image=photo)
        therm_label.image = photo 
        ipcam_label.image = photo
        # root.after(10, gui)  # Repeat after 10 milliseconds (adjust the delay as needed)

def focus_therm(event):
    new_relx = 0.5
    if event.num == 1:
        therm_label.place(relx=new_relx)
        ipcam_label.lower()
    elif event.num == 3:
        therm_label.place(relx=0.25)
        ipcam_label.lift()
        range_label.lift()

def focus_ipcam(event):
    new_relx = 0.5
    if event.num == 1:
        ipcam_label.place(relx=new_relx)
        therm_label.lower()
    elif event.num == 3:
        ipcam_label.place(relx=0.75)
        therm_label.lift()
        range_label.lift()

def make_invisible(img_label):
    transparent_image = Image.new('RGBA', (1, 1), (0, 0, 0, 0))
    transparent_photo = ImageTk.PhotoImage(transparent_image)
    img_label.config(image=transparent_photo)
    img_label.image = transparent_photo

def drawHUD(frame):
    height, width = frame.shape[0], frame.shape[1]
    center_xT, center_yT = width // 2, height // 2

    cv2.line(
        frame,
        (center_xT - 20, center_yT),
        (center_xT + 20, center_yT),
        (255, 255, 255),
        1,
    )
    cv2.line(
        frame,
        (center_xT, center_yT - 20),
        (center_xT, center_yT + 20),
        (255, 255, 255),
        1,
    )

    return frame

therm_label.bind("<Button-1>", focus_therm)
ipcam_label.bind("<Button-1>", focus_ipcam)
therm_label.bind("<Button-3>", focus_therm)
ipcam_label.bind("<Button-3>", focus_ipcam)



if __name__ == "__main__":
    # HOST = "192.168.2.13"
    HOST = "127.0.0.1"

    cams_stream = client.cvStreamClient(HOST)
    ipcam_comm = client.camSend(HOST)
    distance_rec = client.rangeClient(HOST)

    range_thread = threading.Thread(target=distance_rec.rangeClientConnect)
    video_thread = threading.Thread(target=cams_stream.cvStreamConnect)
    comm_thread = threading.Thread(target=ipcam_comm.camSendConnect)

    range_thread.start()
    video_thread.start()
    comm_thread.start()

    interface = threading.Thread(target=gui)
    interface.start()
    root.mainloop()
    interface.join()


    range_thread.join()
    video_thread.join()
    comm_thread.join()
