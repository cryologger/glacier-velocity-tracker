# Cryologger GNSS - Assembly

## 1.0 Introduction
This guide provides detailed instructions on the assembly of a Cryologger GNSS system.

## 2.0 Bill of Materials
The components required to assemble a Cryologger GNSS system are split into two groups: 
1) Components required for the assembly of the Cryologger (e.g., PCB, GNSS receiver, microcontroller, etc.)
2) Components required for the mounting, power and deployment of the complete system (e.g., tripod, solar panel, battery, etc.).

This guide focuses exclusively on first group, the assembly of the Cryologger.

**Table 1.** Required tools for asssembly of Cryologger GNSS v2.21 system.
| Tool | Recommended Product |
| --- | --- | 
| Soldering iron | [Hakko FX-888D Digital Soldering Station](https://hakkousa.com/fx-888d-digital-soldering-station.html) |
| Solder | [SparkFun Solder - 1/4lb Spool (0.020") Special Blend](https://www.sparkfun.com/products/10242) |
| Side cutter | [Hakko CHP-170 Micro Cutter](https://hakkousa.com/chp-170-micro-cutter.html) |
| Pliers | [Hakko CHP PN-2001 Pointed Nose Pliers](https://hakkousa.com/products/chp-tools/chp-hand-tools/chp-pn-2001-pointed-nose-pliers.html) |
| Hot-air rework station | [Quick 957DW+](https://www.adafruit.com/product/1869) |
| Screw driver | Phillips head screw driver |
| PCB cleaning brush | [ESD-Safe PCB Cleaning Brush](https://www.adafruit.com/product/1209) |
| PCB cleaning solution | Deionized (DI) water |

**Table 2.** Components required for asssembly of Cryologger GNSS v2.21 system.
| Component | Product | Quantity |
| --- | --- | :---: |
| PCB | [Cryologger Printed Circuit Board (PCB)](https://jlcpcb.com) | 1 |
| Processor | [SparkFun MicroMod Artemis Processor](https://www.sparkfun.com/products/16401) | 1 |
| Carrier Board | [SparkFun MicroMod Data Logging Carrier Board](https://www.sparkfun.com/products/16829) | 1 |
| GNSS Receiver | [SparkFun GPS-RTK-SMA Breakout - ZED-F9P](https://www.sparkfun.com/products/16481) | 1 |
| Display | [SparkFun Qwiic OLED Display](https://www.sparkfun.com/products/17153) | 1 |
| Qwiic Cable | [Qwiic Cable - 50mm](https://www.sparkfun.com/products/14426) | 1 |
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
| GNSS Antenna | [SparkFun GNSS Multi-Band L1/L2 Surveying Antenna (TNC) - TOP106](https://www.sparkfun.com/products/17751) | 1 | 
| GNSS Antenna Cable | [SparkFun Interface Cable - SMA Female to SMA Male (25cm)](https://www.sparkfun.com/products/12861) | 1 |
| GNSS Antenna Cable | [SparkFun Interface Cable - SMA Male to TNC Male (300mm)](https://www.sparkfun.com/products/17833) | 1 | 
| Enclosure | [Hammond Manufacturing 1554Q2GYCL](https://octopart.com/1554q2gycl-hammond-99088964?r=sp) | 1 |
| Enclosure Inner Panel | [Hammond Manufacturing 1554QPL](https://octopart.com/1554qpl-hammond-97562074?r=sp) | 1 |
| Power Panel Mount Connector | Bulgin<sup>1</sup> | 1 |
| Power Cable Connector | Bulgin<sup>1</sup> | 1 |
| Battery Ring Terminal | [Molex Perma-Seal Ring Terminal 14-16 AWG Wire 1/4" (M6) 19164-0026](https://octopart.com/19164-0026-molex-99569?r=sp) | 2 |


## 3.0 Assembly

### 3.1 Cryologger PCB Assembly
**1) Collect all required components**
* It is suggested to first gather all of the components listed in the bill of materials (Table 2), required for the assembly of the Cryologger. Remove the components from their packaging and inspect them for any signs of damage.

**2) Solder through-hole resistors and capacitor**
* The first components recommended to be soldered to the Cryologger PCB are the through-hole resistors and capacitor (Figure 1). 
* Both the capacitor and resistors can be inserted into the PCB in any orientation.
* It is important to pay attention to the placement of the resistors, as they differ in resistance. 
  * R1 = 10 MOhm. Colour code: Brown Black Black Green Brown
  * R2 = 1 MOhm. Colour code: Brown Black Black Yellow Brown
* For more information on resistance colour codes, please see: https://www.digikey.com/en/resources/conversion-calculators/conversion-calculator-resistor-color-code

<p align="center"><img width="720" alt="image" src="https://github.com/adamgarbo/cryologger-glacier-velocity-tracker/assets/22924092/8fa15e3c-aedc-4379-ada3-3029dafc1340"></p>
<p align="center"><b>Figure 1.</b> Location of through-hole resistors and capacitor to be soldered to the PCB.</p>

**3) Solder female headers to the PCB**
* A tip when soldering the female headers is to do two at a time and use a couple of uncut rows of male headers to keep them in their position. The PCB can then be turned upside down and the contacts easily soldered. 
<p align="center"><img width="720" alt="image" src="https://github.com/adamgarbo/cryologger-glacier-velocity-tracker/assets/22924092/56e697ea-1a51-4ec6-9fa5-1ee2ca9e5146"></p>
<p align="center"><b>Figure 2.</b> Locations of female headers to be soldered to the PCB.</p>

**6) Solder screw terminal**
* Once the female headers are soldered , insert the screw terminal into the PCB, which can then be turned upside down for easy access to soldering the contacts.
<p align="center"><img width="720" alt="image" src="https://github.com/adamgarbo/cryologger-glacier-velocity-tracker/assets/22924092/9cda10ee-1666-4f16-8e5e-f8e86fa08617"></p>
<p align="center"><b>Figure 3.</b> Location of screw terminal to be soldered to the PCB.</p>

**7) Solder components**
* Insert male headers into female headers shown in Figure 2.
* Place the SparkFun MicroMod Data Logging Carrier Board and GPS-RTK2 boards onto the male header pins and solder the contacts.
* The Pololu step-down voltage regulator will also need to be soldered, but is more tricky.

**8) Clean solder flux residue**
* It is important that all solder flux residue is properly cleaned, as it can change the resistance of pins and cause issues with the normal operation of the system.
* Pour a small amount of deionized water into a clean container. 
* Dip the PCB cleaning brush into the water and then gently scrub all of the soldered connections on the bottom of the PCB and on the top components.
* If available, compressed air can be used to blow off the water.
* Otherwise, use the hot-air rework station at 300°C and with a medium sized tip to evaporate the remaining water. 
  * Do not leave the tip blowing on any one spot for longer than 3 seconds.
  * Be careful around the following locations:
    * The microSD card of the MicroMod Data Logging Carrier Board.
  
**9) Cut I2C jumpers**
See section 3.1.1 for instructions on cutting the I2C jumpers and verifying the resistance values of the system.

**10) Connect SparkFun Qwiic OLED Display**
* Attach two 3/8" nylon standoffs to the PCB with 1/4" 4-40 Phillips Head screws. 
* Connect 50 mm Qwiic cable.

**11) Upload code and test system**
* Please see https://github.com/adamgarbo/cryologger-glacier-velocity-tracker/blob/main/Documentation/INSTALLATION.md

#### 3.1.1 I2C Jumpers

When working with the Artemis Processor, I2C jumpers must be cut for all components in the system (i.e., MicroMod Data Logging Carrier Board, u-blox ZED-F9P and OLED display). The u-blox ZED-F9P is sensitive to the resistance on the I2C bus and a too high resistance can result in erratic behaviour of the system (e.g., initialization failures, intermittent logging, degraded data quality).

All I2C connections are part of the same bus, so each jumper will affect resistance of all connected components. 

For additional background information on this I2C pull-up resistor problem, see the following GitHub issue:
https://github.com/sparkfun/SparkFun_Ublox_Arduino_Library/issues/40

**1) Measure I2C bus resistance **
* Measure the resistance of following connections listed below in Table 3 before cutting the I2C jumpers. The values of the expected resistance measurements are also included.

**Table 3.** Connections to measure resistance and expected resistance values.
| Component | Connection 1| Connection 2 | Resistance (Jumpers Uncut) | Resistance (Jumpers Cut) |
| --- | :---: | :---: | :---: | :---: | 
| MicroMod Data Logging Carrier Board | G2-3V3 | SDA | ~2.2 kΩ | ~600 kΩ |
| MicroMod Data Logging Carrier Board | G2-3V3 | SCL | ~2.2 kΩ | ~600 kΩ |
| u-blox ZED-F9P | 3V3 | SDA | ~2.2 kΩ | ~250 kΩ |
| u-blox ZED-F9P | 3V3 | SDA | ~2.2 kΩ | ~250 kΩ |

**2) Cut I2C jumpers**
* Using a sharp utility knife, carefully cut each of the I2C jumpers.
* A good strategy is to make two cuts, along each of the pads so that the middle thin part of the PCB trace is removed entirely
* A digital multimeter can be used to confirm the jumper is "open" by measuring each side of the pads.



**3) Measure I2C bus resistance (again)**
After the I2C jumpers are cut, very high resistance values should be measured (see Table 3). Please note the resistance values may differ slightly depending on the quality of the digital multimeter (DMM) that is used. 

If the resistance measurement is lower than expected, it likely means there's still a jumper that has a connection. This can be troubleshooted by removing each component and measureing the I2C jumpers directly.
