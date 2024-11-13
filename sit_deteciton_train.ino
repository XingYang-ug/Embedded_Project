#include <Arduino_LSM6DS3.h>

const int numSamples = 1000; // Number of baseline samples to collect
float baselineMean[3];      // To store mean of X, Y, Z in stationary state
float baselineStdDev[3];    // To store standard deviation of X, Y, Z in stationary state

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
            delay(100); // Sample rate
        }
    }

    // Calculate mean and standard deviation of baseline data
    for (int axis = 0; axis < 3; axis++)
    {
        float sum = 0.0;
        for (int i = 0; i < numSamples; i++)
        {
            sum += baselineData[i][axis];
        }
        baselineMean[axis] = sum / numSamples;

        float variance = 0.0;
        for (int i = 0; i < numSamples; i++)
        {
            variance += pow(baselineData[i][axis] - baselineMean[axis], 2);
        }
        baselineStdDev[axis] = sqrt(variance / numSamples);
    }

    // Print the mean and standard deviation values for production use
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
