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
int IN2 = 10;

void setup() {
    pinMode(enable, OUTPUT);
    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);

    // 设置反转方向
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);

    Serial.begin(9600);
}

void loop() {
    analogWrite(enable, 180); // 控制转速
}
