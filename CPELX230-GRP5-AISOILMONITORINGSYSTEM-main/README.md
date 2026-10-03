# AI-Based Soil Monitoring System

A GitHub-ready embedded AI/ML prototype that predicts whether a plant needs water using:

- Soil moisture
- Temperature
- Humidity

The machine-learning model is a **Random Forest Classifier** with three output classes:

- `NO_WATER`
- `WATER_SOON`
- `WATER_NOW`

## System Flow

```text
Capacitive Soil Sensor ─┐
                        │
DHT22 Temperature ──────┼──> ESP32 ──> Flask API ──> Random Forest ──> Watering Decision
                        │
DHT22 Humidity ─────────┘
```

## Project Structure

```text
AI-Soil-Monitoring-System/
│
├── app/
│   ├── app.py
│   ├── static/
│   │   ├── script.js
│   │   └── style.css
│   └── templates/
│       └── index.html
│
├── dataset/
│   ├── generate_dataset.py
│   └── soil_data.csv
│
├── esp32/
│   └── soil_monitor.ino
│
├── model/
│   ├── metrics.json
│   ├── soil_model.joblib
│   └── train_model.py
│
├── requirements.txt
├── run_windows.bat
├── test_api.py
└── README.md
```

## 1. Requirements

Install Python 3.10+.

Check Python:

```powershell
py --version
```

Create a virtual environment:

```powershell
py -m venv .venv
```

### Windows PowerShell

If PowerShell allows script activation:

```powershell
.venv\Scripts\Activate.ps1
```

If PowerShell blocks scripts, you can skip activation and run the virtual-environment Python directly:

```powershell
.venv\Scripts\python.exe -m pip install -r requirements.txt
.venv\Scripts\python.exe app\app.py
```

Or temporarily allow scripts only for the current PowerShell window:

```powershell
Set-ExecutionPolicy -Scope Process -ExecutionPolicy Bypass
.venv\Scripts\Activate.ps1
```

Install packages:

```powershell
python -m pip install -r requirements.txt
```

## 2. Generate a New Dataset

A demo dataset is already included.

To regenerate it:

```powershell
python dataset\generate_dataset.py
```

The included dataset is **synthetic** and is meant for software prototyping/class demonstration. For a final research implementation, collect real sensor data from actual plants and replace the generated labels with observed or expert-validated watering decisions.

## 3. Train the Random Forest Model

A trained model is already included.

To retrain:

```powershell
python model\train_model.py
```

This creates:

```text
model/soil_model.joblib
model/metrics.json
```

## 4. Run the Flask Dashboard

From the project root:

```powershell
python app\app.py
```

Open:

```text
http://127.0.0.1:5000
```

Or double-click:

```text
run_windows.bat
```

## 5. Test the API

Start Flask first, then in another terminal run:

```powershell
python test_api.py
```

Example input:

```json
{
  "soil_moisture": 43,
  "temperature": 31,
  "humidity": 55
}
```

Example response:

```json
{
  "soil_moisture": 43.0,
  "temperature": 31.0,
  "humidity": 55.0,
  "prediction": "WATER_SOON",
  "confidence": 0.93,
  "probabilities": {
    "NO_WATER": 0.04,
    "WATER_SOON": 0.93,
    "WATER_NOW": 0.03
  }
}
```

## API Endpoints

### `GET /api/health`

Checks whether the server and model are loaded.

### `POST /api/predict`

Manual prediction endpoint.

Required JSON fields:

```json
{
  "soil_moisture": 50,
  "temperature": 30,
  "humidity": 60
}
```

### `POST /api/sensor`

Used by the ESP32 to upload live sensor readings. The Flask server predicts the watering class and stores the most recent reading in memory.

### `GET /api/latest`

Returns the latest reading sent by the ESP32.

## 6. ESP32 Setup

Open:

```text
esp32/soil_monitor.ino
```

Hardware:

- ESP32
- Capacitive soil moisture sensor
- DHT22 temperature/humidity sensor

Default pins:

```text
Soil moisture analog output -> GPIO 34
DHT22 data                  -> GPIO 4
```

Update these lines:

```cpp
const char* WIFI_SSID = "YOUR_WIFI_NAME";
const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";
const char* SERVER_URL = "http://192.168.1.5:5000/api/sensor";
```

The computer and ESP32 should be on the same Wi-Fi network.

To find your computer's IPv4 address on Windows:

```powershell
ipconfig
```

Use that local IPv4 address in `SERVER_URL`.

Example:

```cpp
const char* SERVER_URL = "http://192.168.1.8:5000/api/sensor";
```

## Soil Sensor Calibration

The sketch contains:

```cpp
const int AIR_VALUE = 3200;
const int WATER_VALUE = 1300;
```

These are only example values.
- Save readings to SQLite or Firebase.
- Add plant-type selection.
- Compare Random Forest with KNN and SVM.
- Export a lightweight model for on-device ESP32 inference.
- Add charts and historical sensor logging.
