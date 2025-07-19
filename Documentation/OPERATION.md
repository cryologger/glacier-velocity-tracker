# Operation

The Cryologger Glacier Velocity Tracker (GVT) supports flexible logging schedules to accommodate various deployment needs. As of software version 3.0 and up, users can configure the device using a JSON file placed on the microSD card, or by directly modifying the firmware.

## Logging Modes

GVT supports three primary logging modes:

| Mode        | Description |
|-------------|-------------|
| **Continuous** | Logs GNSS data continuously and creates a new file each day at 00:00 UTC. |
| **Rolling**    | Alternates between logging and sleep periods. For example, log for 30 minutes, sleep for 3 hours. |
| **Daily** *(default)* | Wakes and sleeps at fixed times each day. Ideal for conserving power during long-term deployments. |

Each mode can be tailored to match the user's operational or scientific requirements.

## Seasonal Logging Mode

Seasonal logging is an optional enhancement to the primary logging modes. When enabled, the GVT operates normally (e.g., in **daily** or **rolling** mode) for most of the year. During a defined seasonal window, it overrides the current mode and switches to **continuous** logging to collect high-frequency data.

- Implements an automated logging mode that adjusts sampling intervals based on seasonal conditions.
- Enables automated transitions across year boundaries.
- Supports both Northern and Southern Hemisphere deployments.

This feature is useful for long-term Arctic or Antarctic deployments where high-resolution data is only needed seasonally without requiring field intervention.


## Configuration Methods

GVT settings can be defined in two ways:

1. **Via JSON (`config.json`) on the SD card** – Recommended for field flexibility  
2. **Hardcoded in firmware** – Best for controlled or repeat deployments

## 1. Configuring via JSON (Recommended)

Place a file named `config.json` in the root of the microSD card. The system reads and applies these settings automatically at boot.

### Example Configuration


```json title="test.json"
{
  "uid": "GVT_25_TST",
  "operationMode": "DAILY",
  "seasonalLoggingMode": "ENABLED",
  "dailyStartHour": 17,
  "dailyStartMinute": 0,
  "dailyStopHour": 20,
  "dailyStopMinute": 0,
  "rollingAwakeHours": 1,
  "rollingAwakeMinutes": 0,
  "rollingSleepHours": 1,
  "rollingSleepMinutes": 0,
  "seasonalStartDay": 1,
  "seasonalStartMonth": 6,
  "seasonalEndDay": 30,
  "seasonalEndMonth": 9,
  "gnssMeasurementRate": 1000,
  "gnssGpsEnabled": 1,
  "gnssGloEnabled": 1,
  "gnssGalEnabled": 1,
  "gnssBdsEnabled": 0,
  "gnssSbasEnabled": 0,
  "gnssQzssEnabled": 0
}
```

### Key Parameters

| Key                   | Description |
|------------------------|-------------|
| `uid` | Unique identifier for the logger |
| `operationMode` | Logging mode: `DAILY`, `ROLLING`, or `CONTINUOUS` |
| `seasonalLoggingMode` | `ENABLED` or `DISABLED` |
| `dailyStartHour`, `dailyStartMinute` | Start of daily logging window (UTC) |
| `dailyStopHour`, `dailyStopMinute` | End of daily logging window (UTC) |
| `rollingAwakeHours`, `rollingAwakeMinutes` | Awake duration in ROLLING mode |
| `rollingSleepHours`, `rollingSleepMinutes` | Sleep duration in ROLLING mode |
| `seasonalStartDay`, `seasonalStartMonth` | Start of seasonal logging |
| `seasonalEndDay`, `seasonalEndMonth` | End of seasonal logging |
| `gnssMeasurementRate` | GNSS sampling rate (ms, 250–30000) |
| `gnssGpsEnabled`, `gnssGloEnabled`, `gnssGalEnabled`, `gnssBdsEnabled`, `gnssSbasEnabled`, `gnssQzssEnabled` | Enable (1) or disable (0) GNSS constellations |


**Tip:** Use a JSON linter to verify structure before deployment.

## 2. Hardcoded Configuration

Advanced users may opt to define settings directly in the firmware (e.g., [`main.ino`](https://github.com/cryologger/glacier-velocity-tracker/blob/76f1cdd5b8cbc1692aacf82086db6821f060ec1d/Software/Arduino/cryologger_gvt/cryologger_gvt.ino#L20-L62)). This is useful for permanent or high-stakes deployments where runtime changes are undesirable.

If using this approach, ensure that the config.json file is not present on the microSD card, as it will take precendence over the hard-code values.

```c++
// ----------------------------------------------------------------------------
// USER CONFIGURATION
// ----------------------------------------------------------------------------

// Device Identifier
char uid[20] = "GVT_25_TST";  // Default unique identifier (UID)

// Select the default operation mode (for normal periods when NOT in seasonal)
#define OPERATION_MODE DAILY  // Options: DAILY, ROLLING, CONTINUOUS

// Daily mode parameters (only used if OPERATION_MODE == DAILY)
#define DAILY_START_HOUR 17   // Logging start hour (UTC)
#define DAILY_START_MINUTE 0  // Logging start minute (UTC)
#define DAILY_STOP_HOUR 20    // Logging stop hour (UTC)
#define DAILY_STOP_MINUTE 0   // Logging stop minute (UTC)

// Rolling mode parameters (only used if OPERATION_MODE == ROLLING)
#define ROLLING_AWAKE_HOURS 1    // Awake period (hours)
#define ROLLING_AWAKE_MINUTES 0  // Awake period (minutes)
#define ROLLING_SLEEP_HOURS 1    // Sleep period (hours)
#define ROLLING_SLEEP_MINUTES 0  // Sleep period (minutes)

// Seasonal logging override
// If ENABLED and the current date is within the seasonal window,
// we switch to CONTINUOUS mode automatically.
#define SEASONAL_LOGGING_MODE ENABLED  // ENABLED or DISABLED
#define SEASONAL_START_DAY 1           // Seasonal logging start day
#define SEASONAL_START_MONTH 6         // Seasonal logging start month
#define SEASONAL_END_DAY 30            // Seasonal logging stop day
#define SEASONAL_END_MONTH 9           // Seasonal logging stop month

// GNSS Satellite Signal configuration (0=DISABLE, 1=ENABLE)
#define GNSS_MEASUREMENT_RATE 1000
#define GNSS_GPS_ENABLED 1
#define GNSS_GLO_ENABLED 1
#define GNSS_GAL_ENABLED 1
#define GNSS_BDS_ENABLED 0
#define GNSS_SBAS_ENABLED 0
#define GNSS_QZSS_ENABLED 0

// ----------------------------------------------------------------------------
//  END OF USER CONFIGURATION
// ----------------------------------------------------------------------------

```
## Best Practices

- Always test configuration changes in a controlled environment before field use.  
- Disable unused GNSS constellations to reduce power consumption.  
- Use DAILY mode with seasonal control for long-duration polar deployments.  
