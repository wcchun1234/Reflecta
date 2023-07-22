#include <Servo.h>  // Include the Servo library which allows you to control servo motors.

// Defines the number of pins that are using.
#define NUM_SERVOS 9
#define STEP_DELAY 15    // The delay in milliseconds between each step the servos make.
#define PAUSE_DELAY 500  // The delay in milliseconds between reaching the target position and choosing a new one.
#define TRIGGER_PIN 12
#define ECHO_PIN 11
#define MAX_DISTANCE 30  // The maximum distance in cm at which you want the sensor to react.

Servo servos[NUM_SERVOS];                                    // Creates an array of Servo objects.
int servoPins[NUM_SERVOS] = { 2, 3, 4, 5, 6, 7, 8, 9, 10 };  // An array that holds the pins where your servo control wires are connected.
int targetPos[NUM_SERVOS];                                   // An array to store the target position of each servo.
int currentPos[NUM_SERVOS];                                  // An array to store the current position of each servo.
int step[NUM_SERVOS];                                        // An array to store the step size of each servo.
unsigned long lastChange[NUM_SERVOS];                        // An array to store the time of the last change of each servo.

// This function is called once when the program starts.
void setup() {
  randomSeed(analogRead(0));  // Initialize random number generator with a seed value.
  pinMode(TRIGGER_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  Serial.begin(9600);

  // Loop through each servo.
  for (int i = 0; i < NUM_SERVOS; i++) {
    servos[i].attach(servoPins[i]);                     // Attach each servo to its corresponding pin.
    targetPos[i] = random(0, 180);                      // Randomly set a target position for each servo.
    currentPos[i] = 90;                                 // Set the initial current position to the middle.
    step[i] = (targetPos[i] > currentPos[i]) ? 1 : -1;  // Determine the step direction based on the current and target position.
    servos[i].write(currentPos[i]);                     // Move the servo to its initial position.
    lastChange[i] = millis();                           // Record the time of the last change.
  }
}

// This function loops repeatedly after the setup() has finished.
void loop() {
  long duration, distance;
  digitalWrite(TRIGGER_PIN, LOW);
  delayMicroseconds(2);  // Wait for 2 microseconds.
  digitalWrite(TRIGGER_PIN, HIGH);
  delayMicroseconds(10);  // Wait for 10 microseconds.
  digitalWrite(TRIGGER_PIN, LOW);
  duration = pulseIn(ECHO_PIN, HIGH);  // Reads the echo pin, returns the sound wave travel time in microseconds.
  distance = (duration / 2) / 29.1;    // Calculate the distance.

  // Print the distance value to the Serial Monitor.
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  if (distance < MAX_DISTANCE) {                              // If the distance measured by the sensor is less than the maximum distance.
    for (int i = 0; i < NUM_SERVOS; i++) {                    // Loop through each servo.
      if (millis() - lastChange[i] >= PAUSE_DELAY) {          // If we've paused long enough.
        if (currentPos[i] == targetPos[i]) {                  // If we've reached the target position.
          targetPos[i] = random(0, 180);                      // Choose a new target position.
          step[i] = (targetPos[i] > currentPos[i]) ? 1 : -1;  // Determine the new step direction.
          lastChange[i] = millis();                           // Reset the change time.
        } else {
          currentPos[i] += step[i];        // Move one step towards the target position.
          servos[i].write(currentPos[i]);  // Write the new position to the servo.
        }
      }
    }
  } else {
    for (int i = 0; i < NUM_SERVOS; i++) {
      servos[i].write(90);  // Move the servo to the middle when no hand is detected.
    }
  }
  delay(STEP_DELAY);  // Delay between each step.
}
