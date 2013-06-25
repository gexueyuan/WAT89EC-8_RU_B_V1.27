/*****************************************************************************
 * Simple 4 wire touch screen driver
 * Landscape orientation
 *****************************************************************************
 * FileName:        TouchScreen.h
 * Dependencies:    
 * Processor:       PIC24, PIC32
 * Compiler:       	MPLAB C30, MPLAB C32
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
 * Anton Alkhimenok		01/08/07	...
 * Anton Alkhimenok		06/06/07	Basic calibration and GOL messaging are added
 * Anton Alkhimenok     01/14/08    Graphics PICtail Version 2 support is added
 * Anton Alkhimenok     02/05/08    Portrait orientation is added
 * Sean Justice         02/07/08    PIC32 support is added
 *****************************************************************************/
#ifndef _TOUCHSCREEN_H
#define _TOUCHSCREEN_H

#include "MainDemo.h"



// Default calibration values

#define YMINCAL         0x0200
#define YMAXCAL         0x0d00
#define XMINCAL         0x0200
#define XMAXCAL         0x0ddd



#define ADS7843_BUSY_TRIS TRISBbits.TRISB13
#define ADS7843_CS_TRIS TRISBbits.TRISB11
#define ADS7843_DIN_TRIS TRISBbits.TRISB12
#define ADS7843_DCLK_TRIS TRISBbits.TRISB10
#define ADS7843_DOUT_TRIS TRISBbits.TRISB14
#define ADS7843_INT_TRIS TRISBbits.TRISB15

#define ADS7843_BUSY PORTBbits.RB13
#define ADS7843_CS  PORTBbits.RB11
#define ADS7843_DCLK PORTBbits.RB10 
#define ADS7843_DOUT PORTBbits.RB14
#define ADS7843_DIN PORTBbits.RB12
#define ADS7843_INT PORTBbits.RB15

void InitializeADS7843(void);
void InitializeINT2();
void inti_time3();
// Max/Min ADC values for each direction
extern volatile WORD _calXMin;
extern volatile WORD _calXMax;
extern volatile WORD _calYMin;
extern volatile WORD _calYMax;




// Current ADC values for X and Y channels and potentiometer R6
extern volatile SHORT adcX;
extern volatile SHORT adcY;
extern volatile SHORT adcPot;
/*********************************************************************
* Function: void TouchInit(void)
*
* PreCondition: none
*
* Input: none
*
* Output: none
*
* Side Effects: none
*
* Overview: sets ADC
*
* Note: none
*
********************************************************************/
void TouchInit(void);

/*********************************************************************
* Function: SHORT TouchGetX()
*
* PreCondition: none
*
* Input: none
*
* Output: x coordinate
*
* Side Effects: none
*
* Overview: returns x coordinate if touch screen is pressed
*           and -1 if not
*
* Note: none
*
********************************************************************/
SHORT TouchGetX();

/*********************************************************************
* Function: SHORT TouchGetY()
*
* PreCondition: none
*
* Input: none
*
* Output: y coordinate
*
* Side Effects: none
*
* Overview: returns y coordinate if touch screen is pressed
*           and -1 if not
*
* Note: none
*
********************************************************************/
SHORT TouchGetY();

/*********************************************************************
* Function: void TouchGetMsg(GOL_MSG* pMsg)
*
* PreCondition: none
*
* Input: pointer to the message structure to be populated
*
* Output: none
*
* Side Effects: none
*
* Overview: populates GOL message structure
*
* Note: none
*
********************************************************************/
void TouchGetMsg(GOL_MSG* pMsg);


/*********************************************************************
* Macros: ADCGetX()
*
* PreCondition: none
*
* Input: none
*
* Output: ADC result
*
* Side Effects: none
*
* Overview: returns ADC value for x direction if touch screen is pressed
*           and -1 if not
*
* Note: none
*
********************************************************************/
#define ADCGetX() adcX

/*********************************************************************
* Macros: ADCGetY()
*
* PreCondition: none
*
* Input: none
*
* Output: ADC result
*
* Side Effects: none
*
* Overview: returns ADC value for y direction if touch screen is pressed
*           and -1 if not
*
* Note: none
*
********************************************************************/
#define ADCGetY() adcY

/*********************************************************************
* Macros: ADCGetPot()
*
* PreCondition: none
*
* Input: none
*
* Output: ADC result
*
* Side Effects: none
*
* Overview: returns ADC value for potentiometer
*
* Note: none
*
********************************************************************/
#define ADCGetPot() adcPot

#ifndef		OK
	#define		OK		        0
	#define		NOT_OK		   -1
#endif
#define			INT32				long
#define		MAX_SAMPLES		    3

typedef struct Point {
                        INT32     x,
                                 y ;
                     } POINT ;
typedef struct dPoint {
                        INT32     x,
                                 y ;
                     } dPOINT ;
typedef struct Matrix {
							/* This arrangement of values facilitates 
							 *  calculations within getDisplayPoint() 
							 */
                        INT32     An,     /* A = An/Divider */
                                 Bn,     /* B = Bn/Divider */
                                 Cn,     /* C = Cn/Divider */
                                 Dn,     /* D = Dn/Divider */
                                 En,     /* E = En/Divider */
                                 Fn,     /* F = Fn/Divider */
                                 Divider ;
                     } MATRIX ;

extern int setCalibrationMatrix( POINT * display,
                                 POINT * screen,
                                 MATRIX * matrix) ;
extern int getDisplayPoint( POINT * display,
                            POINT * screen,
                            MATRIX * matrix ) ;
#endif

