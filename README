Neuroprosthesis Control with MPU6050 and Neural Network

This project uses the MPU6050 accelerometer and gyroscope sensor and a neural network to control pulse sequences for neuroprosthesis activation. The system reads motion data from the MPU6050 sensor, extracts relevant features, and processes them through a trained neural network model. Depending on the predicted class from the neural network, it triggers different pulse sequences to control an external device (e.g., a neuroprosthesis) through digital pins.

Hardware Requirements

- ESP32 microcontroller
- MPU6050 sensor (connected via I2C)
- External device or neuroprosthesis connected to digital pins (Pin 16, Pin 4, Pin 5, and Pin 17)
- LED (optional for visual feedback, connected to Pin 2)

Features

- Motion sensing: The MPU6050 reads accelerometer data to detect motion.
- Feature extraction: Mean absolute value (MAV), root mean square (RMS), and wavelength are calculated from the motion data.
- Neural network inference: A simple neural network processes the features and classifies the motion into one of three classes.
- Pulse sequences: Based on the classification, different pulse sequences are activated to control a connected neuroprosthesis.

Setup Instructions

1. Connect the MPU6050 Sensor to the ESP32

Ensure the following connections:

- VCC → 3.3V (ESP32)
- GND → GND (ESP32)
- SDA → GPIO 21 (ESP32)
- SCL → GPIO 22 (ESP32)

2. Connect External Devices to Digital Pins

- Connect external actuators or devices to the following pins:
  - Pin 16
  - Pin 4
  - Pin 5
  - Pin 17
- Connect an optional LED to Pin 2 for motion detection feedback.

3. Code Compilation and Upload

1. Install the required libraries:
   - Adafruit MPU6050 Library
   - Adafruit Sensor Library

   To install the libraries:
   - Open the Arduino IDE.
   - Go to Sketch > Include Library > Manage Libraries.
   - Search for "Adafruit MPU6050" and "Adafruit Unified Sensor" and install them.

2. Configure the Arduino IDE for ESP32:
   - Go to File > Preferences, and in the Additional Boards Manager URLs field, add:  
     https://dl.espressif.com/dl/package_esp32_index.json
   - Go to Tools > Board > Boards Manager, search for "ESP32", and install the ESP32 boards.

3. Upload the Code:
   - Open the provided .ino file in Arduino IDE.
   - Select the appropriate ESP32 board from Tools > Board.
   - Select the correct port under Tools > Port.
   - Upload the code to your ESP32.

4. Neural Network Model

This project uses a simple neural network model with the following layers:

- Input layer: 9 features (extracted from accelerometer data).
- Hidden layers: Two hidden layers.
- Output layer: 3 classes representing different types of motion.

The model weights and biases are pre-trained and hardcoded in the code. The network predicts motion types in real time and adjusts the pulse sequences accordingly.

5. Running the Code

Once uploaded, the code will perform the following steps:

1. Initialize the MPU6050 sensor. If the sensor cannot be found, the program will stop.
2. Read accelerometer data. The sensor continuously reads acceleration on the X, Y, and Z axes.
3. Detect motion: If motion is detected (i.e., if the acceleration exceeds a certain threshold), the system extracts features from the motion data.
4. Classify motion: The extracted features are passed to the neural network to classify the type of motion into one of three classes.
5. Activate pulse sequences: Based on the predicted class:
   - Class 0: Pulse sequence 1 is triggered.
   - Class 1: Pulse sequence 2 is triggered.
   - Class 2: Reserved for future functionality.

The LED will turn on during motion detection and pulse sequence activation.

