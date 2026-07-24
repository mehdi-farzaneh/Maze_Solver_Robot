#ifndef MAZE_ROBOT_LIBRARY_H
#define MAZE_ROBOT_LIBRARY_H

#include "VL53L0X.h"
#include "Wire.h"

// ****************************************************************************
// ToF_Sensor Class - Time of Flight Distance Sensor
// Uses VL53L0X I2C sensor for precise distance measurement
// ****************************************************************************
class ToFSensor
{
private:
  byte address;              // I2C address of the sensor
  byte shutdownPin;          // GPIO pin for sensor shutdown/enable control
  VL53L0X distanceSensor;    // VL53L0X sensor object

public:
  /// Constructor
  /// @param sensorAddress I2C address of the ToF sensor
  /// @param shutdownPin GPIO pin used for sensor enable control
  ToFSensor(byte sensorAddress, byte shutdownPin)
    : address(sensorAddress), shutdownPin(shutdownPin) {}

  /// Initialize shutdown pin as output and reset to LOW
  void begin() {
    pinMode(shutdownPin, OUTPUT);
    digitalWrite(shutdownPin, LOW);
    delay(10);
  }

  /// Change sensor I2C address by enabling with shutdown pin
  /// Must be called after begin() and only one sensor should be enabled at a time
  void changeAddress() {
    digitalWrite(shutdownPin, HIGH);
    delay(10);
    distanceSensor.setAddress((uint8_t)address);
    digitalWrite(shutdownPin, LOW);
  }

  /// Initialize the ToF sensor with high accuracy continuous measurement mode
  /// Configures measurement timing budget and starts continuous ranging
  void init() {
    digitalWrite(shutdownPin, HIGH);
    if (!distanceSensor.init()) {
      while (1); // Halt if initialization fails
    }
    // Configure for continuous measurement in HIGH ACCURACY mode
    distanceSensor.setTimeout(500);
    distanceSensor.setMeasurementTimingBudget(200000); // 200ms budget
    distanceSensor.startContinuous();
  }

  /// Read distance from sensor in continuous measurement mode
  /// @return Distance in millimeters
  int distance() {
    return distanceSensor.readRangeContinuousMillimeters();
  }
};

// ****************************************************************************
// Us_Sensor Class - Ultrasound Distance Sensor (HC-SR04 compatible)
// Measures distance using ultrasonic pulse echo method
// ****************************************************************************
class UltrasonicSensor
{
private:
  byte trigPin;    // GPIO pin for trigger signal (5V pulse input)
  byte echoPin;    // GPIO pin for echo signal (pulse length output)

  // Conversion constant: speed of sound ~340 m/s = 0.34 mm/µs
  static constexpr float SPEED_OF_SOUND_CONVERSION = 0.34 / 2.0;

public:
  /// Constructor
  /// @param trigPin GPIO pin connected to HC-SR04 trigger pin
  /// @param echoPin GPIO pin connected to HC-SR04 echo pin
  UltrasonicSensor(byte trigPin, byte echoPin)
    : trigPin(trigPin), echoPin(echoPin) {}

  /// Configure trigger and echo pins as outputs/inputs
  void begin() {
    pinMode(echoPin, INPUT);
    pinMode(trigPin, OUTPUT);
  }

  /// Measure distance using ultrasonic pulse method
  /// Sends 10µs trigger pulse and measures echo time duration
  /// @return Distance in millimeters
  long distance() {
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    // Measure pulse duration (timeout 20ms)
    long duration = pulseIn(echoPin, HIGH, 20000);

    // Convert duration to distance: distance = (speed * time) / 2
    return (long)(duration * SPEED_OF_SOUND_CONVERSION);
  }
};

// ****************************************************************************
// DC_Motor Class - Dual DC Motor Control via L298N Motor Driver
// Controls two independent DC motors with PWM speed control and direction
// ****************************************************************************
class DCMotor
{
private:
  // Right Motor pins
  byte rightMotorPin1;      // Direction control pin 1
  byte rightMotorPin2;      // Direction control pin 2
  byte rightMotorPwm;       // PWM speed control pin

  // Left Motor pins
  byte leftMotorPin1;       // Direction control pin 1
  byte leftMotorPin2;       // Direction control pin 2
  byte leftMotorPwm;        // PWM speed control pin

