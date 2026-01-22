/**
 * @file    thermal_driver.h
 * @author  Louis Remacle
 * @author  Aldo Lupio
 * @brief   Header file for Thermal Driver
 *
 * @copyright Copyright (c) TOLOSAT 2025
 */

/**
 * @defgroup middlewares Middlewares
 * @{
 * @defgroup thermal-drv Thermal Driver
 * @brief High level driver for the Thermal sensors and actuators.
 * @{
 */

#ifndef THERMAL_DRIVER_H
#define THERMAL_DRIVER_H

/******************************* Include Files *******************************/

#include "common_types.h"
#include "kernel_types.h"

/***************************** Macros Definitions ****************************/

#define MATCH_ROM_CMD_SIZE  1u /* Match ROM command size */
#define SKIP_ROM_CMD_SIZE   1u /* Skip ROM command size */
#define ROM_CMD_SIZE        8u /* ROM size (64 bits) */
#define READ_ROM_CMD_SIZE   1u /* Read ROM command size */
#define RES_MSG_SIZE        8u /* Response message size */

#define MEAS_BROAD_CMD_SIZE (MATCH_ROM_CMD_SIZE + SKIP_ROM_CMD_SIZE) /* Broadcast measure cmd */
#define MEAS_CMD_SIZE       (MATCH_ROM_CMD_SIZE + ROM_CMD_SIZE)      /* Targeted measure cmd */
#define READ_CMD_SIZE       (READ_ROM_CMD_SIZE + ROM_CMD_SIZE)       /* ROM read cmd */

#define MATCH_ROM_CMD       0x55u /* Match ROM opcode */
#define SKIP_ROM_CMD        0xCCu /* Skip ROM opcode */
#define READ_ROM_CMD        0x33u /* Read ROM opcode */

/**
 * @def     IS_ENV_INIT(env)
 * @brief   Indicates if the environment exists in order to initialize it
 */
#define IS_ENV_INIT(env)                                        \
    (((env) != NULL) &&                /* Environment exists */ \
     ((env)->temp_sensors != NULL) &&  /* Temp sensors exist */ \
     ((env)->temp_sensor_count > 0u) && /* Sensors exist */      \
     ((env)->ow_device_init_state == INIT_NOT_DONE))

/**
 * @def     IS_ENV_VALID(env)
 * @brief   Indicates if the environment has been initialized properly
 */
#define IS_ENV_VALID(env)                                       \
    (((env) != NULL) &&                /* Environment exists */ \
     ((env)->temp_sensors != NULL) &&  /* Temp sensors exist */ \
     ((env)->temp_sensor_count > 0u) && /* Sensors exist */      \
     ((env)->ow_device_init_state == INIT_DONE))

/***************************** Types Definitions *****************************/

/**
 * @enum      temSensorModel_t
 * @brief     Models of temperature sensors
 */
typedef enum
{
    DS18B20_MODEL, /* Temp sensor DS18B2O - Can be extracted from ROM*/
    DS18S20_MODEL  /* Temp sensor DS18S20 - Can be extracted from ROM */
} temSensorModel_t;

/**
 * @enum      initState_t
 * @brief     State explaining the initialization of the device (OneWire)
 */
typedef enum
{
    INIT_DONE,     /* OneWire already initialized */
    INIT_NOT_DONE, /* OneWire not initialized */
    INIT_ERROR     /* OneWire error */
} initState_t;

/**
 * @typedef temSensorInfo_t
 * @brief   Temperature sensor informations
 */
typedef struct
{
    uint8_t temp_sensor_rom_code[8];    /* ROM code (8 bytes) */
    temSensorModel_t temp_sensor_model; /* Model of sensor (DS18B20 or DS18S20) */
} temSensorInfo_t;

/**
 * @typedef temSensorContext_t
 * @brief   Environment for the temperature middle-ware

 */
typedef struct
{
    temSensorInfo_t *temp_sensors;    /* Temperature sensor information (ROM, Model) */
    uint8_t temp_sensor_count;        /* Number of sensors available */
    deviceNo_t ow_device;             /* Device specifying OneWire transactions */
    uint32_t peripheral;              /* Peripheral specifying OneWire transactions */
    initState_t ow_device_init_state; /* State of Initialization */
} temSensorContext_t;

/*************************** Variables Declarations **************************/

/*************************** Functions Declarations **************************/

extern returnCode_t DS18B20Init(temSensorContext_t *temp_context);
extern returnCode_t DS18B20StartMeasurementBroadcast(temSensorContext_t *temp_context);
extern returnCode_t DS18B20ReadTemperaturesBroadcast(temSensorContext_t *temp_context, int16_t *raw_temperatures, size_t len);
extern returnCode_t DS18B20StartMeasurement(temSensorContext_t *temp_context, uint8_t sensor_index);
extern returnCode_t DS18B20ReadTemperature(temSensorContext_t *temp_context, uint8_t sensor_index, int16_t *raw_temperature);
extern returnCode_t DS18B20ConvertRawToFloat(temSensorModel_t temp_sensor_model, int16_t raw_temperature, float *temperature);

#endif /* THERMAL_DRIVER_H */

/**
 * @}
 * @}
 */