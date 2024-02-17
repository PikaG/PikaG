import tkinter as tk
from tkinter import scrolledtext, font
import serial
import threading

# Update this with your serial port and baud rate
SERIAL_PORT = '/dev/cu.SLAB_USBtoUART'  # Adjust for your device, example for macOS/Linux
BAUD_RATE = 115200

def read_serial():
    while True:
        try:
            data = ser.readline().decode('utf-8').rstrip()
            if data:
                # Ensure GUI updates happen in the main thread
                text_widget.insert(tk.END, data + '\n')
                text_widget.yview(tk.END)  # Auto-scroll
        except serial.SerialException:
            break

# Initialize serial connection outside the thread function to avoid re-initialization per thread call
ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)

# Set up the GUI window using Tkinter
root = tk.Tk()
root.title("Serial Content Display")

# Maximize window to full screen
root.state('zoomed')  # This works for Windows. For Linux, you might use root.attributes('-zoomed', True)

# Custom font for text widgets
custom_font = font.Font(size=20) 

# Create a scrolled text widget that fills the entire window
text_widget = scrolledtext.ScrolledText(root, font=custom_font)
text_widget.pack(padx=10, pady=10, fill=tk.BOTH, expand=True)  # Fill both directions and allow expansion

# Start the thread for reading serial data
thread = threading.Thread(target=read_serial)
thread.daemon = True  # Ensures thread ends when main thread ends
thread.start()

# Start the Tkinter event loop
root.mainloop()

# Ensure the serial connection is closed when the window is closed
ser.close()
