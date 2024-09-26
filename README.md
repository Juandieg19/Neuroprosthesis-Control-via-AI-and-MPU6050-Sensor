# Dataset for Prosthesis Control Using MPU6050 Sensor Data
## Overview
This dataset was created to train a neural network that classifies movements using data from the MPU6050 sensor. The dataset consists of six subsets representing three types of movements: "Apertura," "Cierre," and "Otros." The movements were recorded from three different subjects and divided into training and test sets.

This dataset was used to train and evaluate a neural network model for controlling a prosthesis based on the classification of movement data.

## Dataset Structure
The dataset is organized into the following six subsets:

Apertura.npy: Contains 100 samples of the "Apertura" movement.
Cierre.npy: Contains 100 samples of the "Cierre" movement.
Otros.npy: Contains 100 samples of movements that do not correspond to "Apertura" or "Cierre."
Apertura Test.npy: Contains 20 samples of the "Apertura" movement for testing.
Cierre Test.npy: Contains 20 samples of the "Cierre" movement for testing.
Otros Test.npy: Contains 20 samples of other movements for testing.
Total Data Summary:
Training Data:
100 samples for "Apertura."
100 samples for "Cierre."
100 samples for "Otros."
Test Data:
20 samples for "Apertura."
20 samples for "Cierre."
20 samples for "Otros."
Each sample contains a series of readings from the sensor obtained from the MPU6050 accelerometer and gyroscope.

## Data Collection Process
The data was collected from three different subjects performing the "Apertura," "Cierre," and other random movements. The subjects performed each type of movement multiple times to ensure a diverse and adequate sample set for training the model.

The MPU6050 sensor was used to collect movement data, which consists of readings from the accelerometer across 3 axes and the gyroscope across 3 axes.
Sensor data was processed to generate features such as Mean Absolute Value (MAV), Root Mean Square (RMS), and Wave Length (WL) for each axis of movement.
## Usage
Training Data:
Apertura.npy: Use this file to train the model to recognize "Apertura" movements.
Cierre.npy: Use this file to train the model to recognize "Cierre" movements.
Otros.npy: Use this file to train the model to distinguish between "Apertura/Cierre" and other movements.
Test Data:
Apertura Test.npy: Use this file to test the model's performance on "Apertura" movements.
Cierre Test.npy: Use this file to test the model's performance on "Cierre" movements.
Otros Test.npy: Use this file to test the model's performance on other movements.
## How to Load the Data
Each file in the dataset is saved in .npy format, which can be loaded using NumPy in Python.

## License
This dataset is provided for research and educational purposes. Proper attribution is required if this dataset is used in your work
