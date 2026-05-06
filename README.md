# Self-Balancing Robot — FreeRTOS & CMSIS-RTOS v1 / STM32L152RE

> TP Systèmes Embarqués Temps-Réel — MeSE2A  
> Plateforme : STM32 Nucleo-L152RE · FreeRTOS + CMSIS-RTOS v1 API · IMU LSM6DS3

---

## Overview

Embedded application for a **self-balancing (Segway-type) robot**, developed as part of a real-time systems lab.  
The firmware runs on a **STM32L152RE** microcontroller using **FreeRTOS** through the **CMSIS-RTOS v1 API** (`cmsis_os.h`) and implements:

- Real-time control loop at **10 ms** sampling period
- Tilt angle estimation via an **observer algorithm** (accelerometer + gyroscope fusion)
- Motor command calculation via a **regulation law**
- Circular buffer storing the **last 100 angle samples**
- Interactive **UART shell** for live monitoring
- Real-time **angle streaming** with a companion Python script

---

## Architecture

```
┌─────────────────────────────────────────────────────┐
│                    FreeRTOS Tasks                   │
│                                                     │
│  ┌──────────────────┐   ┌──────────────────────┐   │
│  │  Asservissement  │   │    Enregistrement     │   │
│  │   (osPriorityHigh)│   │ (osPriorityAboveNormal)│  │
│  │                  │   │                      │   │
│  │ • Read IMU (I2C) │──►│ • Circular buffer    │   │
│  │ • Angle observer │   │ • LED indicator      │   │
│  │ • Command law    │   │ • Stream queue       │   │
│  │ • Motor control  │   └──────────────────────┘   │
│  └──────────────────┘                               │
│                                                     │
│  ┌──────────────────┐                               │
│  │  Communication   │                               │
│  │  (osPriorityNormal)│                              │
│  │                  │                               │
│  │ UART Shell:      │                               │
│  │  read / dump /   │                               │
│  │  stream / help   │                               │
│  └──────────────────┘                               │
└─────────────────────────────────────────────────────┘
         CMSIS-RTOS v1 sync: 3 osMutex + 2 osMessageQ
```

---

## Hardware

| Component | Description |
|-----------|-------------|
| **MCU** | STM32L152RE (Nucleo-64) — ARM Cortex-M3 @ 32 MHz |
| **IMU** | LSM6DS3 (ST) — Accelerometer + Gyroscope via I2C |
| **Motor Driver** | TB6612FNG — H-bridge for DC motor |
| **LCD** | MeSN shield LCD display |
| **UART** | FTDI USB-to-TTL bridge (9600 baud) |

---

## Software Stack

```
Application (freertos.c)
    │
    ├── CMSIS-RTOS v1 API (cmsis_os.h)
    │       └── FreeRTOS kernel
    ├── STM32L1xx HAL Drivers
    └── Custom MeSN Drivers
            ├── LSM6DS3  (IMU — I2C)
            ├── MotorDriver (TB6612FNG — PWM/GPIO)
            ├── MESN_UART (asynchronous serial)
            ├── LCD
            ├── Clavier
            └── TimeBase
```

**Control algorithms** are provided as a pre-compiled static library (`libSBR_autom_obs-corr.a`):
- `autoAlgo_angleObs(acc_mg, rotAng_mDegSec)` — tilt angle observer
- `autoAlgo_commandLaw(tiltAngle_mDeg, rotAng_mDegSec)` — regulation law

---

## UART Shell

Connect with any terminal (TeraTerm, RealTerm, PuTTY) at **9600 baud**.

```
SelfBalancingRobot:~# help

Commands:
 read   - return last measured angle value
 dump   - return last hundred angle values
 stream - continuously return last measured angle value and update display
          press ENTER to quit stream mode
 help   - print this menu
```

---

## Real-Time Streaming (Python)

A companion script `affichageSTREAM.py` plots the tilt angle in real time using **matplotlib**.

```bash
pip install pyserial matplotlib
python affichageSTREAM.py
```

> Edit `PORT` in the script to match your serial port (e.g. `/dev/ttyUSB0` on Linux, `COM3` on Windows).

---

## Build & Flash

1. Open the project in **STM32CubeIDE** (`pjctFiles/`)
2. Build: `Project > Build All`
3. Flash: `Run > Debug` (ST-Link via Nucleo USB)

> Toolchain: GCC ARM Embedded · Debugger: OpenOCD / ST-Link

---

## Project Structure

```
RTOS_PROJECT/
├── pjctFiles/
│   ├── Src/
│   │   ├── freertos.c       ← Application tasks (main logic)
│   │   ├── main.c           ← Peripherals init + scheduler start
│   │   ├── gpio.c / tim.c   ← HAL peripheral configs
│   │   └── ...
│   ├── Inc/                 ← Headers + FreeRTOSConfig.h
│   ├── Drivers/
│   │   ├── CMSIS/           ← ARM & ST CMSIS headers
│   │   ├── STM32L1xx_HAL_Driver/
│   │   └── MeSN/            ← Custom peripheral drivers
│   ├── Middlewares/
│   │   └── Third_Party/FreeRTOS/
│   ├── Lib/Autom/           ← Pre-compiled control algorithms
│   └── Doc/Datasheets/      ← Component datasheets & schematics
└── affichageSTREAM.py       ← Real-time angle plot (Python)
```

---

## Key Concepts Demonstrated

- **Multitasking** with FreeRTOS task priorities and scheduling
- **Inter-task communication** via message queues (`osMessageQ`)
- **Shared resource protection** via mutexes (`osMutex`)
- **Periodic task execution** with `osDelayUntil` (hard real-time)
- **Circular buffer** implementation for sensor data history
- **Sensor fusion** (accelerometer + gyroscope) via observer algorithm
- **UART command shell** with non-blocking I/O

---

## License

HAL drivers © STMicroelectronics — Apache 2.0 / BSD  
FreeRTOS © Amazon — MIT  
Application code © Douadi — educational use
