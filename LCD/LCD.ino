#include <LiquidCrystal.h>

// 初始化 LCD：引脚顺序 (rs, enable, d4, d5, d6, d7)
LiquidCrystal lcd(12, 11, 7, 6, 5, 4);

const int buttonPin = 2;      // 按钮连接到数字引脚 7
int snackCount = 0;           // 零食数量计数
unsigned long startTime;      // 起始时间记录
bool buttonPressed = false;   // 按钮按下标志

void setup() {
  pinMode(buttonPin, INPUT_PULLUP); // 设置按钮引脚，启用内置上拉电阻
  lcd.begin(16, 2);                // 设置 LCD 为 16x2 显示器
  lcd.setCursor(0, 0);             // 设置初始光标位置
  lcd.print("SNACK: ");            // 显示初始文本
  lcd.print(snackCount);           // 显示数量
  startTime = millis();            // 记录起始时间
}

void loop() {
  unsigned long currentTime = millis();   

  // 检查按钮状态
  if (digitalRead(buttonPin) == LOW && !buttonPressed) {  // 如果按钮按下
    snackCount = 0;                                       // 清零计数
    buttonPressed = true;                                 // 防止重复按下

    // 更新 LCD 显示
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("SNACK: ");
    lcd.print(snackCount);
  } else if (digitalRead(buttonPin) == HIGH) {
    buttonPressed = false;                                // 重置按钮状态，允许再次按下
  }

  // 每过 30 分钟（1800000 毫秒），增加 snackCount
  if (currentTime - startTime >= 1800000) { // 1800000 毫秒 = 30 分钟
    snackCount++;                           // 增加数量
    startTime = currentTime;                // 重置计时器

    // 更新 LCD 显示
    lcd.clear();                            // 清除屏幕
    lcd.setCursor(0, 0);                    // 设置光标位置
    lcd.print("SNACK: ");                   // 显示 "SNACK: "
    lcd.print(snackCount);                  // 显示数量
  }
}
