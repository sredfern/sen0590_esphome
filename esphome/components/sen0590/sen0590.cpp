#include "esphome/core/log.h"
#include "esphome/core/hal.h"
#include "esphome/core/component.h"
#include "esphome/components/i2c/i2c.h"
#include "esphome/components/sensor/sensor.h"

namespace esphome {
namespace sen0590 {

static const char *const TAG = "sen0590";
static const uint8_t COMMAND_REG = 0x10;
static const uint8_t DATA_REG = 0x02;
static const uint8_t MEASURE_COMMAND = 0xB0;

class SEN0590Component : public sensor::Sensor, public PollingComponent, public i2c::I2CDevice {
 public:
  void setup() override {
    ESP_LOGD(TAG, "Setting up SEN0590 laser distance sensor...");
    // Power-on delay time is ≤800ms according to specs
    delay(800);
  }

  void update() override {
    // Send measurement command
    if (!this->write_byte(COMMAND_REG, MEASURE_COMMAND)) {
      ESP_LOGE(TAG, "Failed to write command");
      this->status_set_warning();
      return;
    }

    // Wait for measurement to complete
    delay(50);

    // Read measurement result
    uint8_t buffer[2];
    if (!this->read_bytes(DATA_REG, buffer, 2)) {
      ESP_LOGE(TAG, "Failed to read distance data");
      this->status_set_warning();
      return;
    }

    // Calculate distance in mm and convert to cm
    int distance_mm = (buffer[0] << 8) + buffer[1] + 10;
    float distance_cm = distance_mm / 10.0f;

    ESP_LOGD(TAG, "Distance: %.1f cm", distance_cm);
    this->publish_state(distance_cm);
    this->status_clear_warning();
  }

  float get_setup_priority() const override { return setup_priority::DATA; }
};

}  // namespace sen0590
}  // namespace esphome