/**
 * @file    pus178.h
 * @author  Aldo Lupio
 * @brief   Source file for PUS 178 (Thermal) functions
 *
 * @copyright Copyright (c) TOLOSAT 2025
 */

#ifndef PUS178_H
#define PUS178_H

/******************************* Include Files *******************************/

#include "pus_types.h"
#include "common_types.h"
#include "drv/ds18_drv.h"

/***************************** Macros Definitions ****************************/

/***************************** Types Definitions *****************************/

/**
 * @struct  pus178Env_t
 * @brief   Struct type definition of a PUS178 (Thermal) environment instance
 */
typedef struct
{
    pusStatus_t status;            /**< @brief PUS178 environment status */
    ds18Context_t *p_ds18_context; /**< @brief Pointer to the thermal instance */
} pus178Env_t;

/*************************** Variables Declarations **************************/

/*************************** Functions Declarations **************************/

/**
 * @fn              InitS178(pus178Env_t *pus178_env)
 * @brief           This function initializes the PUS178 context if DS18 is initialized
 * @param[in,out]   pus178_env PUS178 environment
 * @retval          #RET_INVALID_PARAM if pus178_env or p_ds18_context are null pointers
 * @retval          #RET_ERROR if DeviceOpen encountered an error
 * @retval          #RET_SUCCESSFUL else
 */
extern returnCode_t InitS178(pus178Env_t *pus178_env);

/**
 * @fn              ExecuteS178SS1(void *env, pusTC_t *tc, pusTM_t *tm, pusExecutionError_t *error_code)
 * @brief           Function that receives S17SS1 TC (Broadcast) and asks for broadcast temperature
 * @param[in,out]   env PUS178 environment
 * @param[in]       tc S178SS1 TC Format: Ignored
 * @param[out]      tm S178SS2 TM Format: [[TEMP1] - [TEMP2] - ... - [TEMPNB]]
 * @param[out]      error_code Indicates which error has been encountered for S178SS2 TM
 * @retval          #RET_INVALID_PARAM if inputs are not valid
 * @retval          #RET_ERROR if error during sensor communication
 * @retval          #RET_SUCCESSFUL else
 */
extern returnCode_t ExecuteS178SS1(void *env, pusTC_t *tc, pusTM_t *tm, pusExecutionError_t *error_code);

/**
 * @fn              ExecuteS178SS3(void *env, pusTC_t *tc, pusTM_t *tm, pusExecutionError_t *error_code)
 * @brief           Function that receives S17SS3 TC (Unicast), which asks for unicast temperature
 * @param[in,out]   env PUS178 environment
 * @param[in]       tc S178SS3 TC Format: [NB SENSOR ONBOARD] - [ID1] - [ID2] - ... - [ID NB]
 * @param[out]      tm S178SS4 TM Format: [[ID1, TEMP1] - [ID2, TEMP2] - ... - [IDNB, TEMPNB]]
 * @param[out]      error_code Indicates which error has been encountered for S178SS4 TM
 * @retval          #RET_INVALID_PARAM if am expected pointer is NULL
 * @retval          #RET_ERROR if cannot build TM
 * @retval          #RET_SUCCESSFUL else
 */
extern returnCode_t ExecuteS178SS3(void *env, pusTC_t *tc, pusTM_t *tm, pusExecutionError_t *error_code);

#endif /* PUS178_H */