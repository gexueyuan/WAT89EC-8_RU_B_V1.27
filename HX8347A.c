/*****************************************************************************
 *  Module for Microchip Graphics Library
 *  HIMAX HX8347 controller driver
 *****************************************************************************
 * FileName:        HX8347.c
 * Dependencies:    Graphics.h
 * Processor:       PIC24, PIC32
 * Compiler:       	MPLAB C30, MPLAB C32
 * Linker:          MPLAB LINK30, MPLAB LINK32
 * Company:         Microchip Technology Incorporated
 *
 * Software License Agreement
 *
 * Copyright © 2009 Microchip Technology Inc.  All rights reserved.
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
 * Anton Alkhimenok     05/26/09
 *****************************************************************************/

#include "Graphics.h"
  #ifdef Use_NewScreen
/*
//#ifdef USE_16BIT_PMP
//volatile __eds__ WORD __attribute__((space(eds),address(0x00020000ul),noload)) pmp_data;
//#else
volatile __eds__ BYTE __attribute__((space(eds),address(0x00020000ul),noload)) pmp_data;
//#endif
*/
// Color
WORD    _color;

// Clipping region control
SHORT   _clipRgn;

// Clipping region borders
SHORT   _clipLeft;
SHORT   _clipTop;
SHORT   _clipRight;
SHORT   _clipBottom;
BYTE registed;

/////////////////////// LOCAL FUNCTIONS PROTOTYPES ////////////////////////////
void    SetReg(BYTE index, BYTE value);
void    PutImage1BPP(SHORT left, SHORT top, FLASH_BYTE *bitmap, BYTE stretch);
void    PutImage4BPP(SHORT left, SHORT top, FLASH_BYTE *bitmap, BYTE stretch);
void    PutImage8BPP(SHORT left, SHORT top, FLASH_BYTE *bitmap, BYTE stretch);
void    PutImage16BPP(SHORT left, SHORT top, FLASH_BYTE *bitmap, BYTE stretch);

void    PutImage1BPPExt(SHORT left, SHORT top, void *bitmap, BYTE stretch);
void    PutImage4BPPExt(SHORT left, SHORT top, void *bitmap, BYTE stretch);
void    PutImage8BPPExt(SHORT left, SHORT top, void *bitmap, BYTE stretch);
void    PutImage16BPPExt(SHORT left, SHORT top, void *bitmap, BYTE stretch);

#define DELAY_1MS 16000/5  // for 16MIPS
void  DelayMs(WORD time){
unsigned delay;
	while(time--)
		for(delay=0; delay<DELAY_1MS; delay++);	
}
/*********************************************************************
* Function:  void DeviceSelect()
*
* Overview: asserts the chip select line
*
* PreCondition: none
*
* Input: none
*
* Output: none
*
* Side Effects: none
*
********************************************************************/
inline void DeviceSelect(){
	CS_LAT_BIT = 0;
}
/*********************************************************************
* Function:  void DeviceDeselect()
*
* Overview: puts the chip select line in inactive state
*
* PreCondition: none
*
* Input: none
*
* Output: none
*
* Side Effects: none
*
********************************************************************/
inline void DeviceDeselect(){
	CS_LAT_BIT = 1;
}

inline void DeviceSetCommand(){
	RS_LAT_BIT = 0;
}

inline void DeviceSetData(){
	RS_LAT_BIT = 1;
}

/*********************************************************************
* Macros:  DeviceWrite(data)
*
* PreCondition:  none
*
* Input: data - value to be written to RAM
*
* Output: none
*
* Side Effects: none
*
* Overview: writes data into controller's RAM
*
* Note: chip select should be enabled
*
********************************************************************/
/*
#ifdef USE_16BIT_PMP

inline void DeviceWrite(WORD data) 
{ 
	pmp_data = data;
	PMPWaitBusy();
}

#else

inline void DeviceWrite(BYTE data)
{
	pmp_data = data;
	PMPWaitBusy();
}

#endif
*/

 void DeviceWrite(BYTE data)
{


	PMDIN1=data;

	PMPWaitBusy();

}

//#define DeviceWrite(byte1) RS_LAT_BIT=1;RD_LAT_BIT=1;PMDIN1=(unsigned char)byte1;WR_LAT_BIT=0;PMPWaitBusy();WR_LAT_BIT=1;
/*********************************************************************
* Macros:  DeviceRead()
*
* PreCondition:  none
*
* Input: none
*
* Output: data read
*
* Side Effects: none
*
* Overview: reads data from controller's RAM
*
* Note: chip select should be enabled
*
********************************************************************/
/*
#ifdef USE_16BIT_PMP

inline WORD DeviceRead()
{
WORD value;
	value = pmp_data;
	PMPWaitBusy();
	return PMDIN1;
}

#else


inline BYTE DeviceRead(){
BYTE value;

	value = pmp_data;
	PMPWaitBusy();
	return PMDIN1;
}

#endif
*/
BYTE DeviceRead(){
BYTE value;
	value = PMDIN1;
	PMPWaitBusy();
	PMCONbits.PMPEN = 0; // disable PMP
	value = PMDIN1;
	PMCONbits.PMPEN = 1; // enable  PMP
	return value;
}

//#define DeviceRead(readByte)	RS_LAT_BIT=1;RD_LAT_BIT=1;WR_LAT_BIT=1;PMPWaitBusy();RD_LAT_BIT=0;readByte=PMDIN1;PMPWaitBusy();
/*********************************************************************
* Macros:  WritePixel(data)
*
* Overview: Writes data
*
* PreCondition: none
*
* Input: data
*
* Output: none
*
* Side Effects: none
*
********************************************************************/

#ifdef USE_16BIT_PMP
#define WritePixel(data)  DeviceWrite(data)
#else

//#define WritePixel(data)  DeviceSetData();PMDIN1=((WORD_VAL)(WORD)data).v[1];PMPWaitBusy();PMDIN1=((WORD_VAL)(WORD)data).v[0];PMPWaitBusy();


#define WritePixel(data) \
    DeviceWrite( ((WORD_VAL) data).v[1] & 0xF8 );        \
    DeviceWrite( ((data >> 3) & 0xFC) );                 \
    DeviceWrite( (((WORD_VAL) data).v[0] << 3) & 0xF8 );
 
#endif
/*********************************************************************
* Function:  DeviceInit()
*
* PreCondition:  none
*
* Input: none
*
* Output: none
*
* Side Effects: none
*
* Overview: initializes the device
*
* Note: none
*
********************************************************************/

