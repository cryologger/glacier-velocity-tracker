# Cryologger - Glacier Velocity Measurement System

![Image](https://github.com/adamgarbo/Cryologger_Glacier_Velocity_Measurement_System/blob/main/Archive/v1.0/Images/DSC_1890.JPG)

**Project status: Under active development as of March 22, 2021.**

## Introduction
A low-cost, open-source glacier velocity measurement system based on the u-blox ZED-F9P intended for use with Precise Point Positioning (PPP) techniques.

## Objectives
* Inexpensive
* Power efficiency
* Reliable
* Robust

## Design


#### Materials 

| Component | Product | Cost (USD) |
| --- | --- | :---: | 
| Processor | SparkFun MicroMod Artemis Processor | $14.95 |
| Carrier Board | SparkFun MicroMod Data Logging Carrier Board | $19.95 |
| GNSS | SparkFun GPS-RTK-SMA Breakout - ZED-F9P (Qwiic) | $219.95 |
| GNSS antenna | GNSS Multi-Band L1/L2 Surveying Antenna (TNC) - TOP106 | $124.95 |
| Voltage Regulator | SparkFun Buck-Boost Converter | $9.95 |
| Enclosure |  | ~$50.00 |
| Solar Panel |  | ~$100.00 |
| Battery |  | ~$100.00 |

## Measurements
The code will be powered by the SparkFun MicroMod Artemis Processor and Data Logging Carrier Board and is dedicated to logging UBX messages from the u-blox ZED-F9P.

Messages are streamed directly to a microSD card in UBX format without being processed. Log files can then be analysed afterwards using u-blox's u-center or RTKLIB and converted to RINEX in preparation for submission to NRCan's Precise Point Positioning (PPP) tool: https://webapp.geod.nrcan.gc.ca/geod/tools-outils/ppp.php

## Repository Contents
* **/Software** - Contains the Arduino code and firmware binaries

* **/Hardware** - Autodesk Eagle PCB schematic and board design files

* **/Bill of Materials** - Information on components and their associated costs

* **/Archive** - Previous design iterations

## Documentation
* Currently in progress

## License Information
This project is released under the MIT License (http://opensource.org/licenses/MIT).

Cheers,

**Adam**
