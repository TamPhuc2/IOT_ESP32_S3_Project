# 🚀 Distributed IoT System
### ESP32-S3 Edge Gateway with Auto-Fallback & M2M Control

This system implements a distributed IoT model consisting of two ESP32 nodes (Sensor & Actuator) and a computer acting as an **Edge Gateway** (Internal Broker + Rule Engine). The system features an automatic fallback mechanism to the Cloud (app.coreiot.io) if the local gateway becomes unavailable.

---

## 🛠 1. Prerequisites

### 💻 Computer (Edge Gateway)
- Python 3.9+ installed.
- Required libraries:
  ```bash
  pip install amqtt paho-mqtt asyncio
  ```

### 📟 Hardware (ESP32-S3)
- **Node A (Sensor):** Reads DHT20/RS485 sensor data.
- **Node B (Actuator):** Controls GPIO 48 (LED) and GPIO 45 (NeoPixel).
- PlatformIO Libraries: `Adafruit NeoPixel`, `PubSubClient`, `ArduinoJson`.

---

## ⚙️ 2. System Configuration

### Step 1: Find your PC's Local IP
Open Terminal/CMD on your computer and type:
- Windows: `ipconfig`
- Linux/Mac: `ifconfig` or `ip a`
*Look for the `IPv4 Address` (Example: `192.168.1.15`).*

### Step 2: Configure ESP32 (C++ Code)
1. Open [include/global.h](include/global.h).
2. Update `LOCAL_BROKER_IP` with your computer's IP:
   ```cpp
   #define LOCAL_BROKER_IP "192.168.x.x" 
   ```
3. Open [src/main.cpp](src/main.cpp) to select the compilation mode for each board:
   - **For Node A:** Enable `#define NODE_MODE_A`.
   - **For Node B:** Enable `#define NODE_MODE_B` (and comment out Node A).

---

## 🏃 3. How to Run

### Step 1: Start the Edge Gateway (Python)
In the project's root directory, run:
```bash
python broker/EdgeGatewayServer.py
```
> [!IMPORTANT]
> The Gateway must be running before powering on the ESP32 nodes so they can discover the local broker.

### Step 2: Power on the ESP32 Nodes
- **Node A:** Automatically connects to the Gateway. If temperature/humidity exceeds the threshold, it sends an M2M trigger. If the Gateway is down, it falls back to `app.coreiot.io`.
- **Node B:** Listens for commands from the Gateway. When Node A reports high humidity (> 60%), Node B will **blink red (0.5s interval)**.

---

## 📊 4. Operational Model (Workflow)

1. **Node A (Sensor)** sends telemetry to the **Gateway**.
2. **Gateway** evaluates the condition (Rule Engine).
3. **Gateway** sends control commands to **Node B (Actuator)** via LAN.
4. **Gateway** simultaneously forwards telemetry to the **CoreIOT Cloud Dashboard**.
5. If the **Gateway** fails, **Node A** automatically switches to pushing data directly to the **Cloud**.

---

## 📝 Notes
- Ensure all devices (PC, Node A, Node B) are connected to the same WiFi network.
- Check your computer's **Firewall** settings to ensure Port **1883** is open for inbound traffic.
- Node B uses GPIO 45 (NeoPixel); it will blink **Red** during alerts and remain Off during normal status.