void DeviceInit(void)
{
	RST_LAT_BIT = 0;            // hold in reset by default
    RST_TRIS_BIT = 0;           // enable RESET line
    RS_LAT_BIT = 1;
    RS_TRIS_BIT = 0;            // enable RS line
    CS_LAT_BIT = 1;             // not selected by default
    CS_TRIS_BIT = 0;            // enable chip select line

/*
	WR_TRIS_BIT=1;
	WR_LAT_BIT=1;
	RD_TRIS_BIT=1;
	RD_LAT_BIT=1;
*/	
    // PMP setup
    PMMODE = 0;
    PMAEN = 0;
    PMCON = 0;
    PMMODEbits.MODE = 2;                    // Intel 80 master interface

    PMMODEbits.WAITB = 0;
    PMMODEbits.WAITM = 2;

    PMMODEbits.WAITE = 0; 

    #ifdef USE_16BIT_PMP
    PMMODEbits.MODE16 = 1;                  // 16 bit mode
    #else
    PMMODEbits.MODE16 = 0;                  // 8 bit mode
    #endif
    
    PMCONbits.PTRDEN = 1;                   // enable RD line
    PMCONbits.PTWREN = 1;                   // enable WR line
    PMCONbits.PMPEN = 1;                    // enable PMP

    DelayMs(40);
    RST_LAT_BIT = 1;                         // release from reset
    DelayMs(400);



}
/*********************************************************************
* Function:  void SetAddress(SHORT x, SHORT y)
*
* Overview: Writes address pointer.
*
* PreCondition: none
*
* Input: x - horizontal position
*        y - vertical position
*
* Output: none
*
* Side Effects: none
*
********************************************************************/
inline void SetAddress(SHORT x, SHORT y)
{
    DeviceSelect();
	DeviceSetCommand();
    DeviceWrite(0x02);
	DeviceSetData();
    DeviceWrite(((WORD_VAL) (WORD) x).v[1]);
	DeviceSetCommand();
    DeviceWrite(0x03);
	DeviceSetData();
    DeviceWrite(((WORD_VAL) (WORD) x).v[0]);
	DeviceSetCommand();
    DeviceWrite(0x06);
	DeviceSetData();
    DeviceWrite(((WORD_VAL) (WORD) y).v[1]);
	DeviceSetCommand();
    DeviceWrite(0x07);
	DeviceSetData();
    DeviceWrite(((WORD_VAL) (WORD) y).v[0]);
	DeviceSetCommand();
    DeviceWrite(0x22);
	DeviceSetData();

}

/*********************************************************************
* Function:  void  SetReg(BYTE index, BYTE value)
*
* PreCondition: none
*
* Input: index - register number
*        value - value to be set
*
* Output: none
*
* Side Effects: none
*
* Overview: sets graphics controller register
*
* Note: none
*
********************************************************************/
void SetReg(BYTE index, BYTE value)
{
	DeviceSelect();
	DeviceSetCommand();
	DeviceWrite(index);
	DeviceSetData();
	DeviceWrite(value);
	DeviceDeselect();
}

