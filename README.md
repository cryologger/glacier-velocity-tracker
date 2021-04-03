# Cryologger - Glacier Velocity Measurement System

![Image](https://github.com/adamgarbo/Cryologger_Glacier_Velocity_Measurement_System/blob/main/Archive/v1.0/Images/DSC_1890.JPG)

** ***Work in progress*** **

## Introduction
A low-cost and open-source glacier velocity measurement system based on the u-blox ZED-F9P GNSS receiver. The code is powered by the SparkFun MicroMod Artemis Processor and Data Logging Carrier Board and is dedicated to logging UBX messages from the u-blox ZED-F9P.

Messages are streamed directly to a microSD card in UBX format. Log files are converted to RINEX using RTKLIB and then submitted to NRCan's Precise Point Positioning (PPP) tool: https://webapp.geod.nrcan.gc.ca/geod/tools-outils/ppp.php

## Design

### Principles
* Inexpensive
* Power efficienct
* Robust
* User friendly
* Off-the-shelf components

### Materials 

| Component | Product | Cost (USD) |
| --- | --- | :---: | 
| Processor | SparkFun MicroMod Artemis Processor | $14.95 |
| Carrier Board | SparkFun MicroMod Data Logging Carrier Board | $19.95 |
| GNSS | SparkFun GPS-RTK-SMA Breakout - ZED-F9P (Qwiic) | $219.95 |
| GNSS Antenna | GNSS Multi-Band L1/L2 Surveying Antenna (TNC) - TOP106 | $124.95 |
| Voltage Regulator | Pololu 5V, 600mA Step-Down Voltage Regulator D36V6F5 | $4.95 |
| microSD Card| Western Digital Industrial Grade Extended Temperature 32 GB | $ 57.00 |
| Solar Panel | Voltaic Systems 9 Watt 18 Volt Solar Panel | $89.00 |
| Solar Panel Bracket | Voltaic Systems Solar Panel Bracket - Large | $18.00 |
| Charge Controller | Genasun GV-5 65W 5A Solar charge controller with MPPT  | $68.00 |
| Battery | Deka SOLAR 48 Ah 8G40C | $150.00 |
| Enclosure | L-Com 14x12x7 Inch Weatherproof NEMA Enclosure | $118.00 |
| Pole Mount | L-Com Enclosure Pole Mounting Kit | $53.30 |
| **Total** | | **$938.05** |


### Carrier Board
A simple carrier board was designed to simplify the wiring and deployment of the Cryologger GVMS. The carrier board also provides a robust 5 mm screw terminal for the battery conneciton, and an optional resistor divider to measure the battery voltage.

![Carrier Board](https://user-images.githubusercontent.com/22924092/113482276-e6d40c00-946b-11eb-8e7a-b277b068f311.png)


## Measurements

While fully configurable, the system is currently programmed to wake daily and log UBX RAWX/SFRBX messages at 1 Hz from both GPS and GLONASS constellations for 3 hours. 


## Repository Contents
* **/Software** - Contains the Arduino code.

* **/Hardware** - Autodesk Eagle PCB schematic and board design files.

* **/Bill of Materials** - Information on components and their associated costs.

* **/Archive** - Previous design iterations.

## Documentation
* Currently in progress.

## License Information
This project is released under the MIT License (http://opensource.org/licenses/MIT).

Cheers,

**Adam**
