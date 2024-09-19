#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <math.h>  // Necessary for the exp() function
#include <WiFi.h>
#include <BluetoothSerial.h>

Adafruit_MPU6050 mpu; // Create the MPU6050 object

const double timeData = 1.5; // Time in seconds to collect data
double accelX = 0, accelY = 0, accelZ = 0; // Variables for acceleration data

double mavXAxis = 0.0, mavYAxis = 0.0, mavZAxis = 0.0;
double rmsXAxis = 0.0, rmsYAxis = 0.0, rmsZAxis = 0.0;
double wlXAxis = 0.0, wlYAxis = 0.0, wlZAxis = 0.0;
double wlaXAxis = 0.0, wlaYAxis = 0.0, wlaZAxis = 0.0; // Auxiliary wavelength

const double threshold = 1.6;  // Threshold for significant movements
const int ledPin = 2;  // Pin for the LED

///////////////////////////////// Neural Network Variables /////////////////////////////////
double a0[9];
double W1[4][9] = {{-0.193,-0.456,0.26,-0.199,-0.911,-0.042,-0.372,-0.794,-0.191},{-0.04,0.051,-0.088,-0.289,-0.413,-0.078,-0.19,1.193,-0.696},{-0.071,0.29,-0.349,0.151,0.266,-0.192,0.14,-0.009,-0.091},{0.418,0.009,-0.417,0.354,-0.184,-0.272,0.252,0.017,-0.07}};
double a1[4];
double W2[16][4] = {{0.0,-0.002,-0.0,0.007},{-0.262,-0.133,0.333,0.073},{-0.0,-0.006,0.185,0.015},{0.886,-0.29,0.174,0.07},{0.236,0.058,0.033,0.142},{0.139,-0.393,0.215,0.2},{-0.286,1.164,0.104,-0.258},{0.08,0.0,-0.0,0.0},{-0.0,0.029,0.19,-0.112},{0.652,0.467,0.161,-0.123},{-0.0,-0.0,-0.0,0.003},{-0.185,-0.384,0.203,0.598},{-0.0,-0.0,-0.0,0.003},{-0.289,0.258,0.315,0.14},{0.796,-0.013,-0.028,-0.023},{-0.0,-0.0,0.0,-0.001}};
double a2[16];
double W3[3][16] = {{0.0,-0.001,-0.0,-0.512,-0.0,-0.374,0.982,0.0,0.0,-0.0,0.0,-0.37,0.0,0.0,-0.013,0.0},{0.0,-0.212,0.0,0.588,0.012,0.0,-0.392,-0.0,-0.0,0.167,-0.0,-0.275,-0.0,-0.499,0.522,0.0},{-0.0,0.331,0.0,0.0,0.0,0.136,-0.44,0.0,0.0,-0.501,0.0,0.486,0.0,0.076,-0.169,-0.0}};
double a3[3]; 
double b1[4]= {0.446,0.366,0.183,0.141};
double b2[16]= {-0.006,0.779,-0.015,0.369,-0.164,0.648,0.207,-0.139,-0.18,-0.112,-0.001,0.644,-0.038,0.709,0.004,0.041};
double b3[3]= {0.192,-0.499,0.341};
double aux = 0.0;
//////////////////////////////////////////////////////////////////

///////////////////////////////// Neural Network Preprocessing /////////////////////////////////
double mean[9]={1.844,1.033,7.941,2.212,1.176,8.121,10.044,6.155,14.587};
double dstd[9]={1.934,0.518,1.36,2.351,0.592,1.03,7.054,3.362,4.395};
/////////////////////////////////////////////////////////////////////////////////////////////////

////////////
const int pin1 = 16;
const int pin2 = 4;
const int pin3 = 5;
const int pin4 = 17;

// Pulse interval in microseconds
const unsigned long intervaloPulsos = 33333;  // 30 Hz (33333 microseconds)
const unsigned long duracionPulso = 200;      // Pulse duration in microseconds
bool secuenciaPulsosActivo = true;

void setup() {
  WiFi.mode(WIFI_OFF); // Turn off WiFi
  WiFi.disconnect(true); // Disconnect WiFi
  btStop(); // Stop Bluetooth

  // Configure pins as input or output
  pinMode(pin1, OUTPUT);
  pinMode(pin2, OUTPUT);
  pinMode(pin3, OUTPUT);
  pinMode(pin4, OUTPUT);
  Serial.begin(9600); // Initialize serial communication
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);  // Ensure the LED is off initially

  // Initialize MPU6050
  if (!mpu.begin()) {
    Serial.println("Could not find a valid MPU6050 sensor, check wiring!");
    while (1); // Stay here if MPU6050 initialization fails
  }
  mpu.setAccelerometerRange(MPU6050_RANGE_2_G);
  mpu.setGyroRange(MPU6050_RANGE_250_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

  Serial.println("MPU6050 initialized successfully");
}

