# Cryologger - Glacier Velocity Measurement System

![Image](https://github.com/adamgarbo/Cryologger_Glacier_Velocity_Measurement_System/Photos/DSC_6546.JPG)

## 1.0 Introduction
The Cryologger glacier velocity measurement system (GVMS) is a low-cost, open-source multi-frequency GNSS receiver and data logger designed to measure the daily displacement and annual velocities of glaciers in the Canadian Arctic. It is based on the u-blox ZED-F9P GNSS receiver and powered by the SparkFun MicroMod Artemis Processor and Data Logging Carrier Board.

## 2.0 Methods

### 2.1 Design

The Cryologger GVMS is constructed using off-the-shelf components, and is intended for deployments of one year or more. Emphasis is placed on a number of design principles, including:
* Inexpensive
* Off-the-shelf components
* Modular
* Extensible
* Power-efficient
* Open-source
* User friendly
* Robust

#### 2.1.1 Bill of materials 

**Table 1.** Components making up the glacier velocity measurement system.

| Component | Product | Cost (USD) |
| --- | --- | :---: | 
| Processor | SparkFun MicroMod Artemis Processor | $14.95 |
| Carrier Board | SparkFun MicroMod Data Logging Carrier Board | $19.95 |
| GNSS Receiver | SparkFun GPS-RTK-SMA Breakout - ZED-F9P | $219.95 |
| GNSS Antenna | SparkFun GNSS Multi-Band L1/L2 Surveying Antenna (TNC) - TOP106 | $124.95 |
| Voltage Regulator | Pololu 5V, 600mA Step-Down Voltage Regulator D36V6F5 | $4.95 |
| microSD Card| SanDisk MAX Endurance 64 GB microSD | $20.00 |
| Electronics Enclosure | Hammond Manufacturing 1554JL2GYCL | $20.00 |
| Solar Panel | Voltaic Systems 9 Watt 18 Volt Solar Panel | $89.00 |
| Solar Panel Bracket | Voltaic Systems Solar Panel Bracket - Medium | $18.00 |
| Solar Panel Hardware | Voltaic Systems Panel Post Nuts - Stainless Steel | $ 4.00 |
| Charge Controller | Genasun GV-5 65W 5A Solar charge controller with MPPT  | $68.00 |
| Battery | Deka SOLAR Gel 48 Ah 8G40C | $150.00 |
| Enclosure | L-Com 14x12x7 Inch Weatherproof NEMA Enclosure | $118.00 |
| Pole Mount | L-Com Enclosure Pole Mounting Kit | $53.30 |
| Tripod | Onset HOBO Weather Station m 3m Tripod |   | 


#### 2.1.2 Carrier board
A carrier board was designed to simplify assembly and deployment of the Cryologger GVMS. It provides 5 mm, 2-pin screw terminals for wiring the battery connection, and an optional resistor divider to measure battery voltage.


<p align="center"><img width="720" src="https://user-images.githubusercontent.com/22924092/114622089-874fda80-9c7b-11eb-9545-e24993ba051d.png"></p>
<p align="center"><b>Figure 5.</b> Open-source carrier board design.</p>


#### 2.1.3 Mock-up
![L-Com v16](https://user-images.githubusercontent.com/22924092/113727907-9525be80-96c3-11eb-9db5-a16260b8cdfe.png)


### 2.2 Data Logging

The system is programmed to wake daily and log UBX RAWX/SFRBX messages at 1 Hz from both GPS and GLONASS constellations for a period of 3 hours. Messages are streamed processed through the SparkFun u-blox GNSS library and written to a microSD card in UBX format. 

### 2.3 Operation

When initially powered on, the system will attempt to establish a GNSS fix and sychronize the real-time clock (RTC) of the MicroMod Artemis Processor for up to 5 minutes. During this time, the LED will blink once a second. Once the RTC is synchronized, the system will set an alarm to wake at the user-specified time and then enter a low-power deep sleep. 

While the system is in deep sleep, the Watchdog Timer (WDT) will wake the system to check the program has not frozen once every 10 seconds. The LED will blink briefly (100 ms) during this time. 


#### 2.3.1 LED Blink Patterns
* 10 seconds between LED blink - Watchdog interrupt. System is in deep sleep.
* 1 second between LED blinks - Attempting to synchronize real-time clock with u-blox receiver.
* 10 quick LED blinks - RTC synchronized with u-blox receiver. Configuration complete.
* Sets of 2 LED blinks - Unable to initialize microSD card
* Sets of 3 LED blinks - Unable to initialize u-blox receiver
* Periodic fast LED blinks - Logging data to microSD card

### 2.4 Data Processing
Data is logged in u-blox's proprietary .ubx format. Log files can be converted to RINEX using RTKLIB or Emlid Studio and submitted to NRCan's Precise Point Positioning (PPP) tool: https://webapp.geod.nrcan.gc.ca/geod/tools-outils/ppp.php

### 3.0 Deployment
A total of five deployments were made in the summer of 2021. Two on the Lowell Glacier in Kluane National Park, Yukon and three on the Belcher Glacier and Southeast 2 Glacier, Devon Island, Nunavut, Canada.

<p align="center"><img src="https://user-images.githubusercontent.com/22924092/113763675-f495c580-96e7-11eb-9eb1-5203dc91fd16.png" width="480"></p>
<p align="center"><b>Figure 5.</b> Map of proposed Cryologger deployments in summer 2021.</p>


## Repository Contents
* **/Software** - Contains the Arduino code.

* **/Hardware** - KiCad PCB schematic and board design files.

* **/Bill of Materials** - Information on components and their associated costs.

* **/Archive** - Previous design iterations.


## License Information
This project is released under the MIT License (http://opensource.org/licenses/MIT).

Cheers,

**Adam**
