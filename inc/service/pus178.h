/**
 * @file    pus178.h
 * @author  Merlin Kooshmanian
 * @brief   Header file for pus178 Task
 *
 * @copyright Copyright (c) TOLOSAT 2025
 */

#ifndef PUS178_H
#define PUS178_H

/******************************* Include Files *******************************/

#include "pus_types.h"
#include "common_types.h"
#include "drv/thermal_driver.h"

/***************************** Macros Definitions ****************************/

/***************************** Types Definitions *****************************/

/**
 * @struct  pus178Env_t
 * @brief   Struct type definition of a PUS178 (Thermal) environment instance
 */
typedef struct
{
    pusContextStatus_t status;             /**< @brief PUS178 environment status */
    temSensorContext_t *p_thermal_context; /**< @brief Pointer to the thermal instance */
} pus178Env_t;

/**
 * @brief   Type of id describing a sensor
 */
typedef uint8_t pus178sensorId_t;

/**
 * @brief   Type describing raw temperature reading from sensor
 */
typedef int16_t pus178rawTemperature_t;

/*************************** Variables Declarations **************************/

/*************************** Functions Declarations **************************/

extern returnCode_t InitS178(pus178Env_t *pus178_env);
extern returnCode_t ExecuteS178SS1(void *env, pusTC_t *tc, pusTM_t *tm, pusExecutionError_t *error_code);
extern returnCode_t ExecuteS178SS3(void *env, pusTC_t *tc, pusTM_t *tm, pusExecutionError_t *error_code);

#endif /* PUS178_H */