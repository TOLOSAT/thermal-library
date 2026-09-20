/**
 * @file    ds18_drv.c
 * @author  Louis Remacle
 * @author  Aldo Lupio
 * @brief   Source file for Iridium Driver
 *
 * @copyright Copyright (c) TOLOSAT 2025
 */

/******************************* Include Files *******************************/

#include <string.h>

#include "kernel.h"
#include "drivers/ds18_drv.h"

/***************************** Macros Definitions ****************************/

/*************************** Functions Declarations **************************/

/*************************** Variables Definitions ***************************/

/*************************** Functions Definitions ***************************/

/**
 * @copydoc DS18Init
 */
returnCode_t DS18Init(ds18Context_t *context)
{
    returnCode_t return_value = RET_SUCCESSFUL;

    // Check if environment is correctly defined with the sensor information
    if (IS_ENV_INIT(context))
    {
        // OneWire (Device) declaration and peripheral status check
        return_value = DeviceOpen(&context->ow_device, DEVICE_TYPE_PERIPHERAL, context->peripheral);
        if (return_value == RET_SUCCESSFUL)
        {
            // Device opened, update environment and its success state
            context->ow_device_init_state = DS18_INIT_DONE;
        }
        else
        {
            // Device not opened, update environment with error state
            context->ow_device_init_state = DS18_INIT_ERROR;
        }
    }
    else
    {
        // Environment not defined properly, update to error state
        return_value = RET_INVALID_PARAM;
        if (context != NULL)
        {
            context->ow_device_init_state = DS18_INIT_ERROR;
        }
    }
    return return_value;
}

/**
 * @copydoc DS18StartMeasurementBroadcast
 */
returnCode_t DS18StartMeasurementBroadcast(ds18Context_t *context)
{
    returnCode_t return_value = RET_SUCCESSFUL;

    // Check if environment is correctly defined with the sensor information
    if (IS_ENV_VALID(context))
    {
        // Declaration of the message size to be sent via one wire
        uint8_t ow_msg[MEAS_BROAD_CMD_SIZE] = { 0 };

        // Commands to be sent -> Operation -> Action
        ow_msg[0] = SKIP_ROM_CMD;
        ow_msg[1] = CONVERT_TEMP_CMD;

        // One wire device call
        return_value = DeviceIoctl(context->ow_device, IOCTL_OW_INIT_CONNECTION, NULL, 0u);
        if (return_value == RET_SUCCESSFUL)
        {
            // Send command to temperature sensors -> Asks for measurement broadcast
            return_value = DeviceWrite(context->ow_device, ow_msg, MEAS_BROAD_CMD_SIZE);
        }
    }
    else
    {
        return_value = RET_INVALID_PARAM;
    }
    return return_value;
}

/**
 * @copydoc DS18ReadTemperaturesBroadcast
 */
returnCode_t DS18ReadTemperaturesBroadcast(ds18Context_t *context, ds18RawTemperature_t *raw_temp, size_t count_sensor)
{
    returnCode_t return_value = RET_SUCCESSFUL;

    // Check if the environment and other parameters are correctly defined
    if (IS_ENV_VALID(context) && (raw_temp != NULL) && (context->ds18_count == count_sensor))
    {
        for (uint8_t i = 0; i < count_sensor; i++)
        {
            if (return_value == RET_SUCCESSFUL)
            {
                // Call the the targeted reader for each sensor
                return_value = DS18ReadTemperature(context, i, &raw_temp[i]);
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
 * @copydoc DS18StartMeasurement
 */
returnCode_t DS18StartMeasurement(ds18Context_t *context, ds18SensorId_t sensor_id)
{
    returnCode_t return_value = RET_SUCCESSFUL;

    // Check if the environment and other parameters are correctly defined
    if (IS_ENV_VALID(context) && (sensor_id < context->ds18_count))
    {
        // One wire device call
        return_value = DeviceIoctl(context->ow_device, IOCTL_OW_INIT_CONNECTION, NULL, 0u);
        if (return_value == RET_SUCCESSFUL)
        {
            // Declaration of the message size to be sent via one wire
            uint8_t ow_msg_cmd[MEAS_UNIC_CMD_SIZE] = { 0 };

            // Match ROM 1u -> ROM 8u -> Convert Temp 1u
            ow_msg_cmd[0] = MATCH_ROM_CMD;
            for (uint8_t i = 0u; i < ROM_SIZE; i++)
            {
                ow_msg_cmd[i + 1u] = context->ds18_info[sensor_id].ds18_rom_code[i];
            }
            ow_msg_cmd[MEAS_UNIC_CMD_SIZE - 1u] = CONVERT_TEMP_CMD;

            // Send command to specified temperature sensor
            return_value = DeviceWrite(context->ow_device, ow_msg_cmd, MEAS_UNIC_CMD_SIZE);
        }
    }
    else
    {
        return_value = RET_INVALID_PARAM;
    }
    return return_value;
}

/**
 * @copydoc DS18ReadTemperature
 */
returnCode_t DS18ReadTemperature(ds18Context_t *context, ds18SensorId_t sensor_id, ds18RawTemperature_t *raw_temp)
{
    returnCode_t return_value = RET_SUCCESSFUL;

    // Check if the environment and other parameters are correctly defined
    if (IS_ENV_VALID(context) && (sensor_id < context->ds18_count) && (raw_temp != NULL))
    {
        // One wire device call
        return_value = DeviceIoctl(context->ow_device, IOCTL_OW_INIT_CONNECTION, NULL, 0u);
        if (return_value == RET_SUCCESSFUL)
        {
            // Declaration of the message size to be sent and written via One-Wire
            uint8_t ow_msg_cmd[READ_UNIC_CMD_SIZE] = { 0 };

            // Match Rom 1u -> Rom 8u -> Read Scratchpad 1u
            ow_msg_cmd[0] = MATCH_ROM_CMD;
            for (uint8_t i = 0u; i < ROM_SIZE; i++)
            {
                ow_msg_cmd[i + 1u] = context->ds18_info[sensor_id].ds18_rom_code[i];
            }
            ow_msg_cmd[READ_UNIC_CMD_SIZE - 1u] = READ_SCRATCHPAD_CMD;

            // Send command to specific temperature sensor -> Ask for measurement readout
            return_value = DeviceWrite(context->ow_device, ow_msg_cmd, READ_UNIC_CMD_SIZE);
            if (return_value == RET_SUCCESSFUL)
            {
                uint8_t ow_msg_scratchpad[SCRATCHPAD_SIZE] = { 0 };

                // Copy the measurement readout
                (void *)memset(ow_msg_scratchpad, 0, SCRATCHPAD_SIZE);
                return_value = DeviceRead(context->ow_device, ow_msg_scratchpad, SCRATCHPAD_SIZE);

                if (return_value == RET_SUCCESSFUL)
                {
                    *raw_temp = ((ow_msg_scratchpad[1] << 8)) | ow_msg_scratchpad[0];
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
 * @copydoc DS18ConvertRawToFloat
 */
returnCode_t DS18ConvertRawToFloat(ds18Model_t model, ds18RawTemperature_t raw_temp, float *float_temp)
{
    returnCode_t RC_OK = RET_SUCCESSFUL;

    // Check parameter(s)
    if (float_temp != NULL)
    {
        if (model == DS18B20)
        {
            *float_temp = (float)(((uint16_t)raw_temp << 4) >> 4) * 0.0625;
        }
        else
        {
            *float_temp = (float)raw_temp / 2;
        }
    }
    else
    {
        RC_OK = RET_INVALID_PARAM;
    }
    return RC_OK;
}