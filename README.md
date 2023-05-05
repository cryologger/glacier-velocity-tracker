<p align="left">
<img alt="GitHub" src="https://img.shields.io/github/license/adamgarbo/Cryologger_Glacier_Velocity_Tracker">
<img alt="GitHub release (latest by date)" src="https://img.shields.io/github/v/release/adamgarbo/Cryologger_Glacier_Velocity_Tracker">
<img alt="GitHub issues" src="https://img.shields.io/github/issues/adamgarbo/Cryologger_Glacier_Velocity_Tracker">
</p>


# Cryologger - Glacier Velocity Tracker (GVT)

![Image](https://github.com/adamgarbo/Cryologger_Glacier_Velocity_Measurement_System/blob/main/Images/DSC_6546.JPG)

## 1.0 Introduction
The Cryologger glacier velocity tracker (GVT) is a low-cost, open-source multi-frequency GNSS receiver and data logger designed to measure the daily displacement and annual velocities of glaciers in the Canadian Arctic. It is based on the u-blox ZED-F9P GNSS receiver and powered by the SparkFun MicroMod Artemis Processor and Data Logging Carrier Board.

## 2.0 Methods

### 2.1 Design

The Cryologger GVT is constructed using off-the-shelf components, and is intended for deployments of one year or more. Emphasis is placed on a number of design principles, including:
* Inexpensive
* Off-the-shelf components
* Modular
* Extensible
* Power-efficient
* Open-source
* User friendly
* Robust

#### 2.1.1 Bill of materials 

**Table 1.** Components making up version 2.0 of the Cryologger GVT. Prices current as of October, 2022 and do not include taxes and/or shipping. <sup>1</sup>Denotes optional component.
| Component | Product | Quantity | Cost (USD) |
| --- | --- | :---: | :---: |
| PCB | [Custom Cryologger Printed Circuit Board](https://jlcpcb.com) | 1 | $5.00 | 
| Processor | [SparkFun MicroMod Artemis Processor](https://www.sparkfun.com/products/16401) | 1 | $14.95 | 
| Carrier Board | [SparkFun MicroMod Data Logging Carrier Board](https://www.sparkfun.com/products/16829) | 1 | $21.50 | 
| GNSS Receiver | [SparkFun GPS-RTK-SMA Breakout - ZED-F9P](https://www.sparkfun.com/products/16481) | 1 | $274.95 | 
| GNSS Antenna | [SparkFun GNSS Multi-Band L1/L2 Surveying Antenna (TNC) - TOP106](https://www.sparkfun.com/products/17751) | 1 | $133.95 | 
| Display | [SparkFun Qwiic OLED Display](https://www.sparkfun.com/products/17153) | 1 | $10.95 |
| Qwiic Cable | [Qwiic Cable - 50mm](https://www.sparkfun.com/products/14426) | 1 | $0.95 |
| GNSS Antenna Cable | [SparkFun Interface Cable - SMA Female to SMA Male (25cm)](https://www.sparkfun.com/products/12861) | 1 | $5.50 |
| GNSS Antenna Cable | [SparkFun Interface Cable - SMA Male to TNC Male (300mm)](https://www.sparkfun.com/products/17833) | 1 |	$10.95 | 
| Voltage Regulator | [Pololu 5V, 600mA Step-Down Voltage Regulator D36V6F5](https://www.pololu.com/product/3792) | 1 | $9.95 |
| microSD Card| [SanDisk MAX Endurance 64 GB microSD](https://www.amazon.com/SanDisk-Endurance-microSDXC-Adapter-Security/dp/B084CJ96GT) | 1 | $17.99 | 
| Resistor | [1 Mohms 1% MBB02070C1004FRP00](https://octopart.com/mbb02070c1004frp00-vishay-42819429?r=sp) | 1 | $0.30 |
| Resistor | [10Mohm 1% 50ppm MBB02070C1005FRP00](https://octopart.com/mbb02070c1005frp00-vishay-42881138?r=sp) | 1 |	$0.33 |
| Capacitor | [0.1 uF K104K15X7RF53L2](https://octopart.com/k104k15x7rf53l2-vishay-39696446?r=sp) | 1 | $0.23 |
| Power Connector	| [PCB terminal block - MKDS 1,5/ 2-5,08 1715721](https://octopart.com/1715721-phoenix+contact-12533?r=sp) | 1 | $1.55 |
| Male Header | [SparkFun Break Away Headers - Straight](https://www.sparkfun.com/products/116) | 2 | $3.10 |
| Female Header | [Pololu 0.100" (2.54 mm) Female Header: 1x2-Pin, Straight<sup>1</sup>](https://www.pololu.com/product/1012) | 1 | $0.39 |
| Female Header | [Pololu 0.100" (2.54 mm) Female Header: 1x4-Pin, Straight<sup>1</sup>](https://www.pololu.com/product/1014) | 1 | $0.53 |
| Female Header | [Pololu 0.100" (2.54 mm) Female Header: 1x6-Pin, Straight<sup>1</sup>](https://www.pololu.com/product/1016) | 1 | $0.66 |
| Female Header | [Pololu 0.100" (2.54 mm) Female Header: 1x9-Pin, Straight<sup>1</sup>](https://www.pololu.com/product/1019) | 2 | $0.85 |
| Female Header | [Pololu 0.100" (2.54 mm) Female Header: 1x10-Pin, Straight<sup>1</sup>](https://www.pololu.com/product/1020) | 1 | $0.46 |
| Female Header | [Pololu 0.100" (2.54 mm) Female Header: 1x12-Pin, Straight<sup>1</sup>](https://www.pololu.com/product/1030) | 1 | $0.55 |
| Standoff | [SparkFun Standoff - Nylon (4-40; 3/4"; 10 pack)](https://www.sparkfun.com/products/11796) | 1 | $4.50 |
| Screw | [SparkFun Screw - Phillips Head (1/4", 4-40, 10 pack)](https://www.sparkfun.com/products/10453) | 1 | $1.60 |
| Enclosure | [Hammond Manufacturing 1554Q2GYCL](https://octopart.com/1554q2gycl-hammond-99088964?r=sp) | 1 | $28.50 |
| Enclosure Inner Panel | [Hammond Manufacturing 1554QPL](https://octopart.com/1554qpl-hammond-97562074?r=sp) | 1 | $9.00 |
| Power Panel Mount Connector | Bulgin<sup>1</sup> | 1 | $25.00 |
| Power Cable Connector | Bulgin<sup>1</sup> | 1 | $25.00 |
| Battery Ring Terminal | [Molex Perma-Seal Ring Terminal 14-16 AWG Wire 1/4" (M6) 19164-0026](https://octopart.com/19164-0026-molex-99569?r=sp) | 2 | $1.00 |
| | | **Total** | **~$600** |

**Table 2.** Components making up the enclosure, mounting and power systems for Cryologger GVT version 2.0. <sup>1</sup>Denotes optional component. <sup>2</sup>Denotes educational pricing.
| Component | Product | Quantity | Cost (USD) | 
| --- | --- | :---: | :---: | 
| Solar Panel | [Ameresco Solar 20 Watt Solar Panel 20J<sup>2</sup>](https://www.amerescosolar.com/20j-ameresco-solar-20-watt-solar-panel) | 1 |  $99.00 |
| Solar Panel Bracket | [Ameresco Solar HPM18-30 Solar Panel Pole Mount<sup>2</sup>](https://www.amerescosolar.com/hpm18-30solar-panel-pole-mount) | 1 |  $39.00 
| Solar Charge Controller | [Genasun GV-4 4A MPPT Solar Charge Controller](https://www.altestore.com/store/charge-controllers/solar-charge-controllers/mppt-solar-charge-controllers/genasun-gv-4-pb-12v-4a-mppt-controller-for-12v-lead-acid-batteries-p10622/) | 1 |  $63.00 |
| Solar Panel Power Cable | [Polarwire Arctic Ultraflex Blue Twin Trailer Cable AUF-TC162](https://store.polarwire.com/arctic-trailer-cable-16-2-gray-16ga-2-conductors/) | Variable | $0.55/ft |
| Solar Panel Cable Gland | [Heyco M4340 M20 Cable Gland](https://octopart.com/m4340-heyco-2298364?r=sp) | 2 | $5.94 |
| Battery Ring Terminal | [Molex Perma-Seal Ring Terminal 14-16 AWG Wire 1/4" (M6) 19164-0026](https://octopart.com/19164-0026-molex-99569?r=sp) | 2 | $1.00 |
| Battery | Deka SOLAR Gel 48 Ah 8G40C | 1 | ~$150.00 |
| Antenna Bulkhead Adapter | [Amphenol TNC Jk to TNC Jk 50 Ohm Str Bkhd IP67 AD-TNCJTNCJ-1](https://octopart.com/ad-tncjtncj-1-amphenol+rf-108109770?r=sp) | 1 | $42.79 |
| Antenna Cable | [GPAC GPS Antenna Cable TNC to Right Angle TNC (3 m)](https://www.gpac-llc.com/product.php?product=10) | 1 | $83.00 |
| Enclosure | [L-Com 14x12x7 Inch Weatherproof NEMA Enclosure](https://www.l-com.com/nema-enclosures-14x12x7-inch-weatherproof-nema-enclosure-only) | 1 |  $161.99 |
| Enclosure Panel | [L-Com Blank Aluminum Mounting Plate](https://www.l-com.com/nema-enclosures-blank-aluminum-mounting-plate-for-1412xx-series-enclosures) | 1 |  $32.39 |
| Pole Mount | [L-Com Enclosure Pole Mounting Kit](https://www.l-com.com/nema-enclosures-enclosure-pole-mounting-kit-pole-diameters-1-1-4-to-2-inches) | 1 | $75.99 |
| Tripod | [Onset HOBO Weather Station m 3m Tripod M-TPA](https://www.onsetcomp.com/products/mounting/m-tpa/) | 1 |  $200.00 | 
| | | **Total** | **~$1,000** |

#### 2.1.2 Carrier board
A carrier board was designed to simplify assembly and deployment of the Cryologger GVT. It provides 5.08 mm, 2-pin screw terminals for wiring the battery connection, and a resistor divider to measure battery voltage.

<p align="center"><img width="720" alt="image" src="https://user-images.githubusercontent.com/22924092/186201944-ff52a2b9-d714-46c5-bb36-c9115895dd63.png"></p>
<p align="center"><b>Figure 5.</b> Open-source carrier board design.</p>

#### 2.1.3 Mock-up
![L-Com v16](https://user-images.githubusercontent.com/22924092/113727907-9525be80-96c3-11eb-9db5-a16260b8cdfe.png)

### 2.2 Data Logging

The system is programmed to wake daily and log UBX RAWX/SFRBX messages at 1 Hz from both GPS and GLONASS constellations for a period of 3 hours. Messages are streamed processed through the SparkFun u-blox GNSS library and written to a microSD card in UBX format. 

### 2.3 Operation

When initially powered on, the system will attempt to establish a GNSS fix and sychronize the real-time clock (RTC) of the MicroMod Artemis Processor for up to 5 minutes. Once the RTC is synchronized, the system will set an alarm to wake at the user-specified time and then enter a low-power deep sleep. 

While the system is in deep sleep, the Watchdog Timer (WDT) will wake the system to check the program has not frozen once every 10 seconds. The LED will blink briefly (100 ms) during this time. 

#### 2.3.1 OLED Display Messages
Version 2.x of the Cryologger GVT is equipped with an OLED display, which is used to provided detailed information to the end user. These messages will indicate the status of the Cryologger at each stage of system operation, including booting up, initializing of components, acquiring of GNSS fix, and logging of data. 

#### 2.3.2 LED Blink Patterns
In addition to the OLED display messages, a series of LED blink patterns is also used to indicate system operation. The following table can be used to determine the status of the system, as well as any possible troubleshooting steps that should be attempted if an error is encountered.

**Table 1.** LED blink patterns, associated description and troubleshooting guide.
| Blinks | Interval (s)  |   Pattern   | Description                     | Troubleshooting Steps                                 |
|:------:|:-------------:|:-----------:|---------------------------------|-------------------------------------------------------|
|    1   |      10       |   Single    | System is in deep sleep         | None                                                  |
|   10   |      0.1      |   Single    | Initialization complete         | None                                                  | 
|    2   |      2        |   Repeating | microSD failure                 | Reseat microSD card and reboot system                 |
|    3   |      2        |   Repeating | GNSS receiver failure           | Ensure components are firmly seated and reboot system |
|    1   |      1        |   Single    | GNSS signal acquisition         | None                                                  |
|    5   |      1        |   Single    | GNSS fix found & RTC synced     | None                                                  |
|    5   |      0.5      |   Single    | GNSS fix not found              | Ensure antenna cables are connected and reboot system |
| Random |      Random   |   Random    | Logging data to microSD card    | None                                                  |

### 2.4 Data Processing
Data is logged in u-blox's proprietary .ubx format. Log files can be converted to RINEX using RTKLIB or Emlid Studio and submitted to NRCan's Precise Point Positioning (PPP) tool: https://webapp.geod.nrcan.gc.ca/geod/tools-outils/ppp.php

## 3.0 Deployments
A total of five deployments were made in the summer of 2021. Two on the Lowell Glacier in Kluane National Park, Yukon (Figure 5) and three on the Belcher and Southeast 2 glaciers, Devon Island, Nunavut, Canada (Figure 5).

<p align="center"><img src="https://user-images.githubusercontent.com/22924092/133788170-dc148b5a-be41-4dad-9bea-d87f751e8ac2.png" width="720"></p>
<p align="left"><b>Figure 4.</b> Map of Cryologger glacier velocity system deployments on Lowell Glacier in Kluane National Park, Yukon during the summer 2021 field season. Imagery: Copernicus Sentinel 2 acquired 2021-07-28.</p>

<p align="center"><img src="https://user-images.githubusercontent.com/22924092/133785308-99856546-a479-4e5f-8d66-12250404128f.png" width="720"></p>
<p align="left"><b>Figure 5.</b> Map of Cryologger glacier velocity system deployments on Devon Island, Nunavut during the 2021 Amundsen Expedition. Imagery: Copernicus Sentinel 2 acquired 2021-08-28.</p>

## 4.0 Results
Servicing of systems deployed on Devon Island and  Lowell Glacier  are planned for spring and summer 2022, respectively.

## Repository Contents
* **/Software** - Contains the Arduino code.

* **/Hardware** - KiCad PCB schematic and board design files.

* **/Bill of Materials** - Information on components and their associated costs.

* **/Archive** - Previous design versions.

## License Information
This project is released under the GNU General Public License v3.0 (https://www.gnu.org/licenses/gpl-3.0.en.html)

Cheers,

**Adam**