/*********************************************************************
* Function:  void ResetDevice()
*
* PreCondition: none
*
* Input: none
*
* Output: none
*
* Side Effects: none
*
* Overview: resets LCD, initializes PMP
*
* Note: none
*
********************************************************************/
void ResetDevice(void)
{
	// Initialize the device
	DeviceInit();



	 // Gamma for CMO 2.8
    SetReg(0x46, 0x95);     //
    SetReg(0x47, 0x51);     //
    SetReg(0x48, 0x00);     //
    SetReg(0x49, 0x36);     //
    SetReg(0x4A, 0x11);     //
    SetReg(0x4B, 0x66);     //
    SetReg(0x4C, 0x14);     //
    SetReg(0x4D, 0x77);     //
    SetReg(0x4E, 0x13);     //
    SetReg(0x4F, 0x4C);     //
    SetReg(0x50, 0x46);     //
    SetReg(0x51, 0x46);     //

    // Display
    SetReg(0x90, 0x7F);     // SAP=0111 1111
    SetReg(0x01, 0x06);     // IDMON=0, INVON=1, NORON=1, PTLON=0
    #if (DISP_ORIENTATION == 0)
    SetReg(0x16, 0xC8);     // MY=1, MX=1, MV=0, BGR=1
    #else
    SetReg(0x16, 0xA8);     // MY=1, MX=0, MV=1, BGR=1
    #endif
    SetReg(0x23, 0x95);     // N_DC=1001 0101
    SetReg(0x24, 0x95);     // P_DC=1001 0101
    SetReg(0x25, 0xFF);     // I_DC=1111 1111
    SetReg(0x27, 0x06);     // N_BP=0000 0110
    SetReg(0x28, 0x06);     // N_FP=0000 0110
    SetReg(0x29, 0x06);     // P_BP=0000 0110
    SetReg(0x2A, 0x06);     // P_FP=0000 0110
    SetReg(0x2C, 0x06);     // I_BP=0000 0110
    SetReg(0x2D, 0x06);     // I_FP=0000 0110
    SetReg(0x3A, 0x01);     // N_RTN=0000, N_NW=001
    SetReg(0x3B, 0x01);     // P_RTN=0000, P_NW=001
    SetReg(0x3C, 0xF0);     // I_RTN=1111, I_NW=000
    SetReg(0x3D, 0x00);     // DIV=00
    DelayMs(20);

    SetReg(0x10, 0xA6);     // SS=0,GS=0 CSEL=110

    // Power Supply Setting
    SetReg(0x19, 0x49);     // OSCADJ=10 0000, OSD_EN=1 //60Hz
    SetReg(0x93, 0x0C);     // RADJ=1100,
    DelayMs(10);

    SetReg(0x20, 0x40);     // BT=0100
    SetReg(0x1D, 0x07);     // VC1=111
    SetReg(0x1E, 0x00);     // VC3=000
    SetReg(0x1F, 0x04);     // VRH=0100          4.12V
    SetReg(0x44, 0x4D);     // VCM=101 0000   3.21V
    SetReg(0x45, 0x11);     // VDV=1 0001           -1.19V
    DelayMs(10);

    SetReg(0x1C, 0x04);     // AP=100
    DelayMs(20);
    SetReg(0x43, 0x80);     //set VCOMG=1
    DelayMs(5);
    SetReg(0x1B, 0x18);     // GASENB=0, PON=1, DK=1, XDK=0, DDVDH_TRI=0, STB=0
    DelayMs(40);

    SetReg(0x1B, 0x10);     // GASENB=0, PON=1, DK=0, XDK=0, DDVDH_TRI=0, STB=0
    DelayMs(40);

    // Display ON Setting
    SetReg(0x26, 0x04);     // GON=0, DTE=0, D=01
    DelayMs(40);
    SetReg(0x26, 0x24);     // GON=1, DTE=0, D=01
    SetReg(0x26, 0x2C);     // GON=1, DTE=0, D=11
    DelayMs(40);

    SetReg(0x26, 0x3C);     // GON=1, DTE=1, D=11
    SetReg(0x35, 0x38);     // EQS=38h
    SetReg(0x36, 0x78);     // EQP=78h
    SetReg(0x3E, 0x38);     // SON=38h
    SetReg(0x40, 0x0F);     // GDON=0Fh
    SetReg(0x41, 0xF0);     // GDOFF

    // Set spulse & rpulse
    SetReg(0x57, 0x02);     // Test mode='1'
    SetReg(0x56, 0x84);     // set Rpulse='1000',spulse='0100'
    SetReg(0x57, 0x00);     // Test mode= '0'
    #if (DISP_ORIENTATION == 0)
    SetReg(0x04, 0x00);
    SetReg(0x05, 0xEF);
    SetReg(0x08, 0x01);
    SetReg(0x09, 0x3F);
    #else
    SetReg(0x04, 0x01);
    SetReg(0x05, 0x3F);
    SetReg(0x08, 0x00);
    SetReg(0x09, 0xEF);
    #endif
    DelayMs(20);

 /*
RST_LAT_BIT=0;
delayms(20);
RST_LAT_BIT=1;
delayms(100);
*/
/*
//WriteDirect(CMD_MODE,0x46);       
 //   WriteDirect(DATA_MODE,0x00D2);  

	SetReg(0x46,0x00D2);
   
 //   WriteDirect(CMD_MODE,0x47);       
 //   WriteDirect(DATA_MODE,0x0050); 

	SetReg(0x47,0x0050);
   
  //  WriteDirect(CMD_MODE,0x48);       
  //  WriteDirect(DATA_MODE,0x0007);  

	SetReg(0x48,0x0007);
   
//    WriteDirect(CMD_MODE,0x49);       
//    WriteDirect(DATA_MODE,0x0043);  

	SetReg(0x49,0x0043);
   
 //   WriteDirect(CMD_MODE,0x4A);       
 //   WriteDirect(DATA_MODE,0x0044); 

	SetReg(0x4A,0x0044);
   
 //   WriteDirect(CMD_MODE,0x4B);       
 //   WriteDirect(DATA_MODE,0x0077); 

 	SetReg(0x4B,0x0077);

	
   
//    WriteDirect(CMD_MODE,0x4C);       
 //   WriteDirect(DATA_MODE,0x0066);   

	SetReg(0x4C,0x0066);
   
//    WriteDirect(CMD_MODE,0x4D);       
 //   WriteDirect(DATA_MODE,0x0016);  

	SetReg(0x4D,0x0016);
   
//    WriteDirect(CMD_MODE,0x4E);       
 //   WriteDirect(DATA_MODE,0x0012);  

	SetReg(0x4E,0x0012);
   
 //   WriteDirect(CMD_MODE,0x4F);       
 //   WriteDirect(DATA_MODE,0x004C); 

	SetReg(0x4F,0x004C);
   
 //   WriteDirect(CMD_MODE,0x50);       
 //   WriteDirect(DATA_MODE,0x0046); 

	SetReg(0x50,0x0046);
   
  //  WriteDirect(CMD_MODE,0x51);       
 //   WriteDirect(DATA_MODE,0x0044);   

	SetReg(0x51,0x0044);
   
//240*320window setting      
    SetReg(0x0002,0x0000); // Column address start2
    SetReg(0x0003,0x0000); // Column address start1
    SetReg(0x0004,0x0000); // Column address end2
    SetReg(0x0005,0x00EF); // Column address end1
    SetReg(0x0006,0x0000); // Row address start2
    SetReg(0x0007,0x0000); // Row address start1
    SetReg(0x0008,0x0001); // Row address end2
    SetReg(0x0009,0x003F); // Row address end1   

    
   
    //WriteDirect(CMD_MODE,0x22);    
    //Display setting     
  //  WriteDirect(CMD_MODE,0x01);       
  //  WriteDirect(DATA_MODE,0x0006); //0x0006 

	 SetReg(0x01,0x0006);
   
 //   WriteDirect(CMD_MODE,0x16);       
 //   WriteDirect(DATA_MODE,0x00A8);//0X48//0x00C8//0x0068//0x0008//0x00A8    

	 SetReg(0x16,0x00A8);
   
  //  WriteDirect(CMD_MODE,0x23);       
  //  WriteDirect(DATA_MODE,0x0095);    

	 SetReg(0x23,0x0095);
   
 //   WriteDirect(CMD_MODE,0x24);       
 //   WriteDirect(DATA_MODE,0x0095);  

	 SetReg(0x24,0x0095);
   
 //   WriteDirect(CMD_MODE,0x25);       
 //   WriteDirect(DATA_MODE,0x00FF);    

	SetReg(0x25,0x00FF);
   
  //  WriteDirect(CMD_MODE,0x27);       
  //  WriteDirect(DATA_MODE,0x0002);  

    SetReg(0x27,0x0002);
   
 //   WriteDirect(CMD_MODE,0x28);       
 //   WriteDirect(DATA_MODE,0x0002);  

	SetReg(0x28,0x0002);
   
 //   WriteDirect(CMD_MODE,0x29);       
 //   WriteDirect(DATA_MODE,0x0002); 

	SetReg(0x29,0x0002);
   
 //   WriteDirect(CMD_MODE,0x2A);       
 //   WriteDirect(DATA_MODE,0x0002);  

	SetReg(0x2A,0x0002);
   
 //   WriteDirect(CMD_MODE,0x2C);       
 //   WriteDirect(DATA_MODE,0x0002); 

	SetReg(0x2C,0x0002);
   
//    WriteDirect(CMD_MODE,0x2D);       
//    WriteDirect(DATA_MODE,0x0002);  

	SetReg(0x2D,0x0002);
   
//    WriteDirect(CMD_MODE,0x3A);       
 //   WriteDirect(DATA_MODE,0x0001);  

	SetReg(0x3A,0x0001);
   
//    WriteDirect(CMD_MODE,0x3B);       
//    WriteDirect(DATA_MODE,0x0001); 

	SetReg(0x3B,0x0001);
   
//    WriteDirect(CMD_MODE,0x3C);       
//    WriteDirect(DATA_MODE,0x00F0);  

	SetReg(0x3C,0x00F0);
   
 //   WriteDirect(CMD_MODE,0x3D);       
//    WriteDirect(DATA_MODE,0x0000); 

	SetReg(0x3D,0x0000);
    delay(20);   
   
//    WriteDirect(CMD_MODE,0x35);       
//    WriteDirect(DATA_MODE,0x0038); 

	SetReg(0x35,0x0038);
   
 //   WriteDirect(CMD_MODE,0x36);       
 //   WriteDirect(DATA_MODE,0x0078); 

	SetReg(0x36,0x0078);
   
 //   WriteDirect(CMD_MODE,0x3E);       
 //   WriteDirect(DATA_MODE,0x0038); 

	SetReg(0x3E,0x0038);
   
 //   WriteDirect(CMD_MODE,0x40);       
 //   WriteDirect(DATA_MODE,0x000F);   

	SetReg(0x40,0x000F);
   
 //   WriteDirect(CMD_MODE,0x41);       
 //   WriteDirect(DATA_MODE,0x00F0); 

	SetReg(0x41,0x00F0);
   
 //   WriteDirect(CMD_MODE,0x19);       
 //   WriteDirect(DATA_MODE,0x0049);   

	SetReg(0x19,0x0049);
   
 //   WriteDirect(CMD_MODE,0x93);       
 //   WriteDirect(DATA_MODE,0x0003);   
	
	SetReg(0x93,0x0003);
     delay(10);   
   
//    WriteDirect(CMD_MODE,0x20);       
//    WriteDirect(DATA_MODE,0x0020); 

	SetReg(0x20,0x0020);
   
//    WriteDirect(CMD_MODE,0x1D);       
//    WriteDirect(DATA_MODE,0x0005);   

	SetReg(0x1D,0x0005);
    delay(10);   
   
//    WriteDirect(CMD_MODE,0x1E);       
//    WriteDirect(DATA_MODE,0x0006); 

	SetReg(0x1E,0x0006);
   
 //   WriteDirect(CMD_MODE,0x1F);       
//    WriteDirect(DATA_MODE,0x0008);

	SetReg(0x1F,0x0008);
   
    //VCOM setting    
//    WriteDirect(CMD_MODE,0x44);       
 //   WriteDirect(DATA_MODE,0x0040); 

	SetReg(0x44,0x0040);
   
//    WriteDirect(CMD_MODE,0x45);       
//    WriteDirect(DATA_MODE,0x0012); 

	SetReg(0x45,0x0012);
    delay(10);   
   
//    WriteDirect(CMD_MODE,0x1C);       
//    WriteDirect(DATA_MODE,0x0004); 

	SetReg(0x1C,0x0004);
    delay(20);   
   
//    WriteDirect(CMD_MODE,0x43);       
//    WriteDirect(DATA_MODE,0x0080); 

	SetReg(0x43,0x0080);
    delay(5);   
   
//    WriteDirect(CMD_MODE,0x1B);       
//    WriteDirect(DATA_MODE,0x0008);//0X00,0X08    

	SetReg(0x1B,0x0008);
    delay(40);   
   
//    WriteDirect(CMD_MODE,0x1B);       
//    WriteDirect(DATA_MODE,0x0010);//0X00,0X10 

	SetReg(0x1B,0x0010);
    delay(40);   
   
//    WriteDirect(CMD_MODE,0x43);       
//    WriteDirect(DATA_MODE,0x0080);

	SetReg(0x43,0x0080);
    delay(10);   
    //Display ON setting    
//    WriteDirect(CMD_MODE,0x90);       
//    WriteDirect(DATA_MODE,0x007F); 

	SetReg(0x90,0x007F);
   
//    WriteDirect(CMD_MODE,0x26);       
//    WriteDirect(DATA_MODE,0x0004); 

	SetReg(0x26,0x0004);
    delay(40);   
   
//    WriteDirect(CMD_MODE,0x26);       
//    WriteDirect(DATA_MODE,0x0024);  

	SetReg(0x26,0x0024);
    delay(40);   
   
//    WriteDirect(CMD_MODE,0x26);       
//    WriteDirect(DATA_MODE,0x002C);  

	SetReg(0x26,0x002C);
    delay(40);   
   
 //   WriteDirect(CMD_MODE,0x26);       
 //   WriteDirect(DATA_MODE,0x003C);  

	SetReg(0x26,0x003C);
   
//Set internal VDDD voltage    
//    WriteDirect(CMD_MODE,0x57);       
//    WriteDirect(DATA_MODE,0x0002); 

	SetReg(0x57,0x0002);
   
//    WriteDirect(CMD_MODE,0x55);       
 //   WriteDirect(DATA_MODE,0x0000); 

	SetReg(0x55,0x0000);
   
//    WriteDirect(CMD_MODE,0x57);       
 //   WriteDirect(DATA_MODE,0x0000);   

	SetReg(0x57,0x0000);
*/

/*
// Gamma for CMO 3.2¡±
SetReg(0x0046,0x00A4);
SetReg(0x0047,0x0053);
SetReg(0x0048,0x0000);
SetReg(0x0049,0x0044);
SetReg(0x004A,0x0004);
SetReg(0x004B,0x0067);
SetReg(0x004C,0x0033);
SetReg(0x004D,0x0077);
SetReg(0x004E,0x0012);
SetReg(0x004F,0x004C);
SetReg(0x0050,0x0046);
SetReg(0x0051,0x0044);
//240x320 window setting
//SetReg(0x001a,0x0001);
SetReg(0x0002,0x0000); // Column address start2
SetReg(0x0003,0x0000); // Column address start1
SetReg(0x0004,0x0000); // Column address end2
SetReg(0x0005,0x00EF); // Column address end1
SetReg(0x0006,0x0000); // Row address start2
SetReg(0x0007,0x0000); // Row address start1
SetReg(0x0008,0x0001); // Row address end2
SetReg(0x0009,0x003F); // Row address end1
// Display Setting
SetReg(0x0001,0x0006); // IDMON=0, INVON=1, NORON=1, PTLON=0
SetReg(0x0016,0x00c8); // MY=0, MX=0, MV=0, ML=1, BGR=0, TEON=0
SetReg(0x0038,0x0000); // RGB_EN=0, use MPU Interface
SetReg(0x0023,0x0095); // N_DC=1001 0101
SetReg(0x0024,0x0095); // PI_DC=1001 0101
SetReg(0x0025,0x00FF); // I_DC=1111 1111
SetReg(0x0027,0x0002); // N_BP=0000 0010
SetReg(0x0028,0x0002); // N_FP=0000 0010
SetReg(0x0029,0x0002); // PI_BP=0000 0010
SetReg(0x002A,0x0002); // PI_FP=0000 0010
SetReg(0x002C,0x0002); // I_BP=0000 0010
SetReg(0x002D,0x0002); // I_FP=0000 0010
SetReg(0x003A,0x0001); // N_RTN=0000, N_NW=001
SetReg(0x003B,0x0000); // PI_RTN=0000, PI_NW=000
SetReg(0x003C,0x00F0); // I_RTN=1111, I_NW=000
SetReg(0x003D,0x0000); // DIV=00
delayms(20);
SetReg(0x0035,0x0038); // EQS=38h
SetReg(0x0036,0x0078); // EQP=78h
SetReg(0x003E,0x0038); // SON=38h
SetReg(0x0040,0x000F); // GDON=0Fh
SetReg(0x0041,0x00F0); // GDOFF
// Power Supply Setting
SetReg(0x0019,0x0049); // CADJ=0100, CUADJ=100(FR:60Hz),, OSD_EN=1
SetReg(0x0093,0x000F); // RADJ=1111, 100%
delayms(10);
SetReg(0x0020,0x0040); // BT=0100
SetReg(0x001D,0x0007); // VC1=111
SetReg(0x001E,0x0000); // VC3=000
SetReg(0x001F,0x0004); // VRH=0100
// VCOM Setting for CMO 3.2¡± Panel
SetReg(0x0044,0x004D); // VCM=100 1101
SetReg(0x0045,0x0011); // VDV=1 0001
delayms(10);
SetReg(0x001C,0x0004); // AP=100
delayms(20);
SetReg(0x001B,0x0018); // GASENB=0, PON=1, DK=1, XDK=0, VLCD_TRI=0, STB=0
delayms(40);
SetReg(0x001B,0x0010); // GASENB=0, PON=1, DK=0, XDK=0, VLCD_TRI=0, STB=0
delayms(40);
SetReg(0x0043,0x0080); //Set VCOMG=1
delayms(100);
// Display ON Setting
SetReg(0x0090,0x007F); // SAP=0111 1111
SetReg(0x0026,0x0004); //GON=0, DTE=0, D=01
delayms(40);
SetReg(0x0026,0x0024); //GON=1, DTE=0, D=01
SetReg(0x0026,0x002C); //GON=1, DTE=0, D=11
delayms(40);
SetReg(0x0026,0x003C); //GON=1, DTE=1, D=11
// Internal register setting
SetReg(0x0057,0x0002); //Test_Mode Enable
SetReg(0x0095,0x0001); // Set Display clock and Pumping clock to synchronize
SetReg(0x0057,0x0000); // Test_Mode Disable
*/

/*
// Gamma for CMO 2.4
SetReg(0x0046,0x0094);
SetReg(0x0047,0x0041);
SetReg(0x0048,0x0000);
SetReg(0x0049,0x0033);
SetReg(0x004A,0x0023);
SetReg(0x004B,0x0045);
SetReg(0x004C,0x0044);
SetReg(0x004D,0x0077);
SetReg(0x004E,0x0012);
SetReg(0x004F,0x00CC);
SetReg(0x0050,0x0046);
SetReg(0x0051,0x0082);
//240x320 window setting
SetReg(0x0002,0x0000); // Column address start2
SetReg(0x0003,0x0000); // Column address start1
SetReg(0x0004,0x0000); // Column address end2
SetReg(0x0005,0x00EF); // Column address end1
SetReg(0x0006,0x0000); // Row address start2
SetReg(0x0007,0x0000); // Row address start1
SetReg(0x0008,0x0001); // Row address end2
SetReg(0x0009,0x003F); // Row address end1
// Display Setting
SetReg(0x0001,0x0006); // IDMON=0, INVON=1, NORON=1, PTLON=0
SetReg(0x0016,0x0008); // MY=0, MX=0, MV=0, ML=1, BGR=0, TEON=0
SetReg(0x0070,0x00C6); // SS=1, GS=1, MV=0, ML=1, BGR=0, TEON=0
SetReg(0x0038,0x0000); // RGB_EN=0, use MPU Interface
SetReg(0x0023,0x0095); // N_DC=1001 0101
SetReg(0x0024,0x0095); // PI_DC=1001 0101
SetReg(0x0025,0x00FF); // I_DC=1111 1111
SetReg(0x0027,0x0002); // N_BP=0000 0010
SetReg(0x0028,0x0002); // N_FP=0000 0010
SetReg(0x0029,0x0002); // PI_BP=0000 0010
SetReg(0x002A,0x0002); // PI_FP=0000 0010
SetReg(0x002C,0x0002); // I_BP=0000 0010
SetReg(0x002D,0x0002); // I_FP=0000 0010
SetReg(0x003A,0x0001); // N_RTN=0000, N_NW=001
SetReg(0x003B,0x0000); // PI_RTN=0000, PI_NW=000
SetReg(0x003C,0x00F0); // I_RTN=1111, I_NW=000
SetReg(0x003D,0x0000); // DIV=00
delayms(20);
SetReg(0x0035,0x0038); // EQS=38h
SetReg(0x0036,0x0078); // EQP=78h
SetReg(0x003E,0x0038); // SON=38h
SetReg(0x0040,0x000F); // GDON=0Fh
SetReg(0x0041,0x00F0); // GDOFF
// Power Supply Setting
SetReg(0x0019,0x006B); // CADJ=0100, CUADJ=100(FR:60Hz),, OSD_EN=1
SetReg(0x0093,0x000A); // RADJ=1111, 100%
delayms(10);
SetReg(0x0020,0x0040); // BT=0100
SetReg(0x001D,0x0007); // VC1=111
SetReg(0x001E,0x0000); // VC3=000
SetReg(0x001F,0x0004); // VRH=0100
// VCOM Setting for CMO 2.4¡± Panel
SetReg(0x0044,0x003c); // VCM=100 0000       38
SetReg(0x0045,0x0011); // VDV=1 0001         13
delayms(10);
SetReg(0x001C,0x0004); // AP=100
delayms(20);
SetReg(0x001B,0x0018); // GASENB=0, PON=1, DK=1, XDK=0, VLCD_TRI=0, STB=0
delayms(40);
SetReg(0x001B,0x0010); // GASENB=0, PON=1, DK=0, XDK=0, VLCD_TRI=0, STB=0
delayms(40);
SetReg(0x0043,0x0080); //Set VCOMG=1
delayms(100);
// Display ON Setting
SetReg(0x0090,0x007F); // SAP=0111 1111
SetReg(0x0026,0x0004); //GON=0, DTE=0, D=01
delayms(40);
SetReg(0x0026,0x0024); //GON=1, DTE=0, D=01
SetReg(0x0026,0x002C); //GON=1, DTE=0, D=11
delayms(40);
SetReg(0x0026,0x003C); //GON=1, DTE=1, D=11
// Internal register setting
SetReg(0x0057,0x0002); //Test_Mode Enable
SetReg(0x0095,0x0001); // Set Display clock and Pumping clock to synchronize
SetReg(0x0057,0x0000); // Test_Mode Disable
*/
/*
    DeviceSelect();
	DeviceSetCommand();
    DeviceWrite(0x72);
    DeviceSetData();
	registed=DeviceRead();
	DeviceDeselect();
*/

}

