# Installation

## Step 1: Download Arduino IDE
https://www.arduino.cc/en/software

## Step 2: Add Additional Boards Manager

<img src="https://user-images.githubusercontent.com/22924092/125138608-f2734680-e0dc-11eb-94d7-24572fc70e37.png" width="800">

Add the following Additional Boards Manager URL:
https://raw.githubusercontent.com/sparkfun/Arduino_Boards/master/IDE_Board_Manager/package_sparkfun_index.json

## Step 3: Add libraries
The glacier velocity measurement system requires two external libraries, which can either be downloaded using the Arduino IDE's Library Manager, or directly from GitHub.
### SparkFun u-blox Arduino GNSS Library
- Version: 2.0.9
- GitHub: https://github.com/sparkfun/SparkFun_u-blox_GNSS_Arduino_Library
- 
<img src="https://user-images.githubusercontent.com/22924092/125138969-a4ab0e00-e0dd-11eb-9df3-e183d7cd2e73.png" width="640">

### SdFat
- Version: 2.0.6
- Github URL: https://github.com/greiman/SdFat
<img src="https://user-images.githubusercontent.com/22924092/125138913-83e2b880-e0dd-11eb-9d29-7a32b79902ea.png" width="640">

## Step 4: Download Cryologger GVMS code
* https://github.com/adamgarbo/Cryologger_Glacier_Velocity_Measurement_System

## Step 5: Select microcontroller and configure serial port settings
Navigate to Tools, click on Board.

<img src="https://user-images.githubusercontent.com/22924092/125139193-1b480b80-e0de-11eb-9849-7009c75e1093.png" width="480">

Navigate to SparkFun Apollo 3 and select SparkFun MicroMod Artemis.

<img src="https://user-images.githubusercontent.com/22924092/125139213-23a04680-e0de-11eb-80c7-7317da2d5db8.png" width="480"><img src="https://user-images.githubusercontent.com/22924092/125139223-2864fa80-e0de-11eb-836b-aa2a1bb73f7d.png" width="240">

Return to the main menu, navigate to SVL Baud Rate and select 460800 baud.

<img src="https://user-images.githubusercontent.com/22924092/125139245-31ee6280-e0de-11eb-9bea-371367445301.png" width="480">

##Step 6: Attempt to compile code

![image](https://user-images.githubusercontent.com/22924092/125139930-6f9fbb00-e0df-11eb-8bb1-87d007d0d7ff.png)


