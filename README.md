# Self-Balancing Robot — STM32L152RE · FreeRTOS · CMSIS-RTOS v1

**ESIX MeSN 2A — TP Systèmes Temps-Réel | ENSICaen**

Firmware embarqué pour un robot auto-balancé (type Segway) développé dans le cadre du TP4 d'un cours de systèmes temps-réel. L'application est **multitâche**, tourne sous **FreeRTOS** via l'API **CMSIS-RTOS v1**, et implémente un asservissement d'équilibre en temps réel sur microcontrôleur **ARM Cortex-M3**.

---

## Matériel utilisé

| Composant | Rôle |
|-----------|------|
| STM32 Nucleo-L152RE (ARM Cortex-M3 @ 32 MHz) | Microcontrôleur principal |
| LSM6DS3 (ST) — I2C | IMU : accéléromètre + gyroscope |
| TB6612FNG | Pont en H — pilotage moteur CC |
| LED (on-board PA5) | Indicateur d'état de l'équilibre |
| UART (FTDI USB-TTL) | Shell de communication série |
| LCD 16×2 + clavier matriciel 12 touches | IHM locale (extension avancée) |

---

## Architecture logicielle

L'application repose sur **3 tâches FreeRTOS** communicant via des queues et protégées par des mutex :

```
┌──────────────────────────────────────────────────────────────┐
│                        CMSIS-RTOS v1                         │
│                                                              │
│  ┌─────────────────────┐     ┌────────────────────────────┐  │
│  │   Asservissement    │     │      Enregistrement        │  │
│  │  osPriorityHigh     │────►│  osPriorityAboveNormal     │  │
│  │                     │     │                            │  │
│  │ • Lecture IMU (I2C) │     │ • Buffer circulaire 100val │  │
│  │ • Observateur angle │     │ • LED (seuil ±25°)         │  │
│  │ • Loi de commande   │     │ • Queue streaming          │  │
│  │ • Moteur (PWM)      │     └────────────────────────────┘  │
│  │ • osDelayUntil 10ms │                                     │
│  └─────────────────────┘                                     │
│                                                              │
│  ┌─────────────────────┐                                     │
│  │   Communication     │                                     │
│  │  osPriorityNormal   │                                     │
│  │                     │                                     │
│  │ • Shell UART        │                                     │
│  │ • read/dump/stream  │                                     │
│  │ • help              │                                     │
│  └─────────────────────┘                                     │
│                                                              │
│         Synchronisation : 3 osMutex · 2 osMessageQ          │
└──────────────────────────────────────────────────────────────┘
```

---

## Fonctionnalités implémentées

### 1. Asservissement temps-réel (10 ms strictement)
- Lecture accéléromètre (axe X) et gyroscope (axe Y) via I2C
- Calcul de l'angle d'inclinaison par un **algorithme d'observateur**
- Calcul de la commande moteur par une **loi de régulation**
- Exécution périodique garantie avec `osDelayUntil`

### 2. Enregistrement
- Stockage des **100 dernières valeurs** d'angle dans un buffer circulaire
- **LED indicateur** : allumée fixe si `|angle| > 25°`, clignotante (100 ms / 0.5 s) sinon

### 3. Shell UART

Connexion via terminal série (TeraTerm, Minicom, PuTTY) à **9600 baud** :

```
SelfBalancingRobot:~# help

Commands:
 read   - return last measured angle value
 dump   - return last hundred angle values
 stream - continuously return last measured angle value (press ENTER to quit)
 help   - print this menu
```

### 4. Extension avancée — IHM LCD + clavier
- Affichage sur LCD 16×2 : menu, valeurs, modes
- Navigation via clavier matriciel 12 touches
- Menu principal : `1:Rd  2:Dmp  3:Str`
- Commandes `Read`, `Dump`, `Stream` accessibles localement

---

## Algorithmes d'asservissement

Fournis sous forme de bibliothèque pré-compilée (`libSBR_autom_obs-corr.a`) :

```c
// Observateur : estimation de l'angle (milli-degrés)
int32_t autoAlgo_angleObs(int32_t acc_mg, int32_t rotAng_mDegSec);

// Loi de commande : vitesse moteur (‰ de la vitesse max, ±1000)
int32_t autoAlgo_commandLaw(int32_t tiltAngle_mDeg, int32_t rotAng_mDegSec);
```

- `acc_mg` : accélération axe X en milli-g
- `rotAng_mDegSec` : vitesse angulaire axe Y en milli-deg/s
- Période d'échantillonnage : **10 ms** (temps discret)

---

## Stack logicielle

```
Application (freertos.c / main.c)
    │
    ├── CMSIS-RTOS v1 API  (cmsis_os.h)
    │       └── FreeRTOS 10.0.1 kernel
    ├── STM32L1xx HAL (STM32CubeMX 6.6.1)
    └── Drivers MeSN
            ├── LSM6DS3     — IMU via I2C
            ├── MotorDriver — TB6612FNG via PWM/GPIO
            ├── MESN_UART   — liaison série asynchrone
            ├── MeSN_I2C    — bus I2C avec support OS
            ├── LCD         — afficheur 16×2
            ├── Clavier     — clavier matriciel
            └── TimeBase    — base de temps
```

**Toolchain :** STM32CubeIDE 1.10.1 · GCC arm-atollic-eabi 10.6.2 · ST-Link · TraceAnalyser 4.6.6

---

## Visualisation temps-réel (Python)

Le script `affichageSTREAM.py` trace l'angle en temps réel via matplotlib :

```bash
pip install pyserial matplotlib
python affichageSTREAM.py
```

> Modifier `PORT` dans le script (`/dev/ttyUSB0` sous Linux, `COM3` sous Windows).

---

## Build & Flash

1. Ouvrir le projet dans **STM32CubeIDE** (`pjctFiles/`)
2. `Project > Build All`
3. `Run > Debug` (ST-Link via USB Nucleo)

---

## Structure du projet

```
RTOS_PROJECT/
├── pjctFiles/
│   ├── Src/
│   │   ├── freertos.c          ← Tâches applicatives (logique principale)
│   │   ├── main.c              ← Init périphériques + démarrage scheduler
│   │   ├── gpio.c / tim.c      ← Config HAL
│   │   └── ...
│   ├── Inc/
│   │   ├── FreeRTOSConfig.h    ← Config OS (préemptif, tick, heap...)
│   │   └── ...
│   ├── Drivers/MeSN/           ← Pilotes périphériques custom
│   ├── Middlewares/FreeRTOS/   ← Noyau FreeRTOS + CMSIS-RTOS wrapper
│   ├── Lib/Autom/              ← Bibliothèque algo observateur/correcteur
│   └── Doc/                    ← Datasheets & schémas
└── affichageSTREAM.py          ← Visualisation angle en temps réel
```

---

## Concepts mis en œuvre

- Ordonnancement **préemptif** par priorités sous FreeRTOS
- Tâche périodique **temps-réel dur** avec `osDelayUntil` (jitter minimal)
- Communication inter-tâches par **queues de messages** (`osMessageQ`)
- Protection des ressources partagées par **mutex** (`osMutex`)
- **Buffer circulaire** thread-safe pour l'historique des mesures
- **Fusion de capteurs** accéléromètre/gyroscope par observateur d'état
- Détection de **stack overflow** (`vApplicationStackOverflowHook`)
- Débogage avec **Tracealyzer** (TraceRecorder intégré)

---

*Cours Systèmes Embarqués Temps-Réel — ESIX MeSN 2A — ENSICaen*  
*Basile Dufay · Christophe Cordier*
