<p align="left">
<img alt="GitHub" src="https://img.shields.io/github/license/adamgarbo/Cryologger_Glacier_Velocity_Tracker">
<img alt="GitHub release (latest by date)" src="https://img.shields.io/github/v/release/adamgarbo/Cryologger_Glacier_Velocity_Tracker">
<img alt="GitHub issues" src="https://img.shields.io/github/issues/adamgarbo/Cryologger_Glacier_Velocity_Tracker">
</p>


# Cryologger - Glacier Velocity Tracker (GVT)
<p align="center"><img src="https://github.com/adamgarbo/Cryologger_Glacier_Velocity_Measurement_System/blob/main/Images/DSC_6546.JPG" ></p>

## 1.0 Introduction

Reliance on expensive and proprietary commercial data acquisition and telemetry systems can present a barrier to monitoring remote polar environments. The development of low-cost, open-source instrumentation can greatly reduce the cost of research in remote locations, improve the spatiotemporal resolution of collected data, and provide new ways to observe and monitor the cryosphere. 

The [Cryologger Glacier Velocity Tracker (GVT)](https://github.com/cryologger/glacier-velocity-tracker) is a novel [Cryologger](https://github.com/cryologger) design configuration intended to collect high temporal resolution velocity measurements to assess glacier dynamics on daily timescales in the Canadian Arctic. The system is based on the Arduino electronics platform, and harnesses the u-blox ZED-F9P multi-frequency Global Navigation Satellite System (GNSS) receiver.

## 2.0 Methods

### 2.1 Design

The design of the Cryologger GVT takes into consideration several key principles, including cost effectiveness, power efficiency, modularity, ruggedness, and ease of use. It is comprised of off-the-shelf components that do not require specialized tools or training, making it accessible to researchers, citizen scientists, and hobbyists alike. These selected components are also resistant to harsh environmental conditions, ensuring year-round continuous cold-weather operation.

#### 2.1.1 Bill of materials

The Cryologger GVT is built using almost exclusively using components from [SparkFun](https://www.sparkfun.com/), with a list of the main components shown below.  

For a more detailed bill of materials, including requirements for power and mounting for a complete deployable system, please see [ASSEMBLY.md](/Documentation/ASSEMBLY.md).

**Table 1.** Components making up Cryologger GVT v2.2.1. Prices are listed in USD and are current as of December 2023. Taxes and/or shipping are not included. <sup>1</sup>Denotes optional component.
| Component | Product | Quantity | Cost (USD) |
| --- | --- | :---: | :---: |
| PCB | [Custom Cryologger Printed Circuit Board](https://jlcpcb.com) | 1 | $5.00 | 
| Processor | [SparkFun MicroMod Artemis Processor](https://www.sparkfun.com/products/16401) | 1 | $14.95 | 
| Carrier Board | [SparkFun MicroMod Data Logging Carrier Board](https://www.sparkfun.com/products/16829) | 1 | $21.50 | 
| GNSS Receiver | [SparkFun GPS-RTK-SMA Breakout - ZED-F9P](https://www.sparkfun.com/products/16481) | 1 | $274.95 | 
| GNSS Antenna | [SparkFun GNSS Multi-Band L1/L2 Surveying Antenna (TNC) - TOP106](https://www.sparkfun.com/products/17751) | 1 | $133.95 | 
| Display | [SparkFun Qwiic OLED Display](https://www.sparkfun.com/products/17153) | 1 | $10.95 |
| Qwiic Cable | [Flexible Qwiic Cable - 50mm](https://www.sparkfun.com/products/17260) | 1 | $1.05 |
| GNSS Antenna Cable | [SparkFun Interface Cable - SMA Female to SMA Male (25cm)](https://www.sparkfun.com/products/12861) | 1 | $5.50 |
| GNSS Antenna Cable | [SparkFun Interface Cable - SMA Male to TNC Male (300mm)](https://www.sparkfun.com/products/17833) | 1 |	$10.95 | 
| Voltage Regulator | [Pololu 5V, 600mA Step-Down Voltage Regulator D36V6F5](https://www.pololu.com/product/3792) | 1 | $6.95 |
| microSD Card| [SanDisk MAX Endurance 64 GB microSD](https://www.amazon.com/SanDisk-Endurance-microSDXC-Adapter-Security/dp/B084CJ96GT) | 1 | $15.99 | 
| Resistor | [1 Mohms 1% MBB02070C1004FRP00](https://octopart.com/mbb02070c1004frp00-vishay-42819429?r=sp) | 1 | $0.29 |
| Resistor | [10Mohm 1% 50ppm MBB02070C1005FRP00](https://octopart.com/mbb02070c1005frp00-vishay-42881138?r=sp) | 1 |	$0.32 |
| Capacitor | [0.1 uF K104K15X7RF53L2](https://octopart.com/k104k15x7rf53l2-vishay-39696446?r=sp) | 1 | $0.23 |
| Power Connector	| [PCB terminal block - MKDS 1,5/ 2-5,08 1715721](https://octopart.com/1715721-phoenix+contact-12533?r=sp) | 1 | $1.46 |
| Male Header | [SparkFun Break Away Headers - Straight](https://www.sparkfun.com/products/116) | 2 | $3.10 |
| Female Header | [Pololu 0.100" (2.54 mm) Female Header: 1x2-Pin, Straight<sup>1</sup>](https://www.pololu.com/product/1012) | 1 | $0.39 |
| Female Header | [Pololu 0.100" (2.54 mm) Female Header: 1x4-Pin, Straight<sup>1</sup>](https://www.pololu.com/product/1014) | 1 | $0.53 |
| Female Header | [Pololu 0.100" (2.54 mm) Female Header: 1x6-Pin, Straight<sup>1</sup>](https://www.pololu.com/product/1016) | 1 | $0.66 |
| Female Header | [Pololu 0.100" (2.54 mm) Female Header: 1x9-Pin, Straight<sup>1</sup>](https://www.pololu.com/product/1019) | 2 | $0.85 |
| Female Header | [Pololu 0.100" (2.54 mm) Female Header: 1x10-Pin, Straight<sup>1</sup>](https://www.pololu.com/product/1020) | 1 | $0.46 |
| Female Header | [Pololu 0.100" (2.54 mm) Female Header: 1x12-Pin, Straight<sup>1</sup>](https://www.pololu.com/product/1030) | 1 | $0.55 |
| Standoff | [SparkFun Standoff - Nylon (4-40; 3/4"; 10 pack)](https://www.sparkfun.com/products/11796) | 1 | $4.50 |
| Screw | [SparkFun Screw - Phillips Head (1/4", 4-40, 10 pack)](https://www.sparkfun.com/products/10453) | 1 | $1.60 |
| Enclosure | [Hammond Manufacturing 1554Q2GYCL](https://octopart.com/1554q2gycl-hammond-99088964?r=sp)<sup>1</sup> | 1 | $27.78 |
| Enclosure Inner Panel | [Hammond Manufacturing 1554QPL](https://octopart.com/1554qpl-hammond-97562074?r=sp)<sup>1</sup> | 1 | $9.00 |
| Power Panel Mount Connector | Bulgin<sup>1</sup> | 1 | $25.00 |
| Power Cable Connector | Bulgin<sup>1</sup> | 1 | $25.00 |
| | | **Total** | **~$600** |

#### Carrier board
In version 2.0, a carrier board was designed to simplify assembly and deployment. It provides 5.08 mm, 2-pin screw terminals for wiring the battery connection, and a resistor divider to measure battery voltage.

<p align="center"><img width="640" alt="image" src="https://user-images.githubusercontent.com/22924092/186201944-ff52a2b9-d714-46c5-bb36-c9115895dd63.png"></p>
<p align="center"><b>Figure 1.</b> Open-source carrier board design.</p>

#### 2.1.3 Mock-up
The Cryologger GVT was originally intended to be installed in a small electronics enclosure that is mounted in a larger enclosure for deployment outdoors.

<p align="center"><img width="640" alt="image" src="https://user-images.githubusercontent.com/22924092/113727907-9525be80-96c3-11eb-9db5-a16260b8cdfe.png"></p>
<p align="center"><b>Figure 2.</b> Open-source carrier board design.</p>


### 2.2 Operation

When initially powered on, the Cryologger GVT will attempt to establish a GNSS fix and sychronize the real-time clock (RTC) of the MicroMod Artemis Processor for up to 5 minutes. Once the RTC is synchronized, the system will set an alarm to wake at the user-specified time and then enter a low-power deep sleep.  Nominally, the system will wake daily and log data from the u-blox GNSS receiver to a microSD card for a period of 3 hours. There are three different logging modes available to the end end-user, which can be tailored to the data collection requirements and available power capacity.

Versions 2.2.1 and above of the Cryologger GVT are equipped with an OLED display. This display is used to provided detailed information to the end user about the status of the Cryologger at each stage of operation, including boot-up, initialization of components, acquisition of GNSS fix, and logging of data. 

In addition to the OLED display messages, a series of LED blink patterns can also be used to indicate system operation and guide possible troubleshooting steps that should be attempted if any errors is encountered. A detailed table of the LED blink patterns can be found in the [DEPLOYMENT.md](/Documentation/DEPLOYMET.md) documentation. 

While the system is in deep sleep, a Watchdog Timer (WDT) will wake the system to check the program has not frozen every 10 seconds and perform a reset, if necessary. This helps to ensure the long-term reliability of the system.

### 2.3 Data Processing

Data is logged in u-blox's proprietary .ubx format. Log files can be converted to RINEX using RTKLIB or Emlid Studio and submitted to NRCan's Precise Point Positioning (PPP) tool: https://webapp.geod.nrcan.gc.ca/geod/tools-outils/ppp.php

## 3.0 Deployments

Between 2021 and 2023, a total of 28 Cryologger GVT deployments have been made by several of different academic institutions, including Carleton University, Univerity of Ottawa, and University of Waterloo (Canada), Carnegie Mellon University (United States), and University of Fribourg (Switzerland).

<p align="center"><img src="https://github.com/adamgarbo/cryologger-glacier-velocity-tracker/assets/22924092/053d6037-26ce-4e87-b219-ee7988b21e0c" width="480"></p>
<p align="left"><b>Figure 3.</b> Map of Cryologger GVT deployments made between 2021 and 2023.</p>

<p align="left"><img src="https://github.com/cryologger/glacier-velocity-tracker/assets/22924092/4bc646a5-447c-4fad-b1d1-11d9234cead3" width="480"><img src="https://github.com/cryologger/glacier-velocity-tracker/assets/22924092/bedc9e69-e742-4c28-83a1-577907ddc525" width="480"></p>
<p align="left"><b>Figure 4.</b> Map of Cryologger GVT deployments made in 2021 on the Lowell Glacier, Yukon and Belcher Glacier, Devon Island, Nunavut.</p>

<p align="left"><img src="https://github.com/cryologger/glacier-velocity-tracker/assets/22924092/4bc646a5-447c-4fad-b1d1-11d9234cead3" width="480"><img src="https://github.com/cryologger/glacier-velocity-tracker/assets/22924092/bedc9e69-e742-4c28-83a1-577907ddc525" width="480"></p>
<p align="left"><b>Figure 4.</b> Map of Cryologger GVT deployments made in 2021 on the Lowell Glacier, Yukon and Belcher Glacier, Devon Island, Nunavut.</p>

<p align="center"><img src="https://github.com/adamgarbo/Cryologger_Glacier_Velocity_Measurement_System/blob/main/Images/DSC_6546.JPG" width="480"></p>

## 4.0 Results

Preliminary results from the 2021 deployments of Cryologger GVTs on Lowell Glacier, Kluane National Park, Yukon and Belcher Glacier, Devon Island, Nunavut are presented below.

### 4.1 Lowell Glacier, Yukon

The Cryologger GVTs deployed on Lowell Glacier were both able to achieve successful operation throughout the entire year. 

Colleceted data that was PPP-processed revealed the GBTs deployed on Lowell Glacier were able to achieve daily position measurements with accuracies of <3 cm. Daily displacements ranged between 0.25 – 4.4 m d<sup>-1</sup> and annual cumulative distances were between 200 – 700 m y<sup>-1</sup>. Both systems showed evidence of ongoing glacier surge cycles, and excitingly were also able to capture periods of marked glacier velocity correspondeding to local rainfall events and enhanced basal sliding.

<p align="center"><img src="https://github.com/cryologger/glacier-velocity-tracker/assets/22924092/a395eb78-2677-4056-a3be-a7efc64793f5" width="480"><img src="https://github.com/cryologger/glacier-velocity-tracker/assets/22924092/a6510e11-2bd1-4351-ab14-6b0e19e49312" width="480"></p>
<p align="left"><b>Figure 6.</b> Cumulative distance and daily displacement of Lowell Glacier, Yukon recorded between between 2021 and 2022.</p>

### 4.2 Belcher Glacier, Devon Island, Nunavut


<p align="center"><img src="https://github.com/cryologger/glacier-velocity-tracker/assets/22924092/681f6bb6-02ef-4884-b190-5a7af0607280" width="480"><img src="https://github.com/cryologger/glacier-velocity-tracker/assets/22924092/3efcf414-432b-4bdb-b325-65d84687d052" width="480"></p>
<p align="left"><b>Figure 7.</b> Cumulative distance and daily displacement of Belcher Glacier, Devon Island recorded between between 2021 and 2022.</p>

## 5.0 Conclusions

These promising results demonstrate that inexpensive, open-source hardware and software can provide a reliable, and cost-effective alternative to commercially available equipment for use in glaciological monitoring.

## Repository Contents

* **/Documentation** - Various assembly, troubleshooting and deployment guides.

* **/Software** - Contains the Arduino code.

* **/Hardware** - KiCad PCB schematic and board design files.

* **/Bill of Materials** - Information on components and their associated costs.

* **/Archive** - Previous design versions.

## License Information
This project is released under the GNU General Public License v3.0 (https://www.gnu.org/licenses/gpl-3.0.en.html)

Cheers,

**Adam**
