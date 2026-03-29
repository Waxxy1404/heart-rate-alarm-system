# 🫀 Heart Rate Alarm System

An embedded systems project that monitors heart rate in real-time using the **MAX30102** optical sensor and triggers an audible alarm when BPM exceeds a configurable threshold. Designed with elderly care and patient monitoring in mind.

![Hardware](./circuit_1.jpg)

---

## 📋 Overview

This system continuously reads heart rate data from a MAX30102 pulse oximeter sensor connected to a **Wemos D1 (ESP8266)** microcontroller. When the average BPM surpasses a defined limit, it activates a 555-timer-based oscillator circuit to sound an alarm. A caregiver can silence the alarm with a physical button — the alarm resets automatically once the heart rate normalizes.

---

## ✨ Features

- 📡 Real-time heart rate measurement via IR optical sensor (MAX30102)
- 🔔 Automatic alarm trigger when BPM exceeds configurable threshold
- 🔇 Manual alarm silence button for caregivers
- 🔄 Auto-reset when heart rate returns to normal range
- 📊 Serial Monitor output for debugging and real-time visualization
- 🔌 Finger detection (no false alarms when sensor is unattended)

---

## 🛠️ Hardware

| Component | Description |
|---|---|
| Wemos D1 (ESP8266) | Main microcontroller with Wi-Fi |
| MAX30102 | Pulse oximeter & heart rate sensor (I2C) |
| NE555 Timer | Oscillator circuit for alarm sound |
| Transistor | Controls the 555 oscillator enable/disable |
| Speaker | Audio output for the alarm |
| Pushbutton | Caregiver silence button |
| Resistors & Capacitors | Supporting components for oscillator circuit |
| Breadboard | Prototyping platform |

### Circuit Photos

<div align="center">
<img src="./images/circuit_1.jpg" width="45%"/>
<img src="./images/circuit_2.jpg" width="45%"/>
</div>

---

## 📌 Pin Configuration

| Pin | Function |
|---|---|
| D1 (SCL) | I2C Clock — MAX30102 |
| D2 (SDA) | I2C Data — MAX30102 |
| D5 | Alarm oscillator control (transistor base) |
| D3 | Caregiver silence button (INPUT_PULLUP) |

---

## ⚙️ How It Works

```
┌─────────────┐     I2C      ┌──────────────┐
│  MAX30102   │ ──────────► │  Wemos D1    │
│  HR Sensor  │             │  (ESP8266)   │
└─────────────┘             └──────┬───────┘
                                   │ D5
                             LOW = alarm ON
                             HIGH = alarm OFF
                                   │
                             ┌─────▼──────┐
                             │ Transistor │
                             └─────┬──────┘
                                   │
                             ┌─────▼──────┐
                             │ 555 Timer  │
                             │ Oscillator │
                             └─────┬──────┘
                                   │
                             ┌─────▼──────┐
                             │  Speaker   │
                             └────────────┘
```

1. The MAX30102 sensor reads IR values from the fingertip
2. The ESP8266 calculates BPM using beat detection and a rolling average of 4 readings
3. If average BPM > `LIMITE_BPM` (default: 70), the alarm activates
4. A LOW signal on D5 allows the 555 oscillator to run, sounding the speaker
5. Pressing the button sets the alarm to silenced — it resets when BPM normalizes
6. If no finger is detected (IR < 50,000), the system stays silent and resets

---

## 🚀 Getting Started

### Prerequisites

- [Arduino IDE](https://www.arduino.cc/en/software) with ESP8266 board support
- Libraries (install via Arduino Library Manager):
  - `SparkFun MAX3010x Pulse and Proximity Sensor Library`
  - `Wire` (built-in)

### Installation

```bash
# Clone this repository
git clone https://github.com/Waxxy1404/heart-rate-alarm-system.git

# Open the sketch in Arduino IDE
# File: sketch_eln2_final.ino
```

### Configuration

Open `sketch_eln2_final.ino` and adjust the threshold as needed:

```cpp
const int LIMITE_BPM = 70;  // Change this value to your desired BPM threshold
```

### Upload

1. Connect the Wemos D1 via USB
2. Select **Board:** `LOLIN(WEMOS) D1 R2 & mini`
3. Select the correct **Port**
4. Click **Upload**

---

## 📊 Serial Monitor Output

```
BPM: 72.3  Media BPM: 68  Status Alarme: OFF
BPM: 85.1  Media BPM: 74  Status Alarme: ON
ALARME SILENCIADO PELO CUIDADOR
BPM: 65.2  Media BPM: 66  Status Alarme: OFF
```

---

## 🔧 Adjusting the Alarm Logic

The alarm threshold and behavior can be customized:

```cpp
const int LIMITE_BPM = 70;      // BPM threshold to trigger alarm
const byte RATE_SIZE = 4;       // Number of readings for rolling average
```

---

## 📁 Project Structure

```
heart-rate-alarm-system/
├── sketch_eln2_final.ino   # Main Arduino sketch
├── images/
│   ├── circuit_1.jpg       # Hardware photo 1
│   └── circuit_2.jpg       # Hardware photo 2
└── README.md
```

---

## 🎓 Context

This project was developed as part of an Electronics course at **UFPE (Universidade Federal de Pernambuco)**. It demonstrates the integration of embedded systems programming, analog circuit design (555 timer oscillator), and sensor interfacing over I2C.

---

## 📄 License

This project is open source and available under the [MIT License](LICENSE).

---

## 👤 Author

**Victor Hugo Camurça**
- GitHub: [@Waxxy1404](https://github.com/Waxxy1404)
- LinkedIn: [victor-hugo-camurça](https://linkedin.com/in/victor-hugo-camurça)
