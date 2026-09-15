/**
 * @file    ds18_drv.h
 * @author  Louis Remacle
 * @author  Aldo Lupio
 * @brief   Header file for DS18 Driver
 *
 * @copyright Copyright (c) TOLOSAT 2025
 */

/**
 * @defgroup middlewares Middlewares
 * @{
 * @defgroup ds18-drv DS18 Driver
 * @brief High level driver for the DS18 sensors and actuators.
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

/**
 * @fn              DS18Init(ds18Context_t *context)
 * @brief           Initializes the ds18 sensors environment and the One-Wire interface.
 * @param[in,out]   context ds18 context (Sensor information)
 * @retval          #RET_INVALID_PARAM if environment not properly initialised
 * @retval          #RET_ERROR if error while opening OneWire device
 * @retval          #RET_SUCCESSFUL else
 */
extern returnCode_t DS18Init(ds18Context_t *context);

/**
 * @fn              DS18StartMeasurementBroadcast(ds18Context_t *context)
 * @brief           Creates OneWire transactions to issue a Broadcast temperature masurement.
 * @param[in,out]   context ds18 context (Sensor information)
 * @retval          #RET_INVALID_PARAM if environment not properly initialised
 * @retval          #RET_ERROR if error while opening OneWire device
 * @retval          #RET_SUCCESSFUL else
 */
extern returnCode_t DS18StartMeasurementBroadcast(ds18Context_t *context);

/**
 * @fn              DS18ReadTemperaturesBroadcast(ds18Context_t *context, ds18RawTemperature_t *raw_temp, size_t count_sensor)
 * @brief           Creates OneWire transactions to read temperature for len (number of) sensors.
 * @param[in,out]   context ds18 context (Sensor information)
 * @param[in,out]   raw_temp Array to store raw temperatures measurements of length len
 * @param[in]       count_sensor Amount of sensors to be read
 * @retval          #RET_INVALID_PARAM if environment or inputs are not properly initialised
 * @retval          #RET_ERROR if error while opening OneWire device
 * @retval          #RET_SUCCESSFUL else
 */
extern returnCode_t DS18ReadTemperaturesBroadcast(ds18Context_t *context, ds18RawTemperature_t *raw_temp, size_t count_sensor);

/**
 * @fn              DS18StartMeasurement(ds18Context_t *context, ds18SensorId_t sensor_id)
 * @brief           Creates OneWire transactions to issue temperature conversion of single sensor given id.
 * @param[in,out]   context ds18 context (Sensor information)
 * @param[in]       sensor_id id of sensor selected (position on N-table)
 * @retval          #RET_INVALID_PARAM if environment or inputs are not properly initialised
 * @retval          #RET_ERROR if error while opening OneWire device
 * @retval          #RET_SUCCESSFUL else
 */
extern returnCode_t DS18StartMeasurement(ds18Context_t *context, ds18SensorId_t sensor_id);

/**
 * @fn              DS18ReadTemperature(ds18Context_t *context, ds18SensorId_t sensor_id, ds18RawTemperature_t *raw_temp)
 * @brief           Creates OneWire transactions to read temperature of single sensor given id.
 * @param[in,out]   context ds18 context (Sensor information)
 * @param[in,out]   raw_temp raw measured temperature data given back
 * @param[in]       sensor_id id of sensor selected (position on N-table)
 * @retval          #RET_INVALID_PARAM if environment or inputs are not properly initialised
 * @retval          #RET_ERROR if error while opening OneWire device
 * @retval          #RET_SUCCESSFUL else
 */
extern returnCode_t DS18ReadTemperature(ds18Context_t *context, ds18SensorId_t sensor_id, ds18RawTemperature_t *raw_temp);

/**
 * @fn              DS18ConvertRawToFloat(ds18Model_t model, ds18RawTemperature_t raw_temp, float *float_temp)
 * @brief           Converts the temperature from raw (int) to float given sensor model (S/B version).
 * @param[in]       model Information of sensor model to apply proper conversion
 * @param[in]       raw_temp Raw measured temperature data
 * @param[out]      float_temp Measured temperature data in float type
 * @retval          #RET_INVALID_PARAM if raw temperature doesnt exist
 * @retval          #RET_SUCCESSFUL else
 */
extern returnCode_t DS18ConvertRawToFloat(ds18Model_t model, ds18RawTemperature_t raw_temp, float *float_temp);

#endif /* DS18_DRV_H */

/**
 * @}
 * @}
 */