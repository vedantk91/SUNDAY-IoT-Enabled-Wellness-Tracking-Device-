# SunDay-Wellness-Tracker

SunDay Wellness Tracker is a comprehensive health monitoring device based on the Atmega328P MCU. It measures UV exposure, differentiates between artificial and natural light exposure for the user, and assesses air quality. The real-time data captured by our advanced sensors is transmitted to a user-friendly Flutter-based mobile application via Bluetooth for drawing inferences from the data. This app not only displays the data but also stores it in the Firebase Cloud Realtime Database for long-term analysis and health tracking.

#### SunDay-Wellness-Tracker
![SunDay-Wellness-Tracker](images/image2.png)

#### SunDay-Wellness-Tracker with 3D printed enclosure
![ SunDay-Wellness-Tracker with 3D printed enclosure](images/image3.png)

## Development Procedure

#### Development Procedure Flowchart
![Development Procedure Flowchart](images/image1.png)

### Firmware Development and System Integration:
- Utilized the ATmega328P microcontroller, interfacing it with HC05 Bluetooth Sensor over UART and GUVA S12-SD sensor over ADC.
- Wrote custom drivers for I2C communication with the ENS160 MOX Gas sensor and the AS7262 Light Sensor.
- Integrated a TFT LCD via SPI to display environmental data in real-time.
- Implemented a buzzer in the device to indicate the user when the AQI went over 3, based on timer interrupts.
- Devised a firmware for synchronizing tasks of the device and smooth wireless operation with the mobile device over Bluetooth.
- Assembled all components on a perfboard, ensuring effective system functionality.

The Firmware written for the device can be found in Sunday Firmware Folder.

#### Device Firmware Flowchart
![Development Procedure Flowchart](images/image9.png)

- LCD and UART Initialization: Sets up the LCD display for data visualization and initializes UART for serial communication.
- ADC Initialization: Configures the Analog-to-Digital Converter for reading UV sensor data. Sensor Initialization: Initializes the air quality and light sensors (ENS160 MOX and AS7262).
- Button Configuration: Sets up a button for user interaction to switch between different data displays.
- Timer Setup: Configures Timer1 for periodic interrupts, used for managing AQI alerts. Main Loop:
- Sensor Data Acquisition:
Reads UV Index from the ADC.
Retrieves calibrated light values from the AS7262 sensor. Reads the air quality index (AQI) from the ENS160 MOX sensor.
- Light Sample Analysis: Analyzes the light data to determine whether the source is natural or artificial, updating respective scores.
After every 10 samples, calculates the exposure percentages for natural and artificial light.
- Display Management: Checks the button state to switch between AQI, Light Exposure, and UV Index displays.Updates the LCD screen accordingly with relevant data visualizations.
- UART Communication: Periodically formats and sends a data string via UART containing AQI, UV Index, and light exposure information.
- Timer Interrupt Routine (ISR): Activates the buzzer if the AQI exceeds a threshold of 3, indicating poor air quality. Deactivates the buzzer when AQI falls below the threshold.
- Button Press Handling: Toggles between AQI, Light Exposure, and UV Index modes upon button presses. Updates the display based on the selected mode. Data Display Modes:
- AQI Mode: Shows current AQI levels and trends on the LCD.
- Light Exposure Mode: Displays percentages of natural and artificial light exposure. UV Index Mode: Visualizes UV Index readings over time.
- Data Processing and Transmission: Processes sensor readings and user inputs. Formats and transmits the data through UART external communication over bluetooth using HC05.

### Mobile Application and Cloud Integration:
- Developed a mobile application using Flutter for intuitive data visualization and interaction.
- Leveraged Firebase for robust cloud-based data management and security.
- Crafted a visually appealing interface to represent sensor data and health insights effectively.
- Focused on enhancing user experience by providing intuitive navigation and actionable insights.

The  Flutter Application written for the device can be found in the Sunday Flutter Application Folder.
#### Mobile Application Flowchart
![Mobile Application Flowchart](images/image10.png)
- Main.dart:
Initializes the Flutter app and Firebase. Sets up routes for different pages (like WelcomePage, HomePage, etc.). Uses Get.put to instantiate a BluetoothController for managing Bluetooth functionalities.
- Dashboard Page:
Presents a grid of options (like Settings, Connections, Scan, etc.) for navigating through the app. Each option in the grid is a DashboardOption widget that navigates to the corresponding route or page when tapped.
- Bluetooth Controller:
Manages Bluetooth operations, like scanning for devices, connecting to a device, and handling data received over Bluetooth. Utilizes the flutter_bluetooth_serial package for Bluetooth operations. Integrates Firebase Realtime Database for storing received data.
- Connection Page:
Displays a list of Bluetooth devices available for connection. Allows the user to initiate a connection to a selected device.
- UV Display Page:
Fetches and displays UV index data from Firebase in a chart format using the fl_chart package. The page processes and visualizes UV data to provide insights on UV exposure.
- Light Exposure Page:
Handles the visualization of light exposure data (natural vs. artificial light). Utilizes pie charts to depict the proportion of natural and artificial light exposure based on the data fetched from Firebase.
- AQI Display Page:
Shows the Air Quality Index (AQI) data in a line chart format. Includes functionalities to fetch AQI data from Firebase and display it along with time-based warnings for high AQI values.
- General Workflow:
The app starts with the main.dart, which sets up the necessary controllers and routes. The dashboard provides navigation to various functionalities like Bluetooth connections, data displays, and settings. Bluetooth Controller manages the connection and data reception from an external device. Data received from the device is stored in Firebase and displayed in different formats (charts, graphs) on respective pages.

#### Mobile Application Interface

<p float="left">
  <img src="images/image8.png" width="48%" />
  <img src="images/image4.png" width="48%" /> 
</p>

<p float="left">
  <img src="images/image5.png" width="48%" />
  <img src="images/image6.png" width="48%" /> 
</p>

#### Firebase Cloud Data Real-time Database
![Firebase Cloud Data Real-time Database](images/image7.png)


## Conclusion

The SunDay Wellness Tracker represents a significant leap in personal health monitoring, merging advanced sensor technology with user-centric software solutions. By harnessing the power of the Atmega328P MCU and integrating it seamlessly with a Flutter-based mobile application, the project sets a new standard in environmental health awareness. The real-time monitoring and analysis of UV exposure, light sources, and air quality, presented through an intuitive and interactive interface, empowers users with actionable insights into their daily wellness routines. This project not only serves as a benchmark in wearable health technology but also opens avenues for future enhancements in personalized healthcare and wellness tracking.