  /// Set motor direction and speed
  /// @param dir1 First direction pin value (HIGH/LOW)
  /// @param dir2 Second direction pin value (HIGH/LOW)
  /// @param pwmPin PWM pin for speed control
  /// @param pwmSpeed PWM value (0-255)
  void setMotorSpeed(byte dir1Pin, byte dir2Pin, byte pwmPin, byte pwmSpeed) {
    digitalWrite(dir1Pin, dir1Pin == rightMotorPin1 ? (dir1 ? HIGH : LOW) : (dir1 ? HIGH : LOW));
    digitalWrite(dir2Pin, dir2Pin == rightMotorPin2 ? (dir2 ? HIGH : LOW) : (dir2 ? HIGH : LOW));
    analogWrite(pwmPin, pwmSpeed);
  }

  /// Set both motors to rotate in same direction
  /// @param rightDir1 Right motor pin 1 value
  /// @param rightDir2 Right motor pin 2 value
  /// @param leftDir1 Left motor pin 1 value
  /// @param leftDir2 Left motor pin 2 value
  /// @param pwmSpeed Motor speed (0-255)
  void setMotorDirection(byte rightDir1, byte rightDir2, byte leftDir1, byte leftDir2, byte pwmSpeed) {
    analogWrite(rightMotorPwm, pwmSpeed);
    digitalWrite(rightMotorPin1, rightDir1);
    digitalWrite(rightMotorPin2, rightDir2);
    analogWrite(leftMotorPwm, pwmSpeed);
    digitalWrite(leftMotorPin1, leftDir1);
    digitalWrite(leftMotorPin2, leftDir2);
  }

public:
  /// Constructor - Configure motor control pins
  /// @param rm1 Right motor direction pin 1
  /// @param rm2 Right motor direction pin 2
  /// @param rmPwm Right motor PWM pin
  /// @param lm1 Left motor direction pin 1
  /// @param lm2 Left motor direction pin 2
  /// @param lmPwm Left motor PWM pin
  DCMotor(byte rm1, byte rm2, byte rmPwm, byte lm1, byte lm2, byte lmPwm)
    : rightMotorPin1(rm1), rightMotorPin2(rm2), rightMotorPwm(rmPwm),
      leftMotorPin1(lm1), leftMotorPin2(lm2), leftMotorPwm(lmPwm) {}

  /// Initialize all motor control pins as outputs
  void begin() {
    pinMode(rightMotorPin1, OUTPUT);
    pinMode(rightMotorPin2, OUTPUT);
    pinMode(rightMotorPwm, OUTPUT);
    pinMode(leftMotorPin1, OUTPUT);
    pinMode(leftMotorPin2, OUTPUT);
    pinMode(leftMotorPwm, OUTPUT);
  }

  /// Move robot forward at specified speed
  /// @param pwmSpeed Motor PWM value (0-255)
  void moveForward(byte pwmSpeed) {
    setMotorDirection(HIGH, LOW, HIGH, LOW, pwmSpeed);
  }

  /// Rotate robot 90 degrees to the right
  /// Right motor spins backward, left motor spins forward
  /// @param pwmSpeed Motor PWM value (0-255)
  void turnRight(byte pwmSpeed) {
    setMotorDirection(LOW, HIGH, HIGH, LOW, pwmSpeed);
  }

  /// Rotate robot 180 degrees (U-turn)
  /// Right motor backward, left motor forward
  /// @param pwmSpeed Motor PWM value (0-255)
  void turnAround(byte pwmSpeed) {
    setMotorDirection(LOW, HIGH, HIGH, LOW, pwmSpeed);
  }

  /// Slight turn to the right (minor adjustment)
  /// @param pwmSpeed Motor PWM value (0-255)
  void tiltRight(byte pwmSpeed) {
    setMotorDirection(LOW, HIGH, HIGH, LOW, pwmSpeed);
  }

  /// Slight turn to the left (minor adjustment)
  /// @param pwmSpeed Motor PWM value (0-255)
  void tiltLeft(byte pwmSpeed) {
    setMotorDirection(HIGH, LOW, LOW, HIGH, pwmSpeed);
  }

  /// Stop both motors immediately
  void stop() {
    analogWrite(rightMotorPwm, 0);
    analogWrite(leftMotorPwm, 0);
    delay(500); // Brief hold before next command
  }
};

#endif // MAZE_ROBOT_LIBRARY_H

