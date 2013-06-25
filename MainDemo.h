/*********************************************************************
 * Microchip Graphic Library Demo Application
 * The header file joins all header files used in the project.
 *********************************************************************
 * FileName:        MainDemo.h
 * Dependencies:    See INCLUDES section below
 * Processor:       PIC24/PIC30/PIC32
 * Compiler:        C30/C32
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
 * SOFTWARE AND DOCUMENTATION ARE PROVIDED �AS IS?WITHOUT WARRANTY OF ANY
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
 * Anton Alkhimenok		05/29/2007
 * Sean Justice         02/07/2008  PIC32 support
 ********************************************************************/
#ifndef _MAINDEMO_H
#define _MAINDEMO_H

////////////////////////////// INCLUDES //////////////////////////////
#ifdef __PIC32MX
#include <plib.h>
#else
#include <p24Fxxxx.h>
#endif

#include "GenericTypeDefs.h"
#include "Graphics.h"
#include "EEPROM.h"
#include "TouchScreen.h"
#include "Beep.h"
#include "SideButtons.h"
#include "rtcc.h"

#include "DefineXY.h"
#include "Printer.h"

#include "usb.h"
#include "usb_function_hid.h" 
#include "HardwareProfile.h"
void	CopyXCHARtoCHAR(unsigned char *Dest, unsigned char *Str, unsigned int uCount);
#endif // _MAINDEMO_H
