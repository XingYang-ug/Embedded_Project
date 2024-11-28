#include <LiquidCrystal.h>
#include <Stepper.h>
#include <ArduinoBLE.h>

// LCD 初始化：引脚顺序 (rs, enable, d4, d5, d6, d7)
LiquidCrystal lcd(2, 3, 4, 5, 6, 7);

// 步进电机初始化
int enable = 8;  // L298N 的 ENA 引脚（PWM 输出）
int IN1 = 9;     // L298N 的 IN1 引脚
int IN2 = 10;
const int motorRPM = 260; // 电机转速 (RPM)
const int motorSpeed = 180; // PWM 信号占空比 (0-255)

// 按钮引脚
const int buttonPin = 7;

// 状态变量
int snackCount = 0; // 零食计数
bool buttonPressed = false; // 按钮按下标志
bool standing = false; // 用户站立状态
unsigned long startTime; // 计时开始时间
unsigned long lastUpdateTime = 0; // 上次 snackCount 更新的时间

// BLE UUID
const char* uuid_service = "1101";
const char* uuid_characteristic = "1102";

BLECharacteristic stateCharacteristic;
BLEDevice peripheral;

void updateLCD() {
    lcd.setCursor(0, 0);
    lcd.print("SNACK: ");
    lcd.setCursor(7, 0);
    lcd.print(snackCount);
    lcd.print("  "); // 防止残留字符
}

void motor(int rotations) {
  // 计算转动时间并延迟
    analogWrite(enable, motorSpeed);
    unsigned long timeToRotate = (rotations * 60 * 1000) / motorRPM; // 转动时间 (毫秒)
    delay(timeToRotate);

    // 停止电机
    analogWrite(enable, 0);
}

void setup() {
    Serial.begin(9600);
    pinMode(buttonPin, INPUT_PULLUP); // 设置按钮为上拉输入
    lcd.begin(16, 2);
    lcd.setCursor(0, 0);
    lcd.print("SNACK: ");
    lcd.print(snackCount);

    pinMode(enable, OUTPUT);
    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);
    // 设置反转方向
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);

    if (!BLE.begin()) {
        Serial.println("Failed to start BLE!");
        while (1);
    }

    BLE.setLocalName("SnackCounter");
    BLE.advertise();
    BLE.scanForUuid(uuid_service); // 搜索指定服务 UUID
    Serial.println("Scanning for BLE devices...");
}

void loop() {
    while (!peripheral) {
      // Serial.println("Finding peripheral...");
      peripheral = BLE.available();
    };

    if (peripheral) {
        Serial.print("Found device: ");
        Serial.println(peripheral.address());
        Serial.println("Connecting...");

        if (peripheral.connect()) {
            Serial.println("Connected successfully!");
            handlePeripheral(peripheral);
        } else {
            Serial.println("Failed to connect. Retrying...");
        }
    }
    delay(1000);
}

void handlePeripheral(BLEDevice& peripheral) {
    while (peripheral.connected()) {
        if (peripheral.discoverAttributes()) {
            BLECharacteristic characteristic = peripheral.characteristic(uuid_characteristic);
            uint8_t state;
            characteristic.readValue(state); // 读取状态值
            // Serial.print("Received state: ");
            // Serial.println(state);

            // 根据 state 更新状态
            if (state == 1) { // 检测到站立
                standing = true;
                Serial.println("Standing detected. Timer paused.");
            } 
            // TODO: another button for rst: standing = false etc

             // 如果是坐下状态，继续计时逻辑
            if (!standing && millis() - lastUpdateTime >= 10000) {
                snackCount++;
                lastUpdateTime = millis();
                updateLCD();
                Serial.print("Snack count incremented: ");
                Serial.println(snackCount);
            }

            // 处理按钮逻辑
            if (standing && digitalRead(buttonPin) == LOW && !buttonPressed) {
                buttonPressed = true;

                // 清零 snackCount 并分发零食
                if (snackCount > 0) {
                    Serial.print("Dispensing snacks: ");
                    Serial.println(snackCount);
                    motor(snackCount);
                    snackCount = 0;
                    updateLCD();
                } else {
                    Serial.println("No snacks to dispense.");
                }
            } else if (digitalRead(buttonPin) == HIGH) {
                buttonPressed = false;
            }
        }
    }
}