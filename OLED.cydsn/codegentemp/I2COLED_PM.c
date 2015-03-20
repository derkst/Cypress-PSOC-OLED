/*******************************************************************************
* File Name: I2COLED_PM.c
* Version 2.0
*
* Description:
*  This file provides the source code to the Power Management support for
*  the SCB Component.
*
* Note:
*
********************************************************************************
* Copyright 2013-2014, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "I2COLED.h"
#include "I2COLED_PVT.h"

#if(I2COLED_SCB_MODE_I2C_INC)
    #include "I2COLED_I2C_PVT.h"
#endif /* (I2COLED_SCB_MODE_I2C_INC) */

#if(I2COLED_SCB_MODE_EZI2C_INC)
    #include "I2COLED_EZI2C_PVT.h"
#endif /* (I2COLED_SCB_MODE_EZI2C_INC) */

#if(I2COLED_SCB_MODE_SPI_INC || I2COLED_SCB_MODE_UART_INC)
    #include "I2COLED_SPI_UART_PVT.h"
#endif /* (I2COLED_SCB_MODE_SPI_INC || I2COLED_SCB_MODE_UART_INC) */


/***************************************
*   Backup Structure declaration
***************************************/

#if(I2COLED_SCB_MODE_UNCONFIG_CONST_CFG || \
   (I2COLED_SCB_MODE_I2C_CONST_CFG   && (!I2COLED_I2C_WAKE_ENABLE_CONST))   || \
   (I2COLED_SCB_MODE_EZI2C_CONST_CFG && (!I2COLED_EZI2C_WAKE_ENABLE_CONST)) || \
   (I2COLED_SCB_MODE_SPI_CONST_CFG   && (!I2COLED_SPI_WAKE_ENABLE_CONST))   || \
   (I2COLED_SCB_MODE_UART_CONST_CFG  && (!I2COLED_UART_WAKE_ENABLE_CONST)))

    I2COLED_BACKUP_STRUCT I2COLED_backup =
    {
        0u, /* enableState */
    };
#endif


/*******************************************************************************
* Function Name: I2COLED_Sleep
********************************************************************************
*
* Summary:
*  Prepares the component to enter Deep Sleep.
*  The "Enable wakeup from Sleep Mode" selection has an influence on
*  this function implementation.
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
void I2COLED_Sleep(void)
{
#if(I2COLED_SCB_MODE_UNCONFIG_CONST_CFG)

    if(I2COLED_SCB_WAKE_ENABLE_CHECK)
    {
        if(I2COLED_SCB_MODE_I2C_RUNTM_CFG)
        {
            I2COLED_I2CSaveConfig();
        }
        else if(I2COLED_SCB_MODE_EZI2C_RUNTM_CFG)
        {
            I2COLED_EzI2CSaveConfig();
        }
    #if(!I2COLED_CY_SCBIP_V1)
        else if(I2COLED_SCB_MODE_SPI_RUNTM_CFG)
        {
            I2COLED_SpiSaveConfig();
        }
        else if(I2COLED_SCB_MODE_UART_RUNTM_CFG)
        {
            I2COLED_UartSaveConfig();
        }
    #endif /* (!I2COLED_CY_SCBIP_V1) */
        else
        {
            /* Unknown mode */
        }
    }
    else
    {
        I2COLED_backup.enableState = (uint8) I2COLED_GET_CTRL_ENABLED;

        if(0u != I2COLED_backup.enableState)
        {
            I2COLED_Stop();
        }
    }

#else

    #if (I2COLED_SCB_MODE_I2C_CONST_CFG && I2COLED_I2C_WAKE_ENABLE_CONST)
        I2COLED_I2CSaveConfig();

    #elif (I2COLED_SCB_MODE_EZI2C_CONST_CFG && I2COLED_EZI2C_WAKE_ENABLE_CONST)
        I2COLED_EzI2CSaveConfig();

    #elif (I2COLED_SCB_MODE_SPI_CONST_CFG && I2COLED_SPI_WAKE_ENABLE_CONST)
        I2COLED_SpiSaveConfig();

    #elif (I2COLED_SCB_MODE_UART_CONST_CFG && I2COLED_UART_WAKE_ENABLE_CONST)
        I2COLED_UartSaveConfig();

    #else

        I2COLED_backup.enableState = (uint8) I2COLED_GET_CTRL_ENABLED;

        if(0u != I2COLED_backup.enableState)
        {
            I2COLED_Stop();
        }

    #endif /* defined (I2COLED_SCB_MODE_I2C_CONST_CFG) && (I2COLED_I2C_WAKE_ENABLE_CONST) */

#endif /* (I2COLED_SCB_MODE_UNCONFIG_CONST_CFG) */
}


/*******************************************************************************
* Function Name: I2COLED_Wakeup
********************************************************************************
*
* Summary:
*  Prepares the component for the Active mode operation after exiting
*  Deep Sleep. The "Enable wakeup from Sleep Mode" option has an influence
*  on this function implementation.
*  This function should not be called after exiting Sleep.
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
void I2COLED_Wakeup(void)
{
#if(I2COLED_SCB_MODE_UNCONFIG_CONST_CFG)

    if(I2COLED_SCB_WAKE_ENABLE_CHECK)
    {
        if(I2COLED_SCB_MODE_I2C_RUNTM_CFG)
        {
            I2COLED_I2CRestoreConfig();
        }
        else if(I2COLED_SCB_MODE_EZI2C_RUNTM_CFG)
        {
            I2COLED_EzI2CRestoreConfig();
        }
    #if(!I2COLED_CY_SCBIP_V1)
        else if(I2COLED_SCB_MODE_SPI_RUNTM_CFG)
        {
            I2COLED_SpiRestoreConfig();
        }
        else if(I2COLED_SCB_MODE_UART_RUNTM_CFG)
        {
            I2COLED_UartRestoreConfig();
        }
    #endif /* (!I2COLED_CY_SCBIP_V1) */
        else
        {
            /* Unknown mode */
        }
    }
    else
    {
        if(0u != I2COLED_backup.enableState)
        {
            I2COLED_Enable();
        }
    }

#else

    #if (I2COLED_SCB_MODE_I2C_CONST_CFG  && I2COLED_I2C_WAKE_ENABLE_CONST)
        I2COLED_I2CRestoreConfig();

    #elif (I2COLED_SCB_MODE_EZI2C_CONST_CFG && I2COLED_EZI2C_WAKE_ENABLE_CONST)
        I2COLED_EzI2CRestoreConfig();

    #elif (I2COLED_SCB_MODE_SPI_CONST_CFG && I2COLED_SPI_WAKE_ENABLE_CONST)
        I2COLED_SpiRestoreConfig();

    #elif (I2COLED_SCB_MODE_UART_CONST_CFG && I2COLED_UART_WAKE_ENABLE_CONST)
        I2COLED_UartRestoreConfig();

    #else

        if(0u != I2COLED_backup.enableState)
        {
            I2COLED_Enable();
        }

    #endif /* (I2COLED_I2C_WAKE_ENABLE_CONST) */

#endif /* (I2COLED_SCB_MODE_UNCONFIG_CONST_CFG) */
}


/* [] END OF FILE */
