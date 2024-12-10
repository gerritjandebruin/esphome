# include "T9602.h"
# include "esphome/core/log.h"

namespace esphome {
namespace T9602 {

static const char *TAG = "T9602";

static const uint8_t REQUEST_CMD[1] = {0x00}; // Measurement Request Format

void TelaireComponent::read_sensor_data_() {
  uint8_t data[4];

  if (this->read(data, sizeof(data)) != i2c::ERROR_OK) {
    ESP_LOGE(TAG, "Communication with Telaire T9602 failed when reading!");
    this->mark_failed();
    return;
  }

  const uint16_t raw_humidity = (data[0] << 8) | data[1];
  float humidity = (125.0 * raw_humidity) / 65536.0 - 6.0;

  uint16_t raw_temperature = (data[2] << 8) | data[3];
  float temperature = (175.72 * raw_temperature) / 65536.0 - 46.85;

  ESP_LOGD(TAG, "Got temperature=%.2f°C humidity=%.2f%%", temperature, humidity);
  if (this->temperature_sensor_ != nullptr)
    this->temperature_sensor_->publish_state(temperature);
  if (this->humidity_sensor_ != nullptr)
    this->humidity_sensor_->publish_state(humidity);
}

void TelaireComponent::start_measurement_() {
  uint8_t data = 0x00;
  if (this->write(&data, 1) != i2c::ERROR_OK) {
    ESP_LOGE(TAG, "Communication with Telaire T9602 failed when starting measurement!");
    this->mark_failed();
  }

  this->measurement_running_ = true;
}

bool TelaireComponent::is_measurement_ready_() {
  uint8_t data[1];

  if (this->read(data, sizeof(data)) != i2c::ERROR_OK) {
    ESP_LOGE(TAG, "Communication with Telaire T9602 failed!");
    this->mark_failed();
    return false;
  }

  return true;
}

void TelaireComponent::measurement_timeout_() {
  ESP_LOGE(TAG, "Telaire T9602 Timeout!");
  this->measurement_running_ = false;
  this->mark_failed();
}

void TelaireComponent::update(){
  ESP_LOGV(TAG, "Update Telaire T9602 Sensor");

  this->start_measurement_();

  this->set_timeout("meas_timeout", 100, [this] { this->measurement_timeout_(); });
}

void TelaireComponent::loop() {
  if (this->measurement_running_ && this->is_measurement_ready_()) {
    this->measurement_running_ = false;
    this->cancel_timeout("meas_timeout");
    this->read_sensor_data_();
  }
}

void TelaireComponent::dump_config() {
  ESP_LOGD(TAG, "Telaire T9602:");
  LOG_I2C_DEVICE(this);
  if (this->is_failed()) { ESP_LOGE(TAG, "Communication with Telaire T9602 failed!");}
  LOG_SENSOR(" ", "Temperature", this->temperature_sensor_);
  LOG_SENSOR(" ", "Humidity", this->humidity_sensor_);
  LOG_UPDATE_INTERVAL(this);
}

float TelaireComponent::get_setup_priority() const { return setup_priority::DATA; }

}  // namespace T9602
}  // namespace esphome