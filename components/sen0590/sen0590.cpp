#include "sen0590.h"
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

void SEN0590Component::setup() {
  ESP_LOGD(TAG, "Setting up SEN0590 laser distance sensor...");
  uint8_t temp_buffer;
  if (this->read_register(DATA_REG, &temp_buffer, 1) != i2c::ERROR_OK) {
    ESP_LOGE(TAG, "SEN0590 not responding. Check wiring and address.");
    this->mark_failed();
    return;
  }
  ESP_LOGD(TAG, "SEN0590 Setup complete.");
}

void SEN0590Component::update() {
  if (!this->write_byte(COMMAND_REG, MEASURE_COMMAND)) {
    ESP_LOGW(TAG, "Failed to send measurement command to reg 0x%02X", COMMAND_REG);
    this->status_set_warning();
    return;
  }

  delay(50);

  uint8_t buffer[2];
  if (this->read_register(DATA_REG, buffer, 2) != i2c::ERROR_OK) {
    ESP_LOGW(TAG, "Failed to read distance data from reg 0x%02X", DATA_REG);
    this->status_set_warning();
    return;
  }

  int distance_mm = ((buffer[0] << 8) | buffer[1]) + 10;

  if (((buffer[0] << 8) | buffer[1]) == 0xFFFF) {
    ESP_LOGW(TAG, "Received invalid distance data (raw 0xFFFF)");
    this->publish_state(NAN);
    this->status_set_warning();
    return;
  }

  float distance_cm = distance_mm / 10.0f;

  ESP_LOGD(TAG, "Distance: %.1f cm (Raw: %d mm)", distance_cm, distance_mm);
  this->publish_state(distance_cm);
  this->status_clear_warning();
}

float SEN0590Component::get_setup_priority() const { return setup_priority::DATA; }

}  // namespace sen0590
}  // namespace esphome