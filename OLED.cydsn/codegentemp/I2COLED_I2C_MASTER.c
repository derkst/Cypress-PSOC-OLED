/*******************************************************************************
* File Name: I2COLED_I2C_MASTER.c
* Version 2.0
*
* Description:
*  This file provides the source code to the API for the SCB Component in
*  I2C Master mode.
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

#if(I2COLED_I2C_MASTER_CONST)

/***************************************
*      I2C Master Private Vars
***************************************/

/* Master variables */
volatile uint16 I2COLED_mstrStatus;      /* Master Status byte  */
volatile uint8  I2COLED_mstrControl;     /* Master Control byte */

/* Receive buffer variables */
volatile uint8 * I2COLED_mstrRdBufPtr;   /* Pointer to Master Read buffer */
volatile uint32  I2COLED_mstrRdBufSize;  /* Master Read buffer size       */
volatile uint32  I2COLED_mstrRdBufIndex; /* Master Read buffer Index      */

/* Transmit buffer variables */
volatile uint8 * I2COLED_mstrWrBufPtr;   /* Pointer to Master Write buffer */
volatile uint32  I2COLED_mstrWrBufSize;  /* Master Write buffer size       */
volatile uint32  I2COLED_mstrWrBufIndex; /* Master Write buffer Index      */
volatile uint32  I2COLED_mstrWrBufIndexTmp; /* Master Write buffer Index Tmp */

#if (!I2COLED_CY_SCBIP_V0 && \
    I2COLED_I2C_MULTI_MASTER_SLAVE_CONST && I2COLED_I2C_WAKE_ENABLE_CONST)
    static void I2COLED_I2CMasterDisableEcAm(void);
#endif /* (!I2COLED_CY_SCBIP_V0) */


/*******************************************************************************
* Function Name: I2COLED_I2CMasterWriteBuf
********************************************************************************
*
* Summary:
* Automatically writes an entire buffer of data to a slave device.
* Once the data transfer is initiated by this function, further data transfer
* is handled by the included ISR.
* Enables the I2C interrupt and clears SCB_ I2C_MSTAT_WR_CMPLT status.
*
* Parameters:
*  slaveAddr: 7-bit slave address.
*  xferData:  Pointer to buffer of data to be sent.
*  cnt:       Size of buffer to send.
*  mode:      Transfer mode defines: start or restart condition generation at
*             begin of the transfer and complete the transfer or halt before
*             generating a stop.
*
* Return:
*  Error status.
*
* Global variables:
*  I2COLED_mstrStatus  - used to store current status of I2C Master.
*  I2COLED_state       - used to store current state of software FSM.
*  I2COLED_mstrControl - used to control master end of transaction with
*  or without the Stop generation.
*  I2COLED_mstrWrBufPtr - used to store pointer to master write buffer.
*  I2COLED_mstrWrBufIndex - used to current index within master write
*  buffer.
*  I2COLED_mstrWrBufSize - used to store master write buffer size.
*
*******************************************************************************/
uint32 I2COLED_I2CMasterWriteBuf(uint32 slaveAddress, uint8 * wrData, uint32 cnt, uint32 mode)
{
    uint32 errStatus;

    errStatus = I2COLED_I2C_MSTR_NOT_READY;

    if(NULL != wrData)  /* Check buffer pointer */
    {
        /* Check FSM state and bus before generating Start/ReStart condition */
        if(I2COLED_CHECK_I2C_FSM_IDLE)
        {
            I2COLED_DisableInt();  /* Lock from interruption */

            /* Check bus state */
            errStatus = I2COLED_CHECK_I2C_STATUS(I2COLED_I2C_STATUS_BUS_BUSY) ?
                            I2COLED_I2C_MSTR_BUS_BUSY : I2COLED_I2C_MSTR_NO_ERROR;
        }
        else if(I2COLED_CHECK_I2C_FSM_HALT)
        {
            I2COLED_mstrStatus &= (uint16) ~I2COLED_I2C_MSTAT_XFER_HALT;
                              errStatus  = I2COLED_I2C_MSTR_NO_ERROR;
        }
        else
        {
            /* Unexpected FSM state: exit */
        }
    }

    /* Check if master is ready to start  */
    if(I2COLED_I2C_MSTR_NO_ERROR == errStatus) /* No error proceed */
    {
    #if (!I2COLED_CY_SCBIP_V0 && \
        I2COLED_I2C_MULTI_MASTER_SLAVE_CONST && I2COLED_I2C_WAKE_ENABLE_CONST)
            I2COLED_I2CMasterDisableEcAm();
    #endif /* (!I2COLED_CY_SCBIP_V0) */

        /* Set up write transaction */
        I2COLED_state = I2COLED_I2C_FSM_MSTR_WR_ADDR;
        I2COLED_mstrWrBufIndexTmp = 0u;
        I2COLED_mstrWrBufIndex    = 0u;
        I2COLED_mstrWrBufSize     = cnt;
        I2COLED_mstrWrBufPtr      = (volatile uint8 *) wrData;
        I2COLED_mstrControl       = (uint8) mode;

        slaveAddress = I2COLED_GET_I2C_8BIT_ADDRESS(slaveAddress);

        I2COLED_mstrStatus &= (uint16) ~I2COLED_I2C_MSTAT_WR_CMPLT;

        I2COLED_ClearMasterInterruptSource(I2COLED_INTR_MASTER_ALL);
        I2COLED_ClearTxInterruptSource(I2COLED_INTR_TX_UNDERFLOW);
        /* The TX and RX FIFO have to be EMPTY */

        /* Generate Start or ReStart */
        if(I2COLED_CHECK_I2C_MODE_RESTART(mode))
        {
            I2COLED_I2C_MASTER_GENERATE_RESTART;
            I2COLED_TX_FIFO_WR_REG = slaveAddress;
        }
        else
        {
            I2COLED_TX_FIFO_WR_REG = slaveAddress;
            I2COLED_I2C_MASTER_GENERATE_START;
        }

         /* Catch when address is sent */
        I2COLED_SetTxInterruptMode(I2COLED_INTR_TX_UNDERFLOW);
    }

    I2COLED_EnableInt();   /* Release lock */

    return(errStatus);
}


