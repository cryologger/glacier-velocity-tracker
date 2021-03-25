/*

   ***Note: All times in UTC. Dates are in EST***
  

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
  
  Results:
  -  Accidentally reset system from movement/static

  Files: 
  - 20210321_160000_HC872.ubx

Test #2:

  - Location: Outside
  - Start: 2021-03-21 16:40
  - End: 2021-03-21 18:15
  - Sleep: 5 minutes
  - Logging: 30 minutes
  - Frequency: 10 Hz
  - Tallysman HC872 antenna
  - SanDisk 64 GB
  - Protocol: I2C
  - u-blox RTK-SMA

  Comments:
  - Accidentally reset system from movement/static
  - First file should be 20210321_170000.ubx
  
  Results:
  -  All files were created
  
  Files: 
  - 20210321_170000_HC872.ubx
  - ...
  - 20210321_192000_HC872.ubx

  Test 3:
  
  - Location: Outside
  - Start: 2021-03-21 16:49
  - End: 2021-03-21 18:15
  - Sleep: 5 minutes
  - Logging: 30 minutes
  - Frequency: 10 Hz
  - Tallysman TW3872 antenna
  - SanDisk 64 GB
  - Protocol: I2C
  - u-blox GPS-RTK2 u.Fl

  Comments:
  - First file should be 20210321_170000.ubx
  
  Results:
  - All files were created 
  - Promising results
  - Moving to 2 hour duration

  Files: 
  - 20210321_170000_TW3872.ubx
  - ...
  - 20210321_192000_TW3872.ubx


  Test 4:
  
  - Location: Outside
  - Start: 2021-03-22 2:00
  - End: 2021-03-22 11:00
  - Sleep: 1 hour
  - Logging: 1 hour
  - Frequency: 1 Hz
  - Tallysman TW3872 antenna
  - SanDisk 64 GB
  - Protocol: I2C
  - u-blox GPS-RTK2 u.Fl

  Comments:
  - 
  
  Results:
  - Tests fully successful
  - No issues with debug logging

  Files: 
  - 20210322_020000_TW.ubx
  - ...
  - 20210322_100000_TW.ubx

  Test 5:
  
  - Location: Outside
  - Start: 2021-03-22 2:00
  - End: 2021-03-21 11:00
  - Sleep: 1 hour
  - Logging: 1 hour
  - Frequency: 1 Hz
  - ANN-MB antenna
  - SanDisk 64 GB
  - Protocol: I2C
  - u-blox GPS-RTK-SMA

  Comments:
  - 
  
  Results:
  - Tests fully successful
  - No issues with debug logging

  Files: 
  - 20210322_020000_ANN.ubx
  - ...
  - 20210322_100000_ANN.ubx


  -------------------------------------------------------------------------------
   March 22, 2021 
  -------------------------------------------------------------------------------

  Test 1:
  
  - Location: Outside
  - Start: 2021-03-23 0:45
  - End: 2021-03-23 10:00
  - Sleep: 1 hour
  - Logging: 2 hour
  - Frequency: 1 Hz
  - Tallysman TW3872 antenna
  - SanDisk 64 GB
  - Protocol: I2C
  - u-blox GPS-RTK2 u.Fl

  Comments:
  - First logfile should be at 01:00
  
  Results:
  - Test successful
  - 

  Files: 
  - 20210323_010000_TW.ubx
  - 20210323_040000_TW.ubx
  - 20210323_070000_TW.ubx
  - 20210323_100000_TW.ubx

  Test 2:
  
  - Location: Outside
  - Start: 2021-03-23 0:45
  - End: 2021-03-23 10:00
  - Sleep: 1 hour
  - Logging: 2 hour
  - Frequency: 1 Hz
  - ANN-MB antenna
  - SanDisk 64 GB
  - Protocol: I2C
  - u-blox GPS-RTK-SMA

  Comments:
  - First logfile should be at 01:00
  
  Results:
  - Test successful

  Files: 
  - 20210323_010000_ANN.ubx
  - 20210323_040000_ANN.ubx
  - 20210323_070000_ANN.ubx
  - 20210323_100000_ANN.ubx

  -------------------------------------------------------------------------------
   March 24, 2021 
  -------------------------------------------------------------------------------
  
  Test 1:
  
  - Location: Outside
  - Start: 2021-03-24 13:35
  - End: 2021-03-23 
  - Sleep: Continuous
  - Logging: 1 hour
  - Frequency: 1 Hz
  - SparkFun TOP106
  - SanDisk 64 GB
  - Protocol: I2C
  - u-blox GPS-RTK-SMA

  Comments:
  - First logfile should be at 14:00
  
  Results:
  - Successful tests

  Files: 
  - 

  Test 2:
  
  - Location: Outside
  - Start: 2021-03-23 13:35
  - End: 2021-03-23 
  - Sleep: Continuous
  - Logging: 1 hour
  - Frequency: 1 Hz
  - Tallysman TW3872
  - SanDisk 64 GB
  - Protocol: I2C
  - u-blox GPS-RTK2 u.Fl

  Comments:
  - First logfile should be at 14:00
  
  Results:
  - Successful tests

  Files: 
  - 


*/
