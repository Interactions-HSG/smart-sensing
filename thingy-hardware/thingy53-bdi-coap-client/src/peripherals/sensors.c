#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/printk.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/settings/settings.h>
#include <dk_buttons_and_leds.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/logging/log.h>

#include <stdlib.h>
#include <unistd.h>

#include "peripherals/gpio.h"


/* Define the the addresses of relevant sensor registers and settings */
#define BH1749_SYSTEM_CONTROL                           0x40
#define BH1749_MODE_CONTROL1                            0x41
#define BH1749_MODE_CONTROL2                            0x42
#define BH1749_RED_DATA_LSB                             0x50
#define BH1749_MODE_CONTROL2_RGB_EN_ENABLE              BIT(4)
/* IR Gain: 1x, RGB Gain: 1x, Measurement mode: 120ms mode */
#define BH1749_MODE_CONTROL1_DEFAULTS                   0x2A
/* Get the node identifier of the sensor */
#define I2C_NODE DT_NODELABEL(bh1749)


LOG_MODULE_REGISTER(sensors, CONFIG_SENSOR_LOG_LEVEL);

/**************************************************************************************************
  BME688 Device Struct.
**************************************************************************************************/
const struct device *dev = DEVICE_DT_GET_ONE(bosch_bme680);
struct sensor_value m_temperature;
struct sensor_value m_humidity;
struct sensor_value m_air_pressure;
struct sensor_value m_gas_resolution;
uint8_t sampleCountBME688 = 0;
double m_temperature_avg = 0.0f;
double m_humidity_avg = 0.0f;
double m_air_pressure_avg = 0.0f;
double m_gas_resolution_avg = 0.0f;

/**************************************************************************************************
  Functions.
**************************************************************************************************/
/*************************************************************************************************/
/*!
 *  \brief  Reads the BME688 enviromental sensor.
 *
 *  \param  None.
 *
 *  \return None.
 */
/*************************************************************************************************/
void ReadBME688(struct sensor_value *temp, struct sensor_value *press, 
				struct sensor_value *humidity, struct sensor_value *gas_res)
{
	gpio_pin_set_dt(&out2, 1); //GPIO
	if (!device_is_ready(dev)) {
		LOG_WRN("sensor: device not ready.\n");
		return;
	}

	/* Fetch enviromental sensor data */
	sensor_sample_fetch(dev);

	/* Read enviromental sensor channels */
	sensor_channel_get(dev, SENSOR_CHAN_AMBIENT_TEMP, temp);
	sensor_channel_get(dev, SENSOR_CHAN_PRESS, press);
	sensor_channel_get(dev, SENSOR_CHAN_HUMIDITY, humidity);
	sensor_channel_get(dev, SENSOR_CHAN_GAS_RES, gas_res);
	gpio_pin_set_dt(&out2, 0); //GPIO
}


void SampleBME688()
{
	gpio_pin_set_dt(&out2, 1); //GPIO
	if (!device_is_ready(dev)) {
		LOG_WRN("sensor: device not ready.\n");
		return;
	}

	/* Fetch enviromental sensor data */
	sensor_sample_fetch(dev);

	/* Read enviromental sensor channels */
	sensor_channel_get(dev, SENSOR_CHAN_AMBIENT_TEMP, &m_temperature);
	sensor_channel_get(dev, SENSOR_CHAN_PRESS, &m_air_pressure);
	sensor_channel_get(dev, SENSOR_CHAN_HUMIDITY, &m_humidity);
	sensor_channel_get(dev, SENSOR_CHAN_GAS_RES, &m_gas_resolution);
	gpio_pin_set_dt(&out2, 0); //GPIO
	m_temperature_avg = ((m_temperature_avg * sampleCountBME688++) + sensor_value_to_double(&m_temperature)) / sampleCountBME688;
}

void GetDataBME888(double *temp, double *press, double *humidity, double *gas_res)
{
	*temp = m_temperature_avg;
}
/*
std::string GetJsonBME688(){
	std::string transmission_string = agent_id + ",temperature:" + double_to_string(m_temperature_avg) + "," +
									  "humidity:" + double_to_string(getAvgHumidity()) + "," +
									  "air_pressure:" + double_to_string(getAvgAirPressure()) + "," +
									  "gas_resolution:" + std::to_string(getAvgGasResolution());
	return transmission_string;
}
*/
void ClearDataBME688()
{
	sampleCountBME688 = 0;
}

/*************************************************************************************************/
/*!
 *  \brief  Reads the BH1749 color and light sensor.
 *
 *  \param  None.
 *
 *  \return None.
 */
/*************************************************************************************************/
void ReadBH1749()
{
	//TODO: Add code here
}
