#include <math.h>
#include "esp_log.h"

int sensorPin = 36;

double Thermister(int RawADC)
{
    double Temp;
    Temp = log(((10240000 / RawADC) - 10000));
    Temp = 1 / (0.001129148 + (0.000234125 + (0.0000000876741 * Temp * Temp)) * Temp);
    Temp = Temp - 273.15;
    return Temp;
}

void setup()
{
    // codigo setup
    Serial.begin(9600);
}

void loop()
{
    // codigo principal que vai repetir infinito
    int adcValue = analogRead(sensorPin);

    Serial.print(Thermister(adcValue));
    Serial.println(" ºC");

    delay(500);
}
