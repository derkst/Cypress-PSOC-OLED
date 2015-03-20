/*******************************************************************************
* File Name: I2COLED_I2C.h
* Version 2.0
*
* Description:
*  This file provides constants and parameter values for the SCB Component in
*  the I2C mode.
*
* Note:
*
********************************************************************************
* Copyright 2013-2014, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_SCB_I2C_I2COLED_H)
#define CY_SCB_I2C_I2COLED_H

#include "I2COLED.h"


/***************************************
*   Initial Parameter Constants
****************************************/

#define I2COLED_I2C_MODE                   (2u)
#define I2COLED_I2C_OVS_FACTOR_LOW         (8u)
#define I2COLED_I2C_OVS_FACTOR_HIGH        (8u)
#define I2COLED_I2C_MEDIAN_FILTER_ENABLE   (1u)
#define I2COLED_I2C_SLAVE_ADDRESS          (8u)
#define I2COLED_I2C_SLAVE_ADDRESS_MASK     (254u)
#define I2COLED_I2C_ACCEPT_ADDRESS         (0u)
#define I2COLED_I2C_WAKE_ENABLE            (0u)
#define I2COLED_I2C_DATA_RATE              (400u)
#define I2COLED_I2C_DATA_RATE_ACTUAL       (381u)
#define I2COLED_I2C_CLOCK_FROM_TERM        (0u)
#define I2COLED_I2C_EXTERN_INTR_HANDLER    (0u)
#define I2COLED_I2C_BYTE_MODE_ENABLE       (0u)
#define I2COLED_I2C_MANUAL_OVS_CONTROL     (0u)


/***************************************
*  Conditional Compilation Parameters
****************************************/

/* I2C sub mode enum */
#define I2COLED_I2C_MODE_SLAVE              (0x01u)
#define I2COLED_I2C_MODE_MASTER             (0x02u)
#define I2COLED_I2C_MODE_MULTI_MASTER       (0x06u)
#define I2COLED_I2C_MODE_MULTI_MASTER_SLAVE (0x07u)
#define I2COLED_I2C_MODE_MULTI_MASTER_MASK  (0x04u)

#if(I2COLED_SCB_MODE_UNCONFIG_CONST_CFG)

    /* Returns true if slave mode is enabled */
    #define I2COLED_I2C_SLAVE  (0u != (I2COLED_I2C_MODE_SLAVE & I2COLED_mode))

    /* Returns true if master mode is enabled */
    #define I2COLED_I2C_MASTER (0u != (I2COLED_I2C_MODE_MASTER & I2COLED_mode))

    /* Returns true if multi-master mode is enabled */
    #define I2COLED_I2C_MULTI_MASTER \
                            (0u != (I2COLED_I2C_MODE_MULTI_MASTER_MASK & I2COLED_mode))

    /* Returns true if mode is multi-master-slave */
    #define I2COLED_I2C_MULTI_MASTER_SLAVE \
                            (I2COLED_I2C_MODE_MULTI_MASTER_SLAVE == I2COLED_mode)

    /* Returns true if address accepts in RX FIFO */
    #define I2COLED_CHECK_I2C_ACCEPT_ADDRESS   (0u != I2COLED_acceptAddr)

    #define I2COLED_I2C_WAKE_ENABLE_CONST          (1u)
    #define I2COLED_I2C_SLAVE_CONST                (1u)
    #define I2COLED_I2C_MASTER_CONST               (1u)
    #define I2COLED_I2C_MULTI_MASTER_SLAVE_CONST   (1u)
    #define I2COLED_CHECK_I2C_ACCEPT_ADDRESS_CONST (1u)

    /* Returns FIFO size */
    #if (I2COLED_CY_SCBIP_V0 || I2COLED_CY_SCBIP_V1)
        #define I2COLED_I2C_FIFO_SIZE      (I2COLED_FIFO_SIZE)
    #else
        #define I2COLED_I2C_FIFO_SIZE      (I2COLED_GET_FIFO_SIZE(I2COLED_CTRL_REG & \
                                                                                    I2COLED_CTRL_BYTE_MODE))
    #endif /* (I2COLED_CY_SCBIP_V0 || I2COLED_CY_SCBIP_V1) */

