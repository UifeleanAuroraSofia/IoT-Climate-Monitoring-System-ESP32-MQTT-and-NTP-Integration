<h1 align="center">IoT Project – Temperature and Humidity Monitoring</h1>

<p align="center">
  <i>This project uses an ESP32 to read temperature and humidity data from a DHT22 sensor and publish it to an MQTT broker (EMQX Cloud). Additionally, it stores the last monitoring timestamp using MQTT retained messages. The project is developed and tested in the <a href="https://wokwi.com">Wokwi Simulator</a>.</i>
</p>

---

<h2>Table of Contents</h2>
<ol>
  <li><a href="#general-description">General Description</a></li>
  <li><a href="#features">Features</a></li>
  <li><a href="#technologies-used">Technologies Used</a></li>
  <li><a href="#project-structure">Project Structure</a></li>
  <li><a href="#how-to-run-the-project">How to Run the Project</a></li>
  <li><a href="#possible-improvements">Possible Improvements</a></li>
  <li><a href="#license">License</a></li>
</ol>

---

<h2 id="general-description">General Description</h2>
<p>
  This project collects data from the DHT22 sensor (temperature and humidity) and publishes it to an MQTT broker at regular intervals.
</p>
<p>
  The ESP32 uses a WiFi connection to communicate with the broker securely via TLS. Timestamp information is retrieved using an NTP server, and the last monitoring timestamp is stored as a retained message in the broker. This allows the ESP32 to retrieve the timestamp of the previous monitoring session upon restarting.
</p>

---

<h2 id="features">Features</h2>
<ul>
  <li>Real-time temperature and humidity readings using the DHT22 sensor.</li>
  <li>Publishing data to different MQTT topics (e.g., <code>/PTIT_Test/p/temp1</code> for temperature and <code>/PTIT_Test/p/hum1</code> for humidity).</li>
  <li>Storing the last monitoring timestamp using MQTT retained messages.</li>
  <li>Retrieving the current date and time from an NTP server (<code>pool.ntp.org</code>).</li>
  <li>Secure communication with the broker using TLS.</li>
  <li>Fully simulated in the Wokwi platform with hardware configuration stored in <code>diagram.json</code>.</li>
</ul>

---

<h2 id="technologies-used">Technologies Used</h2>
<ul>
  <li><strong>ESP32:</strong> The main microcontroller used in the project.</li>
  <li><strong>Wokwi Simulator:</strong> An online platform to simulate ESP32 and other hardware.</li>
  <li><strong>Arduino IDE / PlatformIO:</strong> Development environments for coding and testing.</li>
  <li><strong>C++:</strong> The programming language used for implementation.</li>
  <li><strong>PubSubClient:</strong> A library for MQTT communication.</li>
  <li><strong>NTPClient:</strong> A library for retrieving time from an NTP server.</li>
  <li><strong>DHTesp:</strong> A library for interfacing with the DHT22 sensor.</li>
  <li><strong>EMQX Cloud:</strong> An MQTT broker used for secure data transfer via TLS.</li>
</ul>

---

<h2 id="project-structure">Project Structure</h2>
<pre>
📂 Project Folder
├── src/
│   └── sketch.ino          # Main code written in C++ for the ESP32
├── diagram.json            # Wokwi hardware configuration file
├── libraries.txt           # List of required libraries
├── README.md               # Description of the project
└── LICENSE                 # Optional license file
</pre>
<p>
  <strong>sketch.ino:</strong> Contains the main code for ESP32, including WiFi, MQTT setup, NTP integration, and DHT22 sensor data collection.<br>
  <strong>diagram.json:</strong> The Wokwi hardware configuration file defining the ESP32, DHT22 sensor, and wiring.<br>
  <strong>libraries.txt:</strong> Lists all libraries required for the project to compile and run.
</p>

---

<h2 id="how-to-run-the-project">How to Run the Project</h2>
<ol>
  <li>Install Arduino IDE or PlatformIO and ensure ESP32 support is configured.</li>
  <li>Clone the repository using the following command:
    <pre>
git clone https://github.com/username/temperature-humidity-monitor.git
cd temperature-humidity-monitor
    </pre>
  </li>
  <li>Open the project in Arduino IDE or PlatformIO.</li>
  <li>Install the required libraries from Library Manager or <code>libraries.txt</code>:
    <ul>
      <li>PubSubClient</li>
      <li>NTPClient</li>
      <li>DHTesp</li>
      <li>WiFi (included in ESP32 Arduino Core)</li>
    </ul>
  </li>
  <li>Update the code:
    <ul>
      <li>Set the SSID and password for your WiFi network.</li>
      <li>Configure the MQTT broker details (server name, port, username, password).</li>
    </ul>
  </li>
  <li>Upload the code to the ESP32 using Arduino IDE or PlatformIO.</li>
  <li>Monitor the Serial Output to verify WiFi and MQTT connection, and debug sensor data.</li>
  <li>Use an MQTT client (e.g., <a href="https://mqttx.app/">MQTTX</a> or <a href="http://mqtt-explorer.com/">MQTT Explorer</a>) to view the published data and retained messages.</li>
</ol>

---

<h2 id="possible-improvements">Possible Improvements</h2>
<ul>
  <li><strong>Certificate Validation for TLS:</strong> Replace <code>setInsecure()</code> with proper certificate validation for enhanced security.</li>
  <li><strong>Integration with a Dashboard:</strong> Use tools like Grafana or Node-RED to visualize the temperature and humidity trends.</li>
  <li><strong>Support for Additional Sensors:</strong> Extend the project to include sensors for pressure, light, or other environmental data.</li>
  <li><strong>Bidirectional Communication:</strong> Add topics for remote control (e.g., controlling LEDs or changing parameters on the ESP32).</li>
  <li><strong>Local Data Storage:</strong> Implement an SD card module for local storage of sensor data as a backup.</li>
</ul>

---

<h2 id="license">License</h2>
<p>
  This project is distributed under an open-source license (e.g., <a href="https://choosealicense.com/licenses/mit/">MIT</a> or <a href="https://choosealicense.com/licenses/apache-2.0/">Apache-2.0</a>). You are free to use, modify, and distribute the code, provided you respect the terms of the license. Refer to the <code>LICENSE</code> file for more details.
</p>

---

<p align="center"><i>Developed as an educational project to explore IoT concepts, MQTT, and ESP32 in real-world applications.</i></p>