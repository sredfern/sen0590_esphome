# ESPHome SEN0590 Laser Ranging Sensor Component

This is a custom component for ESPHome to integrate the DFRobot SEN0590 Laser Ranging Sensor (4m).

## Features

*   Measures distances from 2cm up to 400cm.
*   Uses the I2C communication protocol.
*   Provides distance readings in centimeters.
*   Includes error handling for communication issues and invalid readings.

## Sensor Information

*   **Product Wiki:** [DFRobot Laser Ranging Sensor (4m) SKU: SEN0590](https://wiki.dfrobot.com/Laser_Ranging_Sensor_4m_SKU_SEN0590)
*   **Communication:** I2C
*   **I2C Address:** 0x74 (default)
*   **Measurement Range:** 2cm - 400cm
*   **Operating Voltage:** 3.3V - 5V

## Wiring

Connect the sensor to your ESP device's I2C pins:

| Sensor Wire | ESP Pin        | Description             |
| :---------- | :------------- | :---------------------- |
| Red (VCC)   | 3.3V or 5V     | Power Supply Positive   |
| Black (GND) | GND            | Ground                  |
| Yellow (SCL)| I2C SCL Pin    | I2C Clock Line          |
| Green (SDA) | I2C SDA Pin    | I2C Data Line           |

## ESPHome Configuration Example

```yaml
esphome:
  name: laser_distance_test
  # ... other config

# Enable I2C
i2c:
  sda: GPIO21 # Adjust GPIO pins as needed
  scl: GPIO22
  scan: true
  id: bus_a

# Add the custom component directory
external_components:
  - source: github://sredfern/sen0590_esphome@main # Or local path: components/

# Sensor configuration
sensor:
  - platform: sen0590
    name: "Laser Distance"
    update_interval: 100ms # Adjust as needed, min ~50ms due to sensor delay
    # Optional: Specify a different I2C address if changed
    # address: 0x75 


## Usage
The sensor will report the measured distance in centimeters. The `update_interval` should be set considering the sensor's response time (around 19-38ms plus the 50ms delay used in the component). Setting it too low might lead to missed readings or unstable behavior.