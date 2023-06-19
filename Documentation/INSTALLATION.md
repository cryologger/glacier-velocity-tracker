# Cryologger Glacier Velocity Tracker - Installation
This guide provides step-by-step instructions on how to install the Arduino IDE and required board definitons and libraries, which are necessary for uploading code to the Cryologger GVT.

## Step 1: Download Arduino IDE  
* Navigate to https://www.arduino.cc/en/software and download the most recent version of the Arduino IDE.

![image](https://github.com/adamgarbo/cryologger-ice-tracking-beacon/assets/22924092/323c74a1-2ce1-4e39-a8eb-d39ad760a9a6)

## Step 2: Add Additional Boards Manager URL:
* In the Arduino IDE navigate to: Preferences
* Add the following "Additional Boards Manager URL" as shown in the screenshot below:
```https://raw.githubusercontent.com/sparkfun/Arduino_Apollo3/master/package_sparkfun_apollo3_index.json```
* Also during this step, check the "compile" and "upload" boxes for "Show verbose output during" and change "Compiler warnings" to "All"
![image](https://github.com/adamgarbo/cryologger-glacier-velocity-tracker/assets/22924092/3724a095-7dda-4962-ada7-1a28d6a9a971)

## Step 3: Install Board Definition
* Navigate to: Tools > Boards > Boards Manager
* Search for: SparkFun Apollo3
* Select and install version: 1.2.3 
![image](https://github.com/adamgarbo/cryologger-glacier-velocity-tracker/assets/22924092/34f56a5f-723a-4ed3-9104-a4a47bd20019)

## Step 4: Install Libraries
The glacier velocity measurement system requires three external libraries. These can either be downloaded using the Arduino IDE's Library Manager (requires Internet connection), or directly from GitHub.

* Navigate to: Tools > Manage Libraries
* Search for and install the following libraries:

**Table 1.** Libraries required by Cryologger GVT. Last updated 2023-06-19.
| Library Manager                      | Version | GitHub Repository                                               |
|--------------------------------------|:-------:|-----------------------------------------------------------------|
| SparkFun u-blox GNSS v3              |  3.0.16 | https://github.com/sparkfun/SparkFun_u-blox_GNSS_v3             |
| SdFat                                |  2.2.2  | https://github.com/greiman/SdFat                                |
| SparkFun Qwiic OLED Arduino Library  |  1.0.5  | https://github.com/sparkfun/SparkFun_Qwiic_OLED_Arduino_Library |

![image](https://github.com/adamgarbo/cryologger-glacier-velocity-tracker/assets/22924092/d013db74-48f2-4171-a197-daddf3c17149)

## Step 5: Select Hardware & Configure Port Settings
* In the Arduino IDE click on "Select Board", "Select other board and port..." and search for and click on "SparkFun Artemis MicroMod"
![image](https://github.com/adamgarbo/cryologger-glacier-velocity-tracker/assets/22924092/48000d7f-53cd-4889-840e-5d7c2327e9f7)

* Then click on "Tools", navigate to "SVL Baud Rate" and select 460800 baud.
![image](https://github.com/adamgarbo/cryologger-glacier-velocity-tracker/assets/22924092/3d287a82-0d1c-4205-ae10-62b192580b16)
![image](https://github.com/adamgarbo/cryologger-glacier-velocity-tracker/assets/22924092/c38d0f96-3262-4d27-9d40-69c1cd489383)


## Step 6: Download Software
Code for the Cryologger GVT is made available on the following GitHub repository:
* https://github.com/adamgarbo/Cryologger_Glacier_Velocity_Tracker
* Click on "Releases" and download the v2.2.1 .zip file:
![image](https://user-images.githubusercontent.com/22924092/235293956-19fb44e3-4aa3-4652-9721-a310398153c5.png)

## Step 7: Test Program Compilation
* Navigate to the /Software/Arduino/cryologger_gvt folder of the downloaded repository
* Double click on the `cryologger_gvt.ino` file
* Click on the checkmark in the upper left corner of the Arduino IDE program window
* Watch the output window for any compilation errors
![image](https://github.com/adamgarbo/cryologger-glacier-velocity-tracker/assets/22924092/958a64de-168c-4bcc-8089-40b354d04dbe)

## Step 8: Connect Hardware
* Connect the MicroMod Artemis Data Logging Carrier board via a USB C cable. 
* In the Arduino IDE click on "Select Board" and then "SparkFun Artemis MicroMod".
* If the board is not auto-populated, click on "Select other board and port..." and search for "SparkFun Artemis MicroMod":
* Be sure to select the appropriate serial port that is connected to the Arduino.
![image](https://github.com/adamgarbo/cryologger-glacier-velocity-tracker/assets/22924092/ff301dea-9a29-4b84-a85e-f16959d3a2e7)

## Step 9: Upload Program
* Once the code has compiled successfully, click on the right-pointed arrow to upload the code
* Watch the output window for compilation errors and/or success messages
* If no errors are presented, the code has now been successfully uploaded!
![image](https://github.com/adamgarbo/cryologger-glacier-velocity-tracker/assets/22924092/f04429c8-bd06-4495-b4c4-73199b718b03)

## Step 10: Next Steps
* Next to be covered is the detailed operation of the Cryologger GVT, including customized logging schedules and interpreting the OLED display messages.
* This information is made available in OPERATION.md.
