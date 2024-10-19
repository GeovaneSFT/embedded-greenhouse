# Greenhouse Embedded Project

This repository contains two independent projects, based on ESP32 boards, that communicate via MQTT and send sensor data to the ThingsBoard platform. Both projects were developed using the ESP-IDF library in VSCode, focusing on energy efficiency, internal storage (NVS), and efficient communication.

## Project Presentation

The video presentation of the project can be found at this [link](https://youtu.be/N875RAy_jwo).


## Project Structure

The repository is divided into two main folders, each corresponding to one of the ESP32 boards:

- **Board-mqtt**: This folder contains the project for the board responsible for acting as the internal MQTT broker. It manages communication between the two boards and allows commands to be sent to the sensor board.

- **Main**: This folder contains the project for the board responsible for collecting data from various sensors and sending them to the ThingsBoard platform via MQTT. This board also implements Sleep Mode for energy savings and uses NVS (Non-Volatile Storage) for internal data storage.

## Features

- **MQTT**: We implemented an internal MQTT broker to ensure efficient communication between the two ESP32 boards without relying on external services.
- **ThingsBoard Integration**: The sensor board collects data and sends it directly to the ThingsBoard platform, making remote monitoring of the greenhouse easy.
- **ModSleep**: To optimize energy consumption, both boards use deep sleep mode (ModSleep), waking up only when necessary.
- **NVS (Non-Volatile Storage)**: Internal storage used to save configuration data and states between reboots.

## How to use


1. Compile and flash each project on its respective ESP32 board.

2. Open each board in a separate VSCode environment for better management.

3. Open the dashboard to check the operation of each sensor.

## Requirements

- ESP32 with ESP-IDF support.
- ESP-IDF library installed in VSCode.
- ThingsBoard platform configured to receive data.



