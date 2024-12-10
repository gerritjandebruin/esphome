#pragma once

#include "esphome/components/i2c/i2c.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/core/component.h"

namespace esphome {
namespace T9602 {

class TelaireComponent : public i2c::I2CDevice, public PollingComponent {
  public:
    void dump_config() override;
    float get_setup_priority() const override;
    void loop() override;
    void update() override;

    void set_temperature_sensor(sensor::Sensor *temperature_sensor) { this->temperature_sensor_ = temperature_sensor; }
    void set_humidity_sensor(sensor::Sensor *humidity_sensor) { this->humidity_sensor_ = humidity_sensor; }

  protected:
    bool measurement_running_{false};
    sensor::Sensor *temperature_sensor_{nullptr};
    sensor::Sensor *humidity_sensor_{nullptr};

  private:
    void read_sensor_data_();
    void start_measurement_();
    bool is_measurement_ready_();
    void measurement_timeout_();
};

}  // namespace T9602
}  // namespace esphome