void loop() {
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  // Read acceleration values
  double currentAccelX = a.acceleration.x;
  double currentAccelY = a.acceleration.y;
  double currentAccelZ = a.acceleration.z;

  // Check if values exceed a threshold
  if (abs(currentAccelX - accelX) > threshold || abs(currentAccelY - accelY) > threshold || abs(currentAccelZ - accelZ) > threshold) {
    // Update current acceleration values
    accelX = currentAccelX;
    accelY = currentAccelY;
    accelZ = currentAccelZ;

    // Activate the LED to indicate motion is being processed
    digitalWrite(ledPin, HIGH);

    // Capture and process motion data
    featuresExtraction();  // Extract features

    // Normalize the features
    a0[0] = dataNormalized(mavXAxis, mean[0], dstd[0]);
    a0[1] = dataNormalized(mavYAxis, mean[1], dstd[1]);
    a0[2] = dataNormalized(mavZAxis, mean[2], dstd[2]);

    a0[3] = dataNormalized(rmsXAxis, mean[3], dstd[3]);
    a0[4] = dataNormalized(rmsYAxis, mean[4], dstd[4]);
    a0[5] = dataNormalized(rmsZAxis, mean[5], dstd[5]);

    a0[6] = dataNormalized(wlXAxis, mean[6], dstd[6]);
    a0[7] = dataNormalized(wlYAxis, mean[7], dstd[7]);
    a0[8] = dataNormalized(wlZAxis, mean[8], dstd[8]);

    // Process the neural network
    int predictedClass = processNeuralNetwork();

    // Change the pulse sequence based on the predicted class
    if (predictedClass == 0) {
      secuenciaPulsosActivo = true;
      Serial.println("Switching to the first pulse sequence...");
    } else if (predictedClass == 1) {
      secuenciaPulsosActivo = false;
      Serial.println("Switching to the second pulse sequence...");
    }

    // Call the corresponding pulse sequence
    if (secuenciaPulsosActivo) {
      secuenciaPulsos1();
    } else {
      secuenciaPulsos2();
    }

    // Turn off the LED after processing the motion
    digitalWrite(ledPin, LOW);
    
  } else {
    // Execute the pulse sequence continuously
    if (secuenciaPulsosActivo) {
      secuenciaPulsos1();
    } else {
      secuenciaPulsos2();
    }
  }
}

double dataNormalized(double data, double mean, double std) {
  return (data - mean) / std;
}

double sigmoid(double x) {
  return 1.0 / (1.0 + exp(-x));
}

double softmax(double* x, int idx, int len) {
  double sumExp = 0.0;
  for (int i = 0; i < len; i++) {
    sumExp += exp(x[i]);
  }
  return exp(x[idx]) / sumExp;
}

int processNeuralNetwork() {
  // Process the first layer
  for (int i = 0; i < 4; i++) {
    a1[i] = 0.0;
    for (int j = 0; j < 9; j++) {
      a1[i] += W1[i][j] * a0[j];
    }
    a1[i] += b1[i];  // Add bias
    a1[i] = sigmoid(a1[i]);  // Apply activation function
  }

  // Process the second layer
  for (int i = 0; i < 16; i++) {
    a2[i] = 0.0;
    for (int j = 0; j < 4; j++) {
      a2[i] += W2[i][j] * a1[j];
    }
    a2[i] += b2[i];  // Add bias
    a2[i] = sigmoid(a2[i]);  // Apply activation function
  }

  // Process the third layer
  for (int i = 0; i < 3; i++) {
    a3[i] = 0.0;
    for (int j = 0; j < 16; j++) {
      a3[i] += W3[i][j] * a2[j];
    }
    a3[i] += b3[i];  // Add bias
  }

  // Apply softmax to obtain probabilities
  double probClass0 = softmax(a3, 0, 3);
  double probClass1 = softmax(a3, 1, 3);
  double probClass2 = softmax(a3, 2, 3);

  // Return the class with the highest probability
  if (probClass0 >= probClass1 && probClass0 >= probClass2) {
    return 0;
  } else if (probClass1 >= probClass0 && probClass1 >= probClass2) {
    return 1;
  } else {
    return 2;
  }
}

void secuenciaPulsos1() {
  digitalWrite(pin1, HIGH);
  delayMicroseconds(duracionPulso);
  digitalWrite(pin1, LOW);
  delayMicroseconds(intervaloPulsos - duracionPulso);

  digitalWrite(pin2, HIGH);
  delayMicroseconds(duracionPulso);
  digitalWrite(pin2, LOW);
  delayMicroseconds(intervaloPulsos - duracionPulso);
}

void secuenciaPulsos2() {
  digitalWrite(pin3, HIGH);
  delayMicroseconds(duracionPulso);
  digitalWrite(pin3, LOW);
  delayMicroseconds(intervaloPulsos - duracionPulso);

  digitalWrite(pin4, HIGH);
  delayMicroseconds(duracionPulso);
  digitalWrite(pin4, LOW);
  delayMicroseconds(intervaloPulsos - duracionPulso);
}
