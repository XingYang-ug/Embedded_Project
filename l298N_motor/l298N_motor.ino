// #include <ezButton.h>
// int pushButton = 7;
// ezButton button(pushButton);
// int enable = 2;
// int IN1 = 3;

// void setup() {
//     button.setDebounceTime(100);
//     button.setCountMode(COUNT_FALLING);
//     pinMode(IN1, OUTPUT);
//     pinMode(pushButton, INPUT_PULLUP);
//     digitalWrite(IN1, HIGH);

//     Serial.begin(9600);
// }

// void loop() {
//   button.loop();
//   unsigned long countThroughPushButton = button.getCount();
//   Serial.println(countThroughPushButton);
//   switch(countThroughPushButton) {
//     case 1:
//       analogWrite(enable, 43);
//       break;
//     case 2:
//       analogWrite(enable, 86);
//       break;
//     case 3:
//       analogWrite(enable, 129);
//       break;
//     default:
//       button.resetCount();
//       analogWrite(enable, 0);
//   }
// }

int enable = 8;  // L298N 的 ENA 引脚（PWM 输出）
int IN1 = 9;     // L298N 的 IN1 引脚
int IN2 = 10;    // L298N 的 IN2 引脚
const int motorRPM = 260; // 电机转速 (RPM)
const int motorSpeed = 180; // PWM 信号占空比 (0-255)
const float rotations = 1.0; // 转动圈数

void setup() {
    pinMode(enable, OUTPUT);
    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);

    Serial.begin(9600);

    // // 设置电机正转方向
    // digitalWrite(IN1, LOW);
    // digitalWrite(IN2, HIGH);

    // // 计算转动一圈所需时间 (毫秒)
    // unsigned long timeToRotate = (rotations * 60 * 1000) / motorRPM;

    // Serial.print("Time to rotate one revolution: ");
    // Serial.print(timeToRotate);
    // Serial.println(" ms");

    // // 启动电机
    // analogWrite(enable, motorSpeed);
    // delay(timeToRotate); // 运行指定时间

    // // 停止电机
    // analogWrite(enable, 0);
    // Serial.println("Motor stopped.");
}

void loop() {
    // 保持空循环，不再执行其他操作
    // 设置电机正转方向
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);

    // 计算转动一圈所需时间 (毫秒)
    unsigned long timeToRotate = (rotations * 60 * 1000) / (motorSpeed-25);

    Serial.print("Time to rotate one revolution: ");
    Serial.print(timeToRotate);
    Serial.println(" ms");

    // 启动电机
    analogWrite(enable, motorSpeed);
    delay(timeToRotate); // 运行指定时间

    // 停止电机
    analogWrite(enable, 0);
    Serial.println("Motor stopped.");
    delay(100000000);
}
