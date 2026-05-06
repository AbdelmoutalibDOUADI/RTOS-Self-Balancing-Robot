# Self-Balancing Robot — STM32L152RE · FreeRTOS · CMSIS-RTOS v1

**ESIX MeSN 2A — Real-Time Systems Lab**

Embedded firmware for a self-balancing (Segway-type) robot, developed as part of a real-time systems course project. The application is **multitasking**, runs on **FreeRTOS** through the **CMSIS-RTOS v1 API**, and implements a real-time balance control loop on an **ARM Cortex-M3** microcontroller.

---

## Hardware

| Component | Role |
|-----------|------|
| STM32 Nucleo-L152RE (ARM Cortex-M3 @ 32 MHz) | Main microcontroller |
| LSM6DS3 (ST) — I2C | IMU: accelerometer + gyroscope |
| TB6612FNG | H-bridge — DC motor control |
| LED (on-board PA5) | Balance status indicator |
| UART (FTDI USB-TTL) | Serial communication shell |
| LCD 16×2 + 12-key matrix keypad | Local HMI (advanced extension) |

---

## Software Architecture

The application relies on **3 FreeRTOS tasks** communicating through queues and protected by mutexes:

```
┌──────────────────────────────────────────────────────────────┐
│                        CMSIS-RTOS v1                         │
│                                                              │
│  ┌─────────────────────┐     ┌────────────────────────────┐  │
│  │   Asservissement    │     │       Enregistrement       │  │
│  │   osPriorityHigh    │────►│  osPriorityAboveNormal     │  │
│  │                     │     │                            │  │
│  │ • IMU read (I2C)    │     │ • Circular buffer 100 val  │  │
│  │ • Angle observer    │     │ • LED indicator (±25°)     │  │
│  │ • Command law       │     │ • Streaming queue          │  │
│  │ • Motor (PWM)       │     └────────────────────────────┘  │
│  │ • osDelayUntil 10ms │                                     │
│  └─────────────────────┘                                     │
│                                                              │
│  ┌─────────────────────┐                                     │
│  │    Communication    │                                     │
│  │  osPriorityNormal   │                                     │
│  │                     │                                     │
│  │ • UART shell        │                                     │
│  │ • read/dump/stream  │                                     │
│  │ • help              │                                     │
│  └─────────────────────┘                                     │
│                                                              │
│         Synchronization: 3 osMutex · 2 osMessageQ           │
└──────────────────────────────────────────────────────────────┘
```

---

## Features

### 1. Real-Time Control Loop (strict 10 ms period)
- Read accelerometer (X axis) and gyroscope (Y axis) via I2C
- Compute tilt angle using a **state observer algorithm**
- Compute motor command using a **regulation law**
- Periodic execution guaranteed with `osDelayUntil`

### 2. Data Logging
- Store the **last 100 angle values** in a thread-safe circular buffer
- **LED indicator**: steady on if `|angle| > 25°`, blinking (100 ms / 0.5 s period) otherwise

### 3. UART Shell

Connect via any serial terminal (TeraTerm, Minicom, PuTTY) at **9600 baud**:

```
SelfBalancingRobot:~# help

Commands:
 read   - return last measured angle value
 dump   - return last hundred angle values
 stream - continuously return last measured angle value (press ENTER to quit)
 help   - print this menu
```

### 4. Advanced Extension — LCD HMI + Keypad
- Display on 16×2 LCD: menu, values, modes
- Navigation via 12-key matrix keypad
- Main menu: `1:Rd  2:Dmp  3:Str`
- `Read`, `Dump`, `Stream` commands available locally

---

## Control Algorithms

Provided as a pre-compiled static library (`libSBR_autom_obs-corr.a`):

```c
// Observer: tilt angle estimation (milli-degrees)
int32_t autoAlgo_angleObs(int32_t acc_mg, int32_t rotAng_mDegSec);

// Regulation law: wheel speed command (‰ of max speed, range ±1000)
int32_t autoAlgo_commandLaw(int32_t tiltAngle_mDeg, int32_t rotAng_mDegSec);
```

- `acc_mg`: X-axis acceleration in milli-g
- `rotAng_mDegSec`: Y-axis angular velocity in milli-deg/s
- Sampling period: **10 ms** (discrete time)

---

## Software Stack

```
Application (freertos.c / main.c)
    │
    ├── CMSIS-RTOS v1 API  (cmsis_os.h)
    │       └── FreeRTOS 10.0.1 kernel
    ├── STM32L1xx HAL (STM32CubeMX 6.6.1)
    └── MeSN Custom Drivers
            ├── LSM6DS3     — IMU over I2C
            ├── MotorDriver — TB6612FNG via PWM/GPIO
            ├── MESN_UART   — asynchronous serial
            ├── MeSN_I2C    — OS-aware I2C bus
            ├── LCD         — 16×2 display
            ├── Clavier     — matrix keypad
            └── TimeBase    — time base
```

**Toolchain:** STM32CubeIDE 1.10.1 · GCC arm-atollic-eabi 10.6.2 · ST-Link · TraceAnalyser 4.6.6

---

## Real-Time Streaming (Python)

The `affichageSTREAM.py` companion script plots the tilt angle live using matplotlib:

```bash
pip install pyserial matplotlib
python affichageSTREAM.py
```

> Edit `PORT` in the script to match your serial port (`/dev/ttyUSB0` on Linux, `COM3` on Windows).

---

## Build & Flash

1. Open the project in **STM32CubeIDE** (`pjctFiles/`)
2. `Project > Build All`
3. `Run > Debug` (ST-Link via Nucleo USB)

---

## Project Structure

```
RTOS_PROJECT/
├── pjctFiles/
│   ├── Src/
│   │   ├── freertos.c          ← Application tasks (main logic)
│   │   ├── main.c              ← Peripheral init + scheduler start
│   │   ├── gpio.c / tim.c      ← HAL peripheral configs
│   │   └── ...
│   ├── Inc/
│   │   ├── FreeRTOSConfig.h    ← OS config (preemptive, tick, heap...)
│   │   └── ...
│   ├── Drivers/MeSN/           ← Custom peripheral drivers
│   ├── Middlewares/FreeRTOS/   ← FreeRTOS kernel + CMSIS-RTOS wrapper
│   ├── Lib/Autom/              ← Pre-compiled observer/regulator library
│   └── Doc/                    ← Datasheets & schematics
└── affichageSTREAM.py          ← Real-time angle plot (Python)
```

---

## Key Concepts Demonstrated

- **Preemptive scheduling** with task priorities under FreeRTOS
- **Hard real-time** periodic task using `osDelayUntil` (minimal jitter)
- **Inter-task communication** via message queues (`osMessageQ`)
- **Shared resource protection** via mutexes (`osMutex`)
- **Thread-safe circular buffer** for sensor data history
- **Sensor fusion** (accelerometer + gyroscope) via state observer
- **Stack overflow detection** (`vApplicationStackOverflowHook`)
- **Execution tracing** with Tracealyzer (TraceRecorder integrated)

---

*Real-Time Embedded Systems Course — ESIX MeSN 2A*  
*Supervised by Basile Dufay & Christophe Cordier — Implemented by Abdelmoutalib Douadi*
