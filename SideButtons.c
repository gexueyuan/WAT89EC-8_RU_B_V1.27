/*****************************************************************************
 *
 * Side buttons driver
 * (S3(RD6), S4(RD13), S5(RA7) and S6(RD7) on Explorer 16 board)
 *
 *****************************************************************************
 * FileName:        SideButtons.c
 * Dependencies:    MainDemo.h
 * Processor:       PIC24, PIC32
 * Compiler:       	MPLAB C30 V3.00, C32
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
 * Anton Alkhimenok		07/20/07	...
 *****************************************************************************/

#include "MainDemo.h"
extern XCHAR PowerOffStr[];
BOOL Power_ON_OFF(void)
{
/*
	unsigned char i,j;
	unsigned int	Power_Count;
	BTN_S6=1;
	delay();
	if(BTN_S6)
		{
			POWER_CONTROL = 0;
		}
	else
		{
			Power_Count = 0;
			delay();
			if(!BTN_S6)
			{
				for(i=0;i>255;i++)
				for(j=0;j>255;j++)
				{
					if(!BTN_S6)
					{
						Power_Count++;
						if(Power_Count>(255*200))
							return TRUE;// ¿ª¹Ø»ú
						else
						{
//							Power_Count = 0;
//							return FALSE;	//²»×ö´¦Àí
							continue;
						}
					}
					else
						return FALSE;	//²»×ö´¦Àí
				}
			}
			else
				return FALSE;	//²»×ö´¦Àí
					

		}
*/
}
/************************************************************************
* Function: void SideButtonsMsg(GOL_MSG* msg)
*                                                                       
* Overview: this function checks buttons state and creates GOL messages.
*           S3 and S4 are used for ARROW UP PRESSED and ARROW DOWN PRESSED
*           messages. S6 moves focus to the next control.
*           S5 creates CARRIGE RETURN PRESSED/RELEASED messages.
*                                                                       
* Input: pointer to the GOL message structure                                                          
*                                                                       
* Output: none
*                                                                       
************************************************************************/
void SideButtonsMsg(GOL_MSG* msg){
// Previous states of the buttons
static char S3 = 1;
static char S4 = 1;
static char S5 = 1;
static char S6 = 1;
static int S6_count = 1;
char state;
OBJ_HEADER* obj;
    
    msg->uiEvent  = EVENT_INVALID;

    state = BTN_S6;
#if	1
	BTN_S6 = 1;
	DelayMs(10);
	if(BTN_S6 == 0)
		{S6_count ++;}
	else
		{S6_count = 0;}
	BTN_S6 = 1;
	if((S6_count >300)&&(BTN_S3))	// ³¤°´5Ãë¹Ø»ú
	{
		POWER_CONTROL = 0;//¿ª¹Ø»ú
		Off_Printer();
		ErrorTrap(PowerOffStr);
		S6_count = 0;
		return 1;
	}
	else
		{
		if(BTN_S3)
			LED_ON_OF = 0;
		else
			{
			LED_ON_OF = 1;
			S6_count=0;
			}
		}
	return;
	return;

#else

    if(S6 != state){
        if(S6){
            // Get pointer to the next object should be focused
            obj = GOLGetFocusNext();
            if(obj != NULL){
                GOLSetFocus(obj);
                Beep();
            }
        }
        S6 = state;
        return;
    }
#endif

    state = BTN_S5;
    if(S5 != state){
        // Get pointer to the focused object
        obj = GOLGetFocus();
        if(obj != NULL){
            if(S5){
                    msg->type    = TYPE_KEYBOARD;
                    msg->uiEvent = EVENT_KEYSCAN;           
                    msg->param1  = obj->ID;
                    msg->param2  = SCAN_CR_PRESSED;
            }else{
                    msg->type    = TYPE_KEYBOARD;
                    msg->uiEvent = EVENT_KEYSCAN;           
                    msg->param1  = obj->ID;
                    msg->param2  = SCAN_CR_RELEASED;
            }
        }
        S5 = state;
        return;
    }

    state = BTN_S3;
    if(S3 != state){
        // Get pointer to the focused object
        obj = GOLGetFocus();
        if(obj != NULL){
            if(S3){
                    msg->type    = TYPE_KEYBOARD;
                    msg->uiEvent = EVENT_KEYSCAN;           
                    msg->param1  = obj->ID;
                    msg->param2  = SCAN_DOWN_PRESSED;
            }
        }
        S3 = state;
        return;
    }

    state = BTN_S4;
    if(S4 != state){
        // Get pointer to the focused object
        obj = GOLGetFocus();
        if(obj != NULL){
            if(S4){
                    msg->type    = TYPE_KEYBOARD;
                    msg->uiEvent = EVENT_KEYSCAN;           
                    msg->param1  = obj->ID;
                    msg->param2  = SCAN_UP_PRESSED;
            }
        }
        S4 = state;
        return;
    }

}