/*********************************************************************
* Function: void PutPixel(SHORT x, SHORT y)
*
* PreCondition: none
*
* Input: x,y - pixel coordinates
*
* Output: none
*
* Side Effects: none
*
* Overview: puts pixel
*
* Note: none
*
********************************************************************/
void PutPixel(SHORT x, SHORT y)
{
    if(_clipRgn)
    {
        if(x < _clipLeft)
            return;
        if(x > _clipRight)
            return;
        if(y < _clipTop)
            return;
        if(y > _clipBottom)
            return;
    }

    DeviceSelect;
    SetAddress(x, y);
    WritePixel(_color);
    DeviceDeselect;
}

/*********************************************************************
* Function: WORD Bar(SHORT left, SHORT top, SHORT right, SHORT bottom)
*
* PreCondition: none
*
* Input: left,top - top left corner coordinates,
*        right,bottom - bottom right corner coordinates
*
* Output: none
*
* Side Effects: none
*
* Overview: draws rectangle filled with current color
*
* Note: none
*
********************************************************************/
WORD Bar(SHORT left, SHORT top, SHORT right, SHORT bottom)
{
    register SHORT  x, y;

    #ifndef USE_NONBLOCKING_CONFIG
    while(IsDeviceBusy() != 0);

    /* Ready */
    #else
    if(IsDeviceBusy() != 0)
        return (0);
    #endif
    if(_clipRgn)
    {
        if(left < _clipLeft)
            left = _clipLeft;
        if(right > _clipRight)
            right = _clipRight;
        if(top < _clipTop)
            top = _clipTop;
        if(bottom > _clipBottom)
            bottom = _clipBottom;
    }

    DeviceSelect;
    for(y = top; y < bottom + 1; y++)
    {
        SetAddress(left, y);
        for(x = left; x < right + 1; x++)
        {
            WritePixel(_color);
        }
    }

    DeviceDeselect;
    return (1);
}