/*******************************************************************************
* Function Name: I2COLED_I2CMasterReadBuf
********************************************************************************
*
* Summary:
*  Automatically reads an entire buffer of data from a slave device.
*  Once the data transfer is initiated by this function, further data transfer
*  is handled by the included ISR.
* Enables the I2C interrupt and clears SCB_ I2C_MSTAT_RD_CMPLT status.
*
* Parameters:
*  slaveAddr: 7-bit slave address.
*  xferData:  Pointer to buffer where to put data from slave.
*  cnt:       Size of buffer to read.
*  mode:      Transfer mode defines: start or restart condition generation at
*             begin of the transfer and complete the transfer or halt before
*             generating a stop.
*
* Return:
*  Error status.
*
* Global variables:
*  I2COLED_mstrStatus  - used to store current status of I2C Master.
*  I2COLED_state       - used to store current state of software FSM.
*  I2COLED_mstrControl - used to control master end of transaction with
*  or without the Stop generation.
*  I2COLED_mstrRdBufPtr - used to store pointer to master write buffer.
*  I2COLED_mstrRdBufIndex - used to current index within master write
*  buffer.
*  I2COLED_mstrRdBufSize - used to store master write buffer size.
*
*******************************************************************************/
uint32 I2COLED_I2CMasterReadBuf(uint32 slaveAddress, uint8 * rdData, uint32 cnt, uint32 mode)
{
    uint32 errStatus;

    errStatus = I2COLED_I2C_MSTR_NOT_READY;

    if(NULL != rdData)
    {
        /* Check FSM state and bus before generating Start/ReStart condition */
        if(I2COLED_CHECK_I2C_FSM_IDLE)
        {
            I2COLED_DisableInt();  /* Lock from interruption */

            /* Check bus state */
            errStatus = I2COLED_CHECK_I2C_STATUS(I2COLED_I2C_STATUS_BUS_BUSY) ?
                            I2COLED_I2C_MSTR_BUS_BUSY : I2COLED_I2C_MSTR_NO_ERROR;
        }
        else if(I2COLED_CHECK_I2C_FSM_HALT)
        {
            I2COLED_mstrStatus &= (uint16) ~I2COLED_I2C_MSTAT_XFER_HALT;
                              errStatus  =  I2COLED_I2C_MSTR_NO_ERROR;
        }
        else
        {
            /* Unexpected FSM state: exit */
        }
    }

    /* Check master ready to proceed */
    if(I2COLED_I2C_MSTR_NO_ERROR == errStatus) /* No error proceed */
    {
        #if (!I2COLED_CY_SCBIP_V0 && \
        I2COLED_I2C_MULTI_MASTER_SLAVE_CONST && I2COLED_I2C_WAKE_ENABLE_CONST)
            I2COLED_I2CMasterDisableEcAm();
        #endif /* (!I2COLED_CY_SCBIP_V0) */

        /* Set up read transaction */
        I2COLED_state = I2COLED_I2C_FSM_MSTR_RD_ADDR;
        I2COLED_mstrRdBufIndex = 0u;
        I2COLED_mstrRdBufSize  = cnt;
        I2COLED_mstrRdBufPtr   = (volatile uint8 *) rdData;
        I2COLED_mstrControl    = (uint8) mode;

        slaveAddress = (I2COLED_GET_I2C_8BIT_ADDRESS(slaveAddress) | I2COLED_I2C_READ_FLAG);

        I2COLED_mstrStatus &= (uint16) ~I2COLED_I2C_MSTAT_RD_CMPLT;

        I2COLED_ClearMasterInterruptSource(I2COLED_INTR_MASTER_ALL);

        /* TX and RX FIFO have to be EMPTY */

        /* Generate Start or ReStart */
        if(I2COLED_CHECK_I2C_MODE_RESTART(mode))
        {
            I2COLED_I2C_MASTER_GENERATE_RESTART;
            I2COLED_TX_FIFO_WR_REG = (slaveAddress);
        }
        else
        {
            I2COLED_TX_FIFO_WR_REG = (slaveAddress);
            I2COLED_I2C_MASTER_GENERATE_START;
        }

        /* Prepare reading */
        if(I2COLED_mstrRdBufSize < I2COLED_I2C_FIFO_SIZE) /* Reading byte-by-byte */
        {
            I2COLED_SetRxInterruptMode(I2COLED_INTR_RX_NOT_EMPTY);
        }
        else /* Receive RX FIFO chunks */
        {
            I2COLED_ENABLE_MASTER_AUTO_DATA_ACK;
            I2COLED_SetRxInterruptMode(I2COLED_INTR_RX_FULL);
        }
    }

    I2COLED_EnableInt();   /* Release lock */

    return(errStatus);
}


