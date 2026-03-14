# FanRoc Firmware

## 🧩 Project Overview
This repository contains the firmware for the **FanRoc** robot platform. It is built on the **Arduino** ecosystem using **PlatformIO** and controls drive motors, servos, and a PS2 gamepad interface.

### 🛠️ Core Features
- **Drive train control** (tank drive) via PS2 analog sticks
- **Servo-based loader/shooter mechanism** with automatic reload cycle
- **Intake toggling** via controller button
- Uses **Adafruit PWM Servo Driver** for reliable PWM control over multiple servos

## 🧱 Tech Stack
- **Platform:** Arduino (via PlatformIO)
- **Language:** C++
- **Dev Environment:** PlatformIO CLI / VS Code with PlatformIO extension
- **Libraries:**
  - `Adafruit_PWMServoDriver` (for servo PWM output)
  - `PS2X_lib` (for PS2 controller input)
  - `Wire` (I2C bus for PWM driver)

## 📁 Repository Structure
- `platformio.ini` – PlatformIO project configuration
- `src/` – Main firmware source code
  - `main.cpp` – Entry point and main loop
  - `configs.h` – I/O definitions and helper functions
  - `components/` – Reusable robot component code (drive train, etc.)
  - `drivers/` – Peripherals (motor, servo, switch) handling
- `include/` – Additional headers
- `test/` – Unit test scaffold (if used)

## 🚀 How to Build and Upload
### 1) Install PlatformIO
- Use **VS Code + PlatformIO extension** (recommended)
- Or install via CLI: `pip install platformio`

### 2) Build
From the repository root:
```sh
platformio run
```

### 3) Upload to the Robot
Connect your microcontroller via USB and run:
```sh
platformio run --target upload
```

## 🎮 Controls (PS2 Gamepad)
- **Left stick** – drive forward/back + strafe
- **Right stick** – drive rotation
- **L2** – toggle intake on/off
- **R2** – manual shooter servo activation
- **Blue button** – toggle automatic shoot/reload cycle

## 🔄 Auto Shooter Behavior
Pressing the blue button triggers an automatic cycle:
1. Open reload gate
2. Run shooter servo for a fixed time
3. Close reload gate for reload
4. Repeat until paused

---

For questions or improvements, update the code in `src/main.cpp` and follow PlatformIO build/upload steps.