#else

    /* Returns true if slave mode is enabled */
    #define I2COLED_I2C_SLAVE   (0u != (I2COLED_I2C_MODE_SLAVE & I2COLED_I2C_MODE))

    /* Returns true if master mode is enabled */
    #define I2COLED_I2C_MASTER  (0u != (I2COLED_I2C_MODE_MASTER & I2COLED_I2C_MODE))

    /* Returns true if multi-master mode is enabled */
    #define I2COLED_I2C_MULTI_MASTER \
                                    (0u != (I2COLED_I2C_MODE_MULTI_MASTER_MASK & I2COLED_I2C_MODE))

    /* Returns true if mode is multi-master-slave */
    #define I2COLED_I2C_MULTI_MASTER_SLAVE \
                                    (I2COLED_I2C_MODE_MULTI_MASTER_SLAVE == I2COLED_I2C_MODE)

    /* Returns true if address accepts in RX FIFO */
    #define I2COLED_CHECK_I2C_ACCEPT_ADDRESS   (0u != I2COLED_I2C_ACCEPT_ADDRESS)

    /* Returns true if wakeup on address match is enabled */
    #define I2COLED_I2C_WAKE_ENABLE_CONST  (0u != I2COLED_I2C_WAKE_ENABLE)

    #define I2COLED_I2C_SLAVE_CONST    (I2COLED_I2C_SLAVE)
    #define I2COLED_I2C_MASTER_CONST   (I2COLED_I2C_MASTER)
    #define I2COLED_I2C_MULTI_MASTER_SLAVE_CONST (I2COLED_I2C_MULTI_MASTER_SLAVE)
    #define I2COLED_CHECK_I2C_ACCEPT_ADDRESS_CONST (I2COLED_CHECK_I2C_ACCEPT_ADDRESS)

    /* I2C: TX or RX FIFO size */
    #if (I2COLED_CY_SCBIP_V0 || I2COLED_CY_SCBIP_V1)
        #define I2COLED_I2C_FIFO_SIZE  (I2COLED_FIFO_SIZE)
    #else
        #define I2COLED_I2C_FIFO_SIZE  I2COLED_GET_FIFO_SIZE(I2COLED_I2C_BYTE_MODE_ENABLE)
    #endif /* ((I2COLED_CY_SCBIP_V0 || I2COLED_CY_SCBIP_V1) */

    /* Adjust AF and DF filter settings. Ticket ID#176179 */
    #if ((I2COLED_I2C_MODE_SLAVE == I2COLED_I2C_MODE) ||     \
            ((I2COLED_I2C_MODE_SLAVE != I2COLED_I2C_MODE) && \
             (I2COLED_I2C_DATA_RATE_ACTUAL <= I2COLED_I2C_DATA_RATE_FS_MODE_MAX)))

        #define I2COLED_I2C_MEDIAN_FILTER_ENABLE_ADJ       (0u)
        #define I2COLED_I2C_CFG_ANALOG_FITER_ENABLE_ADJ    do{;}while(0)
    #else
        #define I2COLED_I2C_MEDIAN_FILTER_ENABLE_ADJ       (1u)
        #define I2COLED_I2C_CFG_ANALOG_FITER_ENABLE_ADJ    I2COLED_I2C_CFG_ANALOG_FITER_DISABLE
    #endif

    /* Select oversampling factor low and high */
    #define I2COLED_I2C_OVS_FACTOR_LOW_MIN     ((0u != I2COLED_I2C_MANUAL_OVS_CONTROL) ? \
                                                            (8u) : (13u))

    #define I2COLED_I2C_OVS_FACTOR_HIGH_MIN    ((0u != I2COLED_I2C_MANUAL_OVS_CONTROL) ? \
                                                            (8u) : (8u))

#endif /* (I2COLED_SCB_MODE_UNCONFIG_CONST_CFG) */


/***************************************
*       Type Definitions
***************************************/

