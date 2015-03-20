/*******************************************************************************
* File Name: I2COLED_BOOT.h
* Version 2.0
*
* Description:
*  This file provides constants and parameter values for the bootloader
*  communication interface of SCB component.
*
* Note:
*
********************************************************************************
* Copyright 2014, Cypress Semiconductor Corporation. All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_SCB_BOOT_I2COLED_H)
#define CY_SCB_BOOT_I2COLED_H

#include "I2COLED_PVT.h"

#if (I2COLED_SCB_MODE_I2C_INC)
    #include "I2COLED_I2C.h"
#endif /* (I2COLED_SCB_MODE_I2C_INC) */

#if (I2COLED_SCB_MODE_EZI2C_INC)
    #include "I2COLED_EZI2C.h"
#endif /* (I2COLED_SCB_MODE_EZI2C_INC) */

#if (I2COLED_SCB_MODE_SPI_INC || I2COLED_SCB_MODE_UART_INC)
    #include "I2COLED_SPI_UART.h"
#endif /* (I2COLED_SCB_MODE_SPI_INC || I2COLED_SCB_MODE_UART_INC) */


/***************************************
*        Function Prototypes
***************************************/

/* Bootloader communication interface enable */
#define I2COLED_BTLDR_COMM_ENABLED ((CYDEV_BOOTLOADER_IO_COMP == CyBtldr_I2COLED) || \
                                             (CYDEV_BOOTLOADER_IO_COMP == CyBtldr_Custom_Interface))

#if (I2COLED_SCB_MODE_I2C_INC)

    #define I2COLED_I2C_BTLDR_COMM_ENABLED     (I2COLED_BTLDR_COMM_ENABLED && \
                                                            (I2COLED_SCB_MODE_UNCONFIG_CONST_CFG || \
                                                             I2COLED_I2C_SLAVE_CONST))

#if defined(CYDEV_BOOTLOADER_IO_COMP) && (I2COLED_I2C_BTLDR_COMM_ENABLED)
    /* Bootloader physical layer functions */
    void I2COLED_I2CCyBtldrCommStart(void);
    void I2COLED_I2CCyBtldrCommStop (void);
    void I2COLED_I2CCyBtldrCommReset(void);
    cystatus I2COLED_I2CCyBtldrCommRead       (uint8 pData[], uint16 size, uint16 * count, uint8 timeOut);
    cystatus I2COLED_I2CCyBtldrCommWrite(const uint8 pData[], uint16 size, uint16 * count, uint8 timeOut);

    /* Size of Read/Write buffers for I2C bootloader  */
    #define I2COLED_I2C_BTLDR_SIZEOF_READ_BUFFER   (64u)
    #define I2COLED_I2C_BTLDR_SIZEOF_WRITE_BUFFER  (64u)
    #define I2COLED_I2C_MIN_UINT16(a, b)           ( ((uint16)(a) < (uint16) (b)) ? \
                                                                    ((uint32) (a)) : ((uint32) (b)) )
    #define I2COLED_WAIT_1_MS                      (1u)
#endif /* defined(CYDEV_BOOTLOADER_IO_COMP) && (I2COLED_I2C_BTLDR_COMM_ENABLED) */

#endif /* (I2COLED_SCB_MODE_I2C_INC) */


#if (I2COLED_SCB_MODE_EZI2C_INC)

    /* Provide EMPTY bootloader communication functions. EZI2C is NOT supported yet */
    #define I2COLED_EZI2C_BTLDR_COMM_ENABLED   (I2COLED_BTLDR_COMM_ENABLED && \
                                                         I2COLED_SCB_MODE_UNCONFIG_CONST_CFG)

#if defined(CYDEV_BOOTLOADER_IO_COMP) && (I2COLED_EZI2C_BTLDR_COMM_ENABLED)
    /* Bootloader physical layer functions */
    void I2COLED_EzI2CCyBtldrCommStart(void);
    void I2COLED_EzI2CCyBtldrCommStop (void);
    void I2COLED_EzI2CCyBtldrCommReset(void);
    cystatus I2COLED_EzI2CCyBtldrCommRead       (uint8 pData[], uint16 size, uint16 * count, uint8 timeOut);
    cystatus I2COLED_EzI2CCyBtldrCommWrite(const uint8 pData[], uint16 size, uint16 * count, uint8 timeOut);
#endif /* defined(CYDEV_BOOTLOADER_IO_COMP) && (I2COLED_EZI2C_BTLDR_COMM_ENABLED) */

#endif /* (I2COLED_SCB_MODE_EZI2C_INC) */

#if (I2COLED_SCB_MODE_SPI_INC || I2COLED_SCB_MODE_UART_INC)
    /* Provide EMPTY bootloader communication functions. SPI and UART is NOT supported yet */
    #define I2COLED_SPI_BTLDR_COMM_ENABLED     (I2COLED_BTLDR_COMM_ENABLED && \
                                                        I2COLED_SCB_MODE_UNCONFIG_CONST_CFG)

    #define I2COLED_UART_BTLDR_COMM_ENABLED    (I2COLED_BTLDR_COMM_ENABLED && \
                                                        I2COLED_SCB_MODE_UNCONFIG_CONST_CFG)

