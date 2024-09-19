**Using ESP32 Arduino and Python Scripts Together**

Overview:
This repository includes an Arduino script for ESP32 interfacing with an MPU6050 sensor to collect acceleration data. Additionally, a Python script receives this data via serial communication, processes it, and visualizes the results using bar charts.

Files:
- ESP32_Arduino_Script.ino: Arduino script for ESP32 interfacing with MPU6050 and sending sensor data over serial.
- Python_Script.py: Python script for receiving sensor data from ESP32 via serial, storing it in a NumPy array, and visualizing feature data.

Setup:

ESP32 Arduino Setup:
1. Install necessary libraries: Wire.h, Adafruit_MPU6050.h, Adafruit_Sensor.h.
2. Connect MPU6050 to ESP32 according to wiring diagram.
3. Upload ESP32_Arduino_Script.ino to ESP32 using Arduino IDE.
4. Verify serial output to ensure correct sensor data readings.

Python Setup:
1. Install Python (recommended version 3.x) and required packages: numpy, pyserial, matplotlib.
2. Connect ESP32 to computer via USB.
3. Open Python_Script.py in a Python IDE or text editor.
4. Modify COM port (e.g., 'COM3', '/dev/ttyUSB0') in Python script to match ESP32 serial port.
5. Run Python_Script.py to establish serial communication and collect sensor data.
6. Check console for data collection progress and confirm successful reception of data.
7. View generated bar charts for visual analysis of collected data.

Functionality:
- ESP32 Arduino collects accelerometer data from MPU6050 and sends it over serial.
- Python script reads serial data, parses it into features, stores in a NumPy array, and saves to a .npy file.
- Each feature's data is visualized using matplotlib, generating bar charts for each feature.

Usage:
1. Connect ESP32 to MPU6050 and external devices as per wiring diagram.
2. Upload Arduino script to ESP32 and ensure it operates correctly with serial output.
3. Run Python script on computer to receive and process sensor data from ESP32.
4. Adjust Examples in Python script for desired number of data samples to collect.
5. Monitor console output for data reception updates and error handling messages.
6. Visualize feature data using matplotlib plots to analyze collected accelerometer data.

Notes:
- Ensure correct wiring between ESP32, MPU6050, and external devices to avoid connection issues.
- Customize matplotlib plots within Python script to meet specific visualization requirements.
- Modify serial port (COM) and baud rate (9600) settings in Python script according to ESP32 configuration.