typedef struct
{
    uint32 mode;
    uint32 oversampleLow;
    uint32 oversampleHigh;
    uint32 enableMedianFilter;
    uint32 slaveAddr;
    uint32 slaveAddrMask;
    uint32 acceptAddr;
    uint32 enableWake;
    uint8  enableByteMode;
    uint16 dataRate;
} I2COLED_I2C_INIT_STRUCT;


/***************************************
*        Function Prototypes
***************************************/

/* Common functions */
#if(I2COLED_SCB_MODE_UNCONFIG_CONST_CFG)
    void I2COLED_I2CInit(const I2COLED_I2C_INIT_STRUCT *config);
#endif /* (I2COLED_SCB_MODE_UNCONFIG_CONST_CFG) */

/* I2C Master functions prototypes */
#if(I2COLED_I2C_MASTER_CONST)
    /* Read and Clear status functions */
    uint32 I2COLED_I2CMasterStatus(void);
    uint32 I2COLED_I2CMasterClearStatus(void);

    /* Interrupt based operation functions */
    uint32 I2COLED_I2CMasterWriteBuf(uint32 slaveAddress, uint8 * wrData, uint32 cnt, uint32 mode);
    uint32 I2COLED_I2CMasterReadBuf(uint32 slaveAddress, uint8 * rdData, uint32 cnt, uint32 mode);
    uint32 I2COLED_I2CMasterGetReadBufSize(void);
    uint32 I2COLED_I2CMasterGetWriteBufSize(void);
    void   I2COLED_I2CMasterClearReadBuf(void);
    void   I2COLED_I2CMasterClearWriteBuf(void);

    /* Manual operation functions */
    uint32 I2COLED_I2CMasterSendStart(uint32 slaveAddress, uint32 bitRnW);
    uint32 I2COLED_I2CMasterSendRestart(uint32 slaveAddress, uint32 bitRnW);
    uint32 I2COLED_I2CMasterSendStop(void);
    uint32 I2COLED_I2CMasterWriteByte(uint32 theByte);
    uint32 I2COLED_I2CMasterReadByte(uint32 ackNack);
#endif /* (I2COLED_I2C_MASTER_CONST) */

/* I2C Slave functions prototypes */
#if(I2COLED_I2C_SLAVE_CONST)
    /* Read and Clear status functions */
    uint32 I2COLED_I2CSlaveStatus(void);
    uint32 I2COLED_I2CSlaveClearReadStatus(void);
    uint32 I2COLED_I2CSlaveClearWriteStatus(void);

    /* Set Slave address and mask */
    void   I2COLED_I2CSlaveSetAddress(uint32 address);
    void   I2COLED_I2CSlaveSetAddressMask(uint32 addressMask);

    /* Interrupt based operation functions */
    void   I2COLED_I2CSlaveInitReadBuf(uint8 * rdBuf, uint32 bufSize);
    void   I2COLED_I2CSlaveInitWriteBuf(uint8 * wrBuf, uint32 bufSize);
    uint32 I2COLED_I2CSlaveGetReadBufSize(void);
    uint32 I2COLED_I2CSlaveGetWriteBufSize(void);
    void   I2COLED_I2CSlaveClearReadBuf(void);
    void   I2COLED_I2CSlaveClearWriteBuf(void);
#endif /* (I2COLED_I2C_SLAVE_CONST) */

CY_ISR_PROTO(I2COLED_I2C_ISR);


/***************************************
*            API Constants
***************************************/

/* I2C sub mode enum */
#define I2COLED_I2C_MODE_SLAVE              (0x01u)
#define I2COLED_I2C_MODE_MASTER             (0x02u)
#define I2COLED_I2C_MODE_MULTI_MASTER       (0x06u)
#define I2COLED_I2C_MODE_MULTI_MASTER_SLAVE (0x07u)
#define I2COLED_I2C_MODE_MULTI_MASTER_MASK  (0x04u)

/* Master/Slave control constants */
#define I2COLED_I2C_WRITE_XFER_MODE    (0u)    /* Write    */
#define I2COLED_I2C_READ_XFER_MODE     (1u)    /* Read     */
#define I2COLED_I2C_ACK_DATA           (0u)    /* Send ACK */
#define I2COLED_I2C_NAK_DATA           (1u)    /* Send NAK */

