# Smart Thermostat with ESP32, DHT11, and DC Motor with custom API
This Smart Thermostat allows you to turn a basic smart heating thermometer into your individual one, which allows you to communicate with every API you want, for maximum compatibilty with your system.
This project is an open-source smart thermostat built using an ESP32 microcontroller, a DHT11 temperature and humidity sensor, and a modified DC motor-controlled valve. The thermostat communicates with ThingSpeak to fetch user-defined temperature values via TalkBack and adjusts the heating system accordingly.

## Features
- Reads temperature and humidity from a DHT11 sensor.
- Controls a heating valve using a DC motor and an H-bridge.
- Communicates with ThingSpeak to set target temperatures remotely.
- Can be easily adapted to custom APIs or other IoT platforms.

## Components Required
- **ESP32 Development Board**
- **DHT11 Temperature & Humidity Sensor**
- **DC Motor (extracted from a budget thermostat)**
- **H-Bridge Motor Driver (to reverse motor polarity)**
- **Resistors** (as required for pull-up configurations)
- **Breadboard & Jumper Wires**
- **External 5V Power Supply (if needed for motor power)**

## Wiring Diagram
Refer to the images for the wiring setup:
- The **DHT11 sensor** is connected to the ESP32 using the following pin configuration:
  - VCC → 3.3V (ESP32)
  - GND → GND
  - Data → GPIO4
  - 10kΩ pull-up resistor between VCC and Data pin

- The **H-Bridge motor driver** is connected as follows:
  - **Motor terminals** → Heating valve motor
  - **H-Bridge inputs** → ESP32 GPIO18 (Motor_A) and GPIO19 (Motor_B)
  - **Power** → External 5V supply (if required)

## Installation Guide
1. Clone this repository:
   ```sh
   git clone https://github.com/YOUR_GITHUB_USERNAME/smart-thermostat-esp32.git
   cd smart-thermostat-esp32
   ```
2. Install the required Arduino libraries:
   - `DHT11.h`
   - `WiFi.h`
   - `HTTPClient.h`
   - `ThingSpeak.h`
3. Open the `smart_thermostat.ino` file in the Arduino IDE.
4. Modify the WiFi credentials in the code:
   ```cpp
   #define WIFI_NETWORK "YOUR_WIFI_SSID"
   #define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"
   ```
5. Set up a **ThingSpeak account** and create a TalkBack command list.
6. Update your ThingSpeak and TalkBack API keys in the code:
   ```cpp
   #define CHANNEL_ID 1234567
   #define CHANNEL_API_KEY "YOUR_CHANNEL_API_KEY"
   #define TALKBACK_ID 98765
   #define TALKBACK_API_KEY "YOUR_TALKBACK_API_KEY"
   ```
7. Compile and upload the code to the ESP32.
8. Monitor the serial output to verify the connection and sensor readings.

## Code Explanation
The main logic follows these steps:
1. **Connect to WiFi**: The ESP32 connects to the specified WiFi network.
2. **Retrieve Target Temperature**: It fetches the latest temperature setpoint from ThingSpeak TalkBack.
3. **Read Sensor Data**: The DHT11 sensor provides real-time temperature and humidity readings.
4. **Control Heating System**:
   - If the temperature is **below** the setpoint, the heating system is activated.
   - If the temperature is **above** the setpoint, the heating system is deactivated.
   - The DC motor opens or closes the valve accordingly.
5. **Send Data to ThingSpeak**: The measured temperature, humidity, and target temperature are sent to ThingSpeak.
6. **Loop Execution**: The loop runs every **120 seconds** to ensure periodic updates.

## Customization Guide
- **Change API Platform**: Modify the `getTargetTemperatureFromTalkBack()` function to use a different API endpoint.
- **Adjust Motor Control Timing**: Modify the `delay(15000);` in `toggleHeatingMotor()` to change the valve movement duration.
- **Add More Sensors**: Extend the code to integrate additional sensors for enhanced data collection.

## Contribution Guidelines
- Fork the repository and create a new branch for your feature or bugfix.
- Submit a pull request with detailed changes.
- Ensure code quality and maintain consistency with the existing structure.

## License
This project is open-source and available under the MIT License.

---

For any questions or improvements, feel free to open an issue on GitHub!