/*******************************************************************************
* Function Name: I2COLED_I2CMasterSendStart
********************************************************************************
*
* Summary:
*  Generates Start condition and sends slave address with read/write bit.
*  Disables the I2C interrupt.
*  This function is blocking and does not return until start condition and
*  address byte are sent and ACK/NACK response is received or errors occurred.
*
* Parameters:
*  slaveAddress: Right justified 7-bit Slave address (valid range 8 to 120).
*  bitRnW:       Direction of the following transfer. It is defined by
*                read/write bit within address byte.
*
* Return:
*  Erorr status.
*
* Global variables:
*  I2COLED_state - used to store current state of software FSM.
*
*******************************************************************************/
uint32 I2COLED_I2CMasterSendStart(uint32 slaveAddress, uint32 bitRnW)
{
    uint32  resetIp;
    uint32 errStatus;

    resetIp   = 0u;
    errStatus = I2COLED_I2C_MSTR_NOT_READY;

    /* Check FSM state before generating Start condition */
    if(I2COLED_CHECK_I2C_FSM_IDLE)
    {
        /* If bus is free, generate Start condition */
        if(I2COLED_CHECK_I2C_STATUS(I2COLED_I2C_STATUS_BUS_BUSY))
        {
            errStatus = I2COLED_I2C_MSTR_BUS_BUSY;
        }
        else
        {
            I2COLED_DisableInt();  /* Lock from interruption */

        #if (!I2COLED_CY_SCBIP_V0 && \
            I2COLED_I2C_MULTI_MASTER_SLAVE_CONST && I2COLED_I2C_WAKE_ENABLE_CONST)
            I2COLED_I2CMasterDisableEcAm();
        #endif /* (!I2COLED_CY_SCBIP_V0) */

            slaveAddress = I2COLED_GET_I2C_8BIT_ADDRESS(slaveAddress);

            if(0u == bitRnW) /* Write direction */
            {
                I2COLED_state = I2COLED_I2C_FSM_MSTR_WR_DATA;
            }
            else /* Read direction */
            {
                I2COLED_state = I2COLED_I2C_FSM_MSTR_RD_DATA;
                         slaveAddress |= I2COLED_I2C_READ_FLAG;
            }

            /* TX and RX FIFO have to be EMPTY */

            I2COLED_TX_FIFO_WR_REG = slaveAddress; /* Put address in TX FIFO */
            I2COLED_ClearMasterInterruptSource(I2COLED_INTR_MASTER_ALL);

            I2COLED_I2C_MASTER_GENERATE_START;


            while(!I2COLED_CHECK_INTR_MASTER(I2COLED_INTR_MASTER_I2C_ACK      |
                                                      I2COLED_INTR_MASTER_I2C_NACK     |
                                                      I2COLED_INTR_MASTER_I2C_ARB_LOST |
                                                      I2COLED_INTR_MASTER_I2C_BUS_ERROR))
            {
                /*
                * Write: wait until address has been transferred
                * Read : wait until address has been transferred, data byte is going to RX FIFO as well.
                */
            }

            /* Check the results of the address phase */
            if(I2COLED_CHECK_INTR_MASTER(I2COLED_INTR_MASTER_I2C_ACK))
            {
                errStatus = I2COLED_I2C_MSTR_NO_ERROR;
            }
            else if(I2COLED_CHECK_INTR_MASTER(I2COLED_INTR_MASTER_I2C_NACK))
            {
                errStatus = I2COLED_I2C_MSTR_ERR_LB_NAK;
            }
            else if(I2COLED_CHECK_INTR_MASTER(I2COLED_INTR_MASTER_I2C_ARB_LOST))
            {
                I2COLED_state = I2COLED_I2C_FSM_IDLE;
                             errStatus = I2COLED_I2C_MSTR_ERR_ARB_LOST;
                             resetIp   = I2COLED_I2C_RESET_ERROR;
            }
            else /* I2COLED_INTR_MASTER_I2C_BUS_ERROR set is else condition */
            {
                I2COLED_state = I2COLED_I2C_FSM_IDLE;
                             errStatus = I2COLED_I2C_MSTR_ERR_BUS_ERR;
                             resetIp   = I2COLED_I2C_RESET_ERROR;
            }

            I2COLED_ClearMasterInterruptSource(I2COLED_INTR_MASTER_I2C_ACK      |
                                                        I2COLED_INTR_MASTER_I2C_NACK     |
                                                        I2COLED_INTR_MASTER_I2C_ARB_LOST |
                                                        I2COLED_INTR_MASTER_I2C_BUS_ERROR);

            /* Reset block in case of: LOST_ARB or BUS_ERR */
            if(0u != resetIp)
            {
                I2COLED_SCB_SW_RESET;
            }
        }
    }

    return(errStatus);
}


