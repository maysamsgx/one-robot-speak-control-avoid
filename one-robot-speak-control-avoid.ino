#include <Servo.h>      // Include Servo library for controlling the servo motor
#include <AFMotor.h>    // Include Adafruit Motor Shield library for DC motors

// Pin definitions
#define ECHO_PIN        A0    // Ultrasonic sensor echo pin
#define TRIG_PIN        A1    // Ultrasonic sensor trigger pin
#define SERVO_PIN       9     // Servo signal pin (Arduino D9)
#define MOTOR_SPEED     180   // PWM speed for all four DC motors (0ΓÇô255)

// Servo angles and timing constants
#define LEFT_ANGLE       150   // Servo angle to scan left side
#define RIGHT_ANGLE      30    // Servo angle to scan right side
#define CENTER_ANGLE     103   // Servo ΓÇ£homeΓÇ¥ or center position
#define SCAN_DELAY_MS    400   // Delay after moving servo before reading distance
#define TURN_DURATION_MS 450   // How long to run motors to achieve ~90┬░ turn

// Create global objects
Servo servo;                        // Servo object
AF_DCMotor motor1(1),               // Motor shield channel 1
           motor2(2),               // Motor shield channel 2
           motor3(3),               // Motor shield channel 3
           motor4(4);               // Motor shield channel 4

void setup() {
  Serial.begin(9600);              // Start serial communication at 9600 baud
  pinMode(TRIG_PIN, OUTPUT);       // Set trigger pin as output
  pinMode(ECHO_PIN, INPUT);        // Set echo pin as input

  servo.attach(SERVO_PIN);         // Attach servo to its pin
  smoothServoWrite(CENTER_ANGLE);  // Move servo to center on startup

  // Initialize all motors at defined speed
  motor1.setSpeed(MOTOR_SPEED);
  motor2.setSpeed(MOTOR_SPEED);
  motor3.setSpeed(MOTOR_SPEED);
  motor4.setSpeed(MOTOR_SPEED);
}

void loop() {
  // If a byte arrives on Serial, handle it as a manual command
  if (Serial.available() > 0) {
    char cmd = Serial.read();               // Read single character
    Serial.print("Command received: ");
    Serial.println(cmd);
    handleCommand(cmd);                     // Execute manual control
  }
  else {
    autonomousObstacleAvoidance();          // No command ΓåÆ do obstacle avoidance
  }
}

// Interpret Bluetooth or voice command characters
void handleCommand(char c) {
  switch (c) {
    case 'F': case '^':                     // 'F' or '^' means go forward
      Serial.println("Forward");
      moveForward();
      break;

    case 'B': case '-':                     // 'B' or '-' means go backward
      Serial.println("Backward");
      moveBackward();
      break;

    case 'L': case '<':                     // 'L' or '<' means turn left
      Serial.println("Left");
      turnLeft();
      break;

    case 'R': case '>':                     // 'R' or '>' means turn right
      Serial.println("Right");
      turnRight();
      break;

    case 'S': case '*':                     // 'S' or '*' means stop
      Serial.println("Stop");
      stopMotion();
      break;

    default:                                // Any other char ΓåÆ emergency stop
      stopMotion();
      break;
  }
}

// Continuously check distance ahead and steer around obstacles
void autonomousObstacleAvoidance() {
  int frontDistance = readUltrasonic();     // Measure distance ahead
  if (frontDistance <= 12) {                // If obstacle is closer than 12 cm
    stopMotion();                           // Stop forward motion
    moveBackward();                         // Back up slightly
    delay(100);
    stopMotion();

    // Scan left and right distances by moving servo
    int leftDistance  = scanAt(LEFT_ANGLE);
    int rightDistance = scanAt(RIGHT_ANGLE);

    smoothServoWrite(CENTER_ANGLE);         // Re-center servo
    delay(200);

    // Turn toward the side with more free space
    if (leftDistance > rightDistance) {
      Serial.println("Turning left");
      turnLeft();
    } else {
      Serial.println("Turning right");
      turnRight();
    }
    delay(TURN_DURATION_MS);                // Run turn long enough for ~90┬░
    stopMotion();
    delay(200);
  }
  else {
    moveForward();                          // No obstacle ΓåÆ keep moving forward
  }
}

// Trigger ultrasonic pulse and read echo time, return distance in cm
int readUltrasonic() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(4);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  long duration = pulseIn(ECHO_PIN, HIGH); // Measure echo pulse width
  return duration / 29 / 2;                // Convert microseconds to cm
}

// Move servo to a specified angle, wait, then measure distance
int scanAt(int angle) {
  smoothServoWrite(angle);                 // Rotate servo smoothly
  delay(SCAN_DELAY_MS);                    // Allow servo to settle
  return readUltrasonic();                 // Return measured distance
}

// Smoothly increment or decrement servo until reaching targetAngle
void smoothServoWrite(int targetAngle) {
  int current = servo.read();              // Get current servo angle
  while (current != targetAngle) {
    if (current < targetAngle) current++;
    else                current--;
    servo.write(current);                  // Move one degree at a time
    delay(5);                              // Small delay for smooth motion
  }
}

// Drive all four motors forward
void moveForward() {
  motor1.run(FORWARD); motor2.run(FORWARD);
  motor3.run(FORWARD); motor4.run(FORWARD);
}

// Drive all four motors backward
void moveBackward() {
  motor1.run(BACKWARD); motor2.run(BACKWARD);
  motor3.run(BACKWARD); motor4.run(BACKWARD);
}

// Spin in place to the right: left wheels backward, right wheels forward
void turnRight() {
  motor1.run(BACKWARD); motor2.run(BACKWARD);
  motor3.run(FORWARD);  motor4.run(FORWARD);
}

// Spin in place to the left: left wheels forward, right wheels backward
void turnLeft() {
  motor1.run(FORWARD);  motor2.run(FORWARD);
  motor3.run(BACKWARD); motor4.run(BACKWARD);
}

// Stop all motors immediately
void stopMotion() {
  motor1.run(RELEASE);  motor2.run(RELEASE);
  motor3.run(RELEASE);  motor4.run(RELEASE);
}