/* "Mode" constants for MasterWriteBuf() or MasterReadBuf() function */
#define I2COLED_I2C_MODE_COMPLETE_XFER     (0x00u)    /* Full transfer with Start and Stop       */
#define I2COLED_I2C_MODE_REPEAT_START      (0x01u)    /* Begin with a ReStart instead of a Start */
#define I2COLED_I2C_MODE_NO_STOP           (0x02u)    /* Complete the transfer without a Stop    */

/* Master status */
#define I2COLED_I2C_MSTAT_CLEAR            ((uint16) 0x00u)   /* Clear (init) status value */

#define I2COLED_I2C_MSTAT_RD_CMPLT         ((uint16) 0x01u)   /* Read complete               */
#define I2COLED_I2C_MSTAT_WR_CMPLT         ((uint16) 0x02u)   /* Write complete              */
#define I2COLED_I2C_MSTAT_XFER_INP         ((uint16) 0x04u)   /* Master transfer in progress */
#define I2COLED_I2C_MSTAT_XFER_HALT        ((uint16) 0x08u)   /* Transfer is halted          */

#define I2COLED_I2C_MSTAT_ERR_MASK         ((uint16) 0x3F0u) /* Mask for all errors                          */
#define I2COLED_I2C_MSTAT_ERR_SHORT_XFER   ((uint16) 0x10u)  /* Master NAKed before end of packet            */
#define I2COLED_I2C_MSTAT_ERR_ADDR_NAK     ((uint16) 0x20u)  /* Slave did not ACK                            */
#define I2COLED_I2C_MSTAT_ERR_ARB_LOST     ((uint16) 0x40u)  /* Master lost arbitration during communication */
#define I2COLED_I2C_MSTAT_ERR_ABORT_XFER   ((uint16) 0x80u)  /* The Slave was addressed before the Start gen */
#define I2COLED_I2C_MSTAT_ERR_BUS_ERROR    ((uint16) 0x100u) /* The misplaced Start or Stop was occurred     */
#define I2COLED_I2C_MSTAT_ERR_XFER         ((uint16) 0x200u) /* Error during transfer                        */

/* Master API returns */
#define I2COLED_I2C_MSTR_NO_ERROR          (0x00u)  /* Function complete without error                       */
#define I2COLED_I2C_MSTR_ERR_ARB_LOST      (0x01u)  /* Master lost arbitration: INTR_MASTER_I2C_ARB_LOST     */
#define I2COLED_I2C_MSTR_ERR_LB_NAK        (0x02u)  /* Last Byte Naked: INTR_MASTER_I2C_NACK                 */
#define I2COLED_I2C_MSTR_NOT_READY         (0x04u)  /* Master on the bus or Slave operation is in progress   */
#define I2COLED_I2C_MSTR_BUS_BUSY          (0x08u)  /* Bus is busy, process not started                      */
#define I2COLED_I2C_MSTR_ERR_ABORT_START   (0x10u)  /* Slave was addressed before master begin Start gen     */
#define I2COLED_I2C_MSTR_ERR_BUS_ERR       (0x100u) /* Bus error has: INTR_MASTER_I2C_BUS_ERROR              */

/* Slave Status Constants */
#define I2COLED_I2C_SSTAT_RD_CMPLT         (0x01u)    /* Read transfer complete                        */
#define I2COLED_I2C_SSTAT_RD_BUSY          (0x02u)    /* Read transfer in progress                     */
#define I2COLED_I2C_SSTAT_RD_OVFL          (0x04u)    /* Read overflow: master reads above buffer size */
#define I2COLED_I2C_SSTAT_RD_ERR           (0x08u)    /* Read was interrupted by misplaced Start/Stop  */
#define I2COLED_I2C_SSTAT_RD_MASK          (0x0Fu)    /* Read Status Mask                              */
#define I2COLED_I2C_SSTAT_RD_NO_ERR        (0x00u)    /* Read no Error                                 */