/*******************************************************************************
* Function Name: I2COLED_I2CMasterSendRestart
********************************************************************************
*
* Summary:
*  Generates Restart condition and sends slave address with read/write bit.
*  This function is blocking and does not return until start condition and
*  address are sent and ACK/NACK response is received or errors occurred.
*
* Parameters:
*  slaveAddress: Right justified 7-bit Slave address (valid range 8 to 120).
*  bitRnW:       Direction of the following transfer. It is defined by
*                read/write bit within address byte.
*
* Return:
*  Error status
*
*
* Global variables:
*  I2COLED_state - used to store current state of software FSM.
*
*******************************************************************************/
uint32 I2COLED_I2CMasterSendRestart(uint32 slaveAddress, uint32 bitRnW)
{
    uint32 resetIp;
    uint32 errStatus;

    resetIp   = 0u;
    errStatus = I2COLED_I2C_MSTR_NOT_READY;

    /* Check FSM state before generating ReStart condition */
    if(I2COLED_CHECK_I2C_MASTER_ACTIVE)
    {
        slaveAddress = I2COLED_GET_I2C_8BIT_ADDRESS(slaveAddress);

        if(0u == bitRnW) /* Write direction */
        {
            I2COLED_state = I2COLED_I2C_FSM_MSTR_WR_DATA;
        }
        else  /* Read direction */
        {
            I2COLED_state  = I2COLED_I2C_FSM_MSTR_RD_DATA;
                      slaveAddress |= I2COLED_I2C_READ_FLAG;
        }

        /* TX and RX FIFO have to be EMPTY */

        /* Clean-up interrupt status */
        I2COLED_ClearMasterInterruptSource(I2COLED_INTR_MASTER_ALL);

        /* A proper ReStart sequence is: generate ReStart, then put an address byte in the TX FIFO.
        * Otherwise the master treats the address in the TX FIFO as a data byte if a previous transfer is write.
        * The write transfer continues instead of ReStart.
        */
        I2COLED_I2C_MASTER_GENERATE_RESTART;

        while(I2COLED_CHECK_I2C_MASTER_CMD(I2COLED_I2C_MASTER_CMD_M_START))
        {
            /* Wait until ReStart has been generated */
        }

        /* Put address into TX FIFO */
        I2COLED_TX_FIFO_WR_REG = slaveAddress;

        /* Wait for address to be transferred */
        while(!I2COLED_CHECK_INTR_MASTER(I2COLED_INTR_MASTER_I2C_ACK      |
                                                  I2COLED_INTR_MASTER_I2C_NACK     |
                                                  I2COLED_INTR_MASTER_I2C_ARB_LOST |
                                                  I2COLED_INTR_MASTER_I2C_BUS_ERROR))
        {
            /* Wait until address has been transferred */
        }

        /* Check results of address phase */
        if(I2COLED_CHECK_INTR_MASTER(I2COLED_INTR_MASTER_I2C_ACK))
        {
            errStatus = I2COLED_I2C_MSTR_NO_ERROR;
        }
        else if(I2COLED_CHECK_INTR_MASTER(I2COLED_INTR_MASTER_I2C_NACK))
        {
             errStatus = I2COLED_I2C_MSTR_ERR_LB_NAK;
        }
        else if(I2COLED_CHECK_INTR_MASTER(I2COLED_INTR_MASTER_I2C_ARB_LOST))
        {
            I2COLED_state = I2COLED_I2C_FSM_IDLE;
                         errStatus = I2COLED_I2C_MSTR_ERR_ARB_LOST;
                         resetIp   = I2COLED_I2C_RESET_ERROR;
        }
        else /* I2COLED_INTR_MASTER_I2C_BUS_ERROR set is else condition */
        {
            I2COLED_state = I2COLED_I2C_FSM_IDLE;
                         errStatus = I2COLED_I2C_MSTR_ERR_BUS_ERR;
                         resetIp   = I2COLED_I2C_RESET_ERROR;
        }

        I2COLED_ClearMasterInterruptSource(I2COLED_INTR_MASTER_I2C_ACK      |
                                                    I2COLED_INTR_MASTER_I2C_NACK     |
                                                    I2COLED_INTR_MASTER_I2C_ARB_LOST |
                                                    I2COLED_INTR_MASTER_I2C_BUS_ERROR);

        /* Reset block in case of: LOST_ARB or BUS_ERR */
        if(0u != resetIp)
        {
            I2COLED_SCB_SW_RESET;
        }
    }

    return(errStatus);
}


