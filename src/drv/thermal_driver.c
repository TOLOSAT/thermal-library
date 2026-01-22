/**
 * @file    thermal_driver.c
 * @author  Louis Remacle
 * @author  Aldo Lupio
 * @brief   Source file for Iridium Driver
 *
 * @copyright Copyright (c) TOLOSAT 2025
 */

/******************************* Include Files *******************************/

#include <string.h>

#include "kernel.h"
#include "drv/thermal_driver.h"

/***************************** Macros Definitions ****************************/

/*************************** Functions Declarations **************************/

/*************************** Variables Definitions ***************************/

/*************************** Functions Definitions ***************************/

/**
 * @brief Initializes the DS18B20 sensors environment and the One-Wire interface.
 */
returnCode_t DS18B20Init(temSensorContext_t *temp_context)
{
    returnCode_t return_value = RET_SUCCESSFUL;

    // Check if environment is correctly defined with the sensor information
    if (IS_ENV_INIT(temp_context))
    {
        // OneWire (Device) declaration and peripheral status check
        return_value = DeviceOpen(&temp_context->ow_device, DEVICE_TYPE_PERIPHERAL, temp_context->peripheral);
        if (return_value == RET_SUCCESSFUL)
        {
            // Device opened, update environment and its success state
            temp_context->ow_device_init_state = INIT_DONE;
        }
        else
        {
            // Device not opened, update environment with error state
            temp_context->ow_device_init_state = INIT_ERROR;
        }
    }
    else
    {
        // Environment not defined properly, update to error state
        return_value = RET_INVALID_PARAM;
        if (temp_context != NULL)
        {
            temp_context->ow_device_init_state = INIT_ERROR;
        }
    }
    return return_value;
}

/**
 * @brief Start temperature conversion on all DS18B20 sensors (broadcast).
 */
returnCode_t DS18B20StartMeasurementBroadcast(temSensorContext_t *temp_context)
{
    returnCode_t return_value = RET_SUCCESSFUL;

    // Check if environment is correctly defined with the sensor information
    if (IS_ENV_VALID(temp_context))
    {
        // Declaration of the message size to be sent via one wire
        uint8_t ow_msg[MEAS_BROAD_CMD_SIZE] = { 0 };

        // Commands to be sent -> Operation -> Action
        ow_msg[0] = MATCH_ROM_CMD;
        ow_msg[1] = SKIP_ROM_CMD;

        // One wire device call
        return_value = DeviceIoctl(temp_context->ow_device, IOCTL_OW_INIT_CONNECTION, NULL, 0u);
        if (return_value == RET_SUCCESSFUL)
        {
            // Send command to temperature sensors -> Asks for measurement broadcast
            return_value = DeviceWrite(temp_context->ow_device, ow_msg, MEAS_BROAD_CMD_SIZE);
        }
    }
    else
    {
        return_value = RET_INVALID_PARAM;
    }
    return return_value;
}

/**
 * @brief Read raw temperatures from all sensors in broadcast mode.
 */
returnCode_t DS18B20ReadTemperaturesBroadcast(temSensorContext_t *temp_context, int16_t *raw_temperatures, size_t len)
{
    returnCode_t return_value = RET_SUCCESSFUL;

    // Check if the environment and other parameters are correctly defined
    if (IS_ENV_VALID(temp_context) && (raw_temperatures != NULL) && (temp_context->temp_sensor_count == len))
    {
        for (uint8_t i = 0; i < len; i++)
        {
            if (return_value == RET_SUCCESSFUL)
            {
                // Call the the targeted reader for each sensor
                return_value = DS18B20ReadTemperature(temp_context, i, &raw_temperatures[i]);
            }
        }
    }
    else
    {
        return_value = RET_INVALID_PARAM;
    }
    return return_value;
}

/**
 * @brief Start temperature conversion on one sensor (unicast).
 */
