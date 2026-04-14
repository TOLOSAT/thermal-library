/**
 * @file    ds18_drv.h
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

#ifndef DS18_DRV_H
#define DS18_DRV_H

/******************************* Include Files *******************************/

#include "common_types.h"
#include "kernel_types.h"

/***************************** Macros Definitions ****************************/

#define ROM_CMD_SIZE        1u /**< ROM type command size (1 byte) */
#define FCN_CMD_SIZE        1u /**< Function type command size (1 byte) */
#define ROM_SIZE            8u /**< ROM size (8 bytes) */
#define SCRATCHPAD_SIZE     9u /**< Response message size (9 bytes -> 8 bytes scratchpad + CRC 1 byte) */

#define MEAS_BROAD_CMD_SIZE (ROM_CMD_SIZE + FCN_CMD_SIZE)            /**< Broadcast temp cmd size */
#define MEAS_UNIC_CMD_SIZE  (ROM_CMD_SIZE + ROM_SIZE + FCN_CMD_SIZE) /**< Unicast temp cmd size */
#define READ_UNIC_CMD_SIZE  (ROM_CMD_SIZE + ROM_SIZE + FCN_CMD_SIZE) /**< Unicast read temp cmd size */

#define MATCH_ROM_CMD       0x55u /**< Rom cmd - Match ROM opcode */
#define SKIP_ROM_CMD        0xCCu /**< Rom cmd - Skip ROM opcode */
#define CONVERT_TEMP_CMD    0x44u /**< Fcn cmd - Convert temp opcode */
#define READ_SCRATCHPAD_CMD 0xBEu /**< Fcn cmd - Read scratchpad opcode */

/**
 * @def     IS_ENV_INIT(env)
 * @brief   Indicates if the environment exists in order to initialize it
 */
#define IS_ENV_INIT(env)                                    \
    (((env) != NULL) &&            /* Environment exists */ \
     ((env)->ds18_info != NULL) && /* Temp sensors exist */ \
     ((env)->ds18_count > 0u) &&   /* Sensors exist */      \
     ((env)->ow_device_init_state == DS18_INIT_NOT_DONE))

/**
 * @def     IS_ENV_VALID(env)
 * @brief   Indicates if the environment has been initialized properly
 */
#define IS_ENV_VALID(env)                                   \
    (((env) != NULL) &&            /* Environment exists */ \
     ((env)->ds18_info != NULL) && /* Temp sensors exist */ \
     ((env)->ds18_count > 0u) &&   /* Sensors exist */      \
     ((env)->ow_device_init_state == DS18_INIT_DONE))

/***************************** Types Definitions *****************************/

/**
 * @brief   Type describing id of a DS18B20 sensor
 */
typedef uint8_t ds18SensorId_t;

/**
 * @brief   Type describing raw temperature reading from A DS18B20 sensor
 */
typedef int16_t ds18RawTemperature_t;

/**
 * @enum      ds18Model_t
 * @brief     Models of temperature sensors
 */
typedef enum
{
    DS18B20, /**< Temp sensor DS18B2O - Can be extracted from ROM*/
    DS18S20  /**< Temp sensor DS18S20 - Can be extracted from ROM */
} ds18Model_t;

/**
 * @enum      ds18InitState_t
 * @brief     State explaining the initialization of the device (OneWire)
 */
typedef enum
{
    DS18_INIT_DONE,     /**< OneWire already initialized */
    DS18_INIT_NOT_DONE, /**< OneWire not initialized */
    DS18_INIT_ERROR     /**< OneWire error */
} ds18InitState_t;

/**
 * @struct  ds18Info_t
 * @brief   Temperature sensor informations
 */
typedef struct
{
    uint8_t ds18_rom_code[8]; /**< @brief ROM code (8 bytes) */
    ds18Model_t ds18_model;   /**< @brief Model of sensor (DS18B20 or DS18S20) */
} ds18Info_t;

/**
 * @struct  ds18Context_t
 * @brief   Environment for the temperature middle-ware
 */
typedef struct
{
    ds18Info_t *ds18_info;                /**< @brief Temperature sensor information (ROM, Model) */
    uint8_t ds18_count;                   /**< @brief Number of sensors available */
    deviceNo_t ow_device;                 /**< @brief Device specifying OneWire transactions */
    uint32_t peripheral;                  /**< @brief Peripheral specifying OneWire transactions */
    ds18InitState_t ow_device_init_state; /**< @brief State of Initialization */
} ds18Context_t;

/*************************** Variables Declarations **************************/

/*************************** Functions Declarations **************************/

extern returnCode_t DS18Init(ds18Context_t *context);
extern returnCode_t DS18StartMeasurementBroadcast(ds18Context_t *context);
extern returnCode_t DS18ReadTemperaturesBroadcast(ds18Context_t *context, ds18RawTemperature_t *raw_temp, size_t count_sensor);
extern returnCode_t DS18StartMeasurement(ds18Context_t *context, ds18SensorId_t sensor_id);
extern returnCode_t DS18ReadTemperature(ds18Context_t *context, ds18SensorId_t sensor_id, ds18RawTemperature_t *raw_temp);
extern returnCode_t DS18ConvertRawToFloat(ds18Model_t model, ds18RawTemperature_t raw_temp, float *float_temp);

#endif /* DS18_DRV_H */

/**
 * @}
 * @}
 */