/*******************************************************************************
* Function Name: I2COLED_I2CMasterSendStop
********************************************************************************
*
* Summary:
*  Generates Stop condition on the bus.
*  At least one byte has to be read if start or restart condition with read
*  direction was generated before.
*  This function is blocking and does not return until a stop condition
*  is generated or error occurred.
*
* Parameters:
*  None
*
* Return:
*  Error status
*
* Side Effects:
*  A valid Start or ReStart condition must be generated before calling
*  this function. This function does nothing if Start or ReStart condition
*  failed before this function was called.
*  For read transfer, at least one byte has to be read before Stop generation.
*
* Global variables:
*  I2COLED_state - used to store current state of software FSM.
*
*******************************************************************************/
uint32 I2COLED_I2CMasterSendStop(void)
{
    uint32 resetIp;
    uint32 errStatus;

    resetIp   = 0u;
    errStatus = I2COLED_I2C_MSTR_NOT_READY;

    /* Check FSM state before generating Stop condition */
    if(I2COLED_CHECK_I2C_MASTER_ACTIVE)
    {
        /*
        * Write direction: generates Stop
        * Read  direction: generates NACK and Stop
        */
        I2COLED_I2C_MASTER_GENERATE_STOP;

        while(!I2COLED_CHECK_INTR_MASTER(I2COLED_INTR_MASTER_I2C_STOP     |
                                                  I2COLED_INTR_MASTER_I2C_ARB_LOST |
                                                  I2COLED_INTR_MASTER_I2C_BUS_ERROR))
        {
            /* Wait until Stop has been generated */
        }

        /* Check Stop generation */
        if(I2COLED_CHECK_INTR_MASTER(I2COLED_INTR_MASTER_I2C_STOP))
        {
            errStatus = I2COLED_I2C_MSTR_NO_ERROR;
        }
        else if(I2COLED_CHECK_INTR_MASTER(I2COLED_INTR_MASTER_I2C_ARB_LOST))
        {
            errStatus = I2COLED_I2C_MSTR_ERR_ARB_LOST;
            resetIp   = I2COLED_I2C_RESET_ERROR;
        }
        else /* I2COLED_INTR_MASTER_I2C_BUS_ERROR is set */
        {
            errStatus = I2COLED_I2C_MSTR_ERR_BUS_ERR;
            resetIp   = I2COLED_I2C_RESET_ERROR;
        }

        I2COLED_ClearMasterInterruptSource(I2COLED_INTR_MASTER_I2C_STOP     |
                                                    I2COLED_INTR_MASTER_I2C_ARB_LOST |
                                                    I2COLED_INTR_MASTER_I2C_BUS_ERROR);

        I2COLED_state = I2COLED_I2C_FSM_IDLE;

        /* Reset block in case of: LOST_ARB or BUS_ERR */
        if(0u != resetIp)
        {
            I2COLED_SCB_SW_RESET;
        }
    }

    return(errStatus);
}


/*******************************************************************************
* Function Name: I2COLED_I2CMasterWriteByte
********************************************************************************
*
* Summary:
*  Sends one byte to a slave.
*  This function is blocking and does not return until byte is transmitted
*  or error occurred.
*
* Parameters:
*  data: The data byte to send to the slave.
*
* Return:
*  Error status
*
* Side Effects:
*  A valid Start or ReStart condition must be generated before calling
*  this function. This function does nothing if Start or ReStart condition
*  failed before this function was called.
*
* Global variables:
*  I2COLED_state - used to store current state of software FSM.
*
*******************************************************************************/
uint32 I2COLED_I2CMasterWriteByte(uint32 theByte)
{
    uint32 resetIp;
    uint32 errStatus;

    resetIp   = 0u;
    errStatus = I2COLED_I2C_MSTR_NOT_READY;

    /* Check FSM state before write byte */
    if(I2COLED_CHECK_I2C_MASTER_ACTIVE)
    {
        I2COLED_TX_FIFO_WR_REG = theByte;

        while(!I2COLED_CHECK_INTR_MASTER(I2COLED_INTR_MASTER_I2C_ACK      |
                                                  I2COLED_INTR_MASTER_I2C_NACK     |
                                                  I2COLED_INTR_MASTER_I2C_ARB_LOST |
                                                  I2COLED_INTR_MASTER_I2C_BUS_ERROR))
        {
            /* Wait until byte has been transferred */
        }

        /* Check results after byte was sent */
        if(I2COLED_CHECK_INTR_MASTER(I2COLED_INTR_MASTER_I2C_ACK))
        {
            I2COLED_state = I2COLED_I2C_FSM_MSTR_HALT;
                         errStatus = I2COLED_I2C_MSTR_NO_ERROR;
        }
        else if(I2COLED_CHECK_INTR_MASTER(I2COLED_INTR_MASTER_I2C_NACK))
        {
            I2COLED_state = I2COLED_I2C_FSM_MSTR_HALT;
                         errStatus = I2COLED_I2C_MSTR_ERR_LB_NAK;
        }
        else if(I2COLED_CHECK_INTR_MASTER(I2COLED_INTR_MASTER_I2C_ARB_LOST))
        {
            I2COLED_state = I2COLED_I2C_FSM_IDLE;
                         errStatus = I2COLED_I2C_MSTR_ERR_ARB_LOST;
                         resetIp   = I2COLED_I2C_RESET_ERROR;
        }
        else /* I2COLED_INTR_MASTER_I2C_BUS_ERROR set is */
        {
            I2COLED_state = I2COLED_I2C_FSM_IDLE;
                         errStatus = I2COLED_I2C_MSTR_ERR_BUS_ERR;
                         resetIp   = I2COLED_I2C_RESET_ERROR;
        }

        I2COLED_ClearMasterInterruptSource(I2COLED_INTR_MASTER_I2C_ACK      |
                                                    I2COLED_INTR_MASTER_I2C_NACK     |
                                                    I2COLED_INTR_MASTER_I2C_ARB_LOST |
                                                    I2COLED_INTR_MASTER_I2C_BUS_ERROR);

        /* Reset block in case of: LOST_ARB or BUS_ERR */
        if(0u != resetIp)
        {
            I2COLED_SCB_SW_RESET;
        }
    }

    return(errStatus);
}


