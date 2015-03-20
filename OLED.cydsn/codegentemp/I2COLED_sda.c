/*******************************************************************************
* File Name: I2COLED_sda.c  
* Version 2.10
*
* Description:
*  This file contains API to enable firmware control of a Pins component.
*
* Note:
*
********************************************************************************
* Copyright 2008-2014, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#include "cytypes.h"
#include "I2COLED_sda.h"

#define SetP4PinDriveMode(shift, mode)  \
    do { \
        I2COLED_sda_PC =   (I2COLED_sda_PC & \
                                (uint32)(~(uint32)(I2COLED_sda_DRIVE_MODE_IND_MASK << (I2COLED_sda_DRIVE_MODE_BITS * (shift))))) | \
                                (uint32)((uint32)(mode) << (I2COLED_sda_DRIVE_MODE_BITS * (shift))); \
    } while (0)


/*******************************************************************************
* Function Name: I2COLED_sda_Write
********************************************************************************
*
* Summary:
*  Assign a new value to the digital port's data output register.  
*
* Parameters:  
*  prtValue:  The value to be assigned to the Digital Port. 
*
* Return: 
*  None 
*  
*******************************************************************************/
void I2COLED_sda_Write(uint8 value) 
{
    uint8 drVal = (uint8)(I2COLED_sda_DR & (uint8)(~I2COLED_sda_MASK));
    drVal = (drVal | ((uint8)(value << I2COLED_sda_SHIFT) & I2COLED_sda_MASK));
    I2COLED_sda_DR = (uint32)drVal;
}


/*******************************************************************************
* Function Name: I2COLED_sda_SetDriveMode
********************************************************************************
*
* Summary:
*  Change the drive mode on the pins of the port.
* 
* Parameters:  
*  mode:  Change the pins to one of the following drive modes.
*
*  I2COLED_sda_DM_STRONG     Strong Drive 
*  I2COLED_sda_DM_OD_HI      Open Drain, Drives High 
*  I2COLED_sda_DM_OD_LO      Open Drain, Drives Low 
*  I2COLED_sda_DM_RES_UP     Resistive Pull Up 
*  I2COLED_sda_DM_RES_DWN    Resistive Pull Down 
*  I2COLED_sda_DM_RES_UPDWN  Resistive Pull Up/Down 
*  I2COLED_sda_DM_DIG_HIZ    High Impedance Digital 
*  I2COLED_sda_DM_ALG_HIZ    High Impedance Analog 
*
* Return: 
*  None
*
*******************************************************************************/
void I2COLED_sda_SetDriveMode(uint8 mode) 
{
	SetP4PinDriveMode(I2COLED_sda__0__SHIFT, mode);
}


/*******************************************************************************
* Function Name: I2COLED_sda_Read
********************************************************************************
*
* Summary:
*  Read the current value on the pins of the Digital Port in right justified 
*  form.
*
* Parameters:  
*  None 
*
* Return: 
*  Returns the current value of the Digital Port as a right justified number
*  
* Note:
*  Macro I2COLED_sda_ReadPS calls this function. 
*  
*******************************************************************************/
uint8 I2COLED_sda_Read(void) 
{
    return (uint8)((I2COLED_sda_PS & I2COLED_sda_MASK) >> I2COLED_sda_SHIFT);
}


/*******************************************************************************
* Function Name: I2COLED_sda_ReadDataReg
********************************************************************************
*
* Summary:
*  Read the current value assigned to a Digital Port's data output register
*
* Parameters:  
*  None 
*
* Return: 
*  Returns the current value assigned to the Digital Port's data output register
*  
*******************************************************************************/
uint8 I2COLED_sda_ReadDataReg(void) 
{
    return (uint8)((I2COLED_sda_DR & I2COLED_sda_MASK) >> I2COLED_sda_SHIFT);
}


/* If Interrupts Are Enabled for this Pins component */ 
#if defined(I2COLED_sda_INTSTAT) 

    /*******************************************************************************
    * Function Name: I2COLED_sda_ClearInterrupt
    ********************************************************************************
    *
    * Summary:
    *  Clears any active interrupts attached to port and returns the value of the 
    *  interrupt status register.
    *
    * Parameters:  
    *  None 
    *
    * Return: 
    *  Returns the value of the interrupt status register
    *  
    *******************************************************************************/
    uint8 I2COLED_sda_ClearInterrupt(void) 
    {
		uint8 maskedStatus = (uint8)(I2COLED_sda_INTSTAT & I2COLED_sda_MASK);
		I2COLED_sda_INTSTAT = maskedStatus;
        return maskedStatus >> I2COLED_sda_SHIFT;
    }

#endif /* If Interrupts Are Enabled for this Pins component */ 


/* [] END OF FILE */
