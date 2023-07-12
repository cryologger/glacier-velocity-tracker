# u-blox ZED-F9P - Firmware Upgrade
This guide provides step-by-step instructions on how to check and upgrade the firmware of the u-blox ZED-F9P GNSS receiver used in the Cryologger glacier velocity tracker (GVT).

## Step 1: Determine firmware version
The current version of firmware on the u-blox module can be determined by uploading a new program to the Cryologger. The steps are as follows:
* Ensure the Arduino IDE v1.8.19 and SparkFun GNSS v3 library are installed (see Steps 1-4 in [INSTALLATION.md](/Documentation/INSTALLATION.md)
* In the Arduino IDE, navigate to:
  * File > Examples > SparkFun u-blox GNSS v3 > Basics > Example8_GetModuleInfo
* Upload program (see Step 9 in [INSTALLATION.md](/Documentation/INSTALLATION.md))
* In the output (shown below), note the value of FWVER
  * If the firmware version is less than 1.30, it is necessary to perform an upgrade
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
* Upload program (see Step 8 in [INSTALLATION.md](/Documentation/INSTALLATION.md))

## Step 3: Upgrade firmware
* Download the most recent u-blox ZED-F9P firmware (v1.32):
  * https://content.u-blox.com/sites/default/files/2022-05/UBX_F9_100_HPG132.df73486d99374142f3aabf79b7178f48.bin
* Download the SparkFun RTK u-blox Firmware Update Tool:
  * https://github.com/sparkfun/SparkFun_RTK_Firmware_Binaries/raw/main/u-blox_Update_GUI/Windows_exe/RTK_u-blox_Update_GUI.exe
* Run the SparkFun RTK u-blox Firmware Update Tool
* Connect directly to u-blox ZED-F9P USB-C port 
* Confirm the COM port in use (i.e., using Device Manager)
* Click the Firmware File "Browse" button and select the binary file for the v1.32 firmware update
* Select the appropriate COM port from the dropdown (click Refresh if necessary)
* Click "Update Firmware"
  * Once complete, the u-blox module will restart
* Additional documentation on how to use the SparkFun RTK u-blox Firmware Update Tool can be found on SparkFun's website:
  * https://docs.sparkfun.com/SparkFun_RTK_Firmware/firmware_update/#updating-u-blox-firmware

![image](/Images/sparkfun-rtk-update-tool.png)

## Step 4: Confirm firmware version
Once the firmware is successfully upgraded, confirm its version using the same program as in Step 1.
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
After confirming the firmware version, the latest version of Cryologger GVT software can then be reinstalled.
* See Steps 5-9 in [INSTALLATION.md](/Documentation/INSTALLATION.md)
