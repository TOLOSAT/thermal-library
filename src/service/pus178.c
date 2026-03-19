/**
 * @file    pus178.c
 * @author  Louis
 * @author  Aldo
 * @brief   Source file for PUS 178 (Thermal) functions
 *
 * @copyright Copyright (c) TOLOSAT 2025
 */

/******************************* Include Files *******************************/

#include <string.h>

#include "kernel.h"
#include "tm_management.h"
#include "service/pus178.h"

/***************************** Macros Definitions ****************************/

#define MAX_TEMP_SENSORS 16u

/*************************** Functions Declarations **************************/

/*************************** Variables Definitions ***************************/

/*************************** Functions Definitions ***************************/

/**
 * @fn              ExecuteS178SS1(void *env, pusTC_t *tc, pusTM_t *tm, pusExecutionError_t *error_code)
 * @brief           Function that receives S17SS1 TC (Broadcast) which asks for broadcast temperature
 * @param[in,out]   env PUS178 environment
 * @param[in]       tc S178SS1 TC
 * @param[out]      tm S178SS2 TM with temperature sensor env information
 * @param[out]      error_code Indicates which error has been encountered for S178SS2 TM
 * @retval          #RET_INVALID_PARAM if a pointer is NULL
 * @retval          #RET_ERROR if cannot build TM
 * @retval          #RET_SUCCESSFUL else
 */
returnCode_t ExecuteS178SS1(void *env, pusTC_t *tc, pusTM_t *tm, pusExecutionError_t *error_code)
{
    returnCode_t return_value = RET_SUCCESSFUL;

    // Unused (no data useful for broadcast)
    (void)(tc);

    // Check parameters
    if ((env != NULL) && (error_code != NULL))
    {
        pus178Env_t *pus178_env = (pus178Env_t *)env;

        // Check pointer exists
        if (pus178_env->p_thermal_context != NULL)
        {
            *error_code = PUS_EXECUTION_NO_ERROR;

            // Preallocation required thermal driver variables
            uint8_t sensor_count = pus178_env->p_thermal_context->temp_sensor_count;
            int16_t raw_temperatures[MAX_TEMP_SENSORS] = { 0 };

            if (return_value == RET_SUCCESSFUL)
            {
                return_value = DS18B20StartMeasurementBroadcast(pus178_env->p_thermal_context);

                if (return_value == RET_SUCCESSFUL)
                {
                    return_value = DS18B20ReadTemperaturesBroadcast(pus178_env->p_thermal_context, raw_temperatures, sensor_count);

                    if (return_value == RET_SUCCESSFUL)
                    {
                        return_value = BuildTM(tm, 178u, 2u, (pusData_t *)raw_temperatures, sensor_count * sizeof(int16_t));

                        if (return_value != RET_SUCCESSFUL)
                        {
                            *error_code = PUS_EXECUTION_FAILED;
                        }
                    }
                    else
                    {
                        *error_code = PUS_EXECUTION_FAILED;
                    }
                }
                else
                {
                    *error_code = PUS_EXECUTION_FAILED;
                }
            }
            else
            {
                *error_code = PUS_EXECUTION_FAILED;
            }
        }
        else
        {
            *error_code = PUS_EXECUTION_NO_ERROR;
        }
    }
    else
    {
        return_value = RET_INVALID_PARAM;
    }

    return return_value;
}

/**
 * @fn              ExecuteS178SS3(void *env, pusTC_t *tc, pusTM_t *tm, pusExecutionError_t *error_code)
 * @brief           Function that receives S17SS3 TC (Unicast) which asks for unicast temperature
 * @param[in,out]   env PUS178 environment
 * @param[in]       tc S178SS3 TC
 * @param[out]      tm S178SS4 TM with temperature sensor env information
 * @param[out]      error_code Indicates which error has been encountered for S178SS4 TM
 * @retval          #RET_INVALID_PARAM if a pointer is NULL
 * @retval          #RET_ERROR if cannot build TM
 * @retval          #RET_SUCCESSFUL else
 */
returnCode_t ExecuteS178SS3(void *env, pusTC_t *tc, pusTM_t *tm, pusExecutionError_t *error_code)
{
    returnCode_t return_value = RET_SUCCESSFUL;

    pusData_t tm_data[TM_MAX_DATA_SIZE] = { 0 };

    // Check parameters
    if ((env != NULL) && (error_code != NULL))
    {
        pus178Env_t *pus178_env = (pus178Env_t *)env;

        // Check pointer exists
        if (pus178_env->p_thermal_context != NULL)
        {
            *error_code = PUS_EXECUTION_NO_ERROR;

            // Preallocation required for thermal driver variables
            int16_t raw_temperatures[MAX_TEMP_SENSORS] = { 0 };

            // TODO:
            // Extract information from tc (sensor_selected)
            // uint16_t msg_size = tc->spp_header.packet_data_length + 1u - TC_HEADER_SIZE - CRC_TRAILER_SIZE;
            pus178SENSORID_t sensorid = *tc->data;

            if (return_value == RET_SUCCESSFUL)
            {
                return_value = DS18B20StartMeasurement(pus178_env->p_thermal_context, sensorid);

                if (return_value == RET_SUCCESSFUL)
                {
                    return_value = DS18B20ReadTemperature(pus178_env->p_thermal_context, sensorid, &raw_temperatures[sensorid]);

                    if (return_value == RET_SUCCESSFUL)
                    {
                        return_value = BuildTM(tm, 178u, 4u, (pusData_t *)tm_data, sizeof(int16_t));

                        if (return_value != RET_SUCCESSFUL)
                        {
                            *error_code = PUS_EXECUTION_FAILED;
                        }
                    }
                    else
                    {
                        *error_code = PUS_EXECUTION_FAILED;
                    }
                }
                else
                {
                    *error_code = PUS_EXECUTION_FAILED;
                }
            }
            else
            {
                *error_code = PUS_EXECUTION_FAILED;
            }
        }
        else
        {
            *error_code = PUS_EXECUTION_NO_ERROR;
        }
    }
    else
    {
        return_value = RET_INVALID_PARAM;
    }

    return return_value;
}