returnCode_t DS18B20StartMeasurement(temSensorContext_t *temp_context, uint8_t sensor_index)
{
    returnCode_t return_value = RET_SUCCESSFUL;

    // Check if the environment and other parameters are correctly defined
    if (IS_ENV_VALID(temp_context) && (sensor_index < temp_context->temp_sensor_count))
    {
        // One wire device call
        return_value = DeviceIoctl(temp_context->ow_device, IOCTL_OW_INIT_CONNECTION, NULL, 0u);
        if (return_value == RET_SUCCESSFUL)
        {
            // Declaration of the message size to be sent via one wire
            uint8_t ow_msg_cmd[MEAS_CMD_SIZE] = { 0 };

            // Commands to be sent -> Operation -> Action
            ow_msg_cmd[0] = MATCH_ROM_CMD;
            for (uint8_t i = 0u; i < ROM_CMD_SIZE; i++)
            {
                ow_msg_cmd[i + 1u] = temp_context->temp_sensors->temp_sensor_rom_code[i];
            }

            // Send command to temperature sensors -> Asks for measurement broadcast
            return_value = DeviceWrite(temp_context->ow_device, ow_msg_cmd, MEAS_CMD_SIZE);
        }
    }
    else
    {
        return_value = RET_INVALID_PARAM;
    }
    return return_value;
}

/**
 * @brief Read a raw temperature value from one sensor (unicast).
 */
returnCode_t DS18B20ReadTemperature(temSensorContext_t *temp_context, uint8_t sensor_index, int16_t *raw_temperature)
{
    returnCode_t return_value = RET_SUCCESSFUL;

    // Check if the environment and other parameters are correctly defined
    if (IS_ENV_VALID(temp_context) && (sensor_index < temp_context->temp_sensor_count) && (raw_temperature != NULL))
    {
        // One wire device call
        return_value = DeviceIoctl(temp_context->ow_device, IOCTL_OW_INIT_CONNECTION, NULL, 0u);
        if (return_value == RET_SUCCESSFUL)
        {
            // Declaration of the message size to be sent and written via One-Wire
            uint8_t ow_msg_cmd[READ_CMD_SIZE] = { 0 };

            // Commands to be sent -> Operation -> Action
            ow_msg_cmd[0] = READ_ROM_CMD;
            for (uint8_t i = 0u; i < ROM_CMD_SIZE; i++)
            {
                ow_msg_cmd[i + 1u] = temp_context->temp_sensors->temp_sensor_rom_code[i];
            }

            // Send command to temperature sensors -> Ask for measurement readout
            return_value = DeviceWrite(temp_context->ow_device, ow_msg_cmd, READ_CMD_SIZE);

            if (return_value == RET_SUCCESSFUL)
            {
                // Copy the measurement readout
                (void *)memset(&ow_msg_cmd, 0, RES_MSG_SIZE);
                return_value = DeviceRead(temp_context->ow_device, ow_msg_cmd, RES_MSG_SIZE);

                if (return_value == RET_SUCCESSFUL)
                {
                    *raw_temperature = ((ow_msg_cmd[1] << 8)) | ow_msg_cmd[0];
                }
            }
        }
    }
    else
    {
        return_value = RET_INVALID_PARAM;
    }
    return return_value;
}

/**
 * @brief Convert raw temperature to float °C.
 */
returnCode_t DS18B20ConvertRawToFloat(temSensorModel_t temp_sensor_model, int16_t raw_temperature, float *temperature)
{
    returnCode_t RC_OK = RET_SUCCESSFUL;

    // Check parameter(s)
    if (temperature != NULL)
    {
        if (temp_sensor_model == DS18B20_MODEL)
        {
            *temperature = (float)(((uint16_t) raw_temperature << 4) >> 4) * 0.0625;
        }
        else
        {
            *temperature = (float)raw_temperature / 2;
        }
    }
    else
    {
        RC_OK = RET_INVALID_PARAM;
    }
    return RC_OK;
}