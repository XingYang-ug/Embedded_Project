#include <ArduinoBLE.h>

#define BLE_UUID_SIGNAL_SERVICE "1101"
#define BLE_UUID_SIGNAL_X "2101"

BLEDevice peripheral;
BLEService signalService(BLE_UUID_SIGNAL_SERVICE);  // Match the service UUID
BLEFloatCharacteristic signalCharacteristicX(BLE_UUID_SIGNAL_X, BLERead | BLENotify); // Match the characteristic UUID

void setup() {
  Serial.begin(9600);

  // Initialize BLE
  if (!BLE.begin()) {
    Serial.println("BLE not initialized!");
    while (1);
  }

  // Start scanning for BLE peripherals
  BLE.scanForUuid(BLE_UUID_SIGNAL_SERVICE);
  Serial.println("Scanning for peripherals...");
}

void loop() {
  BLEDevice foundPeripheral = BLE.available();
  
  if (foundPeripheral) {
    // If a peripheral is found
    Serial.println("Peripheral found!");
    
    if (foundPeripheral.connect()) {
      Serial.println("Connected to peripheral!");

      // Discover the services and characteristics
      if (foundPeripheral.discoverAttributes()) {
        Serial.println("Discovered services and characteristics!");

        // Read data from characteristic
        String data = myCharacteristic.value();
        Serial.print("Data: ");
        Serial.println(data);
      }
    }
    else {
      Serial.println("Failed to connect to peripheral");
    }
  }

  delay(10);  // Wait for 0.01 second before scanning again
}