/*********************************************************************
* Function: void ClearDevice(void)
*
* PreCondition: none
*
* Input: none
*
* Output: none
*
* Side Effects: none
*
* Overview: clears screen with current color 
*
* Note: none
*
********************************************************************/
void ClearDevice(void)
{
    DWORD   counter;

    DeviceSelect;
    SetAddress(0, 0);
    for(counter = 0; counter < (DWORD) (GetMaxX() + 1) * (GetMaxY() + 1); counter++)
    {
        WritePixel(_color);
    }

    DeviceDeselect;
}

/*********************************************************************
* Function: WORD PutImage(SHORT left, SHORT top, void* bitmap, BYTE stretch)
*
* PreCondition: none
*
* Input: left,top - left top image corner,
*        bitmap - image pointer,
*        stretch - image stretch factor
*
* Output: none
*
* Side Effects: none
*
* Overview: outputs image starting from left,top coordinates
*
* Note: image must be located in flash
*
********************************************************************/
#ifdef USE_DRV_PUTIMAGE

/* */
WORD PutImage(SHORT left, SHORT top, void *bitmap, BYTE stretch)
{
    FLASH_BYTE  *flashAddress;
    BYTE        colorDepth;
    WORD        colorTemp;

        #ifndef USE_NONBLOCKING_CONFIG
    while(IsDeviceBusy() != 0);

    /* Ready */
        #else
    if(IsDeviceBusy() != 0)
        return (0);
        #endif

    // Save current color
    colorTemp = _color;

    switch(*((SHORT *)bitmap))
    {
                #ifdef USE_BITMAP_FLASH

        case FLASH:

            // Image address
            flashAddress = ((BITMAP_FLASH *)bitmap)->address;

            // Read color depth
            colorDepth = *(flashAddress + 1);

            // Draw picture
            switch(colorDepth)
            {
                case 1:     PutImage1BPP(left, top, flashAddress, stretch); break;
                case 4:     PutImage4BPP(left, top, flashAddress, stretch); break;
                case 8:     PutImage8BPP(left, top, flashAddress, stretch); break;
                case 16:    PutImage16BPP(left, top, flashAddress, stretch); break;
            }

            break;
                #endif
                #ifdef USE_BITMAP_EXTERNAL

        case EXTERNAL:

            // Get color depth
            ExternalMemoryCallback(bitmap, 1, 1, &colorDepth);

            // Draw picture
            switch(colorDepth)
            {
                case 1:     PutImage1BPPExt(left, top, bitmap, stretch); break;
                case 4:     PutImage4BPPExt(left, top, bitmap, stretch); break;
                case 8:     PutImage8BPPExt(left, top, bitmap, stretch); break;
                case 16:    PutImage16BPPExt(left, top, bitmap, stretch); break;
                default:    break;
            }

            break;
                #endif

        default:
            break;
    }

    // Restore current color
    _color = colorTemp;
    return (1);
}

    #ifdef USE_BITMAP_FLASH

