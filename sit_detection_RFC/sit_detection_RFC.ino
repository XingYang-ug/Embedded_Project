// #include <Arduino_LSM6DS3.h>
// #include "rfc_model.h"  // Include the exported model

// // Min and Max values used in Python for normalization
// float accel_min[] = {-1.014893, -1.147339, -0.205566};  // Replace with your dataset's actual min values
// float accel_max[] = {1.830688, 0.786255, 1.187622};     // Replace with your dataset's actual max values
// float gyro_min[] = {-164.245605, -153.015137, -129.943848}; // Replace with your dataset's actual min values
// float gyro_max[] = {174.316406, 135.070801, 133.422852};    // Replace with your dataset's actual max values
// // Function to normalize a value using the MinMaxScaler formula
// float normalize(float value, float min, float max) {
//     return 2.0 * (value - min) / (max - min) - 1.0;
// }

// Eloquent::ML::Port::RandomForest RFC;

// const int QUEUE_SIZE = 20; // Size of the rolling buffer
// int predictionQueue[QUEUE_SIZE] = {0}; // Rolling buffer for predictions
// int queueIndex = 0; // Current index in the buffer
// const int THRESHOLD = 15; // Number of "Standing" predictions to assert standing

// void setup() {
//     Serial.begin(9600);

//     // Initialize IMU sensor
//     if (!IMU.begin()) {
//         Serial.println("Failed to initialize IMU!");
//         while (1);
//     }
//     Serial.println("IMU initialized");
// }

// void loop() {
//     float ax, ay, az, gx, gy, gz;
    
//     if (IMU.accelerationAvailable() && IMU.gyroscopeAvailable()) {
//         // Read raw IMU values
//         IMU.readAcceleration(ax, ay, az);
//         IMU.readGyroscope(gx, gy, gz);

//         // Normalize accelerometer values
//         ax = normalize(ax, accel_min[0], accel_max[0]);
//         ay = normalize(ay, accel_min[1], accel_max[1]);
//         az = normalize(az, accel_min[2], accel_max[2]);

//         // Normalize gyroscope values
//         gx = normalize(gx, gyro_min[0], gyro_max[0]);
//         gy = normalize(gy, gyro_min[1], gyro_max[1]);
//         gz = normalize(gz, gyro_min[2], gyro_max[2]);

//         // Create input array for the model
//         float input[] = {ax, ay, az, gx, gy, gz};

//         // Run inference
//         int prediction = RFC.predict(input);

//         // Update the rolling buffer
//         predictionQueue[queueIndex] = prediction;
//         queueIndex = (queueIndex + 1) % QUEUE_SIZE;

//         // Count the number of "Standing" predictions in the buffer
//         int standingCount = 0;
//         for (int i = 0; i < QUEUE_SIZE; i++) {
//             if (predictionQueue[i] == 1) {
//                 standingCount++;
//             }
//         }

//         // Determine the current state
//         if (standingCount >= THRESHOLD) {
//             Serial.println("Current State: Standing");
//         } else {
//             Serial.println("Current State: Not Standing");
//         }
//     }

//     delay(10);  // Adjust delay as needed
// }

#include <Arduino_LSM6DS3.h>
#include <ArduinoBLE.h>
#include "rfc_model.h"

const char* uuid_service = "1101";
const char* uuid_characteristic = "1102";

BLEService stateService(uuid_service);
BLEByteCharacteristic stateCharacteristic(uuid_characteristic, BLERead | BLENotify);

float accel_min[] = {-1.014893, -1.147339, -0.205566};
float accel_max[] = {1.830688, 0.786255, 1.187622};
float gyro_min[] = {-164.245605, -153.015137, -129.943848};
float gyro_max[] = {174.316406, 135.070801, 133.422852};

float normalize(float value, float min, float max) {
    return 2.0 * (value - min) / (max - min) - 1.0;
}

Eloquent::ML::Port::RandomForest RFC;

const int QUEUE_SIZE = 20;
int predictionQueue[QUEUE_SIZE] = {0};
int queueIndex = 0;
const int THRESHOLD = 15;

void setup() {
    Serial.begin(9600);
    while (!Serial);
    Serial.println("Starting setup...");
    
    if (!IMU.begin()) {
        Serial.println("IMU initialization failed!");
        while (1);
    }

    if (!BLE.begin()) {
        Serial.println("BLE initialization failed!");
        while (1);
    }

    BLE.setLocalName("Nano_Tracker");
    BLE.setAdvertisedService(stateService);
    stateService.addCharacteristic(stateCharacteristic);
    BLE.addService(stateService);

    if (BLE.advertise()) {
      Serial.println("Nano 33 IoT ready. Advertising BLE...");
    } else {
      Serial.println("Advertising BLE failed...");
    }
}

void loop() {
    float ax, ay, az, gx, gy, gz;
    BLEDevice central = BLE.central(); // 检查是否有 Central 设备连接
    stateCharacteristic.writeValue(0);
      if (central) {
          Serial.println("Central connected");
          while (central.connected()) {
            if (IMU.accelerationAvailable() && IMU.gyroscopeAvailable()) {
                IMU.readAcceleration(ax, ay, az);
                IMU.readGyroscope(gx, gy, gz);

                ax = normalize(ax, accel_min[0], accel_max[0]);
                ay = normalize(ay, accel_min[1], accel_max[1]);
                az = normalize(az, accel_min[2], accel_max[2]);
                gx = normalize(gx, gyro_min[0], gyro_max[0]);
                gy = normalize(gy, gyro_min[1], gyro_max[1]);
                gz = normalize(gz, gyro_min[2], gyro_max[2]);

                float input[] = {ax, ay, az, gx, gy, gz};
                int prediction = RFC.predict(input);

                predictionQueue[queueIndex] = prediction;
                queueIndex = (queueIndex + 1) % QUEUE_SIZE;

                int standingCount = 0;
                for (int i = 0; i < QUEUE_SIZE; i++) {
                    if (predictionQueue[i] == 1) {
                        standingCount++;
                    }
                }

                if (standingCount >= THRESHOLD) {
                    Serial.println("Current State: Standing");
                    stateCharacteristic.writeValue(1);
                } else {
                    // Serial.println("Current State: Sitting");
                    stateCharacteristic.writeValue(0);
                }
                if(stateCharacteristic.written()){
                  Serial.println("wrote val to stateChar");
                }
            }
          }
          Serial.println("Central disconnected");
      }
}