# Temperature automation program using an ESP32, 5v relay, motor driver l298n and IOT platform Ubidots

## Description

This project implements a temperature control system using an ESP32 microcontroller, L298N motor driver, 5V relay, and sensors. The system aims to maintain a specific temperature range (30 to 32 degrees Celsius) in industrial environments. It utilizes IoT capabilities with Ubidots for remote monitoring and control.

## Materials

- ESP32 microcontroller
- L298N motor driver
- 5V relay module
- DC motor (for demonstration purposes)
- DHT11 temperature and humidity sensor
- Breadboard and jumper wires
- Power supply for the ESP32 and motor

## Libraries Used

### Ubidots for ESP32
- **Installation:**
    1. Download the [esp32-mqtt-main.zip](esp32-mqtt-main.zip) file from the repository.
    2. In the Arduino IDE, go to File > Preferences, and add the path to the folder containing the downloaded ZIP file to the "Sketchbook location."
    3. Go to Sketch > Include Library > Add .ZIP Library and select the downloaded file.

### PubSubClient
- **Installation:**
    1. Download the [pubsubclient-master.zip](pubsubclient-master.zip) file from the repository.
    2. In the Arduino IDE, go to File > Preferences, and add the path to the folder containing the downloaded ZIP file to the "Sketchbook location."
    3. Go to Sketch > Include Library > Add .ZIP Library and select the downloaded file.

### DHT Sensor Library
- **Installation:**
    - In the Arduino IDE, go to Sketch > Include Library > Manage Libraries.
    - Search for "DHT sensor library" and install the version by Adafruit.

## Wiring Diagram
<span>![</span><span>Circuit Diagram</span><span>]</span><span>(</span><span>https://github.com/BryanGaray99/esp32-ubidots-temperature-control/blob/main/control-diagrama.png</span><span>)</span>

## Functionality

1. **Temperature and Humidity Monitoring:**
   - The DHT11 sensor reads ambient temperature and humidity.
   - Data is sent to Ubidots for remote monitoring.

2. **Automatic Temperature Control:**
   - If the temperature falls below 28 degrees Celsius, the light is turned on.
   - If the temperature exceeds 30 degrees Celsius, the motor is activated.
   - Both the light and motor are turned off when the temperature is within the acceptable range.

3. **Manual Mode:**
   - The system supports a manual mode controlled by a variable from Ubidots.
   - When manual mode is activated, the automatic temperature control is overridden.
   - Manual control allows users to turn on/off the light and motor as needed.

## Ubidots Configuration

1. Create an account on [Ubidots](https://ubidots.com/).
2. Create a new device and variables for temperature, humidity, motor, light-bulb and manual-mode.
3. Obtain the Ubidots token and update the code.

## Setup and Configuration

1. Clone this repository.
2. Install the required libraries manually using the provided ZIP files. Ensure the project folder is added to the Arduino IDE preferences under "Sketchbook location."
3. Wire the components according to the provided diagram.
4. Do not forget to put your WIFI connection SSID and Password and your Ubidots Tokens and exact labels. 
5. Upload the code to the ESP32 using the Arduino IDE.

## Contributions

Contributions are welcome! If you find issues or have suggestions, feel free to open an issue or submit a pull request.

## Contact

For any inquiries, please contact the project maintainer: 
- Video Presentation of the project: [https://www.youtube.com/watch?v=9m3REcRIBwo]
- Bryan Enrique Garay Benavidez
- [bryangarayacademico@gmail.com]