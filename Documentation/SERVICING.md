# Servicing Guide: Cryologger - Glacier Velocity Tracker (GVT)

## Location
* Coordinates: 74.972361°N, 80.840889°W
* Southeast 2 Glacier, Devon Island, Nunavut
* Note: The system was deployed on September 4, 2021 and the position may have moved ~100 m or more downglacier.

![map](https://user-images.githubusercontent.com/22924092/181059251-2c47f407-942c-40b6-923e-17af3d927865.png)
**Figure 1.** Deployment map of Cryologger Glacier Velocity Tracker deployed on Southeast 2 Glacer, Devon Island, Nunavut.

## System Description
* The Cryologger system consists of a tripod, enclosure, solar panel, radiation shield and antenna (Figure 2).
* Two cables run externally to connect the GNSS receiver to the antenna and power from the solar panel to the solar charge controller.
* The enclosure holds a 48 Ah battery, solar charge controller and custom electronics/GNSS receiver (Figure 3).

![system](https://github.com/adamgarbo/Cryologger_Glacier_Velocity_Tracker/blob/main/Photos/DSC_6546.JPG)
**Figure 2.** Photo of of Cryologger glacier velocity tracker and ablation stake deployed on Southeast 2 Glacier, Devon Island, Nunavut.

![SE2](https://github.com/adamgarbo/Cryologger_Glacier_Velocity_Tracker/blob/main/Photos/DSC_5133.JPG)
**Figure 3.** View inside the enclosure showing the Deka 48 Ah battery, Genasun solar charge controller and Cryologger glacier velocity tracker enclosure.

## Servicing

### Hardware Kit Contents
* MicroSD card to switch out the existing card with a new one
* Hobo USB base station (https://www.itm.com/product/onset-hobo-base-u-4-universal-optic-usb-base-station)
* Replacement battery for the Hobo temp/RH sensor
* Multimeter to check the voltage of the main battery for the GNSS
* PC laptop loaded with Hoboware (https://www.onsetcomp.com/hoboware-free-download/) to run the USB base station

### Checklist
Once the Cryologger has been located from the air and the helicopter has landed, use the following checklist to service the system:
- [ ] Take several photos of the system as it was found.
- [ ] Inspect the general condition of the system, paying particular attention to the external cables (Arctic fox enjoy chewing through cables).
- [ ] Open the enclosure and inspect the interior, noting the presence of any water or corrosion on metal surfaces (e.g., solar charge controller, battery terminals), and also take several photos of the interior.
- [ ] Measure and record the voltage of the Deka 48 Ah battery.
- [ ] Observe the Cryologger electronics enclosure for several seconds to determine if a blue LED blink pattern can be seen (i.e., once every 10 seconds).
- [ ] Unscrew the 4 screws of the Cryologger enclosure.
- [ ] Disconnect power by unplugging the 2-pin screw terminal connector.
- [ ] Remove the 64 GB microSD card and replace it with the new 128 GB microSD card.
- [ ] Reconnect power by plugging in the 2-pin screw terminal.
- [ ] Observe LED blink patterns (Table 1).
- [ ] If system is operating normally, replace the electronics enclosure cover and tighten the screws.
- [ ] Close the large enclosure.

## System Operation

When powered on, the system will attempt to establish a GNSS fix and sychronize the real-time clock (RTC) for up to 5 minutes. During this time, the LED will blink once a second. Once the RTC is synchronized, the system will set an alarm to wake at the user-specified time and then enter a low-power deep sleep.

While the system is in deep sleep, the system will wake to check the program has not frozen every 10 seconds. The LED will blink briefly (100 ms) during this time.

Please use the following table to determine the status of the operation of the system and possible troubleshooting steps to attempt.

**Table 1.** LED blink patterns and associated description and troubleshooting guide.
| Blinks | Interval (s)  |   Pattern   | Description                     | Troubleshooting Steps                                 |
|:------:|:-------------:|:-----------:|---------------------------------|-------------------------------------------------------|
|    1   |      10       |   Single    | System is in deep sleep         | None                                                  |
|   10   |      0.1      |   Single    | Initialization complete         | None                                                  | 
|    2   |      2        |   Repeating | microSD failure                 | Reseat microSD card and reboot system                 |
|    3   |      2        |   Repeating | GNSS receiver failure           | Ensure components are firmly seated and reboot system |
|    1   |      1        |   Single    | GNSS signal acquisition         | None                                                  |
|    5   |      1        |   Single    | GNSS fix found & RTC synced     | None                                                  |
|    5   |      0.5      |   Single    | GNSS fix not found              | Ensure antenna cables are connected and reboot system |
| Random |      Random   |   Random    | Logging data to microSD card    | None                                                  |


## Disassembly
* If the system is determined to be non-functional, it can be recovered from the field. Please see the table below for the necessary tools. 

**Table 2.** Tools required to disassemble the tripod and enclosure.
| Tool                       | Equipment                              |   
|:---------------------------|:---------------------------------------|
| 1/2" socket/wrench         | Solar panel 1 3/4" u-bolt              |     
| 1/2" socket/wrench         | Tripod 1-3/4" u-bolt                   |          
| 7/16" socket/wrench        | Tripod 1-5/8" u-bolt                   |            
| Phillips head screwdriver  | Cryologger electronics enclosure       |  
| Flat head screwdriver      | Solar charge controller screw terminal |
