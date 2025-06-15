![License](https://img.shields.io/github/license/adamgarbo/Cryologger_Glacier_Velocity_Tracker)
![Release](https://img.shields.io/github/v/release/adamgarbo/Cryologger_Glacier_Velocity_Tracker)
![Issues](https://img.shields.io/github/issues/adamgarbo/Cryologger_Glacier_Velocity_Tracker)
[![DOI](https://zenodo.org/badge/248303047.svg)](https://doi.org/10.5281/zenodo.10522852)

# Cryologger - Glacier Velocity Tracker (GVT)
### Note: This documentation is currently under development.
<p align="center"><img src="https://github.com/adamgarbo/Cryologger_Glacier_Velocity_Measurement_System/blob/main/Images/DSC_6546.JPG" ></p>

## 1.0 Introduction

Reliance on expensive and proprietary commercial data acquisition and telemetry systems can present a barrier to monitoring remote polar environments. The development of low-cost, open-source instrumentation can greatly reduce the cost of research in remote locations, improve the spatiotemporal resolution of collected data, and provide new ways to observe and monitor the cryosphere. 

The [Cryologger Glacier Velocity Tracker (GVT)](https://github.com/cryologger/glacier-velocity-tracker) is a novel [Cryologger](https://github.com/cryologger) design configuration intended to collect high temporal resolution velocity measurements to assess glacier dynamics on daily timescales in the Canadian Arctic. The system is based on the Arduino electronics platform and harnesses the u-blox ZED-F9P multi-frequency Global Navigation Satellite System (GNSS) receiver.

## 2.0 Methods

### 2.1 Design
The Cryologger GVT is based on the open-source Arduino platform (www.arduino.cc). Code is written using the Arduino Integrated Development Environment (IDE) and benefits from the availability of community-generated libraries.

The design of the Cryologger GVT takes into consideration several key principles, including cost-effectiveness, power efficiency, modularity, ruggedness, and ease of use. It is comprised of off-the-shelf components that do not require specialized tools or training, making it accessible to researchers, citizen scientists, and hobbyists alike. These selected components are also resistant to harsh environmental conditions, ensuring year-round continuous cold-weather operation.

#### 2.1.1 Bill of materials

The Cryologger GVT is built using almost exclusively using components from [SparkFun](https://www.sparkfun.com/), with a list of the main components shown below. For a more detailed bill of materials, including requirements for power and mounting of a complete deployable system, please see [ASSEMBLY.md](/Documentation/ASSEMBLY.md).

**Table 1.** Main components making up Cryologger GVT. Prices are listed in USD and are current as of December 2023. Taxes and shipping are not included.
| Component | Product | Quantity | Cost (USD) |
| --- | --- | :---: | :---: |
| PCB | [Custom Cryologger Printed Circuit Board](https://jlcpcb.com) | 1 | $5.00 | 
| Processor | [SparkFun MicroMod Artemis Processor](https://www.sparkfun.com/products/16401) | 1 | $14.95 | 
| Carrier Board | [SparkFun MicroMod Data Logging Carrier Board](https://www.sparkfun.com/products/16829) | 1 | $21.50 | 
| GNSS Receiver | [SparkFun GPS-RTK-SMA Breakout - ZED-F9P](https://www.sparkfun.com/products/16481) | 1 | $274.95 | 
| GNSS Antenna | [SparkFun GNSS Multi-Band L1/L2 Surveying Antenna (TNC) - TOP106](https://www.sparkfun.com/products/17751) | 1 | $133.95 | 
| Display | [SparkFun Qwiic OLED Display](https://www.sparkfun.com/products/17153) | 1 | $10.95 |
| Voltage Regulator | [Pololu 5V, 600mA Step-Down Voltage Regulator D36V6F5](https://www.pololu.com/product/3792) | 1 | $6.95 |

#### 2.1.2 Design Evolution 

A carrier board was designed to simplify assembly and deployment. It provides 5.08 mm, 2-pin screw terminals for wiring the battery connection, and a resistor divider to measure battery voltage.

<p align="center"><img width="640" alt="image" src="https://user-images.githubusercontent.com/22924092/186201944-ff52a2b9-d714-46c5-bb36-c9115895dd63.png"></p>
<p align="center"><b>Figure 1.</b> Open-source carrier board design.</p>

#### 2.1.3 Mock-up
The Cryologger GVT was originally intended to be installed in a small electronics enclosure that is mounted in a larger enclosure for deployment outdoors.

<p align="center"><img width="640" alt="image" src="https://user-images.githubusercontent.com/22924092/113727907-9525be80-96c3-11eb-9db5-a16260b8cdfe.png"></p>
<p align="center"><b>Figure 2.</b> Open-source carrier board design.</p>

### 2.2 Operation

When initially powered on, the Cryologger GVT will attempt to establish a GNSS fix and synchronize the real-time clock (RTC) of the MicroMod Artemis Processor for up to 5 minutes. Once the RTC is synchronized, the system will set an alarm to wake at the user-specified time and then enter a low-power deep sleep.  Nominally, the system will wake daily and log data from the u-blox GNSS receiver to a microSD card for a period of 3 hours. There are three different logging modes available to the end end-user, which can be tailored to the data collection requirements and available power capacity.

Versions 2.2.1 and above of the Cryologger GVT are equipped with an OLED display. This display is used to provide detailed information to the end user about the status of the Cryologger at each stage of operation, including boot-up, initialization of components, acquisition of GNSS fix, and logging of data. 

In addition to the OLED display messages, a series of LED blink patterns can also be used to indicate system operation and guide possible troubleshooting steps that should be attempted if any errors is encountered. A detailed table of the LED blink patterns can be found in the [DEPLOYMENT.md](/Documentation/DEPLOYMET.md) documentation. 

While the system is in deep sleep, a Watchdog Timer (WDT) will wake the system to check the program has not frozen every 10 seconds and perform a reset, if necessary. This helps to ensure the long-term reliability of the system.

### 2.3 Data Processing

Data is logged in u-blox's proprietary .ubx format. Log files can be converted to RINEX using RTKLIB or Emlid Studio and submitted to NRCan's Precise Point Positioning (PPP) tool: https://webapp.geod.nrcan.gc.ca/geod/tools-outils/ppp.php

For a more detailed guide on the processing of data, please see "[PROCESSING.md](/Documentation/PROCESSING.md).

## 3.0 Deployments

Between 2021 and 2023, a total of 28 Cryologger GVT deployments were made by several different academic institutions, including the Univerity of Ottawa, Carleton University, University of Waterloo (Canada), Carnegie Mellon University (United States), and University of Fribourg (Switzerland). These deployments were made on glaciers in several geographically distinct locations, including Canada, the United States and Kyrgyzstan. Two case studies of deployments made by the University of Ottawa and the University of Waterloo on glaciers in the Yukon and Nunvaut, Canada are explored in more detail below.

<p align="center"><img src="https://github.com/cryologger/glacier-velocity-tracker/assets/22924092/bef5b318-c0ab-4b35-86e6-98851041ca5d" width="640"></p>
<p align="left"><b>Figure 3.</b> Map of Cryologger GVT deployments made between 2021 and 2023.</p>

### 3.1 Lowell Glacier, Yukon, Canada

<p align="left"><img src="https://github.com/cryologger/glacier-velocity-tracker/assets/22924092/4bc646a5-447c-4fad-b1d1-11d9234cead3" width="640"></p>
<p align="left"><b>Figure 4.</b> Map of Cryologger GVT deployments made in 2021 on the Lowell Glacier, Yukon.</p>

### 3.2 Belcher Glacier, Devon Island, Nunavut, Canada

<p align="left"><img src="https://github.com/cryologger/glacier-velocity-tracker/assets/22924092/bedc9e69-e742-4c28-83a1-577907ddc525" width="640"></p>
<p align="left"><b>Figure 5.</b> Map of Cryologger GVT deployments made in 2021 on the Lowell Glacier, Yukon and Belcher Glacier, Devon Island, Nunavut.</p>


<p align="center"><img src="https://github.com/adamgarbo/Cryologger_Glacier_Velocity_Measurement_System/blob/main/Images/DSC_5133.JPG" width="480">&nbsp;&nbsp;<img src="https://github.com/adamgarbo/Cryologger_Glacier_Velocity_Measurement_System/blob/main/Images/DSC_5144.JPG" width="480"></p>
<p align="left"><b>Figure 6.</b> Deployment of the "Belcher Lower" Cryologger GVT on the Belcher Glacier, Devon Island, Nunavut.</p>


## 4.0 Results

Preliminary results from the 2021 deployments of Cryologger GVTs on Lowell Glacier, Kluane National Park, Yukon and Belcher Glacier, Devon Island, Nunavut are presented below.

### 4.1 Lowell Glacier, Yukon, Canada

The initial Cryologger GVTs deployments on Lowell Glacier were both successful, achieving continuous operation throughout the year. 

Collected data that was PPP-processed revealed the GVTs deployed on Lowell Glacier were able to achieve daily position measurements with accuracies of <3 cm. Daily displacements ranged between 0.25 – 4.4 m d<sup>-1</sup> and annual cumulative distances were between 200 – 700 m y<sup>-1</sup>. Both systems showed evidence of ongoing glacier surge cycles and excitingly were also able to capture periods of marked glacier velocity corresponding to local rainfall events and enhanced basal sliding.

<p align="center"><img src="https://github.com/cryologger/glacier-velocity-tracker/assets/22924092/a395eb78-2677-4056-a3be-a7efc64793f5" width="480"><img src="https://github.com/cryologger/glacier-velocity-tracker/assets/22924092/a6510e11-2bd1-4351-ab14-6b0e19e49312" width="480"></p>
<p align="left"><b>Figure 7.</b> Cumulative distance and daily displacement of Lowell Glacier, Yukon recorded between between 2021 and 2022.</p>

### 4.2 Belcher Glacier, Devon Island, Nunavut, Canada

Cryologger GVT deployments made on Belcher Glacier were also able to achieve successful operation throughout the entire year. The data collected between 2021-2022 represents the first uninterrupted annual velocity record of the Belcher Glacier ever recorded.

PPP-processed data showed the GVTs deployed on Belcher Glacier were also able to achieve daily position measurements with accuracies of <3 cm. Daily displacements ranged between 0.18 – 0.45 m d<sup>-1</sup> and annual cumulative distances were between 75 – 150 m y<sup>-1</sup>. These results are also in agreement with velocities derived from synthetic-aperture radar (SAR) remote sensing data.

<p align="center"><img src="https://github.com/cryologger/glacier-velocity-tracker/assets/22924092/681f6bb6-02ef-4884-b190-5a7af0607280" width="480"><img src="https://github.com/cryologger/glacier-velocity-tracker/assets/22924092/3efcf414-432b-4bdb-b325-65d84687d052" width="480"></p>
<p align="left"><b>Figure 8.</b> Cumulative distance and daily displacement of Belcher Glacier, Devon Island recorded between between 2021 and 2022.</p>

## 5.0 Conclusions

Cryologger GVT deployments have demonstrated they can achieve continuous year-round operation. Data processed with Precise Point Positioning techniques also reveals the systems can achieve daily position measurements with accuracies of <3 cm. These promising results demonstrate that open-source hardware and software can provide a reliable and cost-effective alternative to commercially available equipment for use in glaciological monitoring.

## Funding

This project is supported by:

<p align="left"><img src="https://github.com/cryologger/glacier-velocity-tracker/assets/22924092/4ab67319-697e-42ac-972a-8859b06c2076" width="250"></p>

## Acknowledgements

This project would not have been possible without the designs, software and tutorials provided by the open-source community. A special thanks to the authors of and contributors to the [SparkFun u-blox GNSS Arduino Library](https://github.com/sparkfun/SparkFun_u-blox_GNSS_Arduino_Library).

## Repository Contents

* **/Documentation** - Assembly, deployment and troubleshooting guides, as well as information on components used and associated costs.

* **/Hardware** - KiCad PCB schematic and design files.

* **/Software** -  Arduino code and Python data analysis scripts.

## License Information
Cryologger code, firmware, and software is licensed under the GNU General Public License v3.0 (https://www.gnu.org/licenses/gpl-3.0.en.html). 

<p xmlns:cc="http://creativecommons.org/ns#" >Cryologger hardware is licensed under <a href="https://creativecommons.org/licenses/by-nc-sa/4.0/?ref=chooser-v1" target="_blank" rel="license noopener noreferrer" style="display:inline-block;">CC BY-NC-SA 4.0 <img style="height:22px!important;margin-left:3px;vertical-align:text-bottom;" src="https://mirrors.creativecommons.org/presskit/icons/cc.svg?ref=chooser-v1" alt=""><img style="height:22px!important;margin-left:3px;vertical-align:text-bottom;" src="https://mirrors.creativecommons.org/presskit/icons/by.svg?ref=chooser-v1" alt=""><img style="height:22px!important;margin-left:3px;vertical-align:text-bottom;" src="https://mirrors.creativecommons.org/presskit/icons/nc.svg?ref=chooser-v1" alt=""><img style="height:22px!important;margin-left:3px;vertical-align:text-bottom;" src="https://mirrors.creativecommons.org/presskit/icons/sa.svg?ref=chooser-v1" alt=""></a></p>

Cheers,

**Adam Garbo**
