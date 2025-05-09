# Cryologger Glacier Velocity Tracker - Installation
This guide provides step-by-step instructions on installing the Arduino IDE and required board definitions and libraries, which are necessary for uploading code to the Cryologger GVT.

## Step 1: Download Arduino IDE  
* Navigate to https://www.arduino.cc/en/software and download the Legacy IDE (1.8.X) version of the Arduino IDE.

![image](/Images/arduino-ide-download.png)

## Step 2: Add Additional Boards Manager URL:
* In the Arduino IDE navigate to: File > Preferences
* Add the following "Additional Boards Manager URL" as shown in the screenshot below:
```https://raw.githubusercontent.com/sparkfun/Arduino_Apollo3/master/package_sparkfun_apollo3_index.json```
* Also during this step, check the "compile" and "upload" boxes for "Show verbose output during" and change "Compiler warnings" to "All"

![image](/Images/arduino-ide-preferences.png)

## Step 3: Install Board Definition
* Navigate to: Tools > Boards > Boards Manager
* Search for: SparkFun Apollo3
* Select and install version: 1.2.3
  * Warning: Do *not* install version 2.0.0 as it is incompatible with the software.
![image](/Images/arduino-ide-boards.png)

## Step 4: Install Libraries
The glacier velocity measurement system requires three external libraries. These can either be downloaded using the Arduino IDE's Library Manager (requires Internet connection), or directly from GitHub.

* Navigate to: Tools > Manage Libraries
* Search for and install the following libraries:

**Table 1.** Libraries required by Cryologger GVT. Last updated 2025-04-24.
| Library                              | Version | GitHub Repository                                               |
|--------------------------------------|:-------:|-----------------------------------------------------------------|
| SparkFun u-blox GNSS v3              |  3.0.16 | https://github.com/sparkfun/SparkFun_u-blox_GNSS_v3             |
| SdFat                                |  2.2.2  | https://github.com/greiman/SdFat                                |
| SparkFun Qwiic OLED Arduino Library  |  1.0.5  | https://github.com/sparkfun/SparkFun_Qwiic_OLED_Arduino_Library |
| ArduinoJ SON                         |  7.4.1  | https://github.com/sparkfun/SparkFun_Qwiic_OLED_Arduino_Library |

![image](/Images/arduino-ide-library.png)

## Step 5: Download Software
Code for the Cryologger GVT is made available on the following GitHub repository:
* https://github.com/adamgarbo/Cryologger_Glacier_Velocity_Tracker
* Click on "Releases" and download the v3.0.1 .zip file:

## Step 6: Connect Hardware & Configure Port Settings
* Connect the MicroMod Artemis Data Logging Carrier board via a USB-C cable. 
* In the Arduino IDE click on Tools > Board: >   
![image](/Images/arduino-ide-board-1.png)
![image](/Images/arduino-ide-board-2.png)
![image](/Images/arduino-ide-board-3.png)

* Then click on "Tools", navigate to "SVL Baud Rate" and select 460800 baud.
![image](/Images/arduino-ide-baud-1.png)
![image](/Images/arduino-ide-baud-2.png)

## Step 7: Verify Program
* Navigate to the /Software/Arduino/cryologger_gvt folder of the downloaded repository
* Double click on the `cryologger_gvt.ino` file
* Click on the checkmark in the upper left corner of the Arduino IDE program window
* Watch the output window for any compilation errors
![image](/Images/arduino-ide-verify.png)

## Step 8: Upload Program
* Once the code has compiled successfully, click on the right-pointed arrow to upload the code
* Watch the output window for compilation errors and/or success messages
* If no errors are presented, the code has been successfully uploaded.
![image](/Images/arduino-ide-upload.png)

## Step 9: Observe Serial Monitor
* After successfully uploading the program, click on the magnifying glass in the top right-hand corner to open the Serial Monitor
* Click on the baud dropdown and select 115200 baud
* Ensure the data logging mode and settings are correct
![image](/Images/arduino-ide-serial-monitor-1.png)
![image](/Images/arduino-ide-serial-monitor-2.png)

## Step 10: Next Steps
* Next to be covered is the detailed operation of the Cryologger GVT, including customized logging schedules and interpreting the OLED display messages.
* This information is made available in OPERATION.md.