/*********************************************************************
* Function: void PutImage1BPP(SHORT left, SHORT top, FLASH_BYTE* bitmap, BYTE stretch)
*
* PreCondition: none
*
* Input: left,top - left top image corner,
*        bitmap - image pointer,
*        stretch - image stretch factor
*
* Output: none
*
* Side Effects: none
*
* Overview: outputs monochrome image starting from left,top coordinates
*
* Note: image must be located in flash
*
********************************************************************/
void PutImage1BPP(SHORT left, SHORT top, FLASH_BYTE *bitmap, BYTE stretch)
{
    register FLASH_BYTE *flashAddress;
    register FLASH_BYTE *tempFlashAddress;
    BYTE                temp;
    WORD                sizeX, sizeY;
    WORD                x, y;
    BYTE                stretchX, stretchY;
    WORD                pallete[2];
    BYTE                mask;

    // Move pointer to size information
    flashAddress = bitmap + 2;

    // Read image size
    sizeY = *((FLASH_WORD *)flashAddress);
    flashAddress += 2;
    sizeX = *((FLASH_WORD *)flashAddress);
    flashAddress += 2;
    pallete[0] = *((FLASH_WORD *)flashAddress);
    flashAddress += 2;
    pallete[1] = *((FLASH_WORD *)flashAddress);
    flashAddress += 2;

    DeviceSelect;
    for(y = 0; y < sizeY; y++)
    {
        tempFlashAddress = flashAddress;
        for(stretchY = 0; stretchY < stretch; stretchY++)
        {
            flashAddress = tempFlashAddress;
            SetAddress((WORD) left, (WORD) top);
            mask = 0;
            for(x = 0; x < sizeX; x++)
            {

                // Read 8 pixels from flash
                if(mask == 0)
                {
                    temp = *flashAddress;
                    flashAddress++;
                    mask = 0x80;
                }

                // Set color
                if(mask & temp)
                {
                    SetColor(pallete[1]);
                }
                else
                {
                    SetColor(pallete[0]);
                }

                // Write pixel to screen
                for(stretchX = 0; stretchX < stretch; stretchX++)
                {
                    WritePixel(_color);
                }

                // Shift to the next pixel
                mask >>= 1;
            }

            top++;
        }
    }

    DeviceDeselect;
}

