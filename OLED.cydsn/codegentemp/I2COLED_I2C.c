/*******************************************************************************
* File Name: I2COLED_I2C.c
* Version 2.0
*
* Description:
*  This file provides the source code to the API for the SCB Component in
*  I2C mode.
*
* Note:
*
*******************************************************************************
* Copyright 2013-2014, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "I2COLED_PVT.h"
#include "I2COLED_I2C_PVT.h"


/***************************************
*      I2C Private Vars
***************************************/

volatile uint8 I2COLED_state;  /* Current state of I2C FSM */

#if(I2COLED_SCB_MODE_UNCONFIG_CONST_CFG)

    /***************************************
    *  Configuration Structure Initialization
    ***************************************/

    /* Constant configuration of I2C */
    const I2COLED_I2C_INIT_STRUCT I2COLED_configI2C =
    {
        I2COLED_I2C_MODE,
        I2COLED_I2C_OVS_FACTOR_LOW,
        I2COLED_I2C_OVS_FACTOR_HIGH,
        I2COLED_I2C_MEDIAN_FILTER_ENABLE,
        I2COLED_I2C_SLAVE_ADDRESS,
        I2COLED_I2C_SLAVE_ADDRESS_MASK,
        I2COLED_I2C_ACCEPT_ADDRESS,
        I2COLED_I2C_WAKE_ENABLE,
        I2COLED_I2C_BYTE_MODE_ENABLE,
        I2COLED_I2C_DATA_RATE,
    };

    /*******************************************************************************
    * Function Name: I2COLED_I2CInit
    ********************************************************************************
    *
    * Summary:
    *  Configures the SCB for I2C operation.
    *
    * Parameters:
    *  config:  Pointer to a structure that contains the following ordered list of
    *           fields. These fields match the selections available in the
    *           customizer.
    *
    * Return:
    *  None
    *
    *******************************************************************************/
    void I2COLED_I2CInit(const I2COLED_I2C_INIT_STRUCT *config)
    {
        uint32 medianFilter;
        uint32 locEnableWake;

        if(NULL == config)
        {
            CYASSERT(0u != 0u); /* Halt execution due to bad function parameter */
        }
        else
        {
            /* Configure pins */
            I2COLED_SetPins(I2COLED_SCB_MODE_I2C, I2COLED_DUMMY_PARAM,
                                                                    I2COLED_DUMMY_PARAM);

            /* Store internal configuration */
            I2COLED_scbMode       = (uint8) I2COLED_SCB_MODE_I2C;
            I2COLED_scbEnableWake = (uint8) config->enableWake;
            I2COLED_scbEnableIntr = (uint8) I2COLED_SCB_IRQ_INTERNAL;

            I2COLED_mode          = (uint8) config->mode;
            I2COLED_acceptAddr    = (uint8) config->acceptAddr;

        #if (I2COLED_CY_SCBIP_V0)
            /* Adjust SDA filter settings. Ticket ID#150521 */
            I2COLED_SET_I2C_CFG_SDA_FILT_TRIM(I2COLED_EC_AM_I2C_CFG_SDA_FILT_TRIM);
        #endif /* (I2COLED_CY_SCBIP_V0) */

            /* Adjust AF and DF filter settings. Ticket ID#176179 */
            if (((I2COLED_I2C_MODE_SLAVE != config->mode) &&
                 (config->dataRate <= I2COLED_I2C_DATA_RATE_FS_MODE_MAX)) ||
                 (I2COLED_I2C_MODE_SLAVE == config->mode))
            {
                /* AF = 1, DF = 0 */
                I2COLED_I2C_CFG_ANALOG_FITER_ENABLE;
                medianFilter = I2COLED_DIGITAL_FILTER_DISABLE;
            }
            else
            {
                /* AF = 0, DF = 1 */
                I2COLED_I2C_CFG_ANALOG_FITER_DISABLE;
                medianFilter = I2COLED_DIGITAL_FILTER_ENABLE;
            }

        #if (!I2COLED_CY_SCBIP_V0)
            locEnableWake = (I2COLED_I2C_MULTI_MASTER_SLAVE) ? (0u) : (config->enableWake);
        #else
            locEnableWake = config->enableWake;
        #endif /* (!I2COLED_CY_SCBIP_V0) */

            /* Configure I2C interface */
            I2COLED_CTRL_REG     = I2COLED_GET_CTRL_BYTE_MODE  (config->enableByteMode) |
                                            I2COLED_GET_CTRL_ADDR_ACCEPT(config->acceptAddr)     |
                                            I2COLED_GET_CTRL_EC_AM_MODE (locEnableWake);

            I2COLED_I2C_CTRL_REG = I2COLED_GET_I2C_CTRL_HIGH_PHASE_OVS(config->oversampleHigh) |
                                            I2COLED_GET_I2C_CTRL_LOW_PHASE_OVS (config->oversampleLow)  |
                                            I2COLED_GET_I2C_CTRL_SL_MSTR_MODE  (config->mode)           |
                                            I2COLED_I2C_CTRL;



            /* Configure RX direction */
            I2COLED_RX_CTRL_REG      = I2COLED_GET_RX_CTRL_MEDIAN(medianFilter) |
                                                I2COLED_I2C_RX_CTRL;
            I2COLED_RX_FIFO_CTRL_REG = I2COLED_CLEAR_REG;

            /* Set default address and mask */
            I2COLED_RX_MATCH_REG    = ((I2COLED_I2C_SLAVE) ?
                                                (I2COLED_GET_I2C_8BIT_ADDRESS(config->slaveAddr) |
                                                 I2COLED_GET_RX_MATCH_MASK(config->slaveAddrMask)) :
                                                (I2COLED_CLEAR_REG));


            /* Configure TX direction */
            I2COLED_TX_CTRL_REG      = I2COLED_I2C_TX_CTRL;
            I2COLED_TX_FIFO_CTRL_REG = I2COLED_CLEAR_REG;

            /* Configure interrupt with I2C handler but do not enable it */
            CyIntDisable    (I2COLED_ISR_NUMBER);
            CyIntSetPriority(I2COLED_ISR_NUMBER, I2COLED_ISR_PRIORITY);
            (void) CyIntSetVector(I2COLED_ISR_NUMBER, &I2COLED_I2C_ISR);

            /* Configure interrupt sources */
        #if(!I2COLED_CY_SCBIP_V1)
            I2COLED_INTR_SPI_EC_MASK_REG = I2COLED_NO_INTR_SOURCES;
        #endif /* (!I2COLED_CY_SCBIP_V1) */

            I2COLED_INTR_I2C_EC_MASK_REG = I2COLED_NO_INTR_SOURCES;
            I2COLED_INTR_RX_MASK_REG     = I2COLED_NO_INTR_SOURCES;
            I2COLED_INTR_TX_MASK_REG     = I2COLED_NO_INTR_SOURCES;

            I2COLED_INTR_SLAVE_MASK_REG  = ((I2COLED_I2C_SLAVE) ?
                                                     (I2COLED_I2C_INTR_SLAVE_MASK) :
                                                     (I2COLED_CLEAR_REG));

            I2COLED_INTR_MASTER_MASK_REG = ((I2COLED_I2C_MASTER) ?
                                                     (I2COLED_I2C_INTR_MASTER_MASK) :
                                                     (I2COLED_CLEAR_REG));

            /* Configure global variables */
            I2COLED_state = I2COLED_I2C_FSM_IDLE;

            /* Internal slave variables */
            I2COLED_slStatus        = 0u;
            I2COLED_slRdBufIndex    = 0u;
            I2COLED_slWrBufIndex    = 0u;
            I2COLED_slOverFlowCount = 0u;

            /* Internal master variables */
            I2COLED_mstrStatus     = 0u;
            I2COLED_mstrRdBufIndex = 0u;
            I2COLED_mstrWrBufIndex = 0u;
        }
    }

