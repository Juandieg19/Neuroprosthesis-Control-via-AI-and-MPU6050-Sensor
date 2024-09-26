#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <math.h>  // Necesary for te exp() function
#include <WiFi.h>
#include <BluetoothSerial.h>

Adafruit_MPU6050 mpu; // Create de MPU6050 object

const double timeData = 1.5; // Time in seconds to collect data
double accelX = 0, accelY = 0, accelZ = 0; // Variables for acceleration data

// Variables for features used in the neural network
double mavXAxis = 0.0, mavYAxis = 0.0, mavZAxis = 0.0;
double rmsXAxis = 0.0, rmsYAxis = 0.0, rmsZAxis = 0.0;
double wlXAxis = 0.0, wlYAxis = 0.0, wlZAxis = 0.0;
double wlaXAxis = 0.0, wlaYAxis = 0.0, wlaZAxis = 0.0; // Auxiliary wave length

const double threshold = 1.6;   // Threshold for significant movements
const int ledPin = 2; // Pin for the LED

///////////////////////////////// Neural Network Variables /////////////////////////////////
double a0[9];// Input layer
// Weights for the layers
double W1[4][9] = {{-0.193,-0.456,0.26,-0.199,-0.911,-0.042,-0.372,-0.794,-0.191},{-0.04,0.051,-0.088,-0.289,-0.413,-0.078,-0.19,1.193,-0.696},{-0.071,0.29,-0.349,0.151,0.266,-0.192,0.14,-0.009,-0.091},{0.418,0.009,-0.417,0.354,-0.184,-0.272,0.252,0.017,-0.07}};
double a1[4];// First hidden layer
double W2[16][4] = {{0.0,-0.002,-0.0,0.007},{-0.262,-0.133,0.333,0.073},{-0.0,-0.006,0.185,0.015},{0.886,-0.29,0.174,0.07},{0.236,0.058,0.033,0.142},{0.139,-0.393,0.215,0.2},{-0.286,1.164,0.104,-0.258},{0.08,0.0,-0.0,0.0},{-0.0,0.029,0.19,-0.112},{0.652,0.467,0.161,-0.123},{-0.0,-0.0,-0.0,0.003},{-0.185,-0.384,0.203,0.598},{-0.0,-0.0,-0.0,0.003},{-0.289,0.258,0.315,0.14},{0.796,-0.013,-0.028,-0.023},{-0.0,-0.0,0.0,-0.001}};
double a2[16];// Second hidden layer
double W3[3][16] = {{0.0,-0.001,-0.0,-0.512,-0.0,-0.374,0.982,0.0,0.0,-0.0,0.0,-0.37,0.0,0.0,-0.013,0.0},{0.0,-0.212,0.0,0.588,0.012,0.0,-0.392,-0.0,-0.0,0.167,-0.0,-0.275,-0.0,-0.499,0.522,0.0},{-0.0,0.331,0.0,0.0,0.0,0.136,-0.44,0.0,0.0,-0.501,0.0,0.486,0.0,0.076,-0.169,-0.0}};
double a3[3]; // Output layer
// Bias for the layers
double b1[4]= {0.446,0.366,0.183,0.141};
double b2[16]= {-0.006,0.779,-0.015,0.369,-0.164,0.648,0.207,-0.139,-0.18,-0.112,-0.001,0.644,-0.038,0.709,0.004,0.041};
double b3[3]= {0.192,-0.499,0.341};
double aux = 0.0;// Auxiliary variable
//////////////////////////////////////////////////////////////////


///////////////////////////////// Neural Network Preprocessing /////////////////////////////////
// Mean and standard deviation for normalization
double mean[9]={1.844,1.033,7.941,2.212,1.176,8.121,10.044,6.155,14.587};
double dstd[9]={1.934,0.518,1.36,2.351,0.592,1.03,7.054,3.362,4.395};
///////////////////////////////////////////////////////////////////////////////////////////////////////

//////////// Pin definitions for pulse sequence ////////////
const int pin1 = 16;
const int pin2 = 4;
const int pin3 = 5;
const int pin4 = 17;
// Pulse interval and duration in microseconds
const unsigned long intervaloPulsos = 33333;  // 30 Hz (33333 microseconds)
const unsigned long duracionPulso = 200;      // Pulse duration in microseconds
// Flag for pulse sequence control
bool secuenciaPulsosActivo = true;
void setup() {
  WiFi.mode(WIFI_OFF); // Turn off WiFi
  WiFi.disconnect(true); // Disconnect WiFi
  btStop(); // Stop Bluetooth

// Configure pins as outputs
  pinMode(pin1, OUTPUT);
  pinMode(pin2, OUTPUT);
  pinMode(pin3, OUTPUT);
  pinMode(pin4, OUTPUT);
  Serial.begin(9600);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);  // Ensure the LED is off initially

  // Initialize MPU6050
  if (!mpu.begin()) {
    Serial.println("No se pudo encontrar un sensor MPU6050 válido, ¡verifique el cableado!");
    while (1);
  }
  // Configure MPU6050 sensor settings  
  mpu.setAccelerometerRange(MPU6050_RANGE_2_G);
  mpu.setGyroRange(MPU6050_RANGE_250_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

  Serial.println("MPU6050 inicializado correctamente");
}