/*********************************************************************
* Function: void PutImage4BPP(SHORT left, SHORT top, FLASH_BYTE* bitmap, BYTE stretch)
*
* PreCondition: none
*
* Input: left,top - left top image corner, bitmap - image pointer,
*        stretch - image stretch factor
*
* Output: none
*
* Side Effects: none
*
* Overview: outputs 16 color image starting from left,top coordinates
*
* Note: image must be located in flash
*
********************************************************************/
void PutImage4BPP(SHORT left, SHORT top, FLASH_BYTE *bitmap, BYTE stretch)
{
    register FLASH_BYTE *flashAddress;
    register FLASH_BYTE *tempFlashAddress;
    WORD                sizeX, sizeY;
    register WORD       x, y;
    BYTE                temp;
    register BYTE       stretchX, stretchY;
    WORD                pallete[16];
    WORD                counter;

    // Move pointer to size information
    flashAddress = bitmap + 2;

    // Read image size
    sizeY = *((FLASH_WORD *)flashAddress);
    flashAddress += 2;
    sizeX = *((FLASH_WORD *)flashAddress);
    flashAddress += 2;

    // Read pallete
    for(counter = 0; counter < 16; counter++)
    {
        pallete[counter] = *((FLASH_WORD *)flashAddress);
        flashAddress += 2;
    }

    DeviceSelect;
    for(y = 0; y < sizeY; y++)
    {
        tempFlashAddress = flashAddress;
        for(stretchY = 0; stretchY < stretch; stretchY++)
        {
            flashAddress = tempFlashAddress;
            SetAddress(left, top);
            for(x = 0; x < sizeX; x++)
            {

                // Read 2 pixels from flash
                if(x & 0x0001)
                {

                    // second pixel in byte
                    SetColor(pallete[temp >> 4]);
                }
                else
                {
                    temp = *flashAddress;
                    flashAddress++;

                    // first pixel in byte
                    SetColor(pallete[temp & 0x0f]);
                }

                // Write pixel to screen
                for(stretchX = 0; stretchX < stretch; stretchX++)
                {
                    WritePixel(_color);
                }
            }

            top++;
        }
    }

    DeviceDeselect;
}

/*********************************************************************
* Function: void PutImage8BPP(SHORT left, SHORT top, FLASH_BYTE* bitmap, BYTE stretch)
*
* PreCondition: none
*
* Input: left,top - left top image corner, bitmap - image pointer,
*        stretch - image stretch factor
*
* Output: none
*
* Side Effects: none
*
* Overview: outputs 256 color image starting from left,top coordinates
*
* Note: image must be located in flash
*
********************************************************************/
void PutImage8BPP(SHORT left, SHORT top, FLASH_BYTE *bitmap, BYTE stretch)
{
    register FLASH_BYTE *flashAddress;
    register FLASH_BYTE *tempFlashAddress;
    WORD                sizeX, sizeY;
    WORD                x, y;
    BYTE                temp;
    BYTE                stretchX, stretchY;
    WORD                pallete[256];
    WORD                counter;

    // Move pointer to size information
    flashAddress = bitmap + 2;

    // Read image size
    sizeY = *((FLASH_WORD *)flashAddress);
    flashAddress += 2;
    sizeX = *((FLASH_WORD *)flashAddress);
    flashAddress += 2;

    // Read pallete
    for(counter = 0; counter < 256; counter++)
    {
        pallete[counter] = *((FLASH_WORD *)flashAddress);
        flashAddress += 2;
    }

    DeviceSelect;
    for(y = 0; y < sizeY; y++)
    {
        tempFlashAddress = flashAddress;
        for(stretchY = 0; stretchY < stretch; stretchY++)
        {
            flashAddress = tempFlashAddress;
            SetAddress(left, top);
            for(x = 0; x < sizeX; x++)
            {

                // Read pixels from flash
                temp = *flashAddress;
                flashAddress++;

                // Set color
                SetColor(pallete[temp]);

                // Write pixel to screen
                for(stretchX = 0; stretchX < stretch; stretchX++)
                {
                    WritePixel(_color);
                }
            }

            top++;
        }
    }

    DeviceDeselect;
}

/*********************************************************************
* Function: void PutImage16BPP(SHORT left, SHORT top, FLASH_BYTE* bitmap, BYTE stretch)
*
* PreCondition: none
*
* Input: left,top - left top image corner, bitmap - image pointer,
*        stretch - image stretch factor
*
* Output: none
*
* Side Effects: none
*
* Overview: outputs hicolor image starting from left,top coordinates
*
* Note: image must be located in flash
*
********************************************************************/
void PutImage16BPP(SHORT left, SHORT top, FLASH_BYTE *bitmap, BYTE stretch)
{
    register FLASH_WORD *flashAddress;
    register FLASH_WORD *tempFlashAddress;
    WORD                sizeX, sizeY;
    register WORD       x, y;
    WORD                temp;
    register BYTE       stretchX, stretchY;

    // Move pointer to size information
    flashAddress = (FLASH_WORD *)bitmap + 1;

    // Read image size
    sizeY = *flashAddress;
    flashAddress++;
    sizeX = *flashAddress;
    flashAddress++;

    DeviceSelect;
    for(y = 0; y < sizeY; y++)
    {
        tempFlashAddress = flashAddress;
        for(stretchY = 0; stretchY < stretch; stretchY++)
        {
            flashAddress = tempFlashAddress;
            SetAddress(left, top);
            for(x = 0; x < sizeX; x++)
            {

                // Read pixels from flash
                temp = *flashAddress;
                flashAddress++;

                // Set color
                SetColor(temp);

                // Write pixel to screen
                for(stretchX = 0; stretchX < stretch; stretchX++)
                {
                    WritePixel(_color);
                }
            }

            top++;
        }
    }

    DeviceDeselect;
}

    #endif
    #ifdef USE_BITMAP_EXTERNAL

/*********************************************************************
* Function: void PutImage1BPPExt(SHORT left, SHORT top, void* bitmap, BYTE stretch)
*
* PreCondition: none
*
* Input: left,top - left top image corner, bitmap - image pointer,
*        stretch - image stretch factor
*
* Output: none
*
* Side Effects: none
*
* Overview: outputs monochrome image starting from left,top coordinates
*
* Note: image must be located in external memory
*
********************************************************************/
void PutImage1BPPExt(SHORT left, SHORT top, void *bitmap, BYTE stretch)
{
    register DWORD  memOffset;
    BITMAP_HEADER   bmp;
    WORD            pallete[2];
    BYTE            lineBuffer[((GetMaxX() + 1) / 8) + 1];
    BYTE            *pData;
    SHORT           byteWidth;

    BYTE            temp;
    BYTE            mask;
    WORD            sizeX, sizeY;
    WORD            x, y;
    BYTE            stretchX, stretchY;

    // Get bitmap header
    ExternalMemoryCallback(bitmap, 0, sizeof(BITMAP_HEADER), &bmp);

    // Get pallete (2 entries)
    ExternalMemoryCallback(bitmap, sizeof(BITMAP_HEADER), 2 * sizeof(WORD), pallete);

    // Set offset to the image data
    memOffset = sizeof(BITMAP_HEADER) + 2 * sizeof(WORD);

    // Line width in bytes
    byteWidth = bmp.width >> 3;
    if(bmp.width & 0x0007)
        byteWidth++;

    // Get size
    sizeX = bmp.width;
    sizeY = bmp.height;

    for(y = 0; y < sizeY; y++)
    {

        // Get line
        ExternalMemoryCallback(bitmap, memOffset, byteWidth, lineBuffer);
        memOffset += byteWidth;
        DeviceSelect;
        for(stretchY = 0; stretchY < stretch; stretchY++)
        {
            pData = lineBuffer;
            SetAddress(left, top);
            mask = 0;
            for(x = 0; x < sizeX; x++)
            {

                // Read 8 pixels from flash
                if(mask == 0)
                {
                    temp = *pData++;
                    mask = 0x80;
                }

                // Set color
                if(mask & temp)
                {
                    SetColor(pallete[1]);
                }
                else
                {
                    SetColor(pallete[0]);
                }

                // Write pixel to screen
                for(stretchX = 0; stretchX < stretch; stretchX++)
                {
                    WritePixel(_color);
                }

                // Shift to the next pixel
                mask >>= 1;
            }

            top++;
        }

        DeviceDeselect;
    }
}

