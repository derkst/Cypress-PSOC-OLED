/*******************************************************************************
* File Name: I2COLED_I2C_INT.c
* Version 2.0
*
* Description:
*  This file provides the source code to the Interrupt Service Routine for
*  the SCB Component in I2C mode.
*
* Note:
*
********************************************************************************
* Copyright 2013-2014, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "I2COLED_PVT.h"
#include "I2COLED_I2C_PVT.h"


/*******************************************************************************
* Function Name: I2COLED_I2C_ISR
********************************************************************************
*
* Summary:
*  Handles the Interrupt Service Routine for the SCB I2C mode.
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
CY_ISR(I2COLED_I2C_ISR)
{
    uint32 diffCount;
    uint32 endTransfer;

    #if(I2COLED_CHECK_I2C_ACCEPT_ADDRESS_CONST)
        uint32 address;
    #endif /* (I2COLED_CHECK_I2C_ACCEPT_ADDRESS_CONST) */

    endTransfer = 0u; /* Continue active transfer */

    /* Calls customer routine if registered */
    if(NULL != I2COLED_customIntrHandler)
    {
        I2COLED_customIntrHandler();
    }

    if(I2COLED_CHECK_INTR_I2C_EC_MASKED(I2COLED_INTR_I2C_EC_WAKE_UP))
    {
        /* Mask-off after wakeup */
        I2COLED_SetI2CExtClkInterruptMode(I2COLED_NO_INTR_SOURCES);
    }

    /* Master and Slave error tracking:
    * Add the master state check to track only the master errors when the master is active or
    * track slave errors when the slave is active or idle.
    * A special MMS case: in the address phase with misplaced Start: the master sets the LOST_ARB and
    * slave BUS_ERR. The valid event is LOST_ARB comes from the master.
    */
    if(I2COLED_CHECK_I2C_FSM_MASTER)
    {
        #if(I2COLED_I2C_MASTER)
        {
            /* INTR_MASTER_I2C_BUS_ERROR:
            * A misplaced Start or Stop condition occurred on the bus: complete the transaction.
            * The interrupt is cleared in I2C_FSM_EXIT_IDLE.
            */
            if(I2COLED_CHECK_INTR_MASTER_MASKED(I2COLED_INTR_MASTER_I2C_BUS_ERROR))
            {
                I2COLED_mstrStatus |= (uint16) (I2COLED_I2C_MSTAT_ERR_XFER |
                                                         I2COLED_I2C_MSTAT_ERR_BUS_ERROR);

                endTransfer = I2COLED_I2C_CMPLT_ANY_TRANSFER;
            }

            /* INTR_MASTER_I2C_ARB_LOST:
            * The MultiMaster lost arbitrage during transaction.
            * A Misplaced Start or Stop condition is treated as lost arbitration when the master drives the SDA.
            * The interrupt source is cleared in I2C_FSM_EXIT_IDLE.
            */
            if(I2COLED_CHECK_INTR_MASTER_MASKED(I2COLED_INTR_MASTER_I2C_ARB_LOST))
            {
                I2COLED_mstrStatus |= (uint16) (I2COLED_I2C_MSTAT_ERR_XFER |
                                                         I2COLED_I2C_MSTAT_ERR_ARB_LOST);

                endTransfer = I2COLED_I2C_CMPLT_ANY_TRANSFER;
            }

            #if(I2COLED_I2C_MULTI_MASTER_SLAVE)
            {
                /* I2C_MASTER_CMD_M_START_ON_IDLE:
                * MultiMaster-Slave does not generate start, because Slave was addressed.
                * Pass control to slave.
                */
                if(I2COLED_CHECK_I2C_MASTER_CMD(I2COLED_I2C_MASTER_CMD_M_START_ON_IDLE))
                {
                    I2COLED_mstrStatus |= (uint16) (I2COLED_I2C_MSTAT_ERR_XFER |
                                                             I2COLED_I2C_MSTAT_ERR_ABORT_XFER);

                    endTransfer = I2COLED_I2C_CMPLT_ANY_TRANSFER;
                }
            }
            #endif

            /* The error handling common part:
            * Sets a completion flag of the master transaction and passes control to:
            *  - I2C_FSM_EXIT_IDLE - to complete transaction in case of: ARB_LOST or BUS_ERR.
            *  - I2C_FSM_IDLE      - to take chance for the slave to process incoming transaction.
            */
            if(0u != endTransfer)
            {
                /* Set completion flags for master */
                I2COLED_mstrStatus |= (uint16) I2COLED_GET_I2C_MSTAT_CMPLT;

                #if(I2COLED_I2C_MULTI_MASTER_SLAVE)
                {
                    if(I2COLED_CHECK_I2C_FSM_ADDR)
                    {
                        /* Start generation is set after another master starts accessing Slave.
                        * Clean-up master and turn to slave. Set state to IDLE.
                        */
                        if(I2COLED_CHECK_I2C_MASTER_CMD(I2COLED_I2C_MASTER_CMD_M_START_ON_IDLE))
                        {
                            I2COLED_I2C_MASTER_CLEAR_START;

                            endTransfer = I2COLED_I2C_CMPLT_ANY_TRANSFER; /* Pass control to Slave */
                        }
                        /* Valid arbitration lost on the address phase happens only when: master LOST_ARB is set and
                        * slave BUS_ERR is cleared. Only in that case set the state to IDLE without SCB IP re-enable.
                        */
                        else if((!I2COLED_CHECK_INTR_SLAVE_MASKED(I2COLED_INTR_SLAVE_I2C_BUS_ERROR))
                               && I2COLED_CHECK_INTR_MASTER_MASKED(I2COLED_INTR_MASTER_I2C_ARB_LOST))
                        {
                            endTransfer = I2COLED_I2C_CMPLT_ANY_TRANSFER; /* Pass control to Slave */
                        }
                        else
                        {
                            endTransfer = 0u; /* Causes I2C_FSM_EXIT_IDLE to be set below */
                        }

                        if(0u != endTransfer) /* Clean-up master to proceed with slave */
                        {
                            I2COLED_CLEAR_TX_FIFO; /* Shifter keeps address, clear it */

                            I2COLED_DISABLE_MASTER_AUTO_DATA_ACK; /* In case of reading disable autoACK */

                            /* Clean-up master interrupt sources */
                            I2COLED_ClearMasterInterruptSource(I2COLED_INTR_MASTER_ALL);

                            /* Disable data processing interrupts: they have to be cleared before */
                            I2COLED_SetRxInterruptMode(I2COLED_NO_INTR_SOURCES);
                            I2COLED_SetTxInterruptMode(I2COLED_NO_INTR_SOURCES);

                            I2COLED_state = I2COLED_I2C_FSM_IDLE;
                        }
                        else
                        {
                            /* Set I2C_FSM_EXIT_IDLE for BUS_ERR and ARB_LOST (that is really bus error) */
                            I2COLED_state = I2COLED_I2C_FSM_EXIT_IDLE;
                        }
                    }
                    else
                    {
                        /* Set I2C_FSM_EXIT_IDLE if any other state than address */
                        I2COLED_state = I2COLED_I2C_FSM_EXIT_IDLE;
                    }
                }
                #else
                {
                    /* In case of LOST*/
                    I2COLED_state = I2COLED_I2C_FSM_EXIT_IDLE;
                }
                #endif
            }
        }
        #endif
    }
    else /* (I2COLED_CHECK_I2C_FSM_SLAVE) */
    {
        #if(I2COLED_I2C_SLAVE)
        {
            /* INTR_SLAVE_I2C_BUS_ERROR or I2COLED_INTR_SLAVE_I2C_ARB_LOST:
            * A Misplaced Start or Stop condition occurred on the bus: set a flag
            * to notify an error condition.
            */
            if(I2COLED_CHECK_INTR_SLAVE_MASKED(I2COLED_INTR_SLAVE_I2C_BUS_ERROR |
                                                        I2COLED_INTR_SLAVE_I2C_ARB_LOST))
            {
                if(I2COLED_CHECK_I2C_FSM_RD)
                {
                    /* TX direction: master reads from slave */
                    I2COLED_slStatus &= (uint8) ~I2COLED_I2C_SSTAT_RD_BUSY;
                    I2COLED_slStatus |= (uint8) (I2COLED_I2C_SSTAT_RD_ERR |
                                                          I2COLED_I2C_SSTAT_RD_CMPLT);
                }
                else
                {
                    /* RX direction: master writes into slave */
                    I2COLED_slStatus &= (uint8) ~I2COLED_I2C_SSTAT_WR_BUSY;
                    I2COLED_slStatus |= (uint8) (I2COLED_I2C_SSTAT_WR_ERR |
                                                          I2COLED_I2C_SSTAT_WR_CMPLT);
                }

                I2COLED_state = I2COLED_I2C_FSM_EXIT_IDLE;
            }
        }
        #endif
    }

    /* States description:
    * Any Master operation starts from: the ADDR_RD/WR state as the master generates traffic on the bus.
    * Any Slave operation starts from: the IDLE state as the slave always waits for actions from the master.
    */

    /* FSM Master */
    if(I2COLED_CHECK_I2C_FSM_MASTER)
    {
        #if(I2COLED_I2C_MASTER)
        {
            /* INTR_MASTER_I2C_STOP:
            * A Stop condition was generated by the master: the end of the transaction.
            * Set completion flags to notify the API.
            */
            if(I2COLED_CHECK_INTR_MASTER_MASKED(I2COLED_INTR_MASTER_I2C_STOP))
            {
                I2COLED_ClearMasterInterruptSource(I2COLED_INTR_MASTER_I2C_STOP);

                I2COLED_mstrStatus |= (uint16) I2COLED_GET_I2C_MSTAT_CMPLT;
                I2COLED_state       = I2COLED_I2C_FSM_IDLE;
            }
            else
            {
                if(I2COLED_CHECK_I2C_FSM_ADDR) /* Address stage */
                {
                    /* INTR_MASTER_I2C_NACK:
                    * The master sent an address but it was NACKed by the slave. Complete transaction.
                    */
                    if(I2COLED_CHECK_INTR_MASTER_MASKED(I2COLED_INTR_MASTER_I2C_NACK))
                    {
                        I2COLED_ClearMasterInterruptSource(I2COLED_INTR_MASTER_I2C_NACK);

                        I2COLED_mstrStatus |= (uint16) (I2COLED_I2C_MSTAT_ERR_XFER |
                                                                 I2COLED_I2C_MSTAT_ERR_ADDR_NAK);

                        endTransfer = I2COLED_I2C_CMPLT_ANY_TRANSFER;
                    }
                    /* INTR_TX_UNDERFLOW. The master sent an address:
                    *  - TX direction: the clock is stretched after the ACK phase, because the TX FIFO is
                    *    EMPTY. The TX EMPTY cleans all the TX interrupt sources.
                    *  - RX direction: the 1st byte is received, but there is no ACK permission,
                    *    the clock is stretched after 1 byte is received.
                    */
                    else
                    {
                        if(I2COLED_CHECK_I2C_FSM_RD) /* Reading */
                        {
                            I2COLED_state = I2COLED_I2C_FSM_MSTR_RD_DATA;
                        }
                        else /* Writing */
                        {
                            I2COLED_state = I2COLED_I2C_FSM_MSTR_WR_DATA;
                            if(0u != I2COLED_mstrWrBufSize)
                            {
                                /* Enable INTR.TX_EMPTY if there is data to transmit */
                                I2COLED_SetTxInterruptMode(I2COLED_INTR_TX_EMPTY);
                            }
                        }
                    }
                }

                if(I2COLED_CHECK_I2C_FSM_DATA) /* Data phase */
                {
                    if(I2COLED_CHECK_I2C_FSM_RD) /* Reading */
                    {
                        /* INTR_RX_FULL:
                        * RX direction: the master received 8 bytes.
                        * Get data from RX FIFO and decide whether to ACK or  NACK the following bytes.
                        */
                        if(I2COLED_CHECK_INTR_RX_MASKED(I2COLED_INTR_RX_FULL))
                        {
                            /* Calculate difference */
                            diffCount =  I2COLED_mstrRdBufSize -
                                        (I2COLED_mstrRdBufIndex + I2COLED_GET_RX_FIFO_ENTRIES);

                            /* Proceed transaction or end it when RX FIFO becomes FULL again */
                            if(diffCount > I2COLED_I2C_FIFO_SIZE)
                            {
                                diffCount = I2COLED_I2C_FIFO_SIZE;
                            }
                            else
                            {
                                if(0u == diffCount)
                                {
                                    I2COLED_DISABLE_MASTER_AUTO_DATA_ACK;

                                    diffCount   = I2COLED_I2C_FIFO_SIZE;
                                    endTransfer = I2COLED_I2C_CMPLT_ANY_TRANSFER;
                                }
                            }

                            for(; (0u != diffCount); diffCount--)
                            {
                                I2COLED_mstrRdBufPtr[I2COLED_mstrRdBufIndex] = (uint8)
                                                                                        I2COLED_RX_FIFO_RD_REG;
                                I2COLED_mstrRdBufIndex++;
                            }
                        }
                        /* INTR_RX_NOT_EMPTY:
                        * RX direction: the master received one data byte, ACK or NACK it.
                        * The last byte is stored and NACKed by the master. The NACK and Stop is
                        * generated by one command generate Stop.
                        */
                        else if(I2COLED_CHECK_INTR_RX_MASKED(I2COLED_INTR_RX_NOT_EMPTY))
                        {
                            /* Put data in component buffer */
                            I2COLED_mstrRdBufPtr[I2COLED_mstrRdBufIndex] = (uint8) I2COLED_RX_FIFO_RD_REG;
                            I2COLED_mstrRdBufIndex++;

                            if(I2COLED_mstrRdBufIndex < I2COLED_mstrRdBufSize)
                            {
                                I2COLED_I2C_MASTER_GENERATE_ACK;
                            }
                            else
                            {
                               endTransfer = I2COLED_I2C_CMPLT_ANY_TRANSFER;
                            }
                        }
                        else
                        {
                            /* Do nothing */
                        }

                        I2COLED_ClearRxInterruptSource(I2COLED_INTR_RX_ALL);
                    }
                    else /* Writing */
                    {
                        /* INTR_MASTER_I2C_NACK :
                        * The master writes data to the slave and NACK was received: not all the bytes were
                        * written to the slave from the TX FIFO. Revert the index if there is data in
                        * the TX FIFO and pass control to a complete transfer.
                        */
                        if(I2COLED_CHECK_INTR_MASTER_MASKED(I2COLED_INTR_MASTER_I2C_NACK))
                        {
                            I2COLED_ClearMasterInterruptSource(I2COLED_INTR_MASTER_I2C_NACK);

                            /* Rollback write buffer index: NACKed byte remains in shifter */
                            I2COLED_mstrWrBufIndexTmp -= (I2COLED_GET_TX_FIFO_ENTRIES +
                                                                   I2COLED_GET_TX_FIFO_SR_VALID);

                            /* Update number of transferred bytes */
                            I2COLED_mstrWrBufIndex = I2COLED_mstrWrBufIndexTmp;

                            I2COLED_mstrStatus |= (uint16) (I2COLED_I2C_MSTAT_ERR_XFER |
                                                                     I2COLED_I2C_MSTAT_ERR_SHORT_XFER);

                            I2COLED_CLEAR_TX_FIFO;

                            endTransfer = I2COLED_I2C_CMPLT_ANY_TRANSFER;
                        }
                        /* INTR_TX_EMPTY :
                        * TX direction: the TX FIFO is EMPTY, the data from the buffer needs to be put there.
                        * When there is no data in the component buffer, the underflow interrupt is
                        * enabled to catch when all the data has been transferred.
                        */
                        else if(I2COLED_CHECK_INTR_TX_MASKED(I2COLED_INTR_TX_EMPTY))
                        {
                            while(I2COLED_I2C_FIFO_SIZE != I2COLED_GET_TX_FIFO_ENTRIES)
                            {
                                /* The temporary mstrWrBufIndexTmp is used because slave could NACK the byte and index
                                * roll-back required in this case. The mstrWrBufIndex is updated at the end of transfer.
                                */
                                if(I2COLED_mstrWrBufIndexTmp < I2COLED_mstrWrBufSize)
                                {
                                #if(!I2COLED_CY_SCBIP_V0)
                                   /* Clear INTR_TX.UNDERFLOW before putting the last byte into TX FIFO. This ensures
                                    * a proper trigger at the end of transaction when INTR_TX.UNDERFLOW single trigger
                                    * event. Ticket ID# 156735.
                                    */
                                    if(I2COLED_mstrWrBufIndexTmp == (I2COLED_mstrWrBufSize - 1u))
                                    {
                                        I2COLED_ClearTxInterruptSource(I2COLED_INTR_TX_UNDERFLOW);
                                        I2COLED_SetTxInterruptMode(I2COLED_INTR_TX_UNDERFLOW);
                                    }
                                 #endif /* (!I2COLED_CY_SCBIP_V0) */

                                    /* Put data into TX FIFO */
                                    I2COLED_TX_FIFO_WR_REG = (uint32) I2COLED_mstrWrBufPtr[I2COLED_mstrWrBufIndexTmp];
                                    I2COLED_mstrWrBufIndexTmp++;
                                }
                                else
                                {
                                    break; /* No more data to put */
                                }
                            }

                        #if(I2COLED_CY_SCBIP_V0)
                            if(I2COLED_mstrWrBufIndexTmp == I2COLED_mstrWrBufSize)
                            {
                                I2COLED_SetTxInterruptMode(I2COLED_INTR_TX_UNDERFLOW);
                            }

                            I2COLED_ClearTxInterruptSource(I2COLED_INTR_TX_ALL);
                        #else
                            I2COLED_ClearTxInterruptSource(I2COLED_INTR_TX_EMPTY);
                        #endif /* (I2COLED_CY_SCBIP_V0) */
                        }
                        /* INTR_TX_UNDERFLOW:
                        * TX direction: all data from the TX FIFO was transferred to the slave.
                        * The transaction needs to be completed.
                        */
                        else if(I2COLED_CHECK_INTR_TX_MASKED(I2COLED_INTR_TX_UNDERFLOW))
                        {
                            /* Update number of transferred bytes */
                            I2COLED_mstrWrBufIndex = I2COLED_mstrWrBufIndexTmp;

                            endTransfer = I2COLED_I2C_CMPLT_ANY_TRANSFER;
                        }
                        else
                        {
                            /* Do nothing */
                        }
                    }
                }

                if(0u != endTransfer) /* Complete transfer */
                {
                    /* Clean-up master after reading: only in case of NACK */
                    I2COLED_DISABLE_MASTER_AUTO_DATA_ACK;

                    /* Disable data processing interrupts: they have to be cleared before */
                    I2COLED_SetRxInterruptMode(I2COLED_NO_INTR_SOURCES);
                    I2COLED_SetTxInterruptMode(I2COLED_NO_INTR_SOURCES);

                    if(I2COLED_CHECK_I2C_MODE_NO_STOP(I2COLED_mstrControl))
                    {
                        /* On-going transaction is suspended: the ReStart is generated by the API request */
                        I2COLED_mstrStatus |= (uint16) (I2COLED_I2C_MSTAT_XFER_HALT |
                                                                 I2COLED_GET_I2C_MSTAT_CMPLT);

                        I2COLED_state = I2COLED_I2C_FSM_MSTR_HALT;
                    }
                    else
                    {
                        /* Complete transaction: exclude the data processing state and generate Stop.
                        * The completion status will be set after Stop generation.
                        * A special case is read: because NACK and Stop are generated by the command below.
                        * Lost arbitration can occur during NACK generation when
                        * the other master is still reading from the slave.
                        */
                        I2COLED_I2C_MASTER_GENERATE_STOP;
                    }
                }
            }

        } /* (I2COLED_I2C_MASTER) */
        #endif

    } /* (I2COLED_CHECK_I2C_FSM_MASTER) */


    /* FSM Slave */
    else if(I2COLED_CHECK_I2C_FSM_SLAVE)
    {
        #if(I2COLED_I2C_SLAVE)
        {
            /* INTR_SLAVE_NACK:
            * The master completes reading the slave: the appropriate flags have to be set.
            * The TX FIFO is cleared after an overflow condition is set.
            */
            if(I2COLED_CHECK_INTR_SLAVE_MASKED(I2COLED_INTR_SLAVE_I2C_NACK))
            {
                I2COLED_ClearSlaveInterruptSource(I2COLED_INTR_SLAVE_I2C_NACK);

                /* All entries that remain in TX FIFO max value is 9: 8 (FIFO) + 1 (SHIFTER) */
                diffCount = (I2COLED_GET_TX_FIFO_ENTRIES + I2COLED_GET_TX_FIFO_SR_VALID);

                if(I2COLED_slOverFlowCount > diffCount) /* Overflow */
                {
                    I2COLED_slStatus |= (uint8) I2COLED_I2C_SSTAT_RD_OVFL;
                }
                else /* No Overflow */
                {
                    /* Roll-back temporary index */
                    I2COLED_slRdBufIndexTmp -= (diffCount - I2COLED_slOverFlowCount);
                }

                /* Update slave of transferred bytes */
                I2COLED_slRdBufIndex = I2COLED_slRdBufIndexTmp;

                /* Clean-up TX FIFO */
                I2COLED_SetTxInterruptMode(I2COLED_NO_INTR_SOURCES);
                I2COLED_slOverFlowCount = 0u;
                I2COLED_CLEAR_TX_FIFO;

                /* Complete master reading */
                I2COLED_slStatus &= (uint8) ~I2COLED_I2C_SSTAT_RD_BUSY;
                I2COLED_slStatus |= (uint8)  I2COLED_I2C_SSTAT_RD_CMPLT;
                I2COLED_state     =  I2COLED_I2C_FSM_IDLE;
            }


            /* INTR_SLAVE_I2C_WRITE_STOP:
            * The master completes writing to the slave: the appropriate flags have to be set.
            * The RX FIFO contains 1-8 bytes from the previous transaction which needs to be read.
            * There is a possibility that RX FIFO contains an address, it needs to leave it there.
            */
            if(I2COLED_CHECK_INTR_SLAVE_MASKED(I2COLED_INTR_SLAVE_I2C_WRITE_STOP))
            {
                I2COLED_ClearSlaveInterruptSource(I2COLED_INTR_SLAVE_I2C_WRITE_STOP);

                /* Read bytes from RX FIFO when auto data ACK receive logic is enabled. Otherwise all data bytes
                * were already read from the RX FIFO except for address byte which has to stay here to be handled by
                * I2C_ADDR_MATCH.
                */
                if (0u != (I2COLED_I2C_CTRL_REG & I2COLED_I2C_CTRL_S_READY_DATA_ACK))
                {
                    while(0u != I2COLED_GET_RX_FIFO_ENTRIES)
                    {
                        #if(I2COLED_CHECK_I2C_ACCEPT_ADDRESS)
                        {
                            if((1u == I2COLED_GET_RX_FIFO_ENTRIES) &&
                               (I2COLED_CHECK_INTR_SLAVE_MASKED(I2COLED_INTR_SLAVE_I2C_ADDR_MATCH)))
                            {
                                break; /* Leave address in RX FIFO */
                            }
                        }
                        #endif

                        /* Put data in component buffer */
                        I2COLED_slWrBufPtr[I2COLED_slWrBufIndex] = (uint8) I2COLED_RX_FIFO_RD_REG;
                        I2COLED_slWrBufIndex++;
                    }

                    I2COLED_DISABLE_SLAVE_AUTO_DATA;
                }

                if(I2COLED_CHECK_INTR_RX(I2COLED_INTR_RX_OVERFLOW))
                {
                    I2COLED_slStatus |= (uint8) I2COLED_I2C_SSTAT_WR_OVFL;
                }

                /* Clears RX interrupt sources triggered on data receiving */
                I2COLED_SetRxInterruptMode(I2COLED_NO_INTR_SOURCES);
                I2COLED_ClearRxInterruptSource(I2COLED_INTR_RX_ALL);

                /* Complete master writing */
                I2COLED_slStatus &= (uint8) ~I2COLED_I2C_SSTAT_WR_BUSY;
                I2COLED_slStatus |= (uint8)  I2COLED_I2C_SSTAT_WR_CMPLT;
                I2COLED_state     =  I2COLED_I2C_FSM_IDLE;
            }


            /* INTR_SLAVE_I2C_ADDR_MATCH:
            * The address match event starts the slave operation: after leaving the TX or RX
            * direction has to be chosen.
            * The wakeup interrupt must be cleared only after an address match is set.
            */
            if(I2COLED_CHECK_INTR_SLAVE_MASKED(I2COLED_INTR_SLAVE_I2C_ADDR_MATCH))
            {
                #if(I2COLED_CHECK_I2C_ACCEPT_ADDRESS)
                {
                    address = I2COLED_RX_FIFO_RD_REG; /* Address in the RX FIFO */

                    /* Clears RX sources if address was received in RX FIFO */
                    I2COLED_ClearRxInterruptSource(I2COLED_INTR_RX_ALL);

                    if(0u != address)
                    {
                        /* Suppress compiler warning */
                    }
                }
                #endif

                if(I2COLED_CHECK_I2C_STATUS(I2COLED_I2C_STATUS_S_READ))
                /* TX direction: master reads from slave */
                {
                    I2COLED_SetTxInterruptMode(I2COLED_INTR_TX_EMPTY);

                    /* Set temporary index to address buffer clear from API */
                    I2COLED_slRdBufIndexTmp = I2COLED_slRdBufIndex;

                    /* Start master reading */
                    I2COLED_slStatus |= (uint8) I2COLED_I2C_SSTAT_RD_BUSY;
                    I2COLED_state     = I2COLED_I2C_FSM_SL_RD;
                }
                else
                /* RX direction: master writes into slave */
                {
                    /* Calculate available buffer size */
                    diffCount = (I2COLED_slWrBufSize - I2COLED_slWrBufIndex);

                #if (I2COLED_CY_SCBIP_V0)

                    if(diffCount < I2COLED_I2C_FIFO_SIZE)
                    /* Receive data: byte-by-byte */
                    {
                        I2COLED_SetRxInterruptMode(I2COLED_INTR_RX_NOT_EMPTY);
                    }
                    else
                    /* Receive data: into RX FIFO */
                    {
                        if(diffCount == I2COLED_I2C_FIFO_SIZE)
                        {
                            /* NACK when RX FIFO become FULL */
                            I2COLED_ENABLE_SLAVE_AUTO_DATA;
                        }
                        else
                        {
                            /* Stretch clock when RX FIFO becomes FULL */
                            I2COLED_ENABLE_SLAVE_AUTO_DATA_ACK;
                            I2COLED_SetRxInterruptMode(I2COLED_INTR_RX_FULL);
                        }
                    }

                #else

                    #if(I2COLED_CHECK_I2C_ACCEPT_ADDRESS)
                    {
                        /* Enable RX.NOT_EMPTY interrupt source to receive byte by byte.
                        * The byte by byte receive is always chosen for the case when an address is accepted in RX FIFO.
                        * Ticket ID#175559.
                        */
                        I2COLED_SetRxInterruptMode(I2COLED_INTR_RX_NOT_EMPTY);
                    }
                    #else
                    {
                        if(diffCount < I2COLED_I2C_FIFO_SIZE)
                        /* Receive data: byte-by-byte */
                        {
                            I2COLED_SetRxInterruptMode(I2COLED_INTR_RX_NOT_EMPTY);
                        }
                        else
                        /* Receive data: into RX FIFO */
                        {
                            if(diffCount == I2COLED_I2C_FIFO_SIZE)
                            {
                                /* NACK when RX FIFO become FULL */
                                I2COLED_ENABLE_SLAVE_AUTO_DATA;
                            }
                            else
                            {
                                /* Stretch clock when RX FIFO becomes FULL */
                                I2COLED_ENABLE_SLAVE_AUTO_DATA_ACK;
                                I2COLED_SetRxInterruptMode(I2COLED_INTR_RX_FULL);
                            }
                        }
                    }
                    #endif

                #endif /* (I2COLED_CY_SCBIP_V0) */

                    /* Start master reading */
                    I2COLED_slStatus |= (uint8) I2COLED_I2C_SSTAT_WR_BUSY;
                    I2COLED_state     = I2COLED_I2C_FSM_SL_WR;
                }

                /* Clear interrupts before ACK address */
                I2COLED_ClearI2CExtClkInterruptSource(I2COLED_INTR_I2C_EC_WAKE_UP);
                I2COLED_ClearSlaveInterruptSource(I2COLED_INTR_SLAVE_ALL);

                /* Preparation complete: ACK the address */
                I2COLED_I2C_SLAVE_GENERATE_ACK;
            }

            /* I2COLED_INTR_RX_FULL:
            * Get data from the RX FIFO and decide whether to ACK or NACK the following bytes
            */
            if(I2COLED_CHECK_INTR_RX_MASKED(I2COLED_INTR_RX_FULL))
            {
                /* Calculate available buffer size to take into account that RX FIFO is FULL */
                diffCount =  I2COLED_slWrBufSize -
                            (I2COLED_slWrBufIndex + I2COLED_I2C_FIFO_SIZE);

                if(diffCount > I2COLED_I2C_FIFO_SIZE) /* Proceed transaction */
                {
                    diffCount   = I2COLED_I2C_FIFO_SIZE;
                    endTransfer = 0u;  /* Continue active transfer */
                }
                else /* End when FIFO becomes FULL again */
                {
                    endTransfer = I2COLED_I2C_CMPLT_ANY_TRANSFER;
                }

                for(; (0u != diffCount); diffCount--)
                {
                    /* Put data in component buffer */
                    I2COLED_slWrBufPtr[I2COLED_slWrBufIndex] = (uint8) I2COLED_RX_FIFO_RD_REG;
                    I2COLED_slWrBufIndex++;
                }

                if(0u != endTransfer) /* End transfer sending NACK */
                {
                    I2COLED_ENABLE_SLAVE_AUTO_DATA_NACK;

                    /* INTR_RX_FULL triggers earlier than INTR_SLAVE_I2C_STOP:
                    * disable all RX interrupt sources.
                    */
                    I2COLED_SetRxInterruptMode(I2COLED_NO_INTR_SOURCES);
                }

                I2COLED_ClearRxInterruptSource(I2COLED_INTR_RX_FULL);
            }
            /* I2COLED_INTR_RX_NOT_EMPTY:
            * The buffer size is less than 8: it requires processing in byte-by-byte mode.
            */
            else if(I2COLED_CHECK_INTR_RX_MASKED(I2COLED_INTR_RX_NOT_EMPTY))
            {
                diffCount = I2COLED_RX_FIFO_RD_REG;

                if(I2COLED_slWrBufIndex < I2COLED_slWrBufSize)
                {
                    I2COLED_I2C_SLAVE_GENERATE_ACK;

                    /* Put data into component buffer */
                    I2COLED_slWrBufPtr[I2COLED_slWrBufIndex] = (uint8) diffCount;
                    I2COLED_slWrBufIndex++;
                }
                else /* Overflow: there is no space in write buffer */
                {
                    I2COLED_I2C_SLAVE_GENERATE_NACK;

                    I2COLED_slStatus |= (uint8) I2COLED_I2C_SSTAT_WR_OVFL;
                }

                I2COLED_ClearRxInterruptSource(I2COLED_INTR_RX_NOT_EMPTY);
            }
            else
            {
                /* Does nothing */
            }


            /* I2COLED_INTR_TX_EMPTY:
            * The master reads the slave: provide data to read or 0xFF in the case of the end of the buffer
            * The overflow condition must be captured, but not set until the end of transaction.
            * There is a possibility of a false overflow due to TX FIFO utilization.
            */
            if(I2COLED_CHECK_INTR_TX_MASKED(I2COLED_INTR_TX_EMPTY))
            {
                while(I2COLED_I2C_FIFO_SIZE != I2COLED_GET_TX_FIFO_ENTRIES)
                {
                    /* Temporary slRdBufIndexTmp is used because the master can NACK the byte and
                    * index roll-back is required in this case. The slRdBufIndex is updated at the end
                    * of the read transfer.
                    */
                    if(I2COLED_slRdBufIndexTmp < I2COLED_slRdBufSize)
                    /* Data from buffer */
                    {
                        I2COLED_TX_FIFO_WR_REG = (uint32) I2COLED_slRdBufPtr[I2COLED_slRdBufIndexTmp];
                        I2COLED_slRdBufIndexTmp++;
                    }
                    else
                    /* Probably Overflow */
                    {
                        I2COLED_TX_FIFO_WR_REG = I2COLED_I2C_SLAVE_OVFL_RETURN;

                        if(0u == (I2COLED_INTR_TX_OVERFLOW & I2COLED_slOverFlowCount))
                        {
                            /* Get counter in range of byte: value 10 is overflow */
                            I2COLED_slOverFlowCount++;
                        }
                    }
                }

                I2COLED_ClearTxInterruptSource(I2COLED_INTR_TX_EMPTY);
            }

        }  /* (I2COLED_I2C_SLAVE) */
        #endif
    }


    /* FSM EXIT:
    * Slave:  INTR_SLAVE_I2C_BUS_ERROR, INTR_SLAVE_I2C_ARB_LOST
    * Master: INTR_MASTER_I2C_BUS_ERROR, INTR_MASTER_I2C_ARB_LOST.
    */
    else
    {
        I2COLED_CTRL_REG &= (uint32) ~I2COLED_CTRL_ENABLED; /* Disable scb IP */

        I2COLED_state = I2COLED_I2C_FSM_IDLE;

        I2COLED_DISABLE_SLAVE_AUTO_DATA;
        I2COLED_DISABLE_MASTER_AUTO_DATA;

    #if(I2COLED_CY_SCBIP_V0)
        I2COLED_SetRxInterruptMode(I2COLED_NO_INTR_SOURCES);
        I2COLED_SetTxInterruptMode(I2COLED_NO_INTR_SOURCES);

        /* Clear interrupt sources as they are not automatically cleared after SCB is disabled */
        I2COLED_ClearTxInterruptSource(I2COLED_INTR_RX_ALL);
        I2COLED_ClearRxInterruptSource(I2COLED_INTR_TX_ALL);
        I2COLED_ClearSlaveInterruptSource(I2COLED_INTR_SLAVE_ALL);
        I2COLED_ClearMasterInterruptSource(I2COLED_INTR_MASTER_ALL);
    #endif /* (I2COLED_CY_SCBIP_V0) */

        I2COLED_CTRL_REG |= (uint32) I2COLED_CTRL_ENABLED;  /* Enable scb IP */
    }
}


/* [] END OF FILE */
