/**
 * @file    pus178.c
 * @author  Aldo Lupio
 * @brief   Source file for PUS 178 (Thermal) functions
 *
 * @copyright Copyright (c) TOLOSAT 2026
 * SPDX-License-Identifier: Apache-2.0
 */

/******************************* Include Files *******************************/

#include <string.h>

#include "kernel.h"
#include "tm_management.h"
#include "service/pus178.h"

/***************************** Macros Definitions ****************************/

#define MAX_NB_DS18_ONBOARD 16u                                                     /**< Maximum number of allocated sensors */
#define DATA_PER_DS18_SIZE  (sizeof(ds18SensorId_t) + sizeof(ds18RawTemperature_t)) /**< Data size per sensor on tm (Id + Temperature) */

/*************************** Functions Declarations **************************/

/*************************** Variables Definitions ***************************/

/*************************** Functions Definitions ***************************/

/**
 * @copydoc InitS178
 */
returnCode_t InitS178(pus178Env_t *pus178_env)
{
    returnCode_t return_value = RET_SUCCESSFUL;

    // Check input parameter(s)
    if ((pus178_env != NULL) && (pus178_env->p_ds18_context != NULL))
    {
        // Check DS18 is initialized
        if (pus178_env->p_ds18_context->ow_device_init_state == DS18_INIT_DONE)
        {
            pus178_env->status = PUS_INITIALIZED;
        }
        else
        {
            pus178_env->status = PUS_NOT_INITIALIZED;
        }
    }
    else
    {
        return_value = RET_INVALID_PARAM;
    }

    return return_value;
}

/**
 * @copydoc ExecuteS178SS1
 */
