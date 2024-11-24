// #include <Stepper.h>

// int motorSpeed = 5; //RPM
// int stepsPerRev = 2048;
// Stepper myStepper(stepsPerRev, 5, 4, 3, 2);

// void setup() {
//   // put your setup code here, to run once:
//   myStepper.setSpeed(motorSpeed);
// }

// void loop() {
//   // put your main code here, to run repeatedly:
//    myStepper.step(stepsPerRev);
//    delay(1000);
// }

#include <Stepper.h>

int motorSpeed = 5; // 转速 (RPM)
int stepsPerRev = 2048; // 电机每转一圈的步数

// 两个步进电机的实例，使用相同的引脚
Stepper motor1(stepsPerRev, 5, 4, 3, 2);
Stepper motor2(stepsPerRev, 5, 4, 3, 2);

void setup() {
  // 设置两个电机的速度
  motor1.setSpeed(motorSpeed);
  motor2.setSpeed(motorSpeed);
}

void loop() {
  // 让两个电机同时运行
  motor1.step(stepsPerRev);
  motor2.step(stepsPerRev);
  delay(1000); // 等待 1 秒
}
