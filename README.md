# Cryologger - Glacier Velocity Tracker

![Image](https://github.com/adamgarbo/Cryologger_Glacier_Velocity_Measurement_System/blob/main/Archive/v1.0/Images/DSC_1890.JPG)

** ***Work in progress*** **

## 1.0 Introduction
The low-cost, open-source Cryologger glacier velocity measurement system (GVMS) is designed to measure the daily displacement and annual velocities of glaciers in the Canadian Arctic. It is based on the u-blox ZED-F9P GNSS receiver and powered by the SparkFun MicroMod Artemis Processor and Data Logging Carrier Board.

## 2.0 Methods

### 2.1 Design

Constructed using off-the-shelf components, the Cryologger GVMS design is planned for extended deployments of one year or more and is capable of providing long-term GNSS measurements.

#### 2.1.1 Design principles

The design of the system places emphasis on the following design principles:
* Inexpensive
* Off-the-shelf components
* Modular
* Extensible
* Power-efficient
* Open-source
* User friendly
* Robust

#### 2.1.2 Bill of materials 

**Table 1.** Components making up the glacier velocity measurement system, current as of April 3, 2021.

| Component | Product | Cost (USD) |
| --- | --- | :---: | 
| Processor | SparkFun MicroMod Artemis Processor | $14.95 |
| Carrier Board | SparkFun MicroMod Data Logging Carrier Board | $19.95 |
| GNSS Receiver | SparkFun GPS-RTK-SMA Breakout - ZED-F9P | $219.95 |
| GNSS Antenna | SparkFun GNSS Multi-Band L1/L2 Surveying Antenna (TNC) - TOP106 | $124.95 |
| Voltage Regulator | Pololu 5V, 600mA Step-Down Voltage Regulator D36V6F5 | $4.95 |
| microSD Card| Western Digital SDSDQED-032G-XI Industrial Grade 32 GB microSD | $57.00 |
| Solar Panel | Voltaic Systems 9 Watt 18 Volt Solar Panel | $89.00 |
| Solar Panel Bracket | Voltaic Systems Solar Panel Bracket - Large | $18.00 |
| Solar Panel Hardware | Voltaic Systems Panel Post Nuts - Stainless Steel | $ 4.00 |
| Charge Controller | Genasun GV-5 65W 5A Solar charge controller with MPPT  | $68.00 |
| Battery | Deka SOLAR Gel 48 Ah 8G40C | $150.00 |
| Enclosure | L-Com 14x12x7 Inch Weatherproof NEMA Enclosure | $118.00 |
| Pole Mount | L-Com Enclosure Pole Mounting Kit | $53.30 |
| Tripod | | | 
| **Total** | | |


#### 2.1.3 Carrier board
A carrier board was designed to simplify assembly and deployment of the Cryologger GVMS. It provides 5 mm, 2-pin screw terminals for wiring the battery connection, and an optional resistor divider to measure battery voltage.

<p align="center"><img src="https://user-images.githubusercontent.com/22924092/113581250-e61ab180-95f4-11eb-9a24-5372ff3434e1.png" width="720"></p>
<p align="center"><b>Figure 5.</b> Open-source carrier board design.</p>


#### 2.1.4 Mock-up
![L-Com v16](https://user-images.githubusercontent.com/22924092/113727907-9525be80-96c3-11eb-9db5-a16260b8cdfe.png)


### 2.2 Measurements

By default, the system is programmed to wake daily and log UBX RAWX/SFRBX messages at 1 Hz from both GPS and GLONASS constellations for a period of 3 hours. Messages are streamed processed through the SparkFun u-blox GNSS library and written to a microSD card in UBX format. Log files can then be converted to RINEX using RTKLIB and submitted to NRCan's Precise Point Positioning (PPP) tool: https://webapp.geod.nrcan.gc.ca/geod/tools-outils/ppp.php

### 2.3 Deployments
Several systems are planned for deployment in the summer of 2021. Potential deployment locations include the Mittie Glacier, Belcher Glacier, and Croker Bay Glacier in Nunavut, Canada.


<p align="center"><img src="https://user-images.githubusercontent.com/22924092/113763675-f495c580-96e7-11eb-9eb1-5203dc91fd16.png" width="480"></p>
<p align="center"><b>Figure 5.</b> Map of proposed Cryologger deployments in summer 2021.</p>


## 3.0 Results

## 4.0 Discussion

## 5.0 Conclusion



## Repository Contents
* **/Software** - Contains the Arduino code.

* **/Hardware** - Autodesk Eagle PCB schematic and board design files.

* **/Bill of Materials** - Information on components and their associated costs.

* **/Archive** - Previous design iterations.


## License Information
This project is released under the MIT License (http://opensource.org/licenses/MIT).

Cheers,

**Adam**
