##### Import necesary libraries for the program's execution #######################################################
import time # Provides time control functions like delays and sleep
import serial # Library for serial communication with devices
import numpy as np # Library for array manipulations and mathematical operations
import matplotlib.pyplot as plt # Library for plotting and data visualization
###################################################################################################################

##### Define the number of  samples to be collected from the serial device#########################################
Examples = 20
###################################################################################################################

#####  Define the serial communication port and number of features to be received #################################
COM = 'COM3' # Serial port where the ESP32 is connected 
numFeatures = 9  # Number of features or data columns to be received per sample
###################################################################################################################

##### Attempt to establish a connection with the specified serial port ############################################
try:
    arduinoSerial = serial.Serial(COM, 9600) # Open serial communication at a baud rate of 9600
    print("Conexión establecida con el puerto", COM) # Confirm successful connection 
except Exception as e:
    print(f"No se pudo establecer la conexión en el puerto {COM}. Error: {e}") # Handle connection errors
    exit() # Terminate the program if connection fails
###################################################################################################################

##### Initialize a NumPy array to store the collected data ########################################################
P = np.zeros((Examples, numFeatures)) # Create a 2D array of shape (Examples, numFeatures) filled with zeros
###################################################################################################################

##### Wait 3 seconds to ensure the Arduino is ready for communication 
time.sleep(3) # Delay to allow time for the ESP32 to initialize 
print('Recolectando Datos') # # Indicate that data collection is starting 
###################################################################################################################

##### Start collecting data from the ESP32 ########################################################################
with arduinoSerial:
    for k in range(Examples): # Loop through the number of examples to be collected
        try:
            line = arduinoSerial.readline().decode('utf-8').strip() # Read and decode a line from the serial input
            data = line.split(",") # Split the received string into individual data points
            if len(data) == numFeatures: # Check if the number of received features matches the expected count
                P[k] = [float(val) for val in data] # Convert each data point to a float and store in array P
            else:
                print(f"Error: Datos incompletos recibidos en el ejemplo {k+1}.") # Warn if data is incomplete
        except ValueError as e:
            print(f"Error al convertir los datos en el ejemplo {k+1}: {e}") # Handle conversion errors
        except Exception as e:
            print(f"Ocurrió un error inesperado en el ejemplo {k+1}: {e}") # Handle any other unexpected errors
        print(f"Ejemplo {k+1} recolectado") # Confirm successful collection of each sample
###################################################################################################################

##### Once all samples are collected, save the data to a .npy file ################################################
print('Datos Recolectados') # Indicate that all data has been successfully collected
np.save('Otros test.npy', P) # Save the NumPy array P to a file for future use
###################################################################################################################

##### Plot each feature from the collected data ###################################################################
for i in range(numFeatures): # Loop through each feature
    fig = plt.figure() # Create a new figure for each feature 
    plt.bar(np.arange(0, Examples), P[:, i], label='Feature' + str(i + 1)) # Plot a bar chart of the feature values
    plt.legend(loc="upper left") # Add a legend to the plot
    plt.grid() # Display a grid on the plot
plt.show()# Display all the generated plots
###################################################################################################################

