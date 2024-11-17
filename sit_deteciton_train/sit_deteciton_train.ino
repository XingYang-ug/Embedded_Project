#include <Arduino_LSM6DS3.h>

const int numSamples = 100; // Number of baseline samples to collect

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

    Serial.println("Collecting baseline data... Please sit still.");

    // Collect baseline data for the stationary state
    float baselineData[numSamples][3];
    for (int i = 0; i < numSamples; i++)
    {
        if (IMU.accelerationAvailable())
        {
            float x, y, z;
            IMU.readAcceleration(x, y, z);
            baselineData[i][0] = x;
            baselineData[i][1] = y;
            baselineData[i][2] = z;
            delay(100); // 100ms delay between samples
        }
    }

    // Calculate mean and standard deviation of baseline data
    float mean = 0.0;
    float stdDev = 0.0;

    for (int i = 0; i < numSamples; i++) {
        double sum = pow(baselineData[i][0], 2) + pow(baselineData[i][1], 2) + pow(baselineData[i][2], 2);
        double magnitude = sqrt(sum);

        mean += magnitude;
    }

    mean /= numSamples;

    for (int i = 0; i < numSamples; i++) {
        double sum = pow(baselineData[i][0], 2) + pow(baselineData[i][1], 2) + pow(baselineData[i][2], 2);
        double magnitude = sqrt(sum);

        stdDev += pow(magnitude - mean, 2);
    }

    stdDev = sqrt(stdDev / numSamples);

    // Print the mean and standard deviation values for production use
    Serial.println("Baseline Mean and Standard Deviation:");
    Serial.print("Magnitude mean: ");
    Serial.println(mean);
    Serial.print("Magnitude stddev: ");
    Serial.println(stdDev);

    Serial.println("Baseline data collected.");
}

void loop() {
  
}