#if defined(CYDEV_BOOTLOADER_IO_COMP) && (I2COLED_SPI_BTLDR_COMM_ENABLED)
    /* SPI Bootloader physical layer functions */
    void I2COLED_SpiCyBtldrCommStart(void);
    void I2COLED_SpiCyBtldrCommStop (void);
    void I2COLED_SpiCyBtldrCommReset(void);
    cystatus I2COLED_SpiCyBtldrCommRead       (uint8 pData[], uint16 size, uint16 * count, uint8 timeOut);
    cystatus I2COLED_SpiCyBtldrCommWrite(const uint8 pData[], uint16 size, uint16 * count, uint8 timeOut);
#endif /* defined(CYDEV_BOOTLOADER_IO_COMP) && (I2COLED_SPI_BTLDR_COMM_ENABLED) */

#if defined(CYDEV_BOOTLOADER_IO_COMP) && (I2COLED_UART_BTLDR_COMM_ENABLED)
    /* UART Bootloader physical layer functions */
    void I2COLED_UartCyBtldrCommStart(void);
    void I2COLED_UartCyBtldrCommStop (void);
    void I2COLED_UartCyBtldrCommReset(void);
    cystatus I2COLED_UartCyBtldrCommRead       (uint8 pData[], uint16 size, uint16 * count, uint8 timeOut);
    cystatus I2COLED_UartCyBtldrCommWrite(const uint8 pData[], uint16 size, uint16 * count, uint8 timeOut);
#endif /* defined(CYDEV_BOOTLOADER_IO_COMP) && (I2COLED_UART_BTLDR_COMM_ENABLED) */

#endif /* (I2COLED_SCB_MODE_SPI_INC || I2COLED_SCB_MODE_UART_INC) */

#if !defined (I2COLED_I2C_BTLDR_COMM_ENABLED)
    #define I2COLED_I2C_BTLDR_COMM_ENABLED     (0u)
#endif /* (I2COLED_I2C_BTLDR_COMM_ENABLED) */

#if !defined (I2COLED_EZI2C_BTLDR_COMM_ENABLED)
    #define I2COLED_EZI2C_BTLDR_COMM_ENABLED   (0u)
#endif /* (I2COLED_EZI2C_BTLDR_COMM_ENABLED) */

#if !defined (I2COLED_SPI_BTLDR_COMM_ENABLED)
    #define I2COLED_SPI_BTLDR_COMM_ENABLED     (0u)
#endif /* (I2COLED_SPI_BTLDR_COMM_ENABLED) */

#if !defined (I2COLED_UART_BTLDR_COMM_ENABLED)
    #define I2COLED_UART_BTLDR_COMM_ENABLED    (0u)
#endif /* (I2COLED_UART_BTLDR_COMM_ENABLED) */

/* Bootloader enabled condition for each mode */
#define I2COLED_BTLDR_COMM_MODE_ENABLED    (I2COLED_I2C_BTLDR_COMM_ENABLED   || \
                                                     I2COLED_EZI2C_BTLDR_COMM_ENABLED || \
                                                     I2COLED_SPI_BTLDR_COMM_ENABLED   || \
                                                     I2COLED_UART_BTLDR_COMM_ENABLED)

#if defined(CYDEV_BOOTLOADER_IO_COMP) && (I2COLED_BTLDR_COMM_ENABLED)
    #if (I2COLED_BTLDR_COMM_MODE_ENABLED)
        /* Bootloader physical layer functions */
        void I2COLED_CyBtldrCommStart(void);
        void I2COLED_CyBtldrCommStop (void);
        void I2COLED_CyBtldrCommReset(void);
        cystatus I2COLED_CyBtldrCommRead       (uint8 pData[], uint16 size, uint16 * count, uint8 timeOut);
        cystatus I2COLED_CyBtldrCommWrite(const uint8 pData[], uint16 size, uint16 * count, uint8 timeOut);
    #endif /* (I2COLED_BTLDR_COMM_MODE_ENABLED) */

    #if (CYDEV_BOOTLOADER_IO_COMP == CyBtldr_I2COLED)
        #define CyBtldrCommStart    I2COLED_CyBtldrCommStart
        #define CyBtldrCommStop     I2COLED_CyBtldrCommStop
        #define CyBtldrCommReset    I2COLED_CyBtldrCommReset
        #define CyBtldrCommWrite    I2COLED_CyBtldrCommWrite
        #define CyBtldrCommRead     I2COLED_CyBtldrCommRead
    #endif /* (CYDEV_BOOTLOADER_IO_COMP == CyBtldr_I2COLED) */
#endif /* defined(CYDEV_BOOTLOADER_IO_COMP) && (I2COLED_BTLDR_COMM_ENABLED) */

#endif /* (CY_SCB_BOOT_I2COLED_H) */

/* [] END OF FILE */
