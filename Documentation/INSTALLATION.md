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
![image](/Images/arduino-ide-boards.png)

## Step 4: Install Libraries
The glacier velocity measurement system requires three external libraries. These can either be downloaded using the Arduino IDE's Library Manager (requires Internet connection), or directly from GitHub.

* Navigate to: Tools > Manage Libraries
* Search for and install the following libraries:

**Table 1.** Libraries required by Cryologger GVT. Last updated 2023-06-19.
| Library                              | Version | GitHub Repository                                               |
|--------------------------------------|:-------:|-----------------------------------------------------------------|
| SparkFun u-blox GNSS v3              |  3.0.16 | https://github.com/sparkfun/SparkFun_u-blox_GNSS_v3             |
| SdFat                                |  2.2.2  | https://github.com/greiman/SdFat                                |
| SparkFun Qwiic OLED Arduino Library  |  1.0.5  | https://github.com/sparkfun/SparkFun_Qwiic_OLED_Arduino_Library |

![image](/Images/arduino-ide-library.png)

## Step 5: Download Software
Code for the Cryologger GVT is made available on the following GitHub repository:
* https://github.com/adamgarbo/Cryologger_Glacier_Velocity_Tracker
* Click on "Releases" and download the v2.2.1 .zip file:
![image](https://user-images.githubusercontent.com/22924092/235293956-19fb44e3-4aa3-4652-9721-a310398153c5.png)

## Step 6: Connect Hardware & Configure Port Settings
* Connect the MicroMod Artemis Data Logging Carrier board via a USB C cable. 
* In the Arduino IDE click on "Select Board" and then "SparkFun Artemis MicroMod".
* If the board is not auto-populated, click on "Select other board and port..." and search for "SparkFun Artemis MicroMod":
* Be sure to select the appropriate serial port that is connected to the Arduino.
![image](https://github.com/adamgarbo/cryologger-glacier-velocity-tracker/assets/22924092/ff301dea-9a29-4b84-a85e-f16959d3a2e7)
* Then click on "Tools", navigate to "SVL Baud Rate" and select 460800 baud.
![image](https://github.com/adamgarbo/cryologger-glacier-velocity-tracker/assets/22924092/3d287a82-0d1c-4205-ae10-62b192580b16)
![image](https://github.com/adamgarbo/cryologger-glacier-velocity-tracker/assets/22924092/c38d0f96-3262-4d27-9d40-69c1cd489383)

## Step 7: Test Program Compilation
* Navigate to the /Software/Arduino/cryologger_gvt folder of the downloaded repository
* Double click on the `cryologger_gvt.ino` file
* Click on the checkmark in the upper left corner of the Arduino IDE program window
* Watch the output window for any compilation errors
![image](https://github.com/adamgarbo/cryologger-glacier-velocity-tracker/assets/22924092/958a64de-168c-4bcc-8089-40b354d04dbe)

## Step 8: Upload Program
* Once the code has compiled successfully, click on the right-pointed arrow to upload the code
* Watch the output window for compilation errors and/or success messages
* If no errors are presented, the code has now been successfully uploaded!
![image](https://github.com/adamgarbo/cryologger-glacier-velocity-tracker/assets/22924092/f04429c8-bd06-4495-b4c4-73199b718b03)

## Step 9: Observe Serial Monitor Output


## Step 10: Next Steps
* Next to be covered is the detailed operation of the Cryologger GVT, including customized logging schedules and interpreting the OLED display messages.
* This information is made available in OPERATION.md.
