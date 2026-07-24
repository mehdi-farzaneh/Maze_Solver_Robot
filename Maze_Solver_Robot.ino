
#include "Wire.h"
#include "MazeRobotLibrary.h"

// ============================================================================
// SENSOR INSTANCES
// ============================================================================

/* Uncomment if using ToF (Time of Flight) sensors instead of ultrasonic
ToFSensor leftSensor(21, 2);    // left ToF sensor (address 21, shutdown pin 2)
ToFSensor midSensor(22, 4);     // center ToF sensor (address 22, shutdown pin 4)
ToFSensor rightSensor(23, 3);   // right ToF sensor (address 23, shutdown pin 3)
*/

// Ultrasonic (HC-SR04) sensors for distance measurement
UltrasonicSensor rightSensor(2, 3);   // right sensor (trigger 2, echo 3)
UltrasonicSensor midSensor(4, 5);     // center sensor (trigger 4, echo 5)
UltrasonicSensor leftSensor(6, 7);    // left sensor (trigger 6, echo 7)

// ============================================================================
// MOTOR INSTANCE
// ============================================================================

// Dual DC motor controller via L298N driver
// Right motor: direction pins 8-9, PWM pin 10
// Left motor: direction pins 7-12, PWM pin 11
DCMotor motor(9, 8, 10, 7, 12, 11);

// ============================================================================
// SENSOR CALIBRATION PARAMETERS
// ============================================================================

#define PHOTO_RESISTOR A0              // Photo resistor analog input pin
#define SIDE_WALL_THRESHOLD 75         // Distance to side walls (mm) - requires calibration
#define FRONT_WALL_THRESHOLD 75        // Distance to front wall (mm) - requires calibration
#define BLACK_SPOT_THRESHOLD 350       // Photo resistor resistance value at black surface
#define MOTOR_SPEED 65                 // Motor PWM speed (0-255)

// ============================================================================
// STATE VARIABLES
// ============================================================================

bool atBlackSpot = false;              // Maze completion flag

void setup() {
  Wire.begin();

  // ========================================================================
  // Initialize distance sensors
  // ========================================================================
  rightSensor.begin();
  midSensor.begin();
  leftSensor.begin();

  /* Uncomment if using ToF sensors instead of ultrasonic sensors
  rightSensor.changeAddress();
  midSensor.changeAddress();
  leftSensor.changeAddress();
  rightSensor.init();
  midSensor.init();
  leftSensor.init();
  */

  // ========================================================================
  // Initialize motors
  // ========================================================================
  motor.begin();
  motor.stop();  // Ensure motors start in stopped state

  // ========================================================================
  // Initialize black spot detection sensor
  // ========================================================================
  pinMode(PHOTO_RESISTOR, INPUT);
}

void loop() {
  // ========================================================================
  // Check if maze is completed (black spot detected)
  // ========================================================================
  atBlackSpot = (analogRead(PHOTO_RESISTOR) > BLACK_SPOT_THRESHOLD);

  // Main maze solving loop - continues until black spot is reached
  while (!atBlackSpot) {
    // ====================================================================
    // Periodic check for completion signal
    // ====================================================================
    if (analogRead(PHOTO_RESISTOR) > BLACK_SPOT_THRESHOLD) {
      atBlackSpot = true;
      motor.stop();
      break;
    }

    // ====================================================================
    // Read distance measurements from all three sensors
    // ====================================================================
    int rightDistance = rightSensor.distance();
    int frontDistance = midSensor.distance();
    int leftDistance = leftSensor.distance();

    // ====================================================================
    // Emergency stop: obstacle detected directly ahead
    // ====================================================================
    if (frontDistance < FRONT_WALL_THRESHOLD) {
      motor.stop();
    }

    // ====================================================================
    // Balancing logic: Keep robot centered between left and right walls
    // (tolerance: 10mm difference)
    // ====================================================================
    int distanceDifference = rightDistance - leftDistance;
    if (distanceDifference > 10) {
      // Right side is farther away - tilt right to center
      motor.tiltRight(MOTOR_SPEED);
      delay(250);  // Allow time for adjustment
      break;
    } else if (distanceDifference < -10) {
      // Left side is farther away - tilt left to center
      motor.tiltLeft(MOTOR_SPEED);
      delay(250);  // Allow time for adjustment
      break;
    }

    // ====================================================================
    // Right-hand wall following algorithm
    // Strategy: Keep right hand on wall and turn when opening is found
    // ====================================================================
    if (rightDistance > SIDE_WALL_THRESHOLD) {
      // No wall on right side - opening detected, turn right 90 degrees
      delay(500);                        // Wait to clear previous position
      motor.turnRight(MOTOR_SPEED);
      delay(1000);                       // Wait for 90 degree turn to complete
      motor.moveForward(MOTOR_SPEED);
      delay(3000);                       // Move forward one maze block
    } else if (frontDistance < FRONT_WALL_THRESHOLD && rightDistance < SIDE_WALL_THRESHOLD) {
      // Wall ahead AND wall on right - dead end detected, turn around 180 degrees
      delay(500);                        // Wait to clear previous position
      motor.turnAround(MOTOR_SPEED);
      delay(2000);                       // Wait for 180 degree turn to complete
    } else {
      // Default behavior: corridor is clear ahead, move forward
      motor.moveForward(MOTOR_SPEED);
    }

    delay(200);  // Main control loop cycle time
  }
}

