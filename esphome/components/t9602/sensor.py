import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor, i2c
from esphome.const import (
    CONF_ID,
    CONF_TEMPERATURE,
    CONF_HUMIDITY,
    STATE_CLASS_MEASUREMENT,
    UNIT_CELSIUS,
    UNIT_PERCENT,
    DEVICE_CLASS_TEMPERATURE,
    DEVICE_CLASS_HUMIDITY,
)

DEPENDENCIES = ['i2c']

telaire_T9602_ns = cg.esphome_ns.namespace('T9602')
TELAIREComponent = telaire_T9602_ns.class_(
    'TelaireComponent', i2c.I2CDevice, cg.PollingComponent
)

CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(TELAIREComponent),
            cv.Optional(CONF_TEMPERATURE): sensor.sensor_schema(
                unit_of_measurement=UNIT_CELSIUS,
                accuracy_decimals=2,
                device_class=DEVICE_CLASS_TEMPERATURE,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
            cv.Optional(CONF_HUMIDITY): sensor.sensor_schema(
                unit_of_measurement=UNIT_PERCENT,
                accuracy_decimals=2,
                device_class=DEVICE_CLASS_HUMIDITY,
                state_class=STATE_CLASS_MEASUREMENT
            ),
        }
    )
    .extend(cv.polling_component_schema('1s'))
    .extend(i2c.i2c_device_schema(0x28))
)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await i2c.register_i2c_device(var, config)

    if temperature_config := config.get(CONF_TEMPERATURE):
        sens = await sensor.new_sensor(temperature_config)
        cg.add(var.set_temperature_sensor(sens))

    if humidity_config := config.get(CONF_HUMIDITY):
        sens = await sensor.new_sensor(humidity_config)
        cg.add(var.set_humidity_sensor(sens))