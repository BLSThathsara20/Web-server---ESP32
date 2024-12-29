# ESP32 Real-time Sound Monitor 🎯

A real-time sound monitoring system built with ESP32 and modern web technologies. This project demonstrates IoT capabilities combined with a responsive web interface.

## Features 🚀

- Real-time sound level monitoring
- Beautiful responsive dashboard
- Live chart visualization
- Digital state detection
- Server health monitoring
- Built-in LED indicator

## Hardware Requirements 🔧

- ESP32-WROOM-32 Development Board
- KY-037 Sound Detection Sensor
- Jumper Wires
- Micro USB Cable

## Pin Configuration 🖌

```cpp
Sound Sensor:
VCC → 3.3V
GND → GND
DO (Digital Out) → GPIO13
AO (Analog Out) → GPIO32
```

## Software Dependencies 💻

- Arduino IDE
- Required Libraries:
  - `WiFi.h`
  - `WebServer.h`

## Installation & Setup 🛠️

1. Clone this repository:

```bash
git clone https://github.com/BLSThathsara20/Web-server---ESP32.git
```

2. Open `web-server.ino` in Arduino IDE

3. Install required libraries through Arduino Library Manager

4. Update WiFi credentials in the code:

```cpp
const char ssid[] = "YOUR_WIFI_SSID";
const char password[] = "YOUR_WIFI_PASSWORD";
```

5. Upload the code to your ESP32

## Usage 📱

1. Connect the hardware according to the pin configuration
2. Power up the ESP32
3. Connect to the same WiFi network
4. Open a web browser and navigate to the ESP32's IP address
5. Monitor real-time sound levels through the dashboard

## Technologies Used 🌟

- ESP32 with Arduino Framework
- TailwindCSS for styling
- Chart.js for visualization
- WebSocket for real-time data
- Modern JavaScript

## License 📄

[MIT](https://choosealicense.com/licenses/mit/)

## Author ✍️

Savindu Thathsara

## Acknowledgments 🙏

- ESP32 Community
- Arduino Community
- TailwindCSS Team
- Chart.js Team
