# Assembly

## 1.0 Introduction
This guide provides detailed instructions on the assembly of a Cryologger GNSS system.

## 2.0 Bill of Materials

The components required to assemble a Cryologger GNSS system are split into two groups: 
1) Components required for the assembly of the Cryologger GNSS (e.g., PCB, GNSS receiver, microcontroller, etc.) 
2) Components required for the mounting, power and deployment of the complete Cryologger GNSS system (e.g., tripod, solar panel, battery, etc.).

**Table 1.** Required tools for asssembly of Cryologger GNSS v2.21 system.
| Tool | Recommended Product | Quantity |
| --- | --- | :---: |
| Soldering iron | [Hakko FX-888D Digital Soldering Station](https://hakkousa.com/fx-888d-digital-soldering-station.html) | 1 |
| Solder | [SparkFun Solder - 1/4lb Spool (0.020") Special Blend](https://www.sparkfun.com/products/10242) | 1 |
| Side cutter | [Hakko CHP-170 Micro Cutter](https://hakkousa.com/chp-170-micro-cutter.html) | 1 |
| Pliers | [Hakko CHP PN-2001 Pointed Nose Pliers](https://hakkousa.com/products/chp-tools/chp-hand-tools/chp-pn-2001-pointed-nose-pliers.html) | | 1 |
| Screw drivers | | 1 |

**Table 2.** Components required for asssembly of Cryologger GNSS v2.21 system.
| Component | Product | Quantity |
| --- | --- | :---: |
| PCB | [Cryologger Printed Circuit Board (PCB)](https://jlcpcb.com) | 1 |
| Processor | [SparkFun MicroMod Artemis Processor](https://www.sparkfun.com/products/16401) | 1 |
| Carrier Board | [SparkFun MicroMod Data Logging Carrier Board](https://www.sparkfun.com/products/16829) | 1 |
| GNSS Receiver | [SparkFun GPS-RTK-SMA Breakout - ZED-F9P](https://www.sparkfun.com/products/16481) | 1 |
| GNSS Antenna | [SparkFun GNSS Multi-Band L1/L2 Surveying Antenna (TNC) - TOP106](https://www.sparkfun.com/products/17751) | 1 | 
| Display | [SparkFun Qwiic OLED Display](https://www.sparkfun.com/products/17153) | 1 |
| Qwiic Cable | [Qwiic Cable - 50mm](https://www.sparkfun.com/products/14426) | 1 |
| GNSS Antenna Cable | [SparkFun Interface Cable - SMA Female to SMA Male (25cm)](https://www.sparkfun.com/products/12861) | 1 |
| GNSS Antenna Cable | [SparkFun Interface Cable - SMA Male to TNC Male (300mm)](https://www.sparkfun.com/products/17833) | 1 | 
| Voltage Regulator | [Pololu 5V, 600mA Step-Down Voltage Regulator D36V6F5](https://www.pololu.com/product/3792) | 1 |
| microSD Card| [SanDisk MAX Endurance 64 GB microSD](https://www.amazon.com/SanDisk-Endurance-microSDXC-Adapter-Security/dp/B084CJ96GT) | 1 |
| Resistor | [1 Mohms 1% MBB02070C1004FRP00](https://octopart.com/mbb02070c1004frp00-vishay-42819429?r=sp) | 1 |
| Resistor | [10Mohm 1% 50ppm MBB02070C1005FRP00](https://octopart.com/mbb02070c1005frp00-vishay-42881138?r=sp) | 1 |
| Capacitor | [0.1 uF K104K15X7RF53L2](https://octopart.com/k104k15x7rf53l2-vishay-39696446?r=sp) | 1 |
| Power Connector	| [PCB terminal block - MKDS 1,5/ 2-5,08 1715721](https://octopart.com/1715721-phoenix+contact-12533?r=sp) | 1 |
| Male Header | [SparkFun Break Away Headers - Straight](https://www.sparkfun.com/products/116) | 2 |
| Female Header | [Pololu 0.100" (2.54 mm) Female Header: 1x2-Pin, Straight](https://www.pololu.com/product/1012) | 1 |
| Female Header | [Pololu 0.100" (2.54 mm) Female Header: 1x4-Pin, Straight](https://www.pololu.com/product/1014) | 1 |
| Female Header | [Pololu 0.100" (2.54 mm) Female Header: 1x6-Pin, Straight](https://www.pololu.com/product/1016) | 1 |
| Female Header | [Pololu 0.100" (2.54 mm) Female Header: 1x9-Pin, Straight](https://www.pololu.com/product/1019) | 2 | 
| Female Header | [Pololu 0.100" (2.54 mm) Female Header: 1x10-Pin, Straight](https://www.pololu.com/product/1020) | 1 |
| Female Header | [Pololu 0.100" (2.54 mm) Female Header: 1x12-Pin, Straight](https://www.pololu.com/product/1030) | 1 |
| Standoff | [SparkFun Standoff - Nylon (4-40; 3/4"; 10 pack)](https://www.sparkfun.com/products/11796) | 4 |
| Screw | [SparkFun Screw - Phillips Head (1/4", 4-40, 10 pack)](https://www.sparkfun.com/products/10453) | 8 |
| Enclosure | [Hammond Manufacturing 1554Q2GYCL](https://octopart.com/1554q2gycl-hammond-99088964?r=sp) | 1 |
| Enclosure Inner Panel | [Hammond Manufacturing 1554QPL](https://octopart.com/1554qpl-hammond-97562074?r=sp) | 1 |
| Power Panel Mount Connector | Bulgin<sup>1</sup> | 1 |
| Power Cable Connector | Bulgin<sup>1</sup> | 1 |
| Battery Ring Terminal | [Molex Perma-Seal Ring Terminal 14-16 AWG Wire 1/4" (M6) 19164-0026](https://octopart.com/19164-0026-molex-99569?r=sp) | 2 |


## 3.0 Assembly


1) Collect all of the necssary components and remove them from their packaging (Figure 1)
![IMG_6855](https://github.com/adamgarbo/cryologger-glacier-velocity-tracker/assets/22924092/9b9700ce-fe95-4ca5-a075-6a302962f196)


2) Solder through-hole resistors and capacitor
The first components to solder to the Cryologger PCB are the through-hole resistors and capacitor. Pay attention to the location of the resistors, as they vary in resistance and have a specific order.

3) Solder female headers to the PCB
* A tip 

4) Insert male headers into female headers

5) Solder male headers


6) Solder screw terminal
* Remove all components and insert the screw terminal. The PCB can then be turned upside for easy access to soldering the pins.

7) Connect SparkFun Qwiic OLED Display
* Attach two 3/8" nylon standoffs to the PCB with 1/4" 4-40 Phillips Head screws. Connect 50 mm Qwiic cable.

8) Cut I2C jumpers 
See section 3.1 for instructions on cutting the I2C jumpers and verifying operation of the system.

### 3.1 I2C Jumpers

When working with the Artemis Processor, I2C jumpers must be cut for all components in the system (i.e., MicroMod Data Logging Carrier Board, u-blox ZED-F9P and OLED display) for normal operation. The u-blox ZED-F9P is sensitive to the resistance on the I2C bus and a too high I2C bus resistance will result in erratic behaviour of the system (e.g., initialization failures, intermittent logging, degraded data quality).

For additional background information on this I2C pull-up resistor problem, see the following GitHub issue:
https://github.com/sparkfun/SparkFun_Ublox_Arduino_Library/issues/40

Measure the resistance of following connections before and after cutting the I2C jumpers:

MicroMod Data Logging Carrier Board:
* G2-3V3 and SDA
* G2-3V3 and SCL

u-blox ZED-F9P
* 3V3 and SDA
* 3V3 and SCL

All I2C connections are part of the same bus, so each jumper will affect resistance of all connected components. 

Prior to cutting the I2C jumpers, the resistance measurement should read approximately 2.2 kOhm. 

After the I2C jumpers are cut, very high resistance values should be measured. If the reading is lower then it means there's likely a jumper that still has a connection.

MicroMod Data Logging Carrier Board:
G2-3V3 and SDA: ~0.6 MOhm
G2-3V3 and SCL:  ~0.6 MOhm

u-blox ZED-F9P
3V3 and SDA: ~250 kOhm
3V3 and SCL: ~250 kOhm

In addition, it is important that all solder flux residue is properly cleaned, as it can change the resistance of pins and cause issues with the normal operation of the components.