void loop() {
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  // Read acceleration values
  double currentAccelX = a.acceleration.x;
  double currentAccelY = a.acceleration.y;
  double currentAccelZ = a.acceleration.z;

  // Check if the values exceed a threshold
  if (abs(currentAccelX - accelX) > threshold || abs(currentAccelY - accelY) > threshold || abs(currentAccelZ - accelZ) > threshold) {
    // Update the current acceleration values
    accelX = currentAccelX;
    accelY = currentAccelY;
    accelZ = currentAccelZ;

    // Turn on the LED to indicate movement is being processed
    digitalWrite(ledPin, HIGH);

    // Capture and process movement data
    featuresExtraction();   // Extract features

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
      Serial.println("Cambiando a la primera secuencia de pulsos...");
    } else if (predictedClass == 1) {
      secuenciaPulsosActivo = false;
      Serial.println("Cambiando a la segunda secuencia de pulsos...");
    }

    // Activate or deactivate pulse sequences accordingly
      if (secuenciaPulsosActivo) {
       secuenciaPulsos1(); // Activate pulse sequence 1
      } else {
       secuenciaPulsos2(); // Activate pulse sequence 2
      }
    // Turn off the LED after capturing and processing the movement
    digitalWrite(ledPin, LOW);
    
  } else{
    // Execute the appropriate pulse sequence based on the active sequence
    if (secuenciaPulsosActivo) {
      secuenciaPulsos1();// Call the first pulse sequence function
    } else {
      secuenciaPulsos2();// Call the second pulse sequence function
    }
  }
}

int processNeuralNetwork() {
  ///////////////////////////////// Neural Network Structure /////////////////////////////////
  // First layer: Multiply input by weights and apply ReLU activation
  for(int i = 0 ; i<4; i++ ) {aux=0.0;for(int j = 0 ; j <9 ; j++ ) { aux=aux+W1[i][j]*a0[j];} a1[i]=relu(aux+b1[i]);}// Apply ReLU and add bias
  // Second layer: Multiply by weights and apply ReLU activation
  for(int i = 0 ; i<16; i++ ) {aux=0.0;for(int j = 0 ; j <4 ; j++ ) { aux=aux+W2[i][j]*a1[j];} a2[i]=relu(aux+b2[i]);}// Apply ReLU and add bias
  // Third layer: Multiply by weights and apply softmax function for classification
  double aux1 = 0;
  for(int i = 0 ; i<3; i++ ) {aux=0.0;for(int j = 0 ; j <16 ; j++ ) { aux=aux+W3[i][j]*a2[j];} a3[i]=aux+b3[i]; aux1+=exp(a3[i]);}// Softmax part
  // Find the class with the highest probability
  double minimo = 0.0;
  int classes = 0;
  for(int i = 0;  i<3; i++){a3[i] = exp(a3[i])/aux1;if(a3[i]>minimo){minimo=a3[i];classes=i;}}
  //////////////////////////////////////////////////////////////////////////////////////////
  return classes;// Return the predicted class
}

void featuresExtraction() {
  int sizeSample = timeData * 10;

  double prevAccelX = 0, prevAccelY = 0, prevAccelZ = 0;
  // Reset values of MAV, RMS, WL, and auxiliary wave lengths
  mavXAxis = mavYAxis = mavZAxis = 0.0;
  rmsXAxis = rmsYAxis = rmsZAxis = 0.0;
  wlXAxis = wlYAxis = wlZAxis = 0.0;
  wlaXAxis = wlaYAxis = wlaZAxis = 0.0;  // Reiniciar valores de longitud de onda auxiliar

  for (int k = 0; k < sizeSample; k++) {
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);

    accelX = a.acceleration.x;
    accelY = a.acceleration.y;
    accelZ = a.acceleration.z;

    // MAV calculation (Mean Absolute Value)
    mavXAxis += abs(accelX);
    mavYAxis += abs(accelY);
    mavZAxis += abs(accelZ);

    // RMS calculation (Root Mean Square)
    rmsXAxis += accelX * accelX;
    rmsYAxis += accelY * accelY;
    rmsZAxis += accelZ * accelZ;

    // WL calculation (Waveform Length)
    wlXAxis += abs(accelX - wlaXAxis);
    wlYAxis += abs(accelY - wlaYAxis);
    wlZAxis += abs(accelZ - wlaZAxis);

    // Update auxiliary wave lengths
    wlaXAxis = accelX;
    wlaYAxis = accelY;
    wlaZAxis = accelZ;

    delay(100);
  }

  // Final calculations of MAV, RMS, and WL
  mavXAxis /= sizeSample;
  mavYAxis /= sizeSample;
  mavZAxis /= sizeSample;

  rmsXAxis = sqrt(rmsXAxis / sizeSample);
  rmsYAxis = sqrt(rmsYAxis / sizeSample);
  rmsZAxis = sqrt(rmsZAxis / sizeSample);
}
// ReLU function
double relu(double x) {
  return x > 0 ? x : 0;// Return x if it's positive, otherwise return 0
}

// Normalization function
double dataNormalized(double input, double mean, double dstd) {
  return (input - mean) / dstd;// Normalize input using mean and standard deviation
}
// Function to activate the first pulse sequence
void secuenciaPulsos1() {
  digitalWrite(pin1, HIGH); 
  delayMicroseconds(duracionPulso);
  digitalWrite(pin1, LOW);
  digitalWrite(pin2, HIGH);
  delayMicroseconds(duracionPulso);
  digitalWrite(pin2, LOW);
  delayMicroseconds(intervaloPulsos - 2 * duracionPulso);
  Serial.println("Abrir");
  }

// Function to activate the second pulse sequence
void secuenciaPulsos2() {
  digitalWrite(pin3, HIGH); 
  delayMicroseconds(duracionPulso);
  digitalWrite(pin3, LOW);
  digitalWrite(pin4, HIGH);
  delayMicroseconds(duracionPulso);
  digitalWrite(pin4, LOW);
  delayMicroseconds(intervaloPulsos - 2 * duracionPulso);
  Serial.println("Cerrar");
}
