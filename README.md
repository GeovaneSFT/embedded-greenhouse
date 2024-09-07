# Projeto Estufa Embarcados

Este repositório contém dois projetos independentes, baseados em placas ESP32, que se comunicam via MQTT e enviam dados de sensores para a plataforma ThingsBoard. Ambos os projetos foram desenvolvidos usando a biblioteca ESP-IDF no VSCode, com foco em economia de energia, armazenamento interno (NVS) e comunicação eficiente.

## Estrutura do Projeto

O repositório é dividido em duas pastas principais, cada uma correspondendo a uma das placas ESP32:

- **Placa-mqtt**: Esta pasta contém o projeto da placa responsável por atuar como broker interno de MQTT. Ela gerencia a comunicação entre as duas placas e permite que os comandos sejam enviados para a placa sensora.

- **Main**: Nesta pasta está o projeto da placa responsável por coletar dados de diversos sensores e enviá-los para a plataforma ThingsBoard via MQTT. Esta placa também implementa o Modo Sleep para economia de energia e utiliza o NVS (Non-Volatile Storage) para armazenamento interno de dados.

## Funcionalidades

- **MQTT**: Implementamos um broker MQTT interno para garantir a comunicação eficiente entre as duas placas ESP32, sem depender de serviços externos.
- **ThingsBoard Integration**: A placa sensora coleta dados e os envia diretamente para a plataforma ThingsBoard, facilitando o monitoramento remoto da estufa.
- **ModSleep**: Para otimizar o consumo de energia, as duas placas utilizam o modo de sono profundo (ModSleep), ativando apenas quando necessário.
- **NVS (Non-Volatile Storage)**: Armazenamento interno utilizado para salvar dados de configuração e estado entre reinicializações.

## Como usar


1. Compile e faça o flash de cada projeto na respectiva placa ESP32.

2. Abra cada placa de preferência em um ambiente separado do VSCode.

3. Abra o dashboard para verificar o funcionamento de cada sensor

## Requisitos

- ESP32 com suporte a ESP-IDF.
- Biblioteca ESP-IDF instalada no VSCode.
- Plataforma ThingsBoard configurada para receber os dados da estufa.



