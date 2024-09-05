Arquivo main:
    - main/app_main.c

JSON do dash:
    - sprintf(telemetry_data, "{\"temperatura\":%.1f,\"maxTemperatura\":%.1f,\"minTemperatura\":%.1f,\"humidade\":%.1f,\"maxHumidade\":%.1f,\"minHumidade\":%.1f}", temperature, maxTemperature, minTemperature, humidity, maxHumidity, minHumidity);