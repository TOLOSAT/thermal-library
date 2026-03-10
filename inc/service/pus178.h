/**
 * @file    thermal.h
 * @author  Merlin Kooshmanian
 * @brief   Header file for THERMAL Task
 *
 * @copyright Copyright (c) TOLOSAT 2025
 */

#ifndef THERMAL_H
#define THERMAL_H

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
    temSensorContext_t *p_thermal_context; /**< Pointer to the thermal instance */
} pus178Env_t;

/*************************** Variables Declarations **************************/

/*************************** Functions Declarations **************************/

extern returnCode_t ExecuteS178SS1(void *env, pusTC_t *tc, pusTM_t *tm, pusExecutionError_t *error_code);
extern returnCode_t ExecuteS178SS3(void *env, pusTC_t *tc, pusTM_t *tm, pusExecutionError_t *error_code);

#endif /* THERMAL_H */