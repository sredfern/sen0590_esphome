#include "sen0590.h"
#include "esphome/core/log.h"
#include "esphome/core/hal.h"
#include "esphome/core/component.h"
#include "esphome/components/i2c/i2c.h"
#include "esphome/components/sensor/sensor.h"

namespace esphome {
namespace sen0590 {

static const char *const TAG = "sen0590";
static const uint8_t COMMAND_REG = 0x00;
static const uint8_t DATA_REG_HIGH = 0x01;
static const uint8_t DATA_REG_LOW = 0x02;
static const uint8_t MEASURE_COMMAND = 0xB0;

class SEN0590Component : public sensor::Sensor, public PollingComponent, public i2c::I2CDevice {
 public:
  void setup() override {
    ESP_LOGD(TAG, "Setting up SEN0590 laser distance sensor...");
    if (!this->write_byte(COMMAND_REG, 0x00)) {
      ESP_LOGE(TAG, "SEN0590 not responding. Check wiring and address.");
      this->mark_failed();
      return;
    }
    ESP_LOGD(TAG, "SEN0590 Setup complete.");
  }

  void update() override {
    if (!this->write_byte(COMMAND_REG, MEASURE_COMMAND)) {
      ESP_LOGW(TAG, "Failed to send measurement command");
      this->status_set_warning();
      return;
    }

    delay(50);

    uint8_t buffer[2];
    if (this->read_register(DATA_REG_HIGH, &buffer[0], 1) != i2c::ERROR_OK ||
        this->read_register(DATA_REG_LOW, &buffer[1], 1) != i2c::ERROR_OK) {
      ESP_LOGW(TAG, "Failed to read distance data");
      this->status_set_warning();
      return;
    }

    int distance_mm = (buffer[0] << 8) | buffer[1];
    if (distance_mm == 0xFFFF) {
      ESP_LOGW(TAG, "Received invalid distance data (0xFFFF)");
      this->status_set_warning();
      return;
    }

    float distance_cm = distance_mm / 10.0f;

    ESP_LOGD(TAG, "Distance: %.1f cm (Raw: %d mm)", distance_cm, distance_mm);
    this->publish_state(distance_cm);
    this->status_clear_warning();
  }

  float get_setup_priority() const override { return setup_priority::DATA; }
};

}  // namespace sen0590
}  // namespace esphome