#define I2COLED_I2C_SSTAT_WR_CMPLT         (0x10u)    /* Write transfer complete                         */
#define I2COLED_I2C_SSTAT_WR_BUSY          (0x20u)    /* Write transfer in progress                      */
#define I2COLED_I2C_SSTAT_WR_OVFL          (0x40u)    /* Write overflow: master writes above buffer size */
#define I2COLED_I2C_SSTAT_WR_ERR           (0x80u)    /* Write was interrupted by misplaced Start/Stop   */
#define I2COLED_I2C_SSTAT_WR_MASK          (0xF0u)    /* Write Status Mask                               */
#define I2COLED_I2C_SSTAT_WR_NO_ERR        (0x00u)    /* Write no Error                                  */

#define I2COLED_I2C_SSTAT_RD_CLEAR         (0x0Du)    /* Read Status clear: do not clear */
#define I2COLED_I2C_SSTAT_WR_CLEAR         (0xD0u)    /* Write Status Clear */

/* Internal I2C component constants */
#define I2COLED_I2C_READ_FLAG              (0x01u)     /* Read flag of the Address */
#define I2COLED_I2C_SLAVE_OVFL_RETURN      (0xFFu)     /* Return by slave when overflow */

#define I2COLED_I2C_RESET_ERROR            (0x01u)     /* Flag to re-enable SCB IP */


/***************************************
*     Vars with External Linkage
***************************************/

#if(I2COLED_SCB_MODE_UNCONFIG_CONST_CFG)
    extern const I2COLED_I2C_INIT_STRUCT I2COLED_configI2C;
#endif /* (I2COLED_SCB_MODE_UNCONFIG_CONST_CFG) */


/***************************************
*           FSM states
***************************************/

#define I2COLED_I2C_FSM_EXIT_IDLE      (0x00u) /* Master and Slave are not active, need to exit to IDLE */

/* Slave mode FSM states */
#define I2COLED_I2C_FSM_IDLE           (0x10u) /* Idle I2C state                */
#define I2COLED_I2C_FSM_SLAVE          (0x10u) /* Slave mask for all its states */

#define I2COLED_I2C_FSM_SL_WR          (0x11u) /* Slave write states */
#define I2COLED_I2C_FSM_SL_RD          (0x12u) /* Slave read  states */

/* Master mode FSM states */
#define I2COLED_I2C_FSM_MASTER         (0x20u) /* Master mask for all its states */
#define I2COLED_I2C_FSM_MSTR_ADDR      (0x08u) /* Master address phase           */
#define I2COLED_I2C_FSM_MSTR_DATA      (0x04u) /* Master data phase              */
#define I2COLED_I2C_FSM_MSTR_RD        (0x01u) /* Master read phase              */

#define I2COLED_I2C_FSM_MSTR_WR_ADDR   (0x28u) /* FSM master transmit address with write          */
#define I2COLED_I2C_FSM_MSTR_RD_ADDR   (0x29u) /* FSM master transmit address with read           */
#define I2COLED_I2C_FSM_MSTR_WR_DATA   (0x24u) /* FSM master writes data into the slave           */
#define I2COLED_I2C_FSM_MSTR_RD_DATA   (0x25u) /* FSM master reads data from the slave            */
#define I2COLED_I2C_FSM_MSTR_HALT      (0x60u) /* FSM master halt state: wait for Stop or ReStart */

/* Requests to complete any on-going transfer */
#define I2COLED_I2C_CMPLT_ANY_TRANSFER     (0x01u)

/* Returns true if FSM is in any Master state */
#define I2COLED_CHECK_I2C_FSM_MASTER   (0u != (I2COLED_I2C_FSM_MASTER & I2COLED_state))

/* Returns true if FSM is in any Slave state */
#define I2COLED_CHECK_I2C_FSM_SLAVE    (0u != (I2COLED_I2C_FSM_SLAVE  & I2COLED_state))

/* Returns true if FSM is in Master send address state */
#define I2COLED_CHECK_I2C_FSM_ADDR (0u != (I2COLED_I2C_FSM_MSTR_ADDR & I2COLED_state))

/* Returns true if FSM is in Master send or receive data state */
#define I2COLED_CHECK_I2C_FSM_DATA (0u != (I2COLED_I2C_FSM_MSTR_DATA  & I2COLED_state))