/*******************************************************************************
* Function Name: I2COLED_I2CMasterReadByte
********************************************************************************
*
* Summary:
*  Reads one byte from a slave and ACKs or NAKs received byte.
*  This function does not generate NAK explicitly. The following call
*  SCB_I2CMasterSendStop() or SCB_I2CMasterSendRestart() will generate NAK and
*  Stop or ReStart condition appropriately.
*  This function is blocking and does not return until byte is received
*  or error occurred.
*
* Parameters:
*  ackNack: Response to received byte.
*
* Return:
*  Byte read from the slave. In case of error the MSB of returned data
*  is set to 1.
*
* Side Effects:
*  A valid Start or ReStart condition must be generated before calling this
*  function. This function does nothing and returns invalid byte value
*  if Start or ReStart conditions failed before this function was called.
*
* Global variables:
*  I2COLED_state - used to store current state of software FSM.
*
*******************************************************************************/
uint32 I2COLED_I2CMasterReadByte(uint32 ackNack)
{
    uint32 theByte;

    /* Return invalid byte in case BUS_ERR happen during receiving */
    theByte = I2COLED_I2C_INVALID_BYTE;

    /* Check FSM state before read byte */
    if(I2COLED_CHECK_I2C_MASTER_ACTIVE)
    {
        while((!I2COLED_CHECK_INTR_RX(I2COLED_INTR_RX_NOT_EMPTY)) &&
              (!I2COLED_CHECK_INTR_MASTER(I2COLED_INTR_MASTER_I2C_ARB_LOST |
                                                  I2COLED_INTR_MASTER_I2C_BUS_ERROR)))
        {
            /* Wait until byte has been received */
        }

        /* Check the results after the byte was sent */
        if(I2COLED_CHECK_INTR_RX(I2COLED_INTR_RX_NOT_EMPTY))
        {
            theByte = I2COLED_RX_FIFO_RD_REG;

            I2COLED_ClearRxInterruptSource(I2COLED_INTR_RX_NOT_EMPTY);

            if(0u == ackNack)
            {
                I2COLED_I2C_MASTER_GENERATE_ACK;
            }
            else
            {
                /* NACK is generated by Stop or ReStart command */
                I2COLED_state = I2COLED_I2C_FSM_MSTR_HALT;
            }
        }
        else
        {
            I2COLED_ClearMasterInterruptSource(I2COLED_INTR_MASTER_ALL);

            /* Reset block in case of: LOST_ARB or BUS_ERR */
            I2COLED_SCB_SW_RESET;
        }
    }

    return(theByte);
}


/*******************************************************************************
* Function Name: I2COLED_I2CMasterGetReadBufSize
********************************************************************************
*
* Summary:
*  Returns the number of bytes that has been transferred with an
*  SCB_I2CMasterReadBuf() function.
*
* Parameters:
*  None
*
* Return:
*  Byte count of transfer. If the transfer is not yet complete, it returns
*  the byte count transferred so far.
*
* Side Effects:
*  This function returns not valid value if SCB_I2C_MSTAT_ERR_ARB_LOST
*  or SCB_I2C_MSTAT_ERR_BUS_ERROR occurred while read transfer.
*
* Global variables:
*  I2COLED_mstrRdBufIndex - used to current index within master read
*  buffer.
*
*******************************************************************************/
uint32 I2COLED_I2CMasterGetReadBufSize(void)
{
    return(I2COLED_mstrRdBufIndex);
}


