# ESP UBUS Control

This project provides a UBUS-based control interface for ESP devices, allowing seamless management and communication via serial port. Designed for integration with the **RUTX router series**, this project enables remote control and data retrieval from ESP microcontrollers using the (RUTX’s) UBUS system.

## Features

- **Remote ESP Management**: Control GPIO pins and communicate with sensors on ESP via UBUS.
- **Serial Communication**: Utilizes the serial port for reliable, bidirectional communication.
- **RUTX Router Compatible**: Integrates with RUTX routers, enabling seamless connectivity and remote access to ESP8266 devices.
- **JSON-Formatted Data Exchange**: Send and receive commands in JSON format for easy interpretation.
  
### Prerequisites

- **RUTX Router** (such as RUTX10 or RUTX11) with UBUS enabled.
- **ESP Microcontroller** connected to the RUTX router's or PC's serial port.
- **Packages**: `libjson-c`, `libtuya`, `libubus`, `libubox`, `libblobmsg-json`, `libserialport`, `argp-standalone`
- **Pre-loaded code**: ESP device must have code saved to handle the commands on, off, and get.
  
### Installation

1. Clone this repository to your RUTX router:
   ```bash
   git clone https://github.com/n4tas/esp-ubus-control.git
   cd esp-ubus-control
   ```

2. Build the package:
   ```bash
   make
   ```

3. If using a RUTX router, configure:
    ```bash
    make menuconfig
    ```
3.5. Compile package:
   ```bash
   make package/ESP-Ubus-Control/{clean,compile}
   ```

4. Start the UBUS service for ESP:
   ```bash
   /etc/init.d/esp start
   ```

### Usage

Control your ESP device using UBUS commands on your RUTX router:

#### Turn On a Pin
```bash
ubus call esp on '{"port": "/dev/ttyUSB0", "pin": 5}'
```

#### Turn Off a Pin
```bash
ubus call esp off '{"port": "/dev/ttyUSB0", "pin": 5}'
```

#### Retrieve Sensor Data
```bash
ubus call esp get '{"port": "/dev/ttyUSB0", "pin": 2, "sensor": "temperature", "model": "DHT11"}'
```