returnCode_t ExecuteS178SS1(void *env, pusTC_t *tc, pusTM_t *tm, pusExecutionError_t *error_code)
{
    returnCode_t return_value = RET_SUCCESSFUL;

    // Unused Parameters
    (void)(tc);

    // Check input parameter(s)
    if ((env != NULL) && (tm != NULL) && (error_code != NULL))
    {
        // Error code and tm initialization
        *error_code = PUS_EXECUTION_NO_ERROR;

        // Get pus178 environment
        pus178Env_t *pus178_env = (pus178Env_t *)env;

        // Check pus178 initialization
        if (pus178_env->status == PUS_INITIALIZED)
        {
            // Check pointer to thermal context exists
            if (pus178_env->p_ds18_context != NULL)
            {
                // Check sensor has been initialized
                if (pus178_env->p_ds18_context->ow_device_init_state == DS18_INIT_DONE)
                {
                    // Ask for broadcast measurement
                    return_value = DS18StartMeasurementBroadcast(pus178_env->p_ds18_context);
                    if (return_value == RET_SUCCESSFUL)
                    {
                        ds18RawTemperature_t raw_temp[MAX_NB_DS18_ONBOARD] = { 0 };

                        // Current number of ds18 sensors onboard from context
                        uint8_t nb_ds18_onboard = pus178_env->p_ds18_context->ds18_count;

                        // Sleep for 500 ms before reading temps
                        Sleep(500);

                        // Read all ds18 sensor temperatures onboard
                        return_value = DS18ReadTemperaturesBroadcast(pus178_env->p_ds18_context, raw_temp, nb_ds18_onboard);
                        if (return_value == RET_SUCCESSFUL)
                        {
                            // Build PUS178SS2 TM (array of temperatures from all sensors and their IDs, (2 bytes + 1 byte) * N)
                            uint8_t tm_data_buffer[(DATA_PER_DS18_SIZE * MAX_NB_DS18_ONBOARD) + sizeof(pusNField_t)] = { 0 };
                            // Set N field
                            tm_data_buffer[0u] = nb_ds18_onboard; // Nb sensor
                            // Set ID and data
                            for (uint32_t i = 0u; i < nb_ds18_onboard; i++)
                            {
                                tm_data_buffer[(i * DATA_PER_DS18_SIZE) + sizeof(pusNField_t)] = i; // Sensor id
                                // Little Endian
                                tm_data_buffer[(i * DATA_PER_DS18_SIZE) + 1u + sizeof(pusNField_t)] =
                                    (uint8_t)((uint16_t)raw_temp[i] & 0xFFu); // Temperature
                                                                              // LSB
                                tm_data_buffer[(i * DATA_PER_DS18_SIZE) + 2u + sizeof(pusNField_t)] =
                                    (uint8_t)((uint16_t)raw_temp[i] >> 8); // Temperature
                                                                           // MSB
                            }

                            return_value = BuildTM(tm, 178u, 2u, tm_data_buffer, sizeof(pusNField_t) + (nb_ds18_onboard * DATA_PER_DS18_SIZE));
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
                    return_value = RET_INVALID_PARAM;
                    *error_code  = PUS_EXECUTION_FAILED;
                }
            }
            else
            {
                return_value = RET_INVALID_PARAM;
                *error_code  = PUS_EXECUTION_FAILED;
            }
        }
        else
        {
            return_value = RET_INVALID_PARAM;
            *error_code  = PUS_EXECUTION_UNAVAILABLE;
        }
    }
    else
    {
        return_value = RET_INVALID_PARAM;
    }

    return return_value;
}

/**
 * @copydoc ExecuteS178SS3
 */
returnCode_t ExecuteS178SS3(void *env, pusTC_t *tc, pusTM_t *tm, pusExecutionError_t *error_code)
{
    returnCode_t return_value = RET_SUCCESSFUL;

    // Check input parameter(s)
    if ((env != NULL) && (tc != NULL) && (tm != NULL) && (error_code != NULL))
    {
        *error_code = PUS_EXECUTION_NO_ERROR;

        // Get pus178 environment
        pus178Env_t *pus178_env = (pus178Env_t *)env;

        // Check pus178 initialization
        if (pus178_env->status == PUS_INITIALIZED)
        {
            // Check pointer to thermal context exists
            if (pus178_env->p_ds18_context != NULL)
            {
                // Check sensor has been initialized
                if (pus178_env->p_ds18_context->ow_device_init_state == DS18_INIT_DONE)
                {
                    // Ask for broadcast measurement
                    return_value = DS18StartMeasurementBroadcast(pus178_env->p_ds18_context);
                    if (return_value == RET_SUCCESSFUL)
                    {
                        uint8_t tm_data_buffer[(MAX_NB_DS18_ONBOARD * DATA_PER_DS18_SIZE) + sizeof(pusNField_t)] = { 0 };

                        // Number of ds18 sensors readings requested from tc and loop parameters
                        pusNField_t tc_nb_ds18_requested = BIG_ENDIAN_ARRAY_TO_UINT16(tc->data);
                        uint8_t i                        = 0;
                        uint16_t offset                  = 0;

                        // Copy NB requested data into tm_data_buffer
                        (void)memcpy(tm_data_buffer, tc->data, sizeof(pusNField_t));
                        offset += sizeof(pusNField_t);

                        // Sleep for 500 ms before reading temps
                        Sleep(500);

                        // Read requested sensors through iteration unless error
                        while ((i < tc_nb_ds18_requested) && (return_value == RET_SUCCESSFUL))
                        {
                            // Iterate sensor Id from tc: [N] - [id 1] - [id 2] - ... - [id N]
                            ds18SensorId_t sensor_id = tc->data[i + 1u];

                            // Check sensor id is valid against preallocation and current sensors onboard
                            if ((sensor_id < MAX_NB_DS18_ONBOARD) && (sensor_id < pus178_env->p_ds18_context->ds18_count))
                            {
                                // Prevent buffer overflow (ex: 4 sensors -> 12 bytes = offset (9) + pair (3))
                                if ((offset + DATA_PER_DS18_SIZE) <= (MAX_NB_DS18_ONBOARD * DATA_PER_DS18_SIZE))
                                {
                                    ds18RawTemperature_t raw_temp[MAX_NB_DS18_ONBOARD] = { 0 };

                                    // Read temperature of current iterated sensor
                                    return_value = DS18ReadTemperature(pus178_env->p_ds18_context, sensor_id, &raw_temp[sensor_id]);

                                    // Fill buffer with 1 sensor data if valid
                                    if (return_value == RET_SUCCESSFUL)
                                    {
                                        // Id (8-bit, little-endian)
                                        tm_data_buffer[offset]  = sensor_id;
                                        offset                 += sizeof(ds18SensorId_t);

                                        // Temperature (16-bit, little-endian) -> LSB then MSB
                                        tm_data_buffer[offset]       = (uint8_t)((uint16_t)raw_temp[sensor_id] & 0xFFu);
                                        tm_data_buffer[offset + 1u]  = (uint8_t)((uint16_t)raw_temp[sensor_id] >> 8);
                                        offset                      += sizeof(ds18RawTemperature_t);
                                    }
                                }
                                else
                                {
                                    return_value = RET_ERROR;
                                }
                            }
                            else
                            {
                                return_value = RET_ERROR;
                            }

                            i++;
                        }

                        // Build TM PUS178SS4 with already filled buffer
                        if (return_value == RET_SUCCESSFUL)
                        {
                            return_value = BuildTM(tm, 178u, 4u, (pusData_t *)tm_data_buffer, offset);

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
                    return_value = RET_INVALID_PARAM;
                    *error_code  = PUS_EXECUTION_FAILED;
                }
            }
            else
            {
                return_value = RET_INVALID_PARAM;
                *error_code  = PUS_EXECUTION_FAILED;
            }
        }
        else
        {
            return_value = RET_INVALID_PARAM;
            *error_code  = PUS_EXECUTION_UNAVAILABLE;
        }
    }
    else
    {
        return_value = RET_INVALID_PARAM;
    }

    return return_value;
}