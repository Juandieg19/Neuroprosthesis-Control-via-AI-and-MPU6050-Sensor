Neural Network for Prosthesis Control Using MPU6050 Sensor Data
Overview
This Python program trains a neural network to classify movement patterns (opening, closing, or others) based on data collected from the MPU6050 sensor. The neural network is trained using datasets of processed movement data, and the resulting trained model is prepared for deployment on an Arduino device.

The model consists of:

A neural network with 2 hidden layers.
An output layer that uses softmax activation to classify movements into 3 categories: "Opening," "Closing," and "Others."
L1 and L2 regularization to prevent overfitting.
The code also includes procedures for feature extraction, normalization, model training, and validation. Additionally, the necessary components are extracted to deploy the model on an embedded device (such as an Arduino) using custom functions from the arduinoKeras library.

Dataset
The input datasets used in this code are stored in .npy format:

Apertura.npy - Data for the "Opening" movement.
Cierre.npy - Data for the "Closing" movement.
Otros.npy - Data for other movements.
Corresponding test datasets are also provided: Apertura test.npy, Cierre test.npy, and Otros test.npy.
The code combines these datasets into training and testing sets.

How the Code Works
Data Loading:

The datasets are loaded from .npy files and concatenated to form the complete training and testing sets.
One-Hot Encoding:

The target labels are transformed using one-hot encoding to create categorical values for training. Movements are categorized as:
0: Opening
1: Closing
2: Others
Data Preprocessing:

Data is standardized using the StandardScaler, which ensures that the data has a mean of 0 and a standard deviation of 1. This is crucial for improving the performance of the neural network.
Neural Network Architecture:

The model consists of:
An input layer with 9 features (input size).
A hidden layer with 4 neurons and ReLU activation.
A second hidden layer with 16 neurons and ReLU activation.
An output layer with 3 neurons and softmax activation for classification.
L1 and L2 regularizers are applied to prevent overfitting.
Training the Model:

The model is compiled with:
Categorical cross-entropy as the loss function.
Adam optimizer.
Early stopping and learning rate reduction callbacks are used to improve model performance and avoid overfitting.
Model Evaluation:

After training, the model's performance is evaluated on the test dataset using accuracy metrics, and a confusion matrix is generated.
Feature Extraction for Arduino:

The trained model’s weights and scaling factors are extracted to be used in an Arduino implementation. This step prepares the model for deployment on an embedded system.
Visualization:

The loss and accuracy curves over training epochs are plotted.
The confusion matrix of the model's performance on the test data is displayed.
How to Use the Code
Prerequisites:
Install the necessary Python libraries.
Ensure that the MPU6050 sensor data is pre-processed and stored in .npy files (Apertura.npy, Cierre.npy, Otros.npy, etc.).
Ensure you have the arduinoKeras package for Arduino-specific functions.
Running the Code:
Load and Preprocess the Data: The code automatically loads the training and testing data from .npy files. Ensure these files are available in the working directory.

Train the Model: The model is trained using the processed movement data. It will automatically stop when the validation loss no longer improves, thanks to early stopping.

Evaluate the Model: After training, the model is tested on the provided test data, and results are displayed using accuracy metrics and a confusion matrix.

Extract Model Parameters: The model’s weights and normalization factors are extracted and prepared for deployment on an Arduino using the arduinoKeras functions.

Deploying to Arduino:
Once the model is trained, the extracted parameters (weights, layers, and scaling) can be used in an Arduino project to classify movement patterns in real-time.