/* Returns true if FSM is in any of read states: applied for Slave and Master */
#define I2COLED_CHECK_I2C_FSM_RD   (0u != (I2COLED_I2C_FSM_MSTR_RD  & I2COLED_state))

/* Returns true if FSM is in IDLE state */
#define I2COLED_CHECK_I2C_FSM_IDLE (I2COLED_I2C_FSM_IDLE == I2COLED_state)

/* Returns true if FSM is HALT state */
#define I2COLED_CHECK_I2C_FSM_HALT (I2COLED_I2C_FSM_MSTR_HALT == I2COLED_state)

/* Set Master read or write completion depends on state */
#define I2COLED_GET_I2C_MSTAT_CMPLT (I2COLED_CHECK_I2C_FSM_RD ?           \
                                                    (I2COLED_I2C_MSTAT_RD_CMPLT) : \
                                                    (I2COLED_I2C_MSTAT_WR_CMPLT))


/***************************************
*       Macro Definitions
***************************************/

/* Returns TRUE if sourceMask is set in I2COLED_I2C_MASTER_CMD_REG: used to check if Start was generated */
#define I2COLED_CHECK_I2C_MASTER_CMD(sourceMask)   (0u != (I2COLED_I2C_MASTER_CMD_REG & (sourceMask)))

/* Returns TRUE if I2COLED_MODE_NO_STOP is set in I2COLED_mstrControl: detects NoStop condition */
#define I2COLED_CHECK_I2C_MODE_NO_STOP(mode)   (0u != (I2COLED_I2C_MODE_NO_STOP & (mode)))

/* Returns TRUE if I2COLED_MODE_REPEAT_START is set: used to detect when generate ReStart condition */
#define I2COLED_CHECK_I2C_MODE_RESTART(mode)   (0u != (I2COLED_I2C_MODE_REPEAT_START  & (mode)))

/* Returns TRUE if I2COLED_state is in one of master states */
#define I2COLED_CHECK_I2C_MASTER_ACTIVE    (I2COLED_CHECK_I2C_FSM_MASTER)


/***************************************
*      Common Register Settings
***************************************/

#define I2COLED_CTRL_I2C       (I2COLED_CTRL_MODE_I2C)

#define I2COLED_I2C_CTRL       (I2COLED_I2C_CTRL_S_GENERAL_IGNORE)

#define I2COLED_I2C_RX_CTRL    ((I2COLED_FIFO_SIZE - 1u)  | \
                                         I2COLED_RX_CTRL_MSB_FIRST | \
                                         I2COLED_RX_CTRL_ENABLED)

#define I2COLED_I2C_TX_CTRL    ((I2COLED_FIFO_SIZE - 1u)  | \
                                         I2COLED_TX_CTRL_MSB_FIRST | \
                                         I2COLED_TX_CTRL_ENABLED)

#define I2COLED_I2C_INTR_SLAVE_MASK    (I2COLED_INTR_SLAVE_I2C_ADDR_MATCH | \
                                                 I2COLED_INTR_SLAVE_I2C_NACK       | \
                                                 I2COLED_INTR_SLAVE_I2C_WRITE_STOP | \
                                                 I2COLED_INTR_SLAVE_I2C_BUS_ERROR  | \
                                                 I2COLED_INTR_SLAVE_I2C_ARB_LOST)

#define I2COLED_I2C_INTR_MASTER_MASK   (I2COLED_INTR_MASTER_I2C_ARB_LOST | \
                                                 I2COLED_INTR_MASTER_I2C_NACK     | \
                                                 I2COLED_INTR_MASTER_I2C_STOP     | \
                                                 I2COLED_INTR_MASTER_I2C_BUS_ERROR)

/* Calculates tLOW in uS units */
#define I2COLED_I2C_TLOW_TIME  ((1000u / I2COLED_I2C_DATA_RATE) + \
                                            ((0u != (1000u % I2COLED_I2C_DATA_RATE)) ? (1u) : (0u)))
/* tHIGH = tLOW */
#define I2COLED_I2C_THIGH_TIME (I2COLED_I2C_TLOW_TIME)

#define I2COLED_I2C_SCL_LOW    (0u)
#define I2COLED_I2C_SCL_HIGH   (1u)


