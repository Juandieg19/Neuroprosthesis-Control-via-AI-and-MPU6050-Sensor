#include <Wire.h> // Library for I2C communication (used by MPU6050)
#include <Adafruit_MPU6050.h> // Library for interfacing with the MPU6050 sensor
#include <Adafruit_Sensor.h> // Unified sensor library for accessing MPU6050 data

Adafruit_MPU6050 mpu; // Create an instance of the MPU6050 sensor

// Define the time for data collection in seconds
const double timeData = 1.5; // Time window to collect data (1.5 seconds)

// Variables to store acceleration values from the sensor
double accelX = 0, accelY = 0, accelZ = 0;

// Variables for extracted features (Mean Absolute Value, RMS, Waveform Length)
double mavXAxis = 0.0, mavYAxis = 0.0, mavZAxis = 0.0; // Mean Absolute Value (MAV) for each axis
double rmsXAxis = 0.0, rmsYAxis = 0.0, rmsZAxis = 0.0; // Root Mean Square (RMS) for each axis
double wlXAxis = 0.0, wlYAxis = 0.0, wlZAxis = 0.0; // Waveform Length (WL) for each axis
double wlaXAxis = 0.0, wlaYAxis = 0.0, wlaZAxis = 0.0; // Auxiliary variables for calculating WL

// Define the pin for starting the data collection (button) and an LED indicator
const int pinStart = 19; // Pin to read the button input
const int ledPin = 2; // Pin to control an LED indicator

boolean flag = false; // Flag to control when to extract features
unsigned long debounce = 0; // Variable to handle button debounce timing

void setup() {
  Serial.begin(9600); // Start serial communication at 9600 baud rate
  pinMode(pinStart, INPUT_PULLUP); // Set pinStart as an input with pull-up resistor
  pinMode(ledPin, OUTPUT); // Set ledPin as an output
  digitalWrite(ledPin, LOW); // Initialize the LED as OFF

// Initialize the MPU6050 sensor
  if (!mpu.begin()) {
    Serial.println("No se pudo encontrar un sensor MPU6050 válido, ¡verifique el cableado!"); // Error message if sensor is not detected
    while (1); // Halt the program if sensor initialization fails
  }

  // Configure the accelerometer and gyroscope settings for the MPU6050
  mpu.setAccelerometerRange(MPU6050_RANGE_2_G); // Set accelerometer range to ±2g
  mpu.setGyroRange(MPU6050_RANGE_250_DEG); // Set gyroscope range to ±250 degrees/second
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ); // Set filter bandwidth for sensor readings to 21Hz

  Serial.println("MPU6050 inicializado correctamente"); // Confirmation message for successful initialization
}

void loop() {
  // Check if the button is pressed (pinStart is LOW when pressed)
  if (!digitalRead(pinStart)) {
    pulse(); // Call pulse function to handle button press
  }

  // If the flag is set, start feature extraction
  if (flag) {
    digitalWrite(ledPin, HIGH); // Turn on the LED to indicate that data collection is in progress

    featuresExtraction(); // Call the function to extract features from the accelerometer data

    // Send extracted feature data via the serial port
    Serial.print(mavXAxis); Serial.print(",");
    Serial.print(mavYAxis); Serial.print(",");
    Serial.print(mavZAxis); Serial.print(",");
    Serial.print(rmsXAxis); Serial.print(",");
    Serial.print(rmsYAxis); Serial.print(",");
    Serial.print(rmsZAxis); Serial.print(",");
    Serial.print(wlXAxis); Serial.print(",");
    Serial.print(wlYAxis); Serial.print(",");
    Serial.print(wlZAxis); Serial.println();

    digitalWrite(ledPin, LOW); // Turn off the LED after data collection is complete
    flag = false; // Reset the flag to prevent repeated data collection until the next button press
  }
}

// Function to extract features from the MPU6050 sensor data
void featuresExtraction() {
  int sizeSample = timeData * 10; // Number of samples to collect (10 samples per second for timeData seconds)
  double prevAccelY = 0; // Variable to store the previous Y-axis acceleration value

  // Reset feature variables before calculating new values
  mavXAxis = mavYAxis = mavZAxis = 0.0;
  rmsXAxis = rmsYAxis = rmsZAxis = 0.0;
  wlXAxis = wlYAxis = wlZAxis = 0.0;
  wlaXAxis = wlaYAxis = wlaZAxis = 0.0; // Reset auxiliary waveform length values

  for (int k = 0; k < sizeSample; k++) {
    sensors_event_t a, g, temp; // Struct to hold sensor event data (acceleration, gyroscope, temperature)
    mpu.getEvent(&a, &g, &temp); // Get sensor readings (acceleration, gyroscope, temperature)
   
    // Store acceleration values for each axis
    accelX = a.acceleration.x;
    accelY = a.acceleration.y;
    accelZ = a.acceleration.z;

    // MAV (Mean Absolute Value) calculation for each axis
    mavXAxis += abs(accelX);
    mavYAxis += abs(accelY);
    mavZAxis += abs(accelZ);

    // RMS (Root Mean Square) calculation for each axis
    rmsXAxis += accelX * accelX;
    rmsYAxis += accelY * accelY;
    rmsZAxis += accelZ * accelZ;

    // WL (Waveform Length) calculation for each axis (difference between current and previous value)
    wlXAxis += abs(accelX - wlaXAxis);
    wlYAxis += abs(accelY - wlaYAxis);
    wlZAxis += abs(accelZ - wlaZAxis);

    // Update auxiliary variables for WL calculation in the next iteration
    wlaXAxis = accelX;
    wlaYAxis = accelY;
    wlaZAxis = accelZ;

    prevAccelY = accelY; // Update previous Y-axis acceleration value for future use

    delay(100); // Control sampling rate
  }

  // Calculate the average MAV for each axis
  mavXAxis /= sizeSample;
  mavYAxis /= sizeSample;
  mavZAxis /= sizeSample;

  // Calculate the final RMS value for each axis
  rmsXAxis = sqrt(rmsXAxis / sizeSample);
  rmsYAxis = sqrt(rmsYAxis / sizeSample);
  rmsZAxis = sqrt(rmsZAxis / sizeSample);
}

// Function to handle button press (debounced) and set the flag for feature extraction
void pulse() {
  if (!digitalRead(pinStart) && (millis() - debounce > 500)) { // Check for button press and ensure 500ms debounce time
    debounce = millis(); // Update debounce timer
    flag = true; // Set the flag to start feature extraction in the next loop iteration
  }
}
