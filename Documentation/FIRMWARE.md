# u-blox ZED-F9P Firmware Upgrade Guide
This guide provides step-by-step instructions on how to check and upgrade the firmware of the u-blox ZED-F9P GNSS receiver used in the Cryologger glacier velocity tracker (GVT).

## Step 1: Determine firmware version
The current version of firmware on the u-blox module can be determined by uploading a new program to the Cryologger. The steps to do so are as follows:
* Ensure Arduino IDE 2.1.0 and SparkFun GNSS v3 library are installed (see Step 1-5 in INSTALLATION.md )
* In the Arduino IDE, navigate to:
  * File > Examples > SparkFun u-blox GNSS v3 > Basics > Example8_GetModuleInfo
* Upload program (see Step 9 in INSTALLATION.md)
* In the output (shown below), note the value of FWVER.
  * If the version is less than 1.30, it will be necessary to upgrade the firmware.
```
SparkFun u-blox Example
FWVER: 1.13
Firmware: HPG
PROTVER: 27.12
MOD: ZED-F9P
Unique chip ID: 0xA44C228CDD
Unique chip ID: 0xA44C228CDD
Unique chip ID: 0xA44C228CDD
```

## Step 2: Reset u-blox module
Before upgrading the firmware, it is necessary to reset the u-blox module to its factory settings.
* In the Arduino IDE, navigate to:
  * File > SparkFun u-blox GNSS v3 > Basics > Example11_ResetModule > Example1_FactoryDefaultviaI2C
* Upload program (see Step 9 in INSTALLATION.md)

## Step 3: Upgrade firmware

* Download the most recent u-blox ZED-F9P firmware (v1.32):
  * https://content.u-blox.com/sites/default/files/2022-05/UBX_F9_100_HPG132.df73486d99374142f3aabf79b7178f48.bin
* Download the SparkFun RTK u-blox Firmware Update Tool:
  * https://github.com/sparkfun/SparkFun_RTK_Firmware_Binaries/raw/main/u-blox_Update_GUI/Windows_exe/RTK_u-blox_Update_GUI.exe
* Run the SparkFun RTK u-blox Firmware Update Tool
* Connect directly to the USB-C port of the u-blox ZED-F9P
* Confirm which COM port is in use (i.e., using Device Manager)
* Click the Firmware File Browse and select the binary file for the v1.32 firmware update
* Select the appropriate COM port (click Refresh if necessary)
* Click Update Firmware
  * Once complete, the u-blox module will restart.
* Additional documentation on how to use the SparkFun RTK u-blox Firmware Update Tool can be found on SparkFun's website:
  * https://docs.sparkfun.com/SparkFun_RTK_Firmware/firmware_update/#updating-u-blox-firmware

![image](https://github.com/adamgarbo/cryologger-glacier-velocity-tracker/assets/22924092/4edc5a68-b6f0-41b8-bcbb-4b9fb00b1c14)

## Step 4: Confirm firmware version
Once the firmware is successfully upgraded, confirm the version using the same program as in Step 1.
* In the Arduino IDE, navigate to:
  * File > Examples > SparkFun u-blox GNSS v3 > Basics > Example8_GetModuleInfo
* Upload the program
* In the output (shown below), confirm the value for FWVER has changed to v1.32
```
SparkFun u-blox Example
FWVER: 1.32
Firmware: HPG
PROTVER: 27.30
MOD: ZED-F9P
Unique chip ID: 0xA44C228CDD
Unique chip ID: 0xA44C228CDD
Unique chip ID: 0xA44C228CDD
```
## Step 5: Upload Cryologger software
After confirming the firmware version, the Cryologger glacier velocity tracker software can be reinstalled.
* See Steps 6-9 of Installation.md