/*******************************************************************************
* Function Name: I2COLED_I2CMasterGetWriteBufSize
********************************************************************************
*
* Summary:
*  Returns the number of bytes that have been transferred with an
*  SCB_I2CMasterWriteBuf() function.
*
* Parameters:
*  None
*
* Return:
*  Byte count of transfer. If the transfer is not yet complete, it returns
*  zero unit transfer completion.
*
* Side Effects:
*  This function returns not valid value if SCB_I2C_MSTAT_ERR_ARB_LOST
*  or SCB_I2C_MSTAT_ERR_BUS_ERROR occurred while read transfer.
*
* Global variables:
*  I2COLED_mstrWrBufIndex - used to current index within master write
*  buffer.
*
*******************************************************************************/
uint32 I2COLED_I2CMasterGetWriteBufSize(void)
{
    return(I2COLED_mstrWrBufIndex);
}


/*******************************************************************************
* Function Name: I2COLED_I2CMasterClearReadBuf
********************************************************************************
*
* Summary:
*  Resets the read buffer pointer back to the first byte in the buffer.
*
* Parameters:
*  None
*
* Return:
*  None
*
* Global variables:
*  I2COLED_mstrRdBufIndex - used to current index within master read
*   buffer.
*  I2COLED_mstrStatus - used to store current status of I2C Master.
*
*******************************************************************************/
void I2COLED_I2CMasterClearReadBuf(void)
{
    I2COLED_DisableInt();  /* Lock from interruption */

    I2COLED_mstrRdBufIndex = 0u;
    I2COLED_mstrStatus    &= (uint16) ~I2COLED_I2C_MSTAT_RD_CMPLT;

    I2COLED_EnableInt();   /* Release lock */
}


/*******************************************************************************
* Function Name: I2COLED_I2CMasterClearWriteBuf
********************************************************************************
*
* Summary:
*  Resets the write buffer pointer back to the first byte in the buffer.
*
* Parameters:
*  None
*
* Return:
*  None
*
* Global variables:
*  I2COLED_mstrRdBufIndex - used to current index within master read
*   buffer.
*  I2COLED_mstrStatus - used to store current status of I2C Master.
*
*******************************************************************************/
void I2COLED_I2CMasterClearWriteBuf(void)
{
    I2COLED_DisableInt();  /* Lock from interruption */

    I2COLED_mstrWrBufIndex = 0u;
    I2COLED_mstrStatus    &= (uint16) ~I2COLED_I2C_MSTAT_WR_CMPLT;

    I2COLED_EnableInt();   /* Release lock */
}


/*******************************************************************************
* Function Name: I2COLED_I2CMasterStatus
********************************************************************************
*
* Summary:
*  Returns the master's communication status.
*
* Parameters:
*  None
*
* Return:
*  Current status of I2C master.
*
* Global variables:
*  I2COLED_mstrStatus - used to store current status of I2C Master.
*
*******************************************************************************/
uint32 I2COLED_I2CMasterStatus(void)
{
    uint32 status;

    I2COLED_DisableInt();  /* Lock from interruption */

    status = (uint32) I2COLED_mstrStatus;

    if (I2COLED_CHECK_I2C_MASTER_ACTIVE)
    {
        /* Add status of master pending transaction: MSTAT_XFER_INP */
        status |= (uint32) I2COLED_I2C_MSTAT_XFER_INP;
    }

    I2COLED_EnableInt();   /* Release lock */

    return(status);
}


/*******************************************************************************
* Function Name: I2COLED_I2CMasterClearStatus
********************************************************************************
*
* Summary:
*  Clears all status flags and returns the master status.
*
* Parameters:
*  None
*
* Return:
*  Current status of I2C master.
*
* Global variables:
*  I2COLED_mstrStatus - used to store current status of I2C Master.
*
*******************************************************************************/
uint32 I2COLED_I2CMasterClearStatus(void)
{
    uint32 status;

    I2COLED_DisableInt();  /* Lock from interruption */

    /* Read and clear master status */
    status = (uint32) I2COLED_mstrStatus;
    I2COLED_mstrStatus = I2COLED_I2C_MSTAT_CLEAR;

    I2COLED_EnableInt();   /* Release lock */

    return(status);
}


