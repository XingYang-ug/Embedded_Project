#include <ArduinoBLE.h>
#include <Arduino_LSM6DS3.h>

#define BLE_UUID_SITTING_SIGNAL "1101"
#define BLE_UUID_SITTING_ACTION "2101"

#define BLE_DEVICE_NAME "Elfo"
#define BLE_LOCAL_NAME "Elfo"

BLEService sittingService(BLE_UUID_SITTING_SIGNAL);

BLEFloatCharacteristic sittingCharacteristics(BLE_UUID_SITTING_ACTION, BLERead | BLENotify);

const float magnitude_mean = 1.01;
const float magnitude_std = 0.02;

const float threshold = 5;

const int QUEUE_SIZE = 10; // Number of recent values to store
int queue[QUEUE_SIZE];
int queueIndex = 0; // Circular buffer index
int standingCountThreshold = 100; // Minimum "standing" values to predict standing

void setup()
{
    Serial.begin(9600);
    while (!Serial)
        ;

    if (!IMU.begin())
    {
        Serial.println("Failed to initialize IMU!");
        while (1)
            ;
    }

    if (!BLE.begin())
    {
        Serial.println("Starting Bluetooth® Low Energy module failed!");
        while (1)
            ;
    }

    // // set advertised local name and service UUID
    // BLE.setDeviceName(BLE_DEVICE_NAME);
    // BLE.setLocalName(BLE_LOCAL_NAME);
    // BLE.setAdvertisedService(sittingService);

    // // add characteristics and service
    // sittingService.addCharacteristic(sittingCharacteristics);

    // // start advertising
    // BLE.addService(sittingService);

    // sittingCharacteristics.writeValue(0);

    Serial.println("BLE Accelerometer Peripheral");
}

void loop()
{
    // if (BLE.connected()) // If BLE is connected
    // {
        if (IMU.accelerationAvailable())
        {
            float x, y, z;
            IMU.readAcceleration(x, y, z);

            float magnitude = sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
            int isSitting = abs(magnitude - magnitude_mean) / magnitude_std;

            // Update the queue with the new value
            queue[queueIndex] = isSitting;
            queueIndex = (queueIndex + 1) % QUEUE_SIZE;

            // Count the number of "standing" values in the queue
            int standingCount = 0;
            for (int i = 0; i < QUEUE_SIZE; i++)
            {
                standingCount += queue[i];
            }

            // Predict standing or sitting
            if (standingCount > standingCountThreshold)
            {
                // sittingCharacteristics.writeValue(0);
                Serial.print("Prediction: Standing ");
                Serial.println(standingCount);
            }
            else
            {
                // sittingCharacteristics.writeValue(1);
                Serial.print("Prediction: Sitting ");
                Serial.println(standingCount);
            }
        }
        // else
        // {
        //     BLE.advertise();
        // }
        delay(10);
    // }
}