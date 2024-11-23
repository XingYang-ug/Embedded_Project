#include <Arduino_LSM6DS3.h>
#include <ArduinoBLE.h>

const char* uuid_service = "1101";
const char* uuid_characteristic = "2101";

BLEService snackService(uuid_service);
BLEByteCharacteristic snackCharacteristic(uuid_characteristic, BLERead | BLENotify);

const int numSamples = 100; // 基线采集样本数量
float baselineMean[3];      // 基线均值
float baselineStdDev[3];    // 基线标准差

float threshold = 20.0;     // 阈值（调节灵敏度）
int standingCount = 0;      // 检测到站立的计数器
const int standingThreshold = 4; // 连续站立次数阈值

void setup() {
  Serial.begin(9600);
  while (!Serial);

  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");
    while (1);
  }

  if (!BLE.begin()) {
    Serial.println("Failed to start BLE!");
    while (1);
  }

  BLE.setLocalName("SnackMonitor");
  BLE.setAdvertisedService(snackService);
  snackService.addCharacteristic(snackCharacteristic);
  BLE.addService(snackService);
  BLE.advertise();

  Serial.println("BLE and IMU initialized!");

  // 收集基线数据
  collectBaseline();
}

void loop() {
  BLEDevice central = BLE.central();

  if (central) {
    Serial.print("Connected to central: ");
    Serial.println(central.address());

    while (central.connected()) {
      if (IMU.accelerationAvailable()) {
        float x, y, z;
        IMU.readAcceleration(x, y, z);

        // 判断是否站立
        if (isStanding(x, y, z)) {
          standingCount++;
          Serial.println("Standing detected!");
        } else {
          standingCount = 0; // 若未连续检测到站立，重置计数器
        }

        // 当连续站立次数达到阈值时，通过 BLE 发送站立状态
        if (standingCount >= standingThreshold) {
          snackCharacteristic.writeValue(1); // 1 表示站立
          Serial.println("BLE Sent: Standing");
          standingCount = 0; // 重置计数器
        }
      }
      delay(100); // 数据采集间隔
    }
    Serial.println("Disconnected from central");
  }
}

void collectBaseline() {
  Serial.println("Collecting baseline data... Please sit still.");
  float baselineData[numSamples][3];

  for (int i = 0; i < numSamples; i++) {
    if (IMU.accelerationAvailable()) {
      float x, y, z;
      IMU.readAcceleration(x, y, z);
      baselineData[i][0] = x;
      baselineData[i][1] = y;
      baselineData[i][2] = z;
      delay(100);
    }
  }

  for (int axis = 0; axis < 3; axis++) {
    float sum = 0.0;
    for (int i = 0; i < numSamples; i++) {
      sum += baselineData[i][axis];
    }
    baselineMean[axis] = sum / numSamples;

    float variance = 0.0;
    for (int i = 0; i < numSamples; i++) {
      variance += pow(baselineData[i][axis] - baselineMean[axis], 2);
    }
    baselineStdDev[axis] = sqrt(variance / numSamples);
  }

  Serial.println("Baseline Mean and Standard Deviation:");
  Serial.print("X-axis mean: ");
  Serial.println(baselineMean[0]);
  Serial.print("Y-axis mean: ");
  Serial.println(baselineMean[1]);
  Serial.print("Z-axis mean: ");
  Serial.println(baselineMean[2]);

  Serial.print("X-axis stddev: ");
  Serial.println(baselineStdDev[0]);
  Serial.print("Y-axis stddev: ");
  Serial.println(baselineStdDev[1]);
  Serial.print("Z-axis stddev: ");
  Serial.println(baselineStdDev[2]);

  Serial.println("Baseline data collected.");
}

bool isStanding(float x, float y, float z) {
  // 判断加速度是否显著偏离基线
  if (fabs(x - baselineMean[0]) > baselineStdDev[0] * threshold ||
      fabs(y - baselineMean[1]) > baselineStdDev[1] * threshold ||
      fabs(z - baselineMean[2]) > baselineStdDev[2] * threshold) {
    return true; // 偏离较大，判断为站立
  }
  return false; // 偏离不显著，判断为非站立
}
