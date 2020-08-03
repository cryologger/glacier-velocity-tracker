# Cryologger - Glacier Velocity Measurement System

## Introduction

## Wiring
**SparkFun GPS-RTK2 Board - ZED-F9P (Qwiic)**
<img src="https://github.com/adamgarbo/Cryologger_Glacier_Velocity_Measurement_System/blob/master/Documentation/wiring1.png" width="720">

**SparkFun GPS-RTK-SMA Breakout - ZED-F9P**
<img src="https://github.com/adamgarbo/Cryologger_Glacier_Velocity_Measurement_System/blob/master/Documentation/wiring2.png" width="720">

## Code


### Menus
```
.
│   Menu: Main Menu
├── 1) Configure Logging
│   ├── Menu: Configure Logging
│   ├── 1) Log to microSD
│   ├── 2) Log to Terminal 
│   ├── 3) Set Serial Baud Rate 
│   ├── 4) Set Log Rate in Hz
│   ├── 5) Set Log Rate in seconds between readings
│   ├── 6) Set logging duration in seconds
│   ├── 7) Set sleep duration in seconds
│   ├── 8) Open new log file after sleep
│   ├── 9) Use pin 32 to Stop Logging 
│   └── x) Exit
├── 2) Configure RTC		
│   ├── Menu: Configure Real-time Clock (RTC)
│   ├── 1) Set RTC date
│   ├── 2) Set RTC time
│   ├── 3) Sync RTC to GNSS
│   ├── 4) Set alarm
│   ├── 5) Set rolling alarm interval
│   ├── 6) Set alarm mode
│   └── x) Exit
├── 3) Configure GNSS
│   ├── Menu: Configure u-blox GNSS Receiver
│   ├── 1) Sensor Logging
│   ├── 2) Use a power management task to put the module to sleep
│   ├── 10) Log UBX-NAV-CLOCK     (Clock Solution)
│   ├── 11) Log UBX-NAV-HPPOSECEF (High Precision Position Earth-Centered Earth-Fixed)
│   ├── 12) Log UBX-NAV-HPPOSLLH  (High Precision Position Lat/Lon/Height)
│   ├── 13) Log UBX-NAV-ODO       (Odometer)
│   ├── 14) Log UBX-NAV-POSECEF   (Position Earth-Centered Earth-Fixed)
│   ├── 15) Log UBX-NAV-POSLLH    (Position Lat/Lon/Height)
│   ├── 16) Log UBX-NAV-PVT       (Position, Velocity, Time)
│   ├── 17) Log UBX-NAV-STATUS    (Receiver Navigation Status)
│   ├── 18) Log UBX-NAV-TIMEUTC   (UTC Time Solution) (** Used to sync the OLA RTC **)
│   ├── 19) Log UBX-NAV-VELECEF   (Velocity Solution Earth-Centered Earth-Fixed)
│   ├── 20) Log UBX-NAV-VELNED    (Velocity Solution North/East/Down)
│   ├── 30) Log UBX-RXM-SFRBX     (Broadcast Navigation Data Subframe)
│   ├── 40) Log UBX-TIM-TM2       (Time Mark Data)
│   ├── 50) Log UBX-NAV-RELPOSNED (Relative Position North/East/Down)
│   ├── 60) Log UBX-RXM-RAWX      (Multi-GNSS Raw Measurement)
│   ├── 90) USB port     (Disabling this can reduce the load on the module)
│   ├── 91) UART1 port   (Disabling this can reduce the load on the module)
│   ├── 92) UART2 port   (Disabling this can reduce the load on the module)
│   ├── 93) SPI port     (Disabling this can reduce the load on the module)
│   └── x) Exit
├── 4) Configure Qwiic Bus
│   ├── Menu: Configure Power Options
│   ├── 1) Turn off Qwiic bus power when sleeping
│   ├── 2) Power LED During Sleep
│   └── x) Exit
├── 5) Configure Power Options
│   ├── Menu: Configure Power Options
│   ├── 1) Turn off Qwiic bus power when sleeping
│   ├── 2) Power LED During Sleep
│   └── x) Exit
├── d) Debug Menu
│   ├── Menu: Configure Debug Settings
│   ├── 1) Major Debug Messages
│   ├── 2) Minor Debug Messages
│   └── x) Exit
├── f) Open New Log File
├── g) Reset GNSS
├── r) Reset all OLA settings to default
├── q) Quit: Close log files and power down
└── x) Return to logging

```