/*******************************************************************************
* Function Name: I2COLED_I2CReStartGeneration
********************************************************************************
*
* Summary:
*  Generates a ReStart condition:
*  SCB IP V1 and later: Generates ReStart using the scb IP functionality
*    Sets the I2C_MASTER_CMD_M_START and I2C_MASTER_CMD_M_NACK (if the previous
*    transaction was read) bits in the SCB.I2C_MASTER_CMD register.
*    This combination forces the master to generate ReStart.
*
*  SCB IP V0: Generates Restart using the GPIO and scb IP functionality.
*   After the master completes write or read, the SCL is stretched.
*   The master waits until SDA line is released by the slave. Then the GPIO
*   function is enabled and the scb IP disabled as it already does not drive
*   the bus. In case of the previous transfer was read, the NACK is generated
*   by the GPIO. The delay of tLOW is added to manage the hold time.
*   Set I2C_M_CMD.START and enable the scb IP. The ReStart generation
*   is started after the I2C function is enabled for the SCL.
*   Note1: the scb IP due re-enable generates Start but on the I2C bus it
*          appears as ReStart.
*   Note2: the I2C_M_CMD.START is queued if scb IP is disabled.
*   Note3: the I2C_STATUS_M_READ is cleared is address was NACKed before.
*
* Parameters:
*  None
*
* Return:
*  None
*
* Side Effects:
*  SCB IP V0: The NACK generation by the GPIO may cause a greater SCL period
*             than expected for the selected master data rate.
*
*******************************************************************************/
void I2COLED_I2CReStartGeneration(void)
{
#if(I2COLED_CY_SCBIP_V0)
    /* Generates Restart use GPIO and scb IP functionality. Ticket ID#143715,
    ID#145238 and ID#173656 */
    uint32 status = I2COLED_I2C_STATUS_REG;

    while(I2COLED_WAIT_SDA_SET_HIGH)
    {
        /* Wait when slave release SDA line: SCL tHIGH is complete */
    }

    /* Prepare DR register to drive SCL line */
    I2COLED_SET_I2C_SCL_DR(I2COLED_I2C_SCL_LOW);

    /* Switch HSIOM to GPIO: SCL goes low */
    I2COLED_SET_I2C_SCL_HSIOM_SEL(I2COLED_HSIOM_GPIO_SEL);

    /* Disable SCB block */
    I2COLED_CTRL_REG &= (uint32) ~I2COLED_CTRL_ENABLED;

    if(0u != (status & I2COLED_I2C_STATUS_M_READ))
    {
        /* Generate NACK use GPIO functionality */
        I2COLED_SET_I2C_SCL_DR(I2COLED_I2C_SCL_LOW);
        CyDelayUs(I2COLED_I2C_TLOW_TIME); /* Count tLOW */

        I2COLED_SET_I2C_SCL_DR(I2COLED_I2C_SCL_HIGH);
        while(I2COLED_WAIT_SCL_SET_HIGH)
        {
            /* Wait until slave releases SCL in case if it stretches */
        }
        CyDelayUs(I2COLED_I2C_THIGH_TIME); /* Count tHIGH */
    }

    /* Count tLOW as hold time for write and read */
    I2COLED_SET_I2C_SCL_DR(I2COLED_I2C_SCL_LOW);
    CyDelayUs(I2COLED_I2C_TLOW_TIME); /* Count tLOW */

    /* Set command for Start generation: it will appear */
    I2COLED_I2C_MASTER_CMD_REG = I2COLED_I2C_MASTER_CMD_M_START;

    /* Enable SCB block */
    I2COLED_CTRL_REG |= (uint32) I2COLED_CTRL_ENABLED;

    /* Switch HSIOM to I2C: */
    I2COLED_SET_I2C_SCL_HSIOM_SEL(I2COLED_HSIOM_I2C_SEL);

    /* Revert SCL DR register */
    I2COLED_SET_I2C_SCL_DR(I2COLED_I2C_SCL_HIGH);
#else
    uint32 cmd;

    /* Generates ReStart use scb IP functionality */
    cmd  = I2COLED_I2C_MASTER_CMD_M_START;
    cmd |= I2COLED_CHECK_I2C_STATUS(I2COLED_I2C_STATUS_M_READ) ?
                (I2COLED_I2C_MASTER_CMD_M_NACK) : (0u);

    I2COLED_I2C_MASTER_CMD_REG = cmd;
#endif /* (I2COLED_CY_SCBIP_V1) */
}

#endif /* (I2COLED_I2C_MASTER_CONST) */


#if (!I2COLED_CY_SCBIP_V0 && \
    I2COLED_I2C_MULTI_MASTER_SLAVE_CONST && I2COLED_I2C_WAKE_ENABLE_CONST)
    /*******************************************************************************
    * Function Name: I2COLED_I2CMasterDisableEcAm
    ********************************************************************************
    *
    * Summary:
    *  Disables externally clocked address match to enable master operation
    *  in active mode.
    *
    * Parameters:
    *  None
    *
    * Return:
    *  None
    *
    *******************************************************************************/
    static void I2COLED_I2CMasterDisableEcAm(void)
    {
        /* Disables externally clocked address match to enable master operation in active mode.
        * This applicable only for Multi-Master-Slave with wakeup enabled. Ticket ID#192742 */
        if (0u != (I2COLED_CTRL_REG & I2COLED_CTRL_EC_AM_MODE))
        {
            /* Enable external address match logic */
            I2COLED_Stop();
            I2COLED_CTRL_REG &= (uint32) ~I2COLED_CTRL_EC_AM_MODE;
            I2COLED_Enable();
        }
    }
#endif /* (!I2COLED_CY_SCBIP_V0) */


/* [] END OF FILE */
