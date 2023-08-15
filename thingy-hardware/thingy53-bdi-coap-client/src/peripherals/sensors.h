#ifndef SENSORS_H_
#define SENSORS_H_

#include <stdio.h>
#include <unistd.h>

/**************************************************************************************************
  Function Declarations.
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
				        struct sensor_value *humidity, struct sensor_value *gas_res);

void SampleBME688();

void GetDataBME888(double *temp, double *press, double *humidity, double *gas_res);

void ClearDataBME688();

/*************************************************************************************************/
/*!
 *  \brief  Reads the BH1749 color and light sensor.
 *
 *  \param  None.
 *
 *  \return None.
 */
/*************************************************************************************************/
void ReadBH1749();

#endif /* SENSORS_H_ */
