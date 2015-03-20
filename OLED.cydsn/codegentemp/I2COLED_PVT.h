/*******************************************************************************
* File Name: .h
* Version 2.0
*
* Description:
*  This private file provides constants and parameter values for the
*  SCB Component.
*  Please do not use this file or its content in your project.
*
* Note:
*
********************************************************************************
* Copyright 2013-2014, Cypress Semiconductor Corporation. All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_SCB_PVT_I2COLED_H)
#define CY_SCB_PVT_I2COLED_H

#include "I2COLED.h"


/***************************************
*     Private Function Prototypes
***************************************/

/* APIs to service INTR_I2C_EC register */
#define I2COLED_SetI2CExtClkInterruptMode(interruptMask) I2COLED_WRITE_INTR_I2C_EC_MASK(interruptMask)
#define I2COLED_ClearI2CExtClkInterruptSource(interruptMask) I2COLED_CLEAR_INTR_I2C_EC(interruptMask)
#define I2COLED_GetI2CExtClkInterruptSource()                (I2COLED_INTR_I2C_EC_REG)
#define I2COLED_GetI2CExtClkInterruptMode()                  (I2COLED_INTR_I2C_EC_MASK_REG)
#define I2COLED_GetI2CExtClkInterruptSourceMasked()          (I2COLED_INTR_I2C_EC_MASKED_REG)

#if (!I2COLED_CY_SCBIP_V1)
    /* APIs to service INTR_SPI_EC register */
    #define I2COLED_SetSpiExtClkInterruptMode(interruptMask) \
                                                                I2COLED_WRITE_INTR_SPI_EC_MASK(interruptMask)
    #define I2COLED_ClearSpiExtClkInterruptSource(interruptMask) \
                                                                I2COLED_CLEAR_INTR_SPI_EC(interruptMask)
    #define I2COLED_GetExtSpiClkInterruptSource()                 (I2COLED_INTR_SPI_EC_REG)
    #define I2COLED_GetExtSpiClkInterruptMode()                   (I2COLED_INTR_SPI_EC_MASK_REG)
    #define I2COLED_GetExtSpiClkInterruptSourceMasked()           (I2COLED_INTR_SPI_EC_MASKED_REG)
#endif /* (!I2COLED_CY_SCBIP_V1) */

#if(I2COLED_SCB_MODE_UNCONFIG_CONST_CFG)
    extern void I2COLED_SetPins(uint32 mode, uint32 subMode, uint32 uartEnableMask);
#endif /* (I2COLED_SCB_MODE_UNCONFIG_CONST_CFG) */


/***************************************
*     Vars with External Linkage
***************************************/

#if !defined (CY_REMOVE_I2COLED_CUSTOM_INTR_HANDLER)
    extern cyisraddress I2COLED_customIntrHandler;
#endif /* !defined (CY_REMOVE_I2COLED_CUSTOM_INTR_HANDLER) */

extern I2COLED_BACKUP_STRUCT I2COLED_backup;

#if(I2COLED_SCB_MODE_UNCONFIG_CONST_CFG)
    /* Common configuration variables */
    extern uint8 I2COLED_scbMode;
    extern uint8 I2COLED_scbEnableWake;
    extern uint8 I2COLED_scbEnableIntr;

    /* I2C configuration variables */
    extern uint8 I2COLED_mode;
    extern uint8 I2COLED_acceptAddr;

    /* SPI/UART configuration variables */
    extern volatile uint8 * I2COLED_rxBuffer;
    extern uint8   I2COLED_rxDataBits;
    extern uint32  I2COLED_rxBufferSize;

    extern volatile uint8 * I2COLED_txBuffer;
    extern uint8   I2COLED_txDataBits;
    extern uint32  I2COLED_txBufferSize;

    /* EZI2C configuration variables */
    extern uint8 I2COLED_numberOfAddr;
    extern uint8 I2COLED_subAddrSize;
#endif /* (I2COLED_SCB_MODE_UNCONFIG_CONST_CFG) */


/***************************************
*        Conditional Macro
****************************************/

#if(I2COLED_SCB_MODE_UNCONFIG_CONST_CFG)
    /* Defines run time operation mode */
    #define I2COLED_SCB_MODE_I2C_RUNTM_CFG     (I2COLED_SCB_MODE_I2C      == I2COLED_scbMode)
    #define I2COLED_SCB_MODE_SPI_RUNTM_CFG     (I2COLED_SCB_MODE_SPI      == I2COLED_scbMode)
    #define I2COLED_SCB_MODE_UART_RUNTM_CFG    (I2COLED_SCB_MODE_UART     == I2COLED_scbMode)
    #define I2COLED_SCB_MODE_EZI2C_RUNTM_CFG   (I2COLED_SCB_MODE_EZI2C    == I2COLED_scbMode)
    #define I2COLED_SCB_MODE_UNCONFIG_RUNTM_CFG \
                                                        (I2COLED_SCB_MODE_UNCONFIG == I2COLED_scbMode)

    /* Defines wakeup enable */
    #define I2COLED_SCB_WAKE_ENABLE_CHECK       (0u != I2COLED_scbEnableWake)
#endif /* (I2COLED_SCB_MODE_UNCONFIG_CONST_CFG) */

/* Defines maximum number of SCB pins */
#if (!I2COLED_CY_SCBIP_V1)
    #define I2COLED_SCB_PINS_NUMBER    (7u)
#else
    #define I2COLED_SCB_PINS_NUMBER    (2u)
#endif /* (!I2COLED_CY_SCBIP_V1) */

#endif /* (CY_SCB_PVT_I2COLED_H) */


/* [] END OF FILE */
