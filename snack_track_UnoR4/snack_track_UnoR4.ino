#include <LiquidCrystal.h>
#include <Stepper.h>
#include <ArduinoBLE.h>

// LCD 初始化：引脚顺序 (rs, enable, d4, d5, d6, d7)
LiquidCrystal lcd(13, 12, 11, 10, 9, 8);

// 步进电机初始化
const int motorSpeed = 10; // 转速 (RPM)
const int stepsPerRev = 2048; // 电机一圈的步数
Stepper myStepper(stepsPerRev, 5, 4, 3, 2);

// 按钮引脚
const int buttonPin = 7;

// 状态变量
int snackCount = 0; // 零食计数
bool buttonPressed = false; // 按钮按下标志
bool standing = false; // 用户站立状态
unsigned long startTime; // 计时开始时间
unsigned long lastUpdateTime = 0; // 上次 snackCount 更新的时间

// BLE UUID
const char* uuid_service = "00001101-0000-1000-8000-00805f9b34fb";
const char* uuid_characteristic = "00002101-0000-1000-8000-00805f9b34fb";

BLECharacteristic stateCharacteristic;

void setup() {
    Serial.begin(9600);
    pinMode(buttonPin, INPUT_PULLUP); // 设置按钮为上拉输入
    lcd.begin(16, 2); // 初始化 LCD
    lcd.setCursor(0, 0);
    lcd.print("SNACK: ");
    lcd.print(snackCount);

    myStepper.setSpeed(motorSpeed); // 设置步进电机转速
    delay(1000); // 延迟 1 秒，等待初始化

    if (!BLE.begin()) {
        Serial.println("Failed to start BLE!");
        while (1);
    }

    BLE.setLocalName("SnackCounter");
    Serial.println("Uno R4 ready to receive BLE data...");
}

void loop() {
    BLEDevice peripheral = BLE.available();

    if (peripheral) {
        Serial.print("Found device: ");
        Serial.println(peripheral.address());
        Serial.println("Connecting...");

        if (peripheral.connect()) {
            Serial.println("Connected successfully!");

            if (peripheral.hasService(uuid_service)) {
                BLEService service = peripheral.service(uuid_service);
                stateCharacteristic = service.characteristic(uuid_characteristic);

                if (stateCharacteristic) {
                    Serial.println("Characteristic found! Ready to receive data.");
                    handlePeripheral(peripheral);
                } else {
                    Serial.println("Characteristic not found. Disconnecting...");
                }
            } else {
                Serial.println("Service not found. Disconnecting...");
            }
            peripheral.disconnect();
        } else {
            Serial.println("Failed to connect. Retrying...");
        }
    }
    BLE.scanForUuid(uuid_service); // 继续扫描
    delay(1000);
}

void handlePeripheral(BLEDevice& peripheral) {
    while (peripheral.connected()) {
        if (stateCharacteristic.valueUpdated()) {
            uint8_t state = stateCharacteristic.value()[0]; // 读取状态值
            Serial.print("Received state: ");
            Serial.println(state);

            if (state == 1) {
                standing = true;
                Serial.println("Standing detected. Timer paused.");
            } else if (state == 0) {
                standing = false;
                startTime = millis();
                Serial.println("Sitting detected. Timer resumed.");
            }
        }

        if (!standing && millis() - lastUpdateTime >= 10000) {
            snackCount++;
            lastUpdateTime = millis();
            updateLCD();
            Serial.print("Snack count incremented: ");
            Serial.println(snackCount);
        }

        if (digitalRead(buttonPin) == LOW && !buttonPressed) {
            buttonPressed = true;
            if (snackCount > 0) {
                Serial.print("Dispensing snacks: ");
                Serial.println(snackCount);
                for (int i = 0; i < snackCount; i++) {
                    myStepper.step(stepsPerRev);
                    delay(1000);
                }
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

// 更新 LCD 显示
void updateLCD() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("SNACK: ");
    lcd.print(snackCount);
}
