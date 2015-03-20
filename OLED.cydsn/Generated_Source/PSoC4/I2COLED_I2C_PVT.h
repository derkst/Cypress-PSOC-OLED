/*******************************************************************************
* File Name: .h
* Version 2.0
*
* Description:
*  This private file provides constants and parameter values for the
*  SCB Component in I2C mode.
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

#if !defined(CY_SCB_I2C_PVT_I2COLED_H)
#define CY_SCB_I2C_PVT_I2COLED_H

#include "I2COLED_I2C.h"


/***************************************
*     Private Global Vars
***************************************/

extern volatile uint8 I2COLED_state; /* Current state of I2C FSM */

#if(I2COLED_I2C_SLAVE_CONST)
    extern volatile uint8 I2COLED_slStatus;          /* Slave Status */

    /* Receive buffer variables */
    extern volatile uint8 * I2COLED_slWrBufPtr;      /* Pointer to Receive buffer  */
    extern volatile uint32  I2COLED_slWrBufSize;     /* Slave Receive buffer size  */
    extern volatile uint32  I2COLED_slWrBufIndex;    /* Slave Receive buffer Index */

    /* Transmit buffer variables */
    extern volatile uint8 * I2COLED_slRdBufPtr;      /* Pointer to Transmit buffer  */
    extern volatile uint32  I2COLED_slRdBufSize;     /* Slave Transmit buffer size  */
    extern volatile uint32  I2COLED_slRdBufIndex;    /* Slave Transmit buffer Index */
    extern volatile uint32  I2COLED_slRdBufIndexTmp; /* Slave Transmit buffer Index Tmp */
    extern volatile uint8   I2COLED_slOverFlowCount; /* Slave Transmit Overflow counter */
#endif /* (I2COLED_I2C_SLAVE_CONST) */

#if(I2COLED_I2C_MASTER_CONST)
    extern volatile uint16 I2COLED_mstrStatus;      /* Master Status byte  */
    extern volatile uint8  I2COLED_mstrControl;     /* Master Control byte */

    /* Receive buffer variables */
    extern volatile uint8 * I2COLED_mstrRdBufPtr;   /* Pointer to Master Read buffer */
    extern volatile uint32  I2COLED_mstrRdBufSize;  /* Master Read buffer size       */
    extern volatile uint32  I2COLED_mstrRdBufIndex; /* Master Read buffer Index      */

    /* Transmit buffer variables */
    extern volatile uint8 * I2COLED_mstrWrBufPtr;   /* Pointer to Master Write buffer */
    extern volatile uint32  I2COLED_mstrWrBufSize;  /* Master Write buffer size       */
    extern volatile uint32  I2COLED_mstrWrBufIndex; /* Master Write buffer Index      */
    extern volatile uint32  I2COLED_mstrWrBufIndexTmp; /* Master Write buffer Index Tmp */
#endif /* (I2COLED_I2C_MASTER_CONST) */


/***************************************
*     Private Function Prototypes
***************************************/

#if(I2COLED_SCB_MODE_I2C_CONST_CFG)
    void I2COLED_I2CInit(void);
#endif /* (I2COLED_SCB_MODE_I2C_CONST_CFG) */

void I2COLED_I2CStop(void);
void I2COLED_I2CSaveConfig(void);
void I2COLED_I2CRestoreConfig(void);

#if(I2COLED_I2C_MASTER_CONST)
    void I2COLED_I2CReStartGeneration(void);
#endif /* (I2COLED_I2C_MASTER_CONST) */

#endif /* (CY_SCB_I2C_PVT_I2COLED_H) */


/* [] END OF FILE */