/***************************************
*    Initialization Register Settings
***************************************/

#if(I2COLED_SCB_MODE_I2C_CONST_CFG)

    #if (!I2COLED_CY_SCBIP_V0)
        #define I2COLED_I2C_WAKE_ENABLE_ADJ  (I2COLED_I2C_MULTI_MASTER_SLAVE ? \
                                                            (0u) : (I2COLED_I2C_WAKE_ENABLE))
    #else
        #define I2COLED_I2C_WAKE_ENABLE_ADJ    (I2COLED_I2C_WAKE_ENABLE)
    #endif /* (!I2COLED_CY_SCBIP_V0) */

    #define I2COLED_I2C_MODE_MASKED    (I2COLED_I2C_MODE & \
                                                (I2COLED_I2C_MODE_SLAVE | I2COLED_I2C_MODE_MASTER))

    #define I2COLED_I2C_DEFAULT_CTRL \
                                (I2COLED_GET_CTRL_BYTE_MODE  (I2COLED_I2C_BYTE_MODE_ENABLE) | \
                                 I2COLED_GET_CTRL_ADDR_ACCEPT(I2COLED_I2C_ACCEPT_ADDRESS)   | \
                                 I2COLED_GET_CTRL_EC_AM_MODE (I2COLED_I2C_WAKE_ENABLE_ADJ))

    #define I2COLED_I2C_DEFAULT_I2C_CTRL \
                            (I2COLED_GET_I2C_CTRL_HIGH_PHASE_OVS(I2COLED_I2C_OVS_FACTOR_HIGH_MIN) | \
                             I2COLED_GET_I2C_CTRL_LOW_PHASE_OVS (I2COLED_I2C_OVS_FACTOR_LOW_MIN)  | \
                             I2COLED_GET_I2C_CTRL_SL_MSTR_MODE  (I2COLED_I2C_MODE_MASKED)         | \
                             I2COLED_I2C_CTRL)

    #define I2COLED_I2C_DEFAULT_RX_MATCH ((I2COLED_I2C_SLAVE) ? \
                                (I2COLED_GET_I2C_8BIT_ADDRESS(I2COLED_I2C_SLAVE_ADDRESS) | \
                                 I2COLED_GET_RX_MATCH_MASK   (I2COLED_I2C_SLAVE_ADDRESS_MASK)) : (0u))

    #define I2COLED_I2C_DEFAULT_RX_CTRL \
                                (I2COLED_GET_RX_CTRL_MEDIAN(I2COLED_I2C_MEDIAN_FILTER_ENABLE_ADJ) | \
                                 I2COLED_I2C_RX_CTRL)

    #define I2COLED_I2C_DEFAULT_TX_CTRL  (I2COLED_I2C_TX_CTRL)

    #define I2COLED_I2C_DEFAULT_RX_FIFO_CTRL (0u)
    #define I2COLED_I2C_DEFAULT_TX_FIFO_CTRL (0u)

    /* Interrupt sources */
    #define I2COLED_I2C_DEFAULT_INTR_I2C_EC_MASK   (I2COLED_NO_INTR_SOURCES)
    #define I2COLED_I2C_DEFAULT_INTR_SPI_EC_MASK   (I2COLED_NO_INTR_SOURCES)
    #define I2COLED_I2C_DEFAULT_INTR_RX_MASK       (I2COLED_NO_INTR_SOURCES)
    #define I2COLED_I2C_DEFAULT_INTR_TX_MASK       (I2COLED_NO_INTR_SOURCES)

    #define I2COLED_I2C_DEFAULT_INTR_SLAVE_MASK    ((I2COLED_I2C_SLAVE) ? \
                                                                (I2COLED_I2C_INTR_SLAVE_MASK) : (0u))

    #define I2COLED_I2C_DEFAULT_INTR_MASTER_MASK   ((I2COLED_I2C_MASTER) ? \
                                                                (I2COLED_I2C_INTR_MASTER_MASK) : (0u))

#endif /* (I2COLED_SCB_MODE_I2C_CONST_CFG) */

#endif /* (CY_SCB_I2C_I2COLED_H) */


/* [] END OF FILE */