#else

    /*******************************************************************************
    * Function Name: I2COLED_I2CInit
    ********************************************************************************
    *
    * Summary:
    *  Configures the SCB for the I2C operation.
    *
    * Parameters:
    *  None
    *
    * Return:
    *  None
    *
    *******************************************************************************/
    void I2COLED_I2CInit(void)
    {
    #if(I2COLED_CY_SCBIP_V0)
        /* Adjust SDA filter settings. Ticket ID#150521 */
        I2COLED_SET_I2C_CFG_SDA_FILT_TRIM(I2COLED_EC_AM_I2C_CFG_SDA_FILT_TRIM);
    #endif /* (I2COLED_CY_SCBIP_V0) */

        /* Adjust AF and DF filter settings. Ticket ID#176179 */
        I2COLED_I2C_CFG_ANALOG_FITER_ENABLE_ADJ;

        /* Configure I2C interface */
        I2COLED_CTRL_REG     = I2COLED_I2C_DEFAULT_CTRL;
        I2COLED_I2C_CTRL_REG = I2COLED_I2C_DEFAULT_I2C_CTRL;

        /* Configure RX direction */
        I2COLED_RX_CTRL_REG      = I2COLED_I2C_DEFAULT_RX_CTRL;
        I2COLED_RX_FIFO_CTRL_REG = I2COLED_I2C_DEFAULT_RX_FIFO_CTRL;

        /* Set default address and mask */
        I2COLED_RX_MATCH_REG     = I2COLED_I2C_DEFAULT_RX_MATCH;

        /* Configure TX direction */
        I2COLED_TX_CTRL_REG      = I2COLED_I2C_DEFAULT_TX_CTRL;
        I2COLED_TX_FIFO_CTRL_REG = I2COLED_I2C_DEFAULT_TX_FIFO_CTRL;

        /* Configure interrupt with I2C handler but do not enable it */
        CyIntDisable    (I2COLED_ISR_NUMBER);
        CyIntSetPriority(I2COLED_ISR_NUMBER, I2COLED_ISR_PRIORITY);
    #if(!I2COLED_I2C_EXTERN_INTR_HANDLER)
        (void) CyIntSetVector(I2COLED_ISR_NUMBER, &I2COLED_I2C_ISR);
    #endif /* (I2COLED_I2C_EXTERN_INTR_HANDLER) */

        /* Configure interrupt sources */
    #if(!I2COLED_CY_SCBIP_V1)
        I2COLED_INTR_SPI_EC_MASK_REG = I2COLED_I2C_DEFAULT_INTR_SPI_EC_MASK;
    #endif /* (!I2COLED_CY_SCBIP_V1) */

        I2COLED_INTR_I2C_EC_MASK_REG = I2COLED_I2C_DEFAULT_INTR_I2C_EC_MASK;
        I2COLED_INTR_SLAVE_MASK_REG  = I2COLED_I2C_DEFAULT_INTR_SLAVE_MASK;
        I2COLED_INTR_MASTER_MASK_REG = I2COLED_I2C_DEFAULT_INTR_MASTER_MASK;
        I2COLED_INTR_RX_MASK_REG     = I2COLED_I2C_DEFAULT_INTR_RX_MASK;
        I2COLED_INTR_TX_MASK_REG     = I2COLED_I2C_DEFAULT_INTR_TX_MASK;

        /* Configure global variables */
        I2COLED_state = I2COLED_I2C_FSM_IDLE;

    #if(I2COLED_I2C_SLAVE)
        /* Internal slave variable */
        I2COLED_slStatus        = 0u;
        I2COLED_slRdBufIndex    = 0u;
        I2COLED_slWrBufIndex    = 0u;
        I2COLED_slOverFlowCount = 0u;
    #endif /* (I2COLED_I2C_SLAVE) */

    #if(I2COLED_I2C_MASTER)
    /* Internal master variable */
        I2COLED_mstrStatus     = 0u;
        I2COLED_mstrRdBufIndex = 0u;
        I2COLED_mstrWrBufIndex = 0u;
    #endif /* (I2COLED_I2C_MASTER) */
    }
