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

#define MAX_TEMP_SENSORS 16u                                                         /**< Maximum number of allocated sensors */
#define SENSOR_PAIR_SIZE (sizeof(pus178sensorId_t) + sizeof(pus178rawTemperature_t)) /**< Data size (Id + Temperature) */

/*************************** Functions Declarations **************************/

/*************************** Variables Definitions ***************************/

/*************************** Functions Definitions ***************************/

/**
 * @fn              InitS178(pus178Env_t *pus178_env)
 * @brief           This function initializes a PUS178 context
 * @param[in,out]   pus178_env PUS178 environment
 * @retval          #RET_INVALID_PARAM if pus178_env or p_thermal_context are null pointers
 * @retval          #RET_ERROR if DeviceOpen encountered an error
 * @retval          #RET_SUCCESSFUL else
 */
returnCode_t InitS178(pus178Env_t *pus178_env)
{
    returnCode_t return_value = RET_SUCCESSFUL;

    // Check input parameter(s)
    if ((pus178_env != NULL) && (pus178_env->p_thermal_context != NULL))
    {
        // Initialize DS18B20 sensor
        return_value = DS18B20Init(pus178_env->p_thermal_context);
        if (return_value == RET_SUCCESSFUL)
        {
            pus178_env->status = PUS_CONTEXT_INITIALIZED;
        }
        else
        {
            pus178_env->status = PUS_CONTEXT_NOT_INITIALIZED;
        }
    }
    else
    {
        return_value = RET_INVALID_PARAM;
    }

    return return_value;
}

/**
 * @fn              ExecuteS178SS1(void *env, pusTC_t *tc, pusTM_t *tm, pusExecutionError_t *error_code)
 * @brief           Function that receives S17SS1 TC (Broadcast) and asks for broadcast temperature
 * @param[in,out]   env PUS178 environment
 * @param[in]       tc S178SS1 TC
 * @param[out]      tm S178SS2 TM with temperature sensor information
 * @param[out]      error_code Indicates which error has been encountered for S178SS2 TM
 * @retval          #RET_INVALID_PARAM if inputs are not valid
 * @retval          #RET_ERROR if error during sensor communication
 * @retval          #RET_SUCCESSFUL else
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
        if (pus178_env->status == PUS_CONTEXT_INITIALIZED)
        {
            // Check pointer to thermal context exists
            if (pus178_env->p_thermal_context != NULL)
            {
                // Check sensor has been initialized
                if (pus178_env->p_thermal_context->ow_device_init_state == INIT_DONE)
                {
                    // Ask for broadcast measurement
                    return_value = DS18B20StartMeasurementBroadcast(pus178_env->p_thermal_context);
                    if (return_value == RET_SUCCESSFUL)
                    {
                        pus178rawTemperature_t raw_temperatures[MAX_TEMP_SENSORS] = { 0 };

                        // Current number of sensors onboard from context (>0 checked)
                        uint8_t env_sensor_n_max = pus178_env->p_thermal_context->temp_sensor_count;

                        // Read all sensors temperature
                        return_value = DS18B20ReadTemperaturesBroadcast(pus178_env->p_thermal_context, raw_temperatures, env_sensor_n_max);
                        if (return_value == RET_SUCCESSFUL)
                        {
                            // Build PUS178SS2 TM (array of temperatures from all sensors, 2 bytes * N)
                            return_value = BuildTM(tm, 178u, 2u, (pusData_t *)raw_temperatures, env_sensor_n_max * sizeof(pus178rawTemperature_t));
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
 * @fn              ExecuteS178SS3(void *env, pusTC_t *tc, pusTM_t *tm, pusExecutionError_t *error_code)
 * @brief           Function that receives S17SS3 TC (Unicast), which asks for unicast temperature
 * @param[in,out]   env PUS178 environment
 * @param[in]       tc S178SS3 TC
 * @param[out]      tm S178SS4 TM with asked temperature sensor information
 * @param[out]      error_code Indicates which error has been encountered for S178SS4 TM
 * @retval          #RET_INVALID_PARAM if am expected pointer is NULL
 * @retval          #RET_ERROR if cannot build TM
 * @retval          #RET_SUCCESSFUL else
 */
