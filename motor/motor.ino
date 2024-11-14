#include <Stepper.h>

int motorSpeed = 10; //RPM
int stepsPerRev = 2048;
Stepper myStepper(stepsPerRev, 8, 9, 10, 11);

void setup() {
  // put your setup code here, to run once:
  myStepper.setSpeed(motorSpeed);
}

void loop() {
  // put your main code here, to run repeatedly:
   myStepper.step(stepsPerRev);
   delay(1000);
}