#endif /* (I2COLED_SCB_MODE_UNCONFIG_CONST_CFG) */


/*******************************************************************************
* Function Name: I2COLED_I2CStop
********************************************************************************
*
* Summary:
*  Resets the I2C FSM into the default state and disables TX interrupt sources.
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
void I2COLED_I2CStop(void)
{
    /* Disable TX interrupt sources in order not to cause a false trigger.
    * The incoming transaction will enable an appropriate TX interrupt.
    */
    I2COLED_SetTxInterruptMode(I2COLED_NO_INTR_SOURCES);

#if(I2COLED_CY_SCBIP_V0)
    /* Clear a pending interrupt as the TX FIFO becomes empty and the block does not gate
    * an interrupt trigger when disabled.
    */
    I2COLED_ClearPendingInt();
#endif /* (I2COLED_CY_SCBIP_V0) */

    I2COLED_state = I2COLED_I2C_FSM_IDLE;
}


#if(I2COLED_I2C_WAKE_ENABLE_CONST)
    /*******************************************************************************
    * Function Name: I2COLED_I2CSaveConfig
    ********************************************************************************
    *
    * Summary:
    *  Enables I2COLED_INTR_I2C_EC_WAKE_UP interrupt source. This interrupt
    *  triggers on address match and wakes up device.
    *
    * Parameters:
    *  None
    *
    * Return:
    *  None
    *
    *******************************************************************************/
    void I2COLED_I2CSaveConfig(void)
    {
    #if (!I2COLED_CY_SCBIP_V0)
        #if (I2COLED_I2C_MULTI_MASTER_SLAVE_CONST && I2COLED_I2C_WAKE_ENABLE_CONST)
            /* Enable externally clocked address match if it was not enabled before.
            * This applicable only for Multi-Master-Slave. Ticket ID#192742 */
            if (0u == (I2COLED_CTRL_REG & I2COLED_CTRL_EC_AM_MODE))
            {
                /* Enable external address match logic */
                I2COLED_Stop();
                I2COLED_CTRL_REG |= I2COLED_CTRL_EC_AM_MODE;
                I2COLED_Enable();
            }
        #endif /* (I2COLED_I2C_MULTI_MASTER_SLAVE_CONST) */

        #if (I2COLED_SCB_CLK_INTERNAL)
            /* Disable clock to internal address match logic. Ticket ID#187931 */
            I2COLED_SCBCLK_Stop();
        #endif /* (I2COLED_SCB_CLK_INTERNAL) */
    #endif /* (!I2COLED_CY_SCBIP_V0) */

        I2COLED_SetI2CExtClkInterruptMode(I2COLED_INTR_I2C_EC_WAKE_UP);
    }


    /*******************************************************************************
    * Function Name: I2COLED_I2CRestoreConfig
    ********************************************************************************
    *
    * Summary:
    *  Disables I2COLED_INTR_I2C_EC_WAKE_UP interrupt source. This interrupt
    *  triggers on address match and wakes up device.
    *
    * Parameters:
    *  None
    *
    * Return:
    *  None
    *
    *******************************************************************************/
    void I2COLED_I2CRestoreConfig(void)
    {
        /* Disable wakeup interrupt on address match */
        I2COLED_SetI2CExtClkInterruptMode(I2COLED_NO_INTR_SOURCES);

    #if (!I2COLED_CY_SCBIP_V0)
        #if (I2COLED_SCB_CLK_INTERNAL)
            /* Enable clock to internal address match logic. Ticket ID#187931 */
            I2COLED_SCBCLK_Start();
        #endif /* (I2COLED_SCB_CLK_INTERNAL) */
    #endif /* (!I2COLED_CY_SCBIP_V0) */
    }
#endif /* (I2COLED_I2C_WAKE_ENABLE_CONST) */


/* [] END OF FILE */
