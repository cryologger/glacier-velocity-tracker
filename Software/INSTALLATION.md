# Cryologger - Glacier Velocity Measurement System Configuration

## Step 1: Download Arduino IDE
https://www.arduino.cc/en/software

## Step 2: Add Additional Boards Manager URL:
* Navigate to: Preferences
* Add the following Additional Boards Manager URL as shown in the sceenshot below:
https://raw.githubusercontent.com/sparkfun/Arduino_Boards/master/IDE_Board_Manager/package_sparkfun_index.json
<img src="https://user-images.githubusercontent.com/22924092/125138608-f2734680-e0dc-11eb-94d7-24572fc70e37.png" width="800">

## Step 3: Install Board Definition
* Navigate to: Tools > Boards > Boards Manager
* Search for: SparkFun Apollo3 Boards
* Select and install version: 1.2.3 
<img src="https://user-images.githubusercontent.com/22924092/125164105-85a09080-e15e-11eb-8152-91c54f27f3a8.png" width="640">

## Step 4: Install Libraries
The glacier velocity measurement system requires two external libraries. These can either be downloaded using the Arduino IDE's Library Manager (requires Internet connection), or directly from GitHub.

* Navigate to: Tools > Manage Libraries
* Search for and install the following libraries:

### 1) SparkFun u-blox Arduino GNSS Library
- Version: 2.0.9
- GitHub: https://github.com/sparkfun/SparkFun_u-blox_GNSS_Arduino_Library
<img src="https://user-images.githubusercontent.com/22924092/125138969-a4ab0e00-e0dd-11eb-9df3-e183d7cd2e73.png" width="640">

### 2) SdFat
- Version: 2.0.7
- Github URL: https://github.com/greiman/SdFat
<img src="https://user-images.githubusercontent.com/22924092/125138913-83e2b880-e0dd-11eb-9d29-7a32b79902ea.png" width="640">

## Step 5: Select Harware & Configure Port Settings
*  Navigate to Tools > Board > SparkFun Apollo 3

<img src="https://user-images.githubusercontent.com/22924092/125139193-1b480b80-e0de-11eb-9849-7009c75e1093.png" width="480"><img src="https://user-images.githubusercontent.com/22924092/125139213-23a04680-e0de-11eb-80c7-7317da2d5db8.png" width="480">

*  Select SparkFun MicroMod Artemis
<img src="https://user-images.githubusercontent.com/22924092/125139223-2864fa80-e0de-11eb-836b-aa2a1bb73f7d.png" width="360">

* Return to the main menu, navigate to SVL Baud Rate and select 460800 baud.

<img src="https://user-images.githubusercontent.com/22924092/125139245-31ee6280-e0de-11eb-9bea-371367445301.png" width="480">

## Step 6: Download Glacier Velocity Measurement System Program
Code for the Cryologger GVMS is available on GitHub from the following repository:
* https://github.com/adamgarbo/Cryologger_Glacier_Velocity_Measurement_System
* The easiest approach is to download the entire repository locallly, which can be accomplished by clicking on "Code" and then "Download Zip"

![image](https://user-images.githubusercontent.com/22924092/125165486-209c6900-e165-11eb-9ba5-05308af564b3.png)

## Step 7: Test Program Compiliation
* Navigate to the /Software/cryologger_gvms folder of the downloaded repository
* Double click on the `cryologger_gvms.ino` file
* Click on the checkmark in the upper left corner of the Arduino IDE program window
* Watch debugging window for compilation errors and/or success messages
<img src="https://user-images.githubusercontent.com/22924092/125139930-6f9fbb00-e0df-11eb-8bb1-87d007d0d7ff.png" width="640">

## Step 7: Upload Program
* Once the code has compiled successfully, connect the MicroMod Artemis Data Logging Carrier board via USB C. 
* The board should appear as a USB device
* Make any necessary changes to the code
* Click on the right pointed arrow to upload the code
* Watch debugging window for compilation errors and/or success messages