returnCode_t ExecuteS178SS3(void *env, pusTC_t *tc, pusTM_t *tm, pusExecutionError_t *error_code)
{
    returnCode_t return_value = RET_SUCCESSFUL;

    // Check input parameter(s)
    if ((env != NULL) && (tc != NULL) && (tm != NULL) && (error_code != NULL))
    {
        *error_code = PUS_EXECUTION_NO_ERROR;

        // TC Format: [N SENSOR] - [ID1] - [ID2] - ... - [ID N]
        // Check TC table by extracting it and checking if is valid and we have at least onse sensor?

        // Get pus178 environment
        pus178Env_t *pus178_env = (pus178Env_t *)env;

        // Check pus178 initialization
        if (pus178_env->status == PUS_CONTEXT_INITIALIZED)
        {
            // Check pointer to thermal context exists
            if (pus178_env->p_thermal_context != NULL)
            {
                // Check sensor has been initialized
                if (pus178_env->p_thermal_context->ow_device_init_state == INIT_DONE)
                {
                    // Ask for broadcast measurement
                    return_value = DS18B20StartMeasurementBroadcast(pus178_env->p_thermal_context);
                    if (return_value == RET_SUCCESSFUL)
                    {
                        // N of sensors requested from tc (>0 checked) and loop parameters
                        uint8_t tc_sensor_n_req = tc->data[0];
                        uint8_t i               = 0;
                        uint16_t offset         = 0;

                        // Current number of sensors onboard from context!!! (>0 checked)
                        uint8_t env_sensor_n_max = pus178_env->p_thermal_context->temp_sensor_count;

                        // Read requested sensors through iteration unless error
                        while ((i < tc_sensor_n_req) && (return_value == RET_SUCCESSFUL))
                        {
                            // Iterate sensor Id from tc: [N] - [id 1] - [id 2] - ... - [id N]
                            pus178sensorId_t sensorid = tc->data[i + 1u];

                            // Check sensor id is valid against preallocation and current sensors onboard
                            if ((sensorid < MAX_TEMP_SENSORS) && (sensorid < env_sensor_n_max))
                            {
                                // Prevent buffer overflow (ex: 4 sensors -> 12 bytes = offset (9) + pair (3))
                                if ((offset + SENSOR_PAIR_SIZE) <= (MAX_TEMP_SENSORS * SENSOR_PAIR_SIZE))
                                {
                                    pus178rawTemperature_t raw_temperatures[MAX_TEMP_SENSORS] = { 0 };

                                    // Read temperature of current iterated sensor
                                    return_value = DS18B20ReadTemperature(pus178_env->p_thermal_context, sensorid, &raw_temperatures[sensorid]);

                                    // Fill buffer with 1 sensor data if valid
                                    if (return_value == RET_SUCCESSFUL)
                                    {
                                        uint8_t tm_buffer[MAX_TEMP_SENSORS * SENSOR_PAIR_SIZE] = { 0 };

                                        // Id (8-bit, big-endian)
                                        tm_buffer[offset]  = sensorid;
                                        offset            += sizeof(pus178sensorId_t);

                                        // Temperature (16-bit, big-endian) -> MSB then LSB
                                        tm_buffer[offset]       = (uint8_t)(((uint16_t)raw_temperatures[sensorid] >> 8) & 0xFFu);
                                        tm_buffer[offset + 1u]  = (uint8_t)((uint16_t)raw_temperatures[sensorid] & 0xFFu);
                                        offset                 += sizeof(pus178rawTemperature_t);
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
                            return_value = BuildTM(tm, 178u, 4u, (pusData_t *)tm_buffer, offset);

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