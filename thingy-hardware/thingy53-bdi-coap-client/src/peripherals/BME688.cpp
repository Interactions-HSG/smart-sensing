#include "BME688.h"
//#include <ram_pwrdn.h>
#include <zephyr/pm/device.h>

LOG_MODULE_REGISTER(BME688, CONFIG_SENSOR_LOG_LEVEL);

static inline std::string sensor_value_to_string(const struct sensor_value *val)
{
	return std::to_string(val->val1) + "." + std::to_string(val->val2);
}

static inline std::string double_to_string(const double input)
{
	struct sensor_value val;
	val.val1 = 0;
	val.val2 = 0;
	sensor_value_from_double(&val, input);
	return sensor_value_to_string (&val);
}

/*************************************************************************************************/
/*!
 *  \brief  Transforms sensor data struct to double value.
 *
 *  \param  data: Sensor value as struct.
 * 
 *  \return Sensor value as double.
 */
/*************************************************************************************************/
double bme688_to_double(struct sensor_value *data)
{
	return ((double)data->val1 + (double)data->val2 / 1000000);
}

/**************************************************************************************************
  BME688 Class.
**************************************************************************************************/

BME688::BME688(std::string aid)
{
    m_p_measurement_buffer = new MeasurementBuffer(10);
    agent_id = aid;
}

void BME688::measure()
{
	ReadBME688(&m_temperature, &m_air_pressure, &m_humidity, &m_gas_resolution);
    m_p_measurement_buffer->addMeasurement(getTemperature(), getHumidity(), getAirPressure(), getGasResolution());
}

double BME688::getTemperature()
{
	return sensor_value_to_double(&m_temperature);
}

double BME688::getHumidity()
{
	return sensor_value_to_double(&m_humidity);
}

double BME688::getAirPressure()
{
	return sensor_value_to_double(&m_air_pressure);
}

double BME688::getGasResolution()
{
	return sensor_value_to_double(&m_gas_resolution);
}

bool BME688::isBufferFull()
{
    return m_p_measurement_buffer->isFull();
}

void BME688::resizeBuffer(int new_size)
{
    m_p_measurement_buffer->resizeBuffer(new_size);
}

void BME688::clearBuffer()
{
    m_p_measurement_buffer->clearBuffer();
}

std::string BME688::getPayload(){
    return m_p_measurement_buffer->getTransmissionData(agent_id);
}

MeasurementBuffer::MeasurementBuffer(int size){
	bufferSize = size;
	headIndex = prevIndex = fillLevel = 0;
	if (size > MAX_BUFFER_SIZE){
		LOG_ERR("Error, tried to set too large BME688 buffer size!");
		bufferSize = MAX_BUFFER_SIZE;
	}
}
void MeasurementBuffer::addMeasurement(double temperature, double humidity, double pressure, double gas_resolution)
{
	m_temperatures[headIndex]=temperature;
	m_humidities[headIndex]=humidity;
	m_air_pressures[headIndex]=pressure;
	m_gas_resolutions[headIndex]=gas_resolution;
	/* Update Index */
	prevIndex = headIndex;
	fillLevel = headIndex+1;
	headIndex < bufferSize-1 ? headIndex++ : headIndex = 0;
}
double MeasurementBuffer::getAvgTemperature()
{
	double sum = 0;
    for (int i = 0; i < fillLevel; i++)
    {
        sum += m_temperatures[i];
    }
    return (sum / fillLevel);
}
double MeasurementBuffer::getAvgHumidity()
{
	double sum = 0;
    for (int i = 0; i < fillLevel; i++)
    {
        sum += m_humidities[i];
    }
    return (sum / fillLevel);
}
double MeasurementBuffer::getAvgAirPressure()
{
	double sum = 0;
    for (int i = 0; i < fillLevel; i++)
    {
        sum += m_air_pressures[i];
    }
    return (sum / fillLevel);
}
int MeasurementBuffer::getAvgGasResolution()
{
	double sum = 0;
    for (int i = 0; i < fillLevel; i++)
    {
        sum += m_gas_resolutions[i];
    }
    return (sum / fillLevel);
}
void MeasurementBuffer::resizeBuffer(int size)
{
	bufferSize = size;
	headIndex = prevIndex = fillLevel = 0;
	if (size > MAX_BUFFER_SIZE){
		LOG_ERR("Error, tried to set too large BME688 buffer size!");
		bufferSize = MAX_BUFFER_SIZE;
	}
}
void MeasurementBuffer::clearBuffer()
{
	headIndex = prevIndex = fillLevel = 0;
}
bool MeasurementBuffer::isFull()
{
	return (fillLevel >= bufferSize);
}
std::string MeasurementBuffer::getTransmissionData(std::string agent_id){
	std::string transmission_string = agent_id + ",temperature:" + double_to_string(getAvgTemperature()) + "," +
									  "humidity:" + double_to_string(getAvgHumidity()) + "," +
									  "air_pressure:" + double_to_string(getAvgAirPressure()) + "," +
									  "gas_resolution:" + std::to_string(getAvgGasResolution());
	return transmission_string;
}