#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <vector>
#include <array>
#include <string>
#include <iostream>
#include <math.h>
#include <numeric>
#include <algorithm>


/**************************************************************************************************
  ZEPHYR include files.
**************************************************************************************************/
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/printk.h>
#include <zephyr/drivers/sensor.h>



extern "C" {
  #include "peripherals/sensors.h"
  //#include "peripherals/battery.h"
  #include "peripherals/gpio.h"
}

#define MAX_BUFFER_SIZE                   10

/* Class for BME688 enviromental sensor data  */

class MeasurementBuffer {
	private:
		int headIndex;
		int prevIndex;
		int fillLevel;
		int bufferSize;
		double m_temperatures[MAX_BUFFER_SIZE];
		double m_humidities[MAX_BUFFER_SIZE];
		double m_air_pressures[MAX_BUFFER_SIZE];
		double m_gas_resolutions[MAX_BUFFER_SIZE];
	public:
		MeasurementBuffer(int size);
		void addMeasurement(double temperature, double humidity, double pressure, double gas_resolution);
		void resizeBuffer(int size);
		bool isFull();
		void clearBuffer();
		std::string getTransmissionData(std::string agent_id);
		double getAvgTemperature();
		double getAvgHumidity();
		double getAvgAirPressure();
		int getAvgGasResolution();
};

class BME688 {
	private:
		struct sensor_value m_temperature;
  		struct sensor_value m_humidity;
		struct sensor_value m_air_pressure;
		struct sensor_value m_gas_resolution;
        MeasurementBuffer* m_p_measurement_buffer;
        std::string agent_id;
	public:
        BME688(std::string aid);
		void measure();
		double getTemperature();
		double getHumidity();
		double getAirPressure();
		double getGasResolution();
        bool isBufferFull();
        void resizeBuffer(int size);
        void clearBuffer();
        std::string getPayload();
};

