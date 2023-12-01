# Cryologger Glacier Velocity Tracker - Deployment

## Pre-deployment
### Logging Configuration

## Field Deployment
### OLED Display Messages

### LED Blink Patterns

In addition to the OLED display messages, a series of LED blink patterns can also be used to indicate system operation. The following table can be used to determine the status of the system, as well as any possible troubleshooting steps that should be attempted if an error is encountered. A detailed table of the LED blink patterns can be found in the [DEPLOYMENT.md](/Documentation/DEPLOYMET.md) documentation.

Additionally, while the system is in deep sleep, the Watchdog Timer (WDT) will wake the system to check the program has not frozen once every 10 seconds and an LED will blink briefly during this time. 

**Table 1.** LED blink patterns, associated description and troubleshooting guide.
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



## Troubleshooting

## Documentation

