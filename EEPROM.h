/*****************************************************************************
 *
 * Basic access to SPI EEPROM 25LC256.
 *
 *****************************************************************************
 * FileName:        EEPROM.h
 * Dependencies:    None
 * Processor:       PIC24, PIC32
 * Compiler:       	MPLAB C30 V3.00, MPLAB C32
 * Linker:          MPLAB LINK30, MPLAB LINK32
 * Company:         Microchip Technology Incorporated
 *
 * Software License Agreement
 *
 * Copyright © 2008 Microchip Technology Inc.  All rights reserved.
 * Microchip licenses to you the right to use, modify, copy and distribute
 * Software only when embedded on a Microchip microcontroller or digital
 * signal controller, which is integrated into your product or third party
 * product (pursuant to the sublicense terms in the accompanying license
 * agreement).  
 *
 * You should refer to the license agreement accompanying this Software
 * for additional information regarding your rights and obligations.
 *
 * SOFTWARE AND DOCUMENTATION ARE PROVIDED “AS IS” WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY
 * OF MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR
 * PURPOSE. IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR
 * OBLIGATED UNDER CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION,
 * BREACH OF WARRANTY, OR OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT
 * DAMAGES OR EXPENSES INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL,
 * INDIRECT, PUNITIVE OR CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA,
 * COST OF PROCUREMENT OF SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY
 * CLAIMS BY THIRD PARTIES (INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF),
 * OR OTHER SIMILAR COSTS.
 *
 * Author               Date        Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Anton Alkhimenok		07/10/07	...
 * Anton Alkhimenok     02/07/08    PIC32 support
 *****************************************************************************/

#ifndef _EEPROM_H
#define _EEPROM_H

/************************************************************************
* EEPROM Commands                                                       
************************************************************************/
#define EEPROM_PAGE_SIZE    (unsigned)64
#define EEPROM_PAGE_MASK    (unsigned)0x003f
#define EEPROM_CMD_READ     (unsigned)0x03
#define EEPROM_CMD_WRITE    (unsigned)0x02
#define EEPROM_CMD_WRDI     (unsigned)0x04
#define EEPROM_CMD_WREN     (unsigned)0x06
#define EEPROM_CMD_RDSR     (unsigned)0x05
#define EEPROM_CMD_WRSR     (unsigned)0x01

/************************************************************************
* Aliases for IOs registers related to SPI connected to EEPROM
************************************************************************/
#if	0	// USE_PICFJ256GB110
#define EEPROM_SS_TRIS       TRISGbits.TRISG0
#define EEPROM_SS_PORT       PORTGbits.RG0
#else	// USE_PICFJ128GA010
#define EEPROM_SS_TRIS       TRISDbits.TRISD12
#define EEPROM_SS_PORT       PORTDbits.RD12

#define FLASH1_SS_TRIS       TRISCbits.TRISC13
#define FLASH1_SS_PORT       PORTCbits.RC13

#define FLASH2_SS_TRIS       TRISDbits.TRISD11
#define FLASH2_SS_PORT       PORTDbits.RD11
#endif
#define EEPROM_SCK_TRIS      TRISGbits.TRISG6
#define EEPROM_SDO_TRIS      TRISGbits.TRISG8
#define EEPROM_SDI_TRIS      TRISGbits.TRISG7

#define	EEPROM_S1			1
#define	EEPROM_S2			2
#define	EEPROM_PCF8563	3


/************************************************************************
* Structure STATREG and union _EEPROMStatus_                            
*                                                                       
* Overview: provide bits and byte access to EEPROM status value      
*                                                                       
************************************************************************/
struct  STATREG{
BYTE    WIP:1;
BYTE    WEL:1;
BYTE    BP0:1;
BYTE    BP1:1;
BYTE    RESERVED:3;
BYTE    WPEN:1;
};

union _EEPROMStatus_{
struct  STATREG Bits;
BYTE    Char;
};

/************************************************************************
* Macro: EEPROMSSLow()                                                   
*                                                                       
* Preconditions: SS IO must be configured as output
*                                                                       
* Overview: this macro pulls down SS line                    
*                                                                       
* Input: none                                                          
*                                                                       
* Output: none                                                         
*                                                                       
************************************************************************/
#define EEPROMSSLow()      EEPROM_SS_PORT=0;

