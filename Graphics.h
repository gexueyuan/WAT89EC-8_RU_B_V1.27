/*********************************************************************
 * Module for Microchip Graphics Library
 * The header file joins all header files used in the graphics library
 * and contains compile options and defaults.
 *********************************************************************
 * FileName:        Graphics.h
 * Dependencies:    See INCLUDES section below
 * Processor:       PIC24/PIC30/PIC32
 * Compiler:        C30 V3.00/C32
 * Company:         Microchip Technology, Inc.
 *
 * Software License Agreement
 *
 * Copyright ?2008 Microchip Technology Inc.  All rights reserved.
 * Microchip licenses to you the right to use, modify, copy and distribute
 * Software only when embedded on a Microchip microcontroller or digital
 * signal controller, which is integrated into your product or third party
 * product (pursuant to the sublicense terms in the accompanying license
 * agreement).  
 *
 * You should refer to the license agreement accompanying this Software
 * for additional information regarding your rights and obligations.
 *
 * SOFTWARE AND DOCUMENTATION ARE PROVIDED “AS IS?WITHOUT WARRANTY OF ANY
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
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Anton Alkhimenok     11/12/07	Version 1.0 release
 ********************************************************************/

#ifndef _GRAPHICS_H
#define _GRAPHICS_H

//////////////////// COMPILE OPTIONS AND DEFAULTS ////////////////////

////// COMPILE OPTIONS FOR APPLICATION //////
#include <stdlib.h>
#include "GenericTypeDefs.h"
#include "GraphicsConfig.h"

//////// GRAPHICS_LIBRARY_VERSION ///////////
// MSB is version, LSB is subversion
#define GRAPHICS_LIBRARY_VERSION  0x0102

#define Use_NewScreen

//#define Use_Editmode

#define NO_BLUETOOTH_BUTTON

//#define TradeMark_Version

//#define Compensation_low_plus

#define Low_Compensation


//#define No_Print_Title

//#define  CER_MODE

//#define PRINTBLANK

#define IDSTATUS  BTN_FOCUSED

//#define USE_BUTTON_MULTI_LINE

//#define TEST

///////// GOL OBJECTS EMBOSS SIZE ///////////

// Emboss size set for 3-D effect
#define GOL_EMBOSS_SIZE                 3		

////////////////////////////// INCLUDES //////////////////////////////
#include "Primitive.h"  // Graphic primitives

#include "ScanCodes.h"  // Scan codes for AT keyboard
#include "GOL.h"        // GOL layer 
#include "Button.h"
#include "Window.h"
#include "GroupBox.h"
#include "StaticText.h"
#include "Slider.h"
#include "CheckBox.h"
#include "RadioButton.h"
#include "Picture.h"
#include "ProgressBar.h"
#include "EditBox.h"
#include "ListBox.h"
#include "RoundDial.h"
#include "Meter.h"
#ifdef USE_CUSTOM
// Included for custom control demo
#include "CustomControlDemo.h"
#endif
extern const FONT_FLASH Arial;

#ifdef Use_NewScreen
#include "HX8347A.h"
#else
#include "LGDP4531_R61505_S6D0129_S6D0139_SPFD5408.h"
#endif
/*********************************************************************
* Macros: RGB565CONVERT(red, green, blue)
*
* Overview: Converts true color into 5:6:5 RGB format.
*
* PreCondition: none
*
* Input: Red, Green, Blue components.
*
* Output: 5 bits red, 6 bits green, 5 bits blue color.
*
* Side Effects: none
*
********************************************************************/
#define RGB565CONVERT(red, green, blue)\
(WORD)( (( red   >> 3 ) << 11 ) | \
(( green >> 2 ) << 5  ) | \
( blue  >> 3 ))


#endif
