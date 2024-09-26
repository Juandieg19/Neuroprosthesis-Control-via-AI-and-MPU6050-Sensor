import numpy as np
import tensorflow as tf
from tensorflow.keras.models import Sequential
from tensorflow.keras.layers import Dense, Dropout
from tensorflow.keras import regularizers
import matplotlib.pyplot as plt
from tensorflow.keras.callbacks import EarlyStopping, ReduceLROnPlateau
from sklearn.preprocessing import StandardScaler
from sklearn.metrics import confusion_matrix
import seaborn as sns
from tensorflow.keras.utils import to_categorical
from sklearn.model_selection import train_test_split

from arduinoKeras import weights
from arduinoKeras import layers
from arduinoKeras import scaling

# Import or generate datasets
sizeInput = 9

# Load datasets for training and testing
P1 = np.load('Apertura.npy')
P2 = np.load('Cierre.npy')
P3 = np.load('Otros.npy')

P_test1 = np.load('Apertura test.npy')
P_test2 = np.load('Cierre test.npy')
P_test3 = np.load('Otros test.npy')

# Training/validation and testing datasets
P_train = np.concatenate((P1, P2, P3), axis=0)
T_train = np.concatenate((0*np.ones((P1.shape[0],1)), 1*np.ones((P2.shape[0],1)), 2*np.ones((P3.shape[0],1))), axis=0)

P_test = np.concatenate((P_test1, P_test2, P_test3), axis=0)
T_test = np.concatenate((0*np.ones((P_test1.shape[0],1)), 1*np.ones((P_test2.shape[0],1)), 2*np.ones((P_test3.shape[0],1))), axis=0)

# One-hot encoding of labels
T_train = to_categorical(T_train, 3)
T_test = to_categorical(T_test, 3)

# Data preprocessing
scaler = StandardScaler().fit(P_train)
P_train = scaler.transform(P_train)
P_test = scaler.transform(P_test)

# Split the dataset into training and validation sets
P_train, P_val, T_train, T_val = train_test_split(P_train, T_train, test_size=0.2, random_state=42)

# Set algorithm hyperparameters (learning rate, epochs)
epochs = 1000

# Define the neural network architecture
model = Sequential()
model.add(Dense(4, activation='relu', input_dim=sizeInput, 
                 kernel_regularizer=regularizers.l2(0.01)))
model.add(Dense(16, activation='relu', 
                 kernel_regularizer=regularizers.l2(0.01)))
model.add(Dense(3, activation='softmax', 
                 kernel_regularizer=regularizers.l1_l2(l1=0.01, l2=0.01)))  # L1 and L2 regularizer in the output layer

model.summary()

# Declare the loss function
loss = 'categorical_crossentropy'

# Optimizer
optimizer = tf.keras.optimizers.Adam()

# Callbacks
early_stopping = EarlyStopping(monitor='val_loss', patience=1, restore_best_weights=True)
reduce_lr = ReduceLROnPlateau(monitor='val_loss', factor=0.5, patience=10, min_lr=1e-6)

# Compile the model
model.compile(loss=loss, optimizer=optimizer, metrics=['accuracy'])

# Train the model with validation data
history = model.fit(P_train, T_train, epochs=epochs, verbose=1, validation_data=(P_val, T_val), callbacks=[early_stopping, reduce_lr])

# Plot the loss over epochs
plt.subplot(1, 2, 1)
plt.plot(history.epoch, np.array(history.history['loss']), 'b', label='Train Loss')
plt.plot(history.epoch, np.array(history.history['val_loss']), 'r', label='Validation Loss')
plt.xlabel('Épocas')  # Epochs
plt.ylabel('Pérdida')  # Loss
plt.title('Pérdida')  # Loss
plt.legend()
plt.grid()

# Plot the accuracy over epochs
plt.subplot(1, 2, 2)
plt.plot(history.epoch, np.array(history.history['accuracy']), 'b', label='Train Accuracy')
plt.plot(history.epoch, np.array(history.history['val_accuracy']), 'r', label='Validation Accuracy')
plt.xlabel('Épocas')  # Epochs
plt.ylabel('Precisión')  # Accuracy
plt.title('Precisión')  # Accuracy
plt.legend()
plt.grid()

plt.tight_layout()

# Evaluate the model on the test set
T_test_pred = model.predict(P_test)
T_test_pred = np.argmax(T_test_pred, axis=1)
T_test_true = np.argmax(T_test, axis=1)

# Generate the confusion matrix
cm = confusion_matrix(T_test_true, T_test_pred)
plt.figure(figsize=(10,7))
sns.heatmap(cm, annot=True, fmt='d', cmap='Blues', xticklabels=['Clase 0', 'Clase 1', 'Clase 2'], yticklabels=['Clase 0', 'Clase 1', 'Clase 2'])
plt.xlabel('Predicción')  # Prediction
plt.ylabel('Etiqueta Verdadera')  # True Label
plt.title('Matriz de Confusión')  # Confusion Matrix

# Extract weights, scaling, and network architecture values for Arduino
weights(model.layers, 3)
scaling(scaler, 3)
layers(model.layers)

plt.show()
