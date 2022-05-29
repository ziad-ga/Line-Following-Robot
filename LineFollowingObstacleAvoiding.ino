#include <AFMotor.h>
#include <NewPing.h>

//Initializing motors.
AF_DCMotor leftBack(1, MOTOR12_1KHZ);
AF_DCMotor rightBack(2, MOTOR12_1KHZ);
AF_DCMotor rightFront(3, MOTOR34_1KHZ);
AF_DCMotor leftFront(4, MOTOR34_1KHZ);

//All variables and constants.
int rightIR = A5, leftIR = A4 ,
    trigPin = A1, echoPin = A0,
    lastDistSeen = 0, leftSeen, rightSeen;
const int MAX_SPEED = 45, MAX_DIST = 15;
float steerMultiplier = 2.5;
bool isAvoiding = false, isRotating = false, isReturning = false, onLine = true; //All booleans used for changing states.


NewPing sonar(trigPin, echoPin, 50);


//Set all motors to default speed.
void setupSpeed() {
  leftBack.setSpeed(MAX_SPEED);
  rightBack.setSpeed(MAX_SPEED);
  rightFront.setSpeed(MAX_SPEED);
  leftFront.setSpeed(MAX_SPEED);
}

//Move all motors forward.
void moveForward() {
  leftBack.run(BACKWARD);
  leftFront.run(FORWARD);
  rightBack.run(FORWARD);
  rightFront.run(BACKWARD);
}

//Stop moving all motors.
void stopAll() {
  leftBack.run(RELEASE);
  leftFront.run(RELEASE);
  rightBack.run(RELEASE);
  rightFront.run(RELEASE);
}

//Turn right
void steerRight() {
  leftBack.setSpeed(steerMultiplier * MAX_SPEED);
  leftFront.setSpeed(steerMultiplier * MAX_SPEED);
  rightBack.setSpeed(MAX_SPEED);
  rightFront.setSpeed(MAX_SPEED);

  leftBack.run(BACKWARD);
  leftFront.run(FORWARD);

  rightBack.run(BACKWARD);
  rightFront.run(FORWARD);
}

//Turn left
void steerLeft() {
  rightBack.setSpeed(steerMultiplier * MAX_SPEED);
  rightFront.setSpeed(steerMultiplier * MAX_SPEED);
  leftBack.setSpeed(MAX_SPEED);
  leftFront.setSpeed(MAX_SPEED);

  rightBack.run(FORWARD);
  rightFront.run(BACKWARD);

  leftBack.run(FORWARD);
  leftFront.run(BACKWARD);

}



//Steer left in a wide radius.
void steerLeftWide() {
  rightBack.setSpeed(MAX_SPEED + 2);
  rightFront.setSpeed(MAX_SPEED + 2);
  leftBack.setSpeed(MAX_SPEED + 2);
  leftFront.setSpeed(MAX_SPEED + 2);

  leftBack.run(BACKWARD);
  leftFront.run(FORWARD);
  rightBack.run(FORWARD);
  rightFront.run(BACKWARD);

}

//Stop any kind of steering and move forward.
void stopSteer() {
  setupSpeed();
  moveForward();
}


void setup() {
  Serial.begin(9600);
  delay(2000);
  setupSpeed();
  moveForward();
}


void loop() {
  //Get readings from all sensors.
  leftSeen = digitalRead(leftIR);
  rightSeen = digitalRead(rightIR);
  lastDistSeen = sonar.ping_cm();

  //Avoiding state
  if (isAvoiding) {

    //If was avoiding and now path is clear, stop avoiding and enter returning to line state.
    if (lastDistSeen == 0 || lastDistSeen > MAX_DIST) {
      isAvoiding = false;
      delay(500);
      isReturning = true;
    }
  }

  //Not in avoiding state, so either going back to the line or already on line.
  else {

    //Returning to the line state.
    if (isReturning) {
      steerLeft();
      delay(70);
      steerLeftWide();
      delay(80);

    }

    // If there is an object in range, leave the line and enter avoiding state.
    if (lastDistSeen < MAX_DIST && lastDistSeen > 0) {
      isAvoiding = true;
      onLine = false;
      steerRight();
    }

    //After making sure path is clear:

    //If on line, perform standard staying on line procedure.
    else if (onLine) {
      if (isRotating && !(leftSeen || rightSeen)) {
        isRotating = false;
        stopSteer();
      }
      else if (leftSeen && rightSeen) {
        stopAll();
      }
      else if (leftSeen) {
        isRotating = true;
        steerLeft();
      }
      else if (rightSeen) {
        isRotating = true;
        steerRight();
      }
    }

    //If not on line, any IR reading indicates we are back on line.
    else if (leftSeen || rightSeen) {
      onLine = true;
      isReturning = false;
      steerRight();
      delay(350);
    }
  }
}
