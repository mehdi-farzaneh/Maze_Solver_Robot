# Maze Solver Robot 

Mechatronic Project: Maze Solver Robot

Furtwangen University - Biomedical Engineering - WS25/26

By: Mehdi Farzaneh

Supervisors: Dr. Kretschmer, Dr. Kubon

---
## Overview
An autonomous maze-solving robot that uses ultrasound sensors and wall-following algorithm to navigate through mazes independently.
---

## Hardware Components

### Sensors
- **Ultrasound Sensors x3** (HC-SR04 compatible)
  
- **Photo Resistor Sensor**

### Actuators
- **DC Motors x2** with gear
  
- **Motor Driver**: L298N

### Other Components
- Arduino board (Uno/Mega)
- Power supply
- Switch

### Optional
- **Time of Flight (ToF) Sensors x3** (if ToF is preferred over ultrasound)

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

---

**Calibration**: Side_Wall and Front_Wall thresholds should be calibrated based on your hardware setup.
---

## Objective
The robot autonomously solves the maze by following the right-hand wall and stops when it reaches the black spot (finish line).
