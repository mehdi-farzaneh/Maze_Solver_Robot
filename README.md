# Maze Solver Robot - HFU 2025/26 - Farzaneh

Mechatronic Project: Maze Solver Robot
Furtwangen University - Biomedical Engineering - WS25/26
By: Mehdi Farzaneh

---
## Overview
An autonomous maze-solving robot that uses ultrasound sensors and wall-following algorithm to navigate through mazes independently.

---

## Hardware Components

### Sensors
- **Ultrasound Sensors x3** (HC-SR04 compatible)
  - Right sensor: Trigger pin 2, Echo pin 3
  - Mid sensor: Trigger pin 4, Echo pin 5
  - Left sensor: Trigger pin 6, Echo pin 7
  - Range: ~75mm (configurable via Side_Wall and Front_Wall constants)
  
- **Photo Resistor Sensor**
  - Pin: A0
  - Used to detect black spot (end point) - triggers when resistance > 350Ω

### Actuators
- **DC Motors x2** with gear
  - Right motor: Direction pins 8-9, PWM pin 10
  - Left motor: Direction pins 7-12, PWM pin 11
  - Motor speed: 65 PWM (configurable via move_speed constant)
  
- **Motor Driver**: L298N

### Other Components
- Arduino board (Uno/Mega)
- Power supply
- Switch

### Optional
- **Time of Flight (ToF) Sensors x3** (if ToF is preferred over ultrasound)
  - Can be enabled by uncommenting the ToF code in setup()

---

## Pin Configuration

| Component | Pin | Type |
|-----------|-----|------|
| Right Motor Pin 1 | 8 | Digital Output |
| Right Motor Pin 2 | 9 | Digital Output |
| Right Motor PWM | 10 | PWM Output |
| Left Motor Pin 1 | 7 | Digital Output |
| Left Motor Pin 2 | 12 | Digital Output |
| Left Motor PWM | 11 | PWM Output |
| Right Sensor Trigger | 2 | Digital Output |
| Right Sensor Echo | 3 | Digital Input |
| Mid Sensor Trigger | 4 | Digital Output |
| Mid Sensor Echo | 5 | Digital Input |
| Left Sensor Trigger | 6 | Digital Output |
| Left Sensor Echo | 7 | Digital Input |
| Photo Resistor | A0 | Analog Input |

---

## Algorithm: Right-Wall Following Maze Solver

The robot implements a right-hand wall following algorithm:

1. **Wall Detection** - Measures distance from three directions (left, right, front)
2. **Turn Right** - If right side is open (>75mm), turn 90° right and move forward
3. **Turn Around** - If wall ahead AND wall on right, turn 180°
4. **Adjust Position** - If difference between left/right distance >10mm, tilt left or right to center
5. **Move Forward** - Default behavior when no obstacles or turns needed
6. **Stop** - When photo resistor detects black spot (resistance >350Ω)

### Movement Functions
- `move_forward()` - Move straight
- `turn_right()` - 90° right turn
- `turn_around()` - 180° turn
- `tilt_left()` - Slight left adjustment
- `tilt_right()` - Slight right adjustment
- `Stop()` - Emergency stop

---

## Configurable Parameters

```cpp
#define Side_Wall 75        // Distance threshold for side walls (mm)
#define Front_Wall 75       // Distance threshold for front wall (mm)
#define Low_Resistance 350  // Photo resistor threshold for black spot detection
#define move_speed 65       // Motor PWM speed (0-255)
```

**Calibration**: Side_Wall and Front_Wall thresholds should be calibrated based on your hardware setup.

---

## Objective
The robot autonomously solves the maze by following the right-hand wall and stops when it reaches the black spot (finish line).
