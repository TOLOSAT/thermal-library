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

/*************************** Functions Declarations **************************/

/*************************** Variables Definitions ***************************/

/*************************** Functions Definitions ***************************/

/**
 * @fn              ExecuteS178SS1(void *env, pusTC_t *tc, pusTM_t *tm, pusExecutionError_t *error_code)
 * @brief           Function that receives S17SS1 TC (Broadcast), asks for temperature and responds back with TM
 * @param[in,out]   env PUS178 environment
 * @param[in]       tc S178SS1 TC
 * @param[out]      tm S178SS2 TM that we will send with temperature env information
 * @param[out]      error_code Indicates which error has been encountered for S178SS2 TM
 * @retval          #RET_INVALID_PARAM if a pointer is NULL
 * @retval          #RET_ERROR if cannot build TM
 * @retval          #RET_SUCCESSFUL else
 */
returnCode_t ExecuteS178SS1(void *env, pusTC_t *tc, pusTM_t *tm, pusExecutionError_t *error_code)
{
    returnCode_t return_value = RET_SUCCESSFUL;

    // Check parameter(s)
    if ((env != NULL) && (error_code != NULL) && (tm != NULL))
    {
        // Get pus178 environment (context with sensor data)
        pus178Env_t *pus178_env = (pus178Env_t *)env;

        if (pus178_env->p_thermal_context != NULL)
        {
            // Pointer to environment exists
            *error_code = PUS_EXECUTION_NO_ERROR;

            // With environment data, demand broadcast temperature conversion
        }
        else
        {
            return_value = RET_INVALID_PARAM;
        }

        // Error code Initialization
        *error_code = PUS_EXECUTION_NO_ERROR;

        // Build PUS178 SS2 TM (Broadcast data)
        returnCode_t test_build = BuildS178SS2(tm, &env);
        if (test_build != RET_SUCCESSFUL)
        {
            return_value = RET_ERROR;
            *error_code  = PUS_EXECUTION_TM_BUILDING_FAILED;
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
 * @brief           Function that send S178SS3 TC (Unicast), asks for temperature and responds back with TM
 * @param[in,out]   env PUS178 environment
 * @param[in]       tc S178SS3 TC
 * @param[out]      tm S178SS4 TM that we will send
 * @param[out]      error_code Indicates which error has been encountered for S1SS8 TM
 * @retval          #RET_INVALID_PARAM if a pointer is NULL
 * @retval          #RET_ERROR if cannot build TM
 * @retval          #RET_SUCCESSFUL else
 */
extern returnCode_t ExecuteS178SS3(void *env, pusTC_t *tc, pusTM_t *tm, pusExecutionError_t *error_code)
{
    returnCode_t return_value = RET_SUCCESSFUL;

    // Unused Parameters
    (void)(tc);

    // Check parameter(s)
    if ((tm != NULL) && (error_code != NULL))
    {
        // Error code Initialization
        *error_code = PUS_EXECUTION_NO_ERROR;

        // Build PUS178 SS4 TM (Unicast data)
        returnCode_t test_build = BuildS178SS4(tm, &env);
        if (test_build != RET_SUCCESSFUL)
        {
            return_value = RET_ERROR;
            *error_code  = PUS_EXECUTION_TM_BUILDING_FAILED;
        }
    }
    else
    {
        return_value = RET_INVALID_PARAM;
    }

    return return_value;
}