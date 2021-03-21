/*

   ***Note: All times in UTC***

  -------------------------------------------------------------------------------
   March 20, 2021
  -------------------------------------------------------------------------------

  Test #1:

  - Location: Outside
  - Start: 2021-03-20 17:00
  - End: 2021-03-20 18:30
  - Sleep: 5 minutes
  - Logging: 30 minutes
  - Frequency: 10 Hz
  - Antenna: Tallysman HC872 antenna
  - SanDisk 64 GB

  Results:
  - u-blox failed to initialze after two power cycles
  - Code wasn't yet set to ignore serial initialization failure

  Files:
  - 20210320_170000.ubx
  - 20210320_173500.ubx

  Test #2:

  - Location: Outside
  - Start: 2021-03-20 17:10
  - End: 2021-03-21 1:45
  - Sleep: 5 minutes
  - Logging: 30 minutes
  - Frequency: 10 Hz
  - Antenna: Tallysman TW3872 antenna
  - SD card: SanDisk 64 GB

  Comments:
  - Fixed serial code to ignore initialization failure

  Results:
  - Watchdog timer triggered
  - Failed after creating 2 log files

  Files:
  - 20210320_180000.ubx
  - 20210320_183500.ubx

  Test #3:

  - Location: Outside
  - Start: 2021-03-20 18:40
  - End: 2021-03-21 1:45
  - Sleep: 5 minutes
  - Logging: 30 minutes
  - Frequency: 10 Hz
  - Tallysman HC872 antenna
  - SanDisk 64 GB

  Comments:
  - Fixed serial code to ignore serial initialization failure
  - Added more watchdog resets

  Results:
  - Watchdog timer triggered
  - Failed after a single file

  Files:
  - 20210320_190000.ubx

  Test #4:

  - Location: Inside
  - Start: 2021-03-21 2:45
  - End: 2021-03-21
  - Sleep: 5 minutes
  - Logging: 30 minutes
  - Frequency: 10 Hz
  - Tallysman HC872 antenna
  - SanDisk 64 GB

  Comments:
  - Same code as Test #3 and #4
  - Testing inside overnight
  - First log file should be timestamped 20210321_030000

  Results:
  - Created 15 files but still failed
  - Watchdog timer did not trigger
  - Will attempt I2C now

  Files:
  - 20210321_030000.ubx
  - ...
  - 20210321_111000.ubx (0 bytes)


  -------------------------------------------------------------------------------
   March 21, 2021
  -------------------------------------------------------------------------------

  Test #1:

  - Location: Outside
  - Start: 2021-03-21 15:53:14
  - End: 2021-03-21 16:40
  - Sleep: 5 minutes
  - Logging: 30 minutes
  - Frequency: 10 Hz
  - Tallysman HC872 antenna
  - SanDisk 64 GB
  - Protocol: I2C
  - u-blox RTK-SMA

  Comments:
  - First log file is 20210321_160000.ubx
  - Manually observed serial stream for first 30 minutes
  - Highest buffer observed was 2560 bytes
  - File size of first file: 16839912 bytes
  - Accidentally reset system from movement/static
  
  Results:
  -  

  Files: 
  - 20210321_160000_HC872.ubx

Test #2:

  - Location: Outside
  - Start: 2021-03-21 16:40
  - End: 2021-03-21 
  - Sleep: 5 minutes
  - Logging: 30 minutes
  - Frequency: 10 Hz
  - Tallysman HC872 antenna
  - SanDisk 64 GB
  - Protocol: I2C
  - u-blox RTK-SMA

  Comments:
  - Accidentally reset system from movement/static
  
  Results:
  -  

  Files: 
  - 

  Test 3:
  
  - Location: Outside
  - Start: 2021-03-21 16:49
  - End: 2021-03-21 
  - Sleep: 5 minutes
  - Logging: 30 minutes
  - Frequency: 10 Hz
  - Tallysman TW3872 antenna
  - SanDisk 64 GB
  - Protocol: I2C
  - u-blox GPS-RTK2 u.Fl

  Comments:
  - 
  
  Results:
  -  

  Files: 
  - 


*/
