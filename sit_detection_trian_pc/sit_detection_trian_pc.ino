#include <ArduinoBLE.h>
#include <Arduino_LSM6DS3.h>

#define BLE_UUID_ACCELEROMETER_SERVICE "1101"
#define BLE_UUID_ACCELEROMETER_X "2101"
#define BLE_UUID_ACCELEROMETER_Y "2102"
#define BLE_UUID_ACCELEROMETER_Z "2103"
#define BLE_UUID_GYROSCOPE_X "1102"
#define BLE_UUID_GYROSCOPE_Y "1103"
#define BLE_UUID_GYROSCOPE_Z "1104"

#define BLE_DEVICE_NAME "Elfo"
#define BLE_LOCAL_NAME "Elfo"

BLEService accelerometerService(BLE_UUID_ACCELEROMETER_SERVICE);

BLEFloatCharacteristic accelerometerCharacteristicX(BLE_UUID_ACCELEROMETER_X, BLERead | BLENotify);
BLEFloatCharacteristic accelerometerCharacteristicY(BLE_UUID_ACCELEROMETER_Y, BLERead | BLENotify);
BLEFloatCharacteristic accelerometerCharacteristicZ(BLE_UUID_ACCELEROMETER_Z, BLERead | BLENotify);
BLEFloatCharacteristic gyroscopeCharacteristicX(BLE_UUID_GYROSCOPE_X, BLERead | BLENotify);
BLEFloatCharacteristic gyroscopeCharacteristicY(BLE_UUID_GYROSCOPE_Y, BLERead | BLENotify);
BLEFloatCharacteristic gyroscopeCharacteristicZ(BLE_UUID_GYROSCOPE_Z, BLERead | BLENotify);

float ax, ay, az;
float gx, gy, gz;

void setup()
{
    Serial.begin(9600);
    while (!Serial)
        ;

    // initialize IMU
    if (!IMU.begin())
    {
        Serial.println("Failed to initialize IMU!");
        while (1)
            ;
    }

    // initialize BLE
    if (!BLE.begin())
    {
        Serial.println("Starting Bluetooth® Low Energy module failed!");
        while (1)
            ;
    }

    // set advertised local name and service UUID
    BLE.setDeviceName("Elfo");
    BLE.setAdvertisedService(accelerometerService);

    // add characteristics and service
    accelerometerService.addCharacteristic(accelerometerCharacteristicX);
    accelerometerService.addCharacteristic(accelerometerCharacteristicY);
    accelerometerService.addCharacteristic(accelerometerCharacteristicZ);
    accelerometerService.addCharacteristic(gyroscopeCharacteristicX);
    accelerometerService.addCharacteristic(gyroscopeCharacteristicY);
    accelerometerService.addCharacteristic(gyroscopeCharacteristicZ);

    BLE.addService(accelerometerService);

    accelerometerCharacteristicX.writeValue(0.0);
    accelerometerCharacteristicY.writeValue(0.0);
    accelerometerCharacteristicZ.writeValue(0.0);
    gyroscopeCharacteristicX.writeValue(0.0);
    gyroscopeCharacteristicY.writeValue(0.0);
    gyroscopeCharacteristicZ.writeValue(0.0);

    // start advertising
    BLE.advertise();

    Serial.println("BLE IMU Peripheral");
}

void loop()
{
    // listen for BLE peripherals to connect:
    BLEDevice central = BLE.central();

    // if a central is connected to peripheral:
    if (central)
    {

        Serial.print("Connected to central: ");
        // print the central's MAC address:
        Serial.println(central.address());

        // while the central is still connected to peripheral:
        while (central.connected())
        {

            if (IMU.accelerationAvailable() && IMU.gyroscopeAvailable())
            {
                IMU.readAcceleration(ax, ay, az);
                IMU.readGyroscope(gx, gy, gz);

                accelerometerCharacteristicX.writeValue(ax);
                accelerometerCharacteristicY.writeValue(ay);
                accelerometerCharacteristicZ.writeValue(az);
                gyroscopeCharacteristicX.writeValue(gx);
                gyroscopeCharacteristicY.writeValue(gy);
                gyroscopeCharacteristicZ.writeValue(gz);
            }
            delay(10);
        }
    }
}