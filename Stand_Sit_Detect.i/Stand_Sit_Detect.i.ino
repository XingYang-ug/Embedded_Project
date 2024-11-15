#include <Arduino_LSM6DS3.h>

const int numSamples = 1000;
float baselineMean[3];
float baselineStdDev[3];

void setup()
{
    Serial.begin(9600);
    while (!Serial);

    if (!IMU.begin())
    {
        Serial.println("Failed to initialize IMU!");
        while (1);
    }

    Serial.println("Collecting baseline data... Please sit still.");

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
            delay(100);
        }
    }

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

    Serial.println("Baseline data collected.");
}

void loop()
{
    if (IMU.accelerationAvailable())
    {
        float x, y, z;
        IMU.readAcceleration(x, y, z);

        if (isStanding(x, y, z))
        {
            Serial.println("Detected: Standing!");
        }
        else
        {
            Serial.println("Not standing.");
        }

        delay(100);
    }
}

bool isStanding(float x, float y, float z)
{
    float threshold = 20.0;

    if (fabs(x - baselineMean[0]) > baselineStdDev[0] * threshold ||
        fabs(y - baselineMean[1]) > baselineStdDev[1] * threshold ||
        fabs(z - baselineMean[2]) > baselineStdDev[2] * threshold)
    {
        return true;
    }

    return false;
}