/************************************************************************
* Macro: EEPROMSSHigh()
*                                                                       
* Preconditions: SS IO must be configured as output
*                                                                       
* Overview: this macro set SS line to high level
*                                                                       
* Input: none                                                          
*                                                                       
* Output: none
*                                                                       
************************************************************************/
#define EEPROMSSHigh()     EEPROM_SS_PORT=1;

/************************************************************************
* Function: EEPROMInit()
*                                                                       
* Overview: this function setups SPI and IOs connected to EEPROM
*                                                                       
* Input: none
*                                                                       
* Output: none  
*                                                                       
************************************************************************/
void EEPROMInit();

/************************************************************************
* Function: union _EEPROMStatus_ EEPROMReadStatus()
*                                                                       
* Overview: this function reads status register
*                                                                       
* Input: none             
*                                                                       
* Output: status register value
*                                                                       
************************************************************************/
union _EEPROMStatus_ EEPROMReadStatus();

/************************************************************************
* Function: void EEPROMWriteByte()                                           
*                                                                       
* Overview: this function writes a byte to the address specified
*                                                                       
* Input: byte to be written and address
*                                                                       
* Output: none
*                                                                       
************************************************************************/
void EEPROMWriteByte(BYTE data, WORD address);

/************************************************************************
* Function: BYTE EEPROMReadByte(WORD address)       
*                                                                       
* Overview: this function reads a byte from the address specified
*                                                                       
* Input: address          
*                                                                       
* Output: data read
*                                                                       
************************************************************************/
BYTE EEPROMReadByte(WORD address);

/************************************************************************
* Function: void EEPROMWriteWord(WODR data, WORD address)                                           
*                                                                       
* Overview: this function writes a 16-bit word to the address specified
*                                                                       
* Input: data to be written and address
*                                                                       
* Output: none                                                         
*                                                                       
************************************************************************/
void EEPROMWriteWord(WORD data, WORD address);

/************************************************************************
* Function: WORD EEPROMReadWord(WORD address)             
*                                                                       
* Overview: this function reads a 16-bit word from the address specified         
*                                                                       
* Input: address                                                     
*                                                                       
* Output: data read
*                                                                       
************************************************************************/
WORD EEPROMReadWord(WORD address);

/************************************************************************
* Function: EEPROMWriteEnable()                                       
*
* Overview: this function allows writing into EEPROM. Must be called
*           before every write/erase command
*
* Input: none
*            
* Output: none
*
************************************************************************/
void EEPROMWriteEnable();

/************************************************************************
* Function: BYTE EEPROMWriteArray(DWORD address, BYTE* pData, nCount)
*                                                                       
* Overview: this function writes data array at the address specified
*                                                                       
* Input: flash memory address, pointer to the data array, data number
*                                                                       
* Output: return 1 if the operation was successfull
*                                                                     
************************************************************************/
BYTE EEPROMWriteArray(DWORD address, BYTE* pData, WORD nCount);

/************************************************************************
* Function: void EEPROMReadArray(WORD address, BYTE* pData, nCount)
*                                                                       
* Overview: this function reads  data into buffer specified
*                                                                       
* Input: flash memory address, pointer to the buffer, data number
*                                                                       
************************************************************************/
void EEPROMReadArray(WORD address, BYTE* pData, WORD nCount);

void initSPI2(void);
void SPI2_WAIT(void);
void FLASH_RD(unsigned int page_address, unsigned int byte_address, unsigned int length, unsigned char *rdptr,unsigned char cs);
void flash_buffer(unsigned char opcode, unsigned int page_address,unsigned char cs);
void write_bufferstart(unsigned char opcode, unsigned int buffer_address,unsigned char cs);
void write_buffer(unsigned char *ptra,unsigned int length);
void write_bufferend(unsigned char cs);
void solid_data(unsigned char opcode, unsigned int page_address,unsigned char cs);
void AUTO_REWR(unsigned char i);
void testflash(void);

#endif //_EEPROM_H