/*********************************************************************
* Function: void PutImage4BPPExt(SHORT left, SHORT top, void* bitmap, BYTE stretch)
*
* PreCondition: none
*
* Input: left,top - left top image corner, bitmap - image pointer,
*        stretch - image stretch factor
*
* Output: none
*
* Side Effects: none
*
* Overview: outputs monochrome image starting from left,top coordinates
*
* Note: image must be located in external memory
*
********************************************************************/
void PutImage4BPPExt(SHORT left, SHORT top, void *bitmap, BYTE stretch)
{
    register DWORD  memOffset;
    BITMAP_HEADER   bmp;
    WORD            pallete[16];
    BYTE            lineBuffer[((GetMaxX() + 1) / 2) + 1];
    BYTE            *pData;
    SHORT           byteWidth;

    BYTE            temp;
    WORD            sizeX, sizeY;
    WORD            x, y;
    BYTE            stretchX, stretchY;

    // Get bitmap header
    ExternalMemoryCallback(bitmap, 0, sizeof(BITMAP_HEADER), &bmp);

    // Get pallete (16 entries)
    ExternalMemoryCallback(bitmap, sizeof(BITMAP_HEADER), 16 * sizeof(WORD), pallete);

    // Set offset to the image data
    memOffset = sizeof(BITMAP_HEADER) + 16 * sizeof(WORD);

    // Line width in bytes
    byteWidth = bmp.width >> 1;
    if(bmp.width & 0x0001)
        byteWidth++;

    // Get size
    sizeX = bmp.width;
    sizeY = bmp.height;

    for(y = 0; y < sizeY; y++)
    {

        // Get line
        ExternalMemoryCallback(bitmap, memOffset, byteWidth, lineBuffer);
        memOffset += byteWidth;
        DeviceSelect;
        for(stretchY = 0; stretchY < stretch; stretchY++)
        {
            pData = lineBuffer;
            SetAddress(left, top);

            for(x = 0; x < sizeX; x++)
            {

                // Read 2 pixels from flash
                if(x & 0x0001)
                {

                    // second pixel in byte
                    SetColor(pallete[temp >> 4]);
                }
                else
                {
                    temp = *pData++;

                    // first pixel in byte
                    SetColor(pallete[temp & 0x0f]);
                }

                // Write pixel to screen
                for(stretchX = 0; stretchX < stretch; stretchX++)
                {
                    WritePixel(_color);
                }
            }

            top++;
        }

        DeviceDeselect;
    }
}

/*********************************************************************
* Function: void PutImage8BPPExt(SHORT left, SHORT top, void* bitmap, BYTE stretch)
*
* PreCondition: none
*
* Input: left,top - left top image corner, bitmap - image pointer,
*        stretch - image stretch factor
*
* Output: none
*
* Side Effects: none
*
* Overview: outputs monochrome image starting from left,top coordinates
*
* Note: image must be located in external memory
*
********************************************************************/
void PutImage8BPPExt(SHORT left, SHORT top, void *bitmap, BYTE stretch)
{
    register DWORD  memOffset;
    BITMAP_HEADER   bmp;
    WORD            pallete[256];
    BYTE            lineBuffer[(GetMaxX() + 1)];
    BYTE            *pData;

    BYTE            temp;
    WORD            sizeX, sizeY;
    WORD            x, y;
    BYTE            stretchX, stretchY;

    // Get bitmap header
    ExternalMemoryCallback(bitmap, 0, sizeof(BITMAP_HEADER), &bmp);

    // Get pallete (256 entries)
    ExternalMemoryCallback(bitmap, sizeof(BITMAP_HEADER), 256 * sizeof(WORD), pallete);

    // Set offset to the image data
    memOffset = sizeof(BITMAP_HEADER) + 256 * sizeof(WORD);

    // Get size
    sizeX = bmp.width;
    sizeY = bmp.height;

    for(y = 0; y < sizeY; y++)
    {

        // Get line
        ExternalMemoryCallback(bitmap, memOffset, sizeX, lineBuffer);
        memOffset += sizeX;
        DeviceSelect;
        for(stretchY = 0; stretchY < stretch; stretchY++)
        {
            pData = lineBuffer;
            SetAddress(left, top);

            for(x = 0; x < sizeX; x++)
            {
                temp = *pData++;
                SetColor(pallete[temp]);

                // Write pixel to screen
                for(stretchX = 0; stretchX < stretch; stretchX++)
                {
                    WritePixel(_color);
                }
            }

            top++;
        }

        DeviceDeselect;
    }
}

/*********************************************************************
* Function: void PutImage16BPPExt(SHORT left, SHORT top, void* bitmap, BYTE stretch)
*
* PreCondition: none
*
* Input: left,top - left top image corner, bitmap - image pointer,
*        stretch - image stretch factor
*
* Output: none
*
* Side Effects: none
*
* Overview: outputs monochrome image starting from left,top coordinates
*
* Note: image must be located in external memory
*
********************************************************************/
void PutImage16BPPExt(SHORT left, SHORT top, void *bitmap, BYTE stretch)
{
    register DWORD  memOffset;
    BITMAP_HEADER   bmp;
    WORD            lineBuffer[(GetMaxX() + 1)];
    WORD            *pData;
    WORD            byteWidth;

    WORD            temp;
    WORD            sizeX, sizeY;
    WORD            x, y;
    BYTE            stretchX, stretchY;

    // Get bitmap header
    ExternalMemoryCallback(bitmap, 0, sizeof(BITMAP_HEADER), &bmp);

    // Set offset to the image data
    memOffset = sizeof(BITMAP_HEADER);

    // Get size
    sizeX = bmp.width;
    sizeY = bmp.height;

    byteWidth = sizeX << 1;

    for(y = 0; y < sizeY; y++)
    {

        // Get line
        ExternalMemoryCallback(bitmap, memOffset, byteWidth, lineBuffer);
        memOffset += byteWidth;
        DeviceSelect;
        for(stretchY = 0; stretchY < stretch; stretchY++)
        {
            pData = lineBuffer;
            SetAddress(left, top);

            for(x = 0; x < sizeX; x++)
            {
                temp = *pData++;
                SetColor(temp);

                // Write pixel to screen
                for(stretchX = 0; stretchX < stretch; stretchX++)
                {
                    WritePixel(_color);
                }
            }

            top++;
        }

        DeviceDeselect;
    }
}

    #endif
#endif // USE_DRV_PUTIMAGE
#endif