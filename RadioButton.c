/*****************************************************************************
 *  Module for Microchip Graphics Library
 *  GOL Layer 
 *  Radio Button
 *****************************************************************************
 * FileName:        RadioButton.c
 * Dependencies:    Graphics.h
 * Processor:       PIC24, PIC32
 * Compiler:       	MPLAB C30 V3.00, MPLAB C32
 * Linker:          MPLAB LINK30, MPLAB LINK32
 * Company:         Microchip Technology Incorporated
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
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Anton Alkhimenok 	11/12/07	Version 1.0 release
 *****************************************************************************/

#include "Graphics.h"
#include "DefineXY.h"
#ifdef USE_RADIOBUTTON

// This pointer is used to create linked list of radio buttons for the group
RADIOBUTTON* _pListButtons = NULL;

/*********************************************************************
* Function: RADIOBUTTON  *RbCreate(WORD ID, SHORT left, SHORT top, SHORT right, 
*                              SHORT bottom, WORD state, XCHAR *pText, GOL_SCHEME *pScheme)
*
* Overview: creates the radio button
*
********************************************************************/
RADIOBUTTON *RbCreate(WORD ID, SHORT left, SHORT top, SHORT right, SHORT bottom,
                      WORD state, XCHAR *pText, GOL_SCHEME *pScheme)
{
	RADIOBUTTON *pRb = NULL;
    RADIOBUTTON *pointer;
	
	pRb = malloc(sizeof(RADIOBUTTON));
	if (pRb == NULL)
		return pRb;
	
	pRb->ID      	= ID;
	pRb->pNxtObj 	= NULL;
	pRb->type    	= OBJ_RADIOBUTTON;
	pRb->left    	= left;
	pRb->top     	= top;
	pRb->right   	= right;
	pRb->bottom  	= bottom;
	pRb->pText   	= pText;
    pRb->pNext      = NULL;                 // last radio button in the list
	pRb->state      = state;

    if(GetState(pRb,RB_GROUP)){
        // If it's first button in the group start new button's list
        _pListButtons = pRb;
        // Attach the button to the list
        pRb->pHead = (OBJ_HEADER*)_pListButtons;
    }else{
        // Attach the button to the list
        pRb->pHead = (OBJ_HEADER*)_pListButtons;
        pointer = _pListButtons;
        while(pointer->pNext != NULL){
            pointer = (RADIOBUTTON*)pointer->pNext;
        }
        pointer->pNext = (OBJ_HEADER*)pRb;
    }

	// Set the style scheme to be used
	if (pScheme == NULL)
		pRb->pGolScheme = _pDefaultGolScheme; 
	else 	
		pRb->pGolScheme = pScheme; 	

	// Set the text height  
    pRb->textHeight = 0;
    if(pText != NULL){
	    pRb->textHeight	= GetTextHeight(pRb->pGolScheme->pFont);
    }

    GOLAddObject((OBJ_HEADER*) pRb);

	return pRb;
}

/*********************************************************************
* Function: void RbSetCheck(RADIOBUTTON *pRb, WORD ID)
*
* Input: pRb - the pointer to any radio button in the group
*        ID - ID of button to be checked
*
* Output: none
*
* Overview: sets radio button to checked state, and marks states of group
*           radio buttons to be redrawn
*
********************************************************************/
void RbSetCheck(RADIOBUTTON *pRb, WORD ID){
RADIOBUTTON* pointer;

    pointer = (RADIOBUTTON*) pRb->pHead;

    while(pointer != NULL){
        if(pointer->ID == ID){
#if	0//def	SaveState
        	EEPROMWriteByte(ID, (0x7F18));// add by Spring.Chen
#endif
            SetState(pointer, RB_CHECKED|RB_DRAW_CHECK); // set check and redraw
        }else{
            ClrState(pointer, RB_CHECKED);     // reset checked   
            SetState(pointer, RB_DRAW_CHECK);  // redraw
        }
        pointer = (RADIOBUTTON*)pointer->pNext;
    }            
}

/*********************************************************************
* Function: WORD RbGetCheck(RADIOBUTTON *pRb)
*
* Input: pRb - the pointer to any radio button in the group
*
* Output: ID of checked button, -1 if there are no checked buttons
*
* Overview: gets ID of checked radio button
*
********************************************************************/
WORD RbGetCheck(RADIOBUTTON *pRb){
RADIOBUTTON* pointer;

    pointer = (RADIOBUTTON*) pRb->pHead;

    while(pointer != NULL){
        if(GetState(pointer,RB_CHECKED)){
            return pointer->ID;
        }
        pointer = (RADIOBUTTON*)pointer->pNext;
    }

    return -1;     
}

/*********************************************************************
* Function: RbSetText(RADIOBUTTON *pRb, XCHAR *pText)
*
* Input: pRb - the pointer to the radio button
*        pText - pointer to the text
*
* Output: none
*
* Overview: sets text
*
********************************************************************/
void RbSetText(RADIOBUTTON *pRb, XCHAR *pText)
{
	pRb->pText = pText;
	pRb->textHeight = GetTextHeight((BYTE *)pRb->pGolScheme->pFont);
}

/*********************************************************************
* Function: RbMsgDefault(WORD translatedMsg, RADIOBUTTON *pRb, GOL_MSG* pMsg)
*
* Overview: changes the state of the radio button by default
*
********************************************************************/
void RbMsgDefault(WORD translatedMsg, RADIOBUTTON* pRb, GOL_MSG* pMsg){
RADIOBUTTON* pointer;
XCHAR	addressTemp;
#ifdef  USE_FOCUS
#ifdef  USE_TOUCHSCREEN
    if(pMsg->type == TYPE_TOUCHSCREEN){
        if(!GetState(pRb,RB_FOCUSED)){
                GOLSetFocus((OBJ_HEADER*)pRb);
        }
    }
#endif
#endif

    if(translatedMsg == RB_MSG_CHECKED){

        // Uncheck radio buttons in the group
        pointer = (RADIOBUTTON*) pRb->pHead;

        while(pointer != NULL){
            if(GetState(pointer,RB_CHECKED)){
               ClrState(pointer, RB_CHECKED);       // reset check
               SetState(pointer, RB_DRAW_CHECK);  // redraw
            }
            pointer = (RADIOBUTTON*)pointer->pNext;
        }            
#ifdef	SaveState
		addressTemp = RButtonAddress;
		if(pRb->ID < BackLightSelectMax)
			{
				addressTemp = RButtonAddress;
				if(pRb->ID <= 1)
					EEPROMWriteWord((pRb->ID+1)*60, BackLightTimeAddress);// add by Spring.Chen
				else	// 0 µ½ID_RADIOBUTTONStar+ 3 Îª¹Ø»úÊ±¼äµ¥Ñ¡¿ð
					EEPROMWriteWord(((pRb->ID)*(pRb->ID)+1)*60, BackLightTimeAddress);// add by Spring.Chen
			}
		else if(pRb->ID < PrintSelectMax)
			addressTemp = PrintSelectAddress;//RButtonAddress+2;
		else if(pRb->ID < ThicknessSelectMax)
			{
			addressTemp = ThicknessAddress;//RButtonAddress+4;
			EEPROMWriteByte(pRb->ID, ThicknessAddress);//ÎªÊ²Ã´¼ÓÔÚÕâÀï¾ÍÃ»ÎÊÌâ å Spring.chen
    			}
		else if(pRb->ID < DownloadSelcetMax)
			addressTemp = DownloadAddress;//RButtonAddress+6;
		else if(pRb->ID < AdjustSelectMin)
			{
			addressTemp = AdjustminAddress;
			EEPROMWriteWord((pRb->ID-DownloadSelcetMax+1)*90, AdjustMthDataAddress);// add by Spring.Chen
			}
		else if(pRb->ID < AdjustSelectMax)
			{
			addressTemp = AdjustAddress;//RButtonAddress+8;
			EEPROMWriteWord((pRb->ID-AdjustSelectMin)*200+100, AdjustTimesDataAddress);// add by Spring.Chen
			}
		else if(pRb->ID < KSelectMax)
			{
				addressTemp = KSelectAddress;//RButtonAddress+0x000A;
				// µ±Ç°IDºÅ¼õÖ®Ç°ËùÓÐIDºÅµÄ×ÜºÍÊ£50ºóÔÙ¼ÓÉÏ2000¾ÍÊÇµ±Ç°µÄKÖµ
				EEPROMWriteWord((pRb->ID-AdjustSelectMax)*50+2000, KSelectDataAddress);// add by Spring.Chen
			}
		else if(pRb->ID < BlowTimeMax)
		{
			addressTemp = BlowTimeAddress;//RButtonAddress+14;
			EEPROMWriteWord((pRb->ID -35 + 4)*10, BlowTimeDataAddress);// add by Spring.Chen( 35Îªµ¥Ñ¡¿ò¿ªÊ¼°´Å¥)
		}
		else if(pRb->ID < BlowPressMax)
		{
			addressTemp = BlowPressAddress;//RButtonAddress+16;
			if(pRb->ID==41)
				EEPROMWriteWord(30, BlowPressDataAddress);
			else if(pRb->ID==42)
				EEPROMWriteWord(40, BlowPressDataAddress);
			else if(pRb->ID==43)
				EEPROMWriteWord(60, BlowPressDataAddress);
			else if(pRb->ID==44)
				EEPROMWriteWord(90, BlowPressDataAddress);
			else if(pRb->ID==45)
				EEPROMWriteWord(170, BlowPressDataAddress);
			else if(pRb->ID==46)
				EEPROMWriteWord(230, BlowPressDataAddress);			
			//EEPROMWriteWord((pRb->ID -41 + 1)*60, BlowPressDataAddress);// add by Spring.Chen( 41 Îªµ¥Ñ¡¿ò¿ªÊ¼°´Å¥)
		}
        	EEPROMWriteByte(pRb->ID, addressTemp);// add by Spring.Chen
#endif
        SetState(pRb, RB_CHECKED|RB_DRAW_CHECK); // set check and redraw
    }
}

/*********************************************************************
* Function: WORD RbTranslateMsg(RADIOBUTTON *pRb, GOL_MSG *pMsg)
*
* Overview: translates the GOL message for the radio button
*
********************************************************************/
WORD RbTranslateMsg(RADIOBUTTON *pRb, GOL_MSG *pMsg)
{
	// Evaluate if the message is for the radio button
    // Check if disabled first
	if ( GetState(pRb,RB_DISABLED) )
		return OBJ_MSG_INVALID;

#ifdef USE_TOUCHSCREEN    
    if(pMsg->type == TYPE_TOUCHSCREEN){
        if(pMsg->uiEvent == EVENT_PRESS){
    	    // Check if it falls in the radio button borders
	        if( (pRb->left   < pMsg->param1) &&
   	            (pRb->right  > pMsg->param1) &&
                (pRb->top    < pMsg->param2) &&
                (pRb->bottom > pMsg->param2) ){

                if(!GetState(pRb,RB_CHECKED))
                    return RB_MSG_CHECKED;
            }
        }

	    return OBJ_MSG_INVALID;	
    }
#endif

#ifdef USE_KEYBOARD

    if(pMsg->type == TYPE_KEYBOARD){

	    if(pMsg->param1 == pRb->ID){

            if(pMsg->uiEvent == EVENT_KEYSCAN){
                if( (pMsg->param2 == SCAN_SPACE_PRESSED)||
                    (pMsg->param2 == SCAN_CR_PRESSED) ) {

                    if(!GetState(pRb,RB_CHECKED))
                        return RB_MSG_CHECKED;
                }
            }
        }
        return OBJ_MSG_INVALID;	
    }

#endif

	return OBJ_MSG_INVALID;	
}

/*********************************************************************
* Function: WORD RbDraw(RADIOBUTTON *pRb)
*
* Output: returns the status of the drawing
*		  0 - not completed
*         1 - done
*
* Overview: draws radio button
*
********************************************************************/
WORD RbDraw(RADIOBUTTON *pRb)
{
typedef enum {
	REMOVE,
	DRAW_BUTTON0,
	DRAW_BUTTON1,
	DRAW_TEXT,
	DRAW_TEXT_RUN,
    DRAW_CHECK,
    DRAW_CHECK_RUN,
    DRAW_FOC,
} RB_DRAW_STATES;

//#define SIN45 46341
static RB_DRAW_STATES state = REMOVE;
SHORT checkIndent;
static SHORT radius;
static SHORT x, y;
XCHAR	addressTemp;// add by Spring.chen
//static DWORD_VAL  temp;

WORD faceClr;

    if(IsDeviceBusy())
        return 0;

    switch(state){

        case REMOVE:
            if(GetState(pRb,(RB_HIDE|RB_DRAW))){
                if(IsDeviceBusy())
                    return 0;
               	SetColor(pRb->pGolScheme->CommonBkColor);
                Bar(pRb->left,pRb->top,pRb->right,pRb->bottom);
            }
            if(GetState(pRb,RB_HIDE))
                return 1;

            radius = ((pRb->bottom-pRb->top)>>1) - RB_INDENT;
            x = pRb->left+((pRb->bottom-pRb->top)>>1) + RB_INDENT;
            y = (pRb->bottom+pRb->top)>>1;
//            temp.Val = SIN45;
            
	        if(GetState(pRb,RB_DRAW)){
	            state = DRAW_BUTTON0;
	        } 
	        else {
	        	state = DRAW_CHECK;
	        	goto rb_draw_check;
	        }

        case DRAW_BUTTON0:
            if(!GetState(pRb,RB_DISABLED)){
	            faceClr = pRb->pGolScheme->Color0;
            }else{
	            faceClr = pRb->pGolScheme->ColorDisabled;
            }
			GOLPanelDraw(x, y, x, y, radius, faceClr, 
							pRb->pGolScheme->EmbossDkColor, pRb->pGolScheme->EmbossLtColor, 
							NULL, GOL_EMBOSS_SIZE);
            state = DRAW_BUTTON1;	

 		case DRAW_BUTTON1:
            if(!GOLPanelDrawTsk()){
                return 0;
            }
   			state = DRAW_TEXT;

        case DRAW_TEXT:

        	if(pRb->pText != NULL){
				SetFont(pRb->pGolScheme->pFont);

                if(GetState(pRb,RB_DISABLED)){
		        	SetColor(pRb->pGolScheme->TextColorDisabled);	                
                }else{
                    SetColor(pRb->pGolScheme->TextColor0);
                }

                MoveTo( pRb->left+pRb->bottom-pRb->top+RB_INDENT,
                		(pRb->bottom+pRb->top-pRb->textHeight)>>1 );

                state = DRAW_TEXT_RUN;

                case DRAW_TEXT_RUN:
                	if(!OutText(pRb->pText))
                    	return 0;
            }
            state = DRAW_CHECK;

rb_draw_check:
        case DRAW_CHECK:
#ifdef	SaveState
	addressTemp = RButtonAddress;
		if(pRb->ID < BackLightSelectMax)
			addressTemp = BackLightAddress;
		else if(pRb->ID < PrintSelectMax)
			addressTemp = PrintSelectAddress;
		else if(pRb->ID < ThicknessSelectMax)
			addressTemp = ThicknessAddress;
		else if(pRb->ID < DownloadSelcetMax)
			addressTemp = DownloadAddress;
		else if(pRb->ID < AdjustSelectMin)
			addressTemp = AdjustminAddress;
		else if(pRb->ID < AdjustSelectMax)
			addressTemp = AdjustAddress;
		else if(pRb->ID < KSelectMax)
			addressTemp = KSelectAddress;
		else if(pRb->ID < BlowTimeMax)
			addressTemp = BlowTimeAddress;
		else if(pRb->ID < BlowPressMax)
			addressTemp = BlowPressAddress;
	if(EEPROMReadByte(addressTemp)== pRb->ID)
        		{
	                SetState(pRb, RB_CHECKED|RB_DRAW_CHECK);
        		}
#endif
            if(GetState(pRb,RB_CHECKED)){
                if(GetState(pRb,RB_DISABLED)){
                    SetColor(pRb->pGolScheme->TextColorDisabled);
                }else{
   	       	    #ifdef USE_MONOCHROME
                	SetColor(BLACK);
                #else
                    SetColor(pRb->pGolScheme->TextColor0);
                #endif    
                }
            }else{
                if(GetState(pRb,RB_DISABLED)){
                    SetColor(pRb->pGolScheme->ColorDisabled);
                }else{
   	       	    #ifdef USE_MONOCHROME
                	SetColor(WHITE);
                #else
                    SetColor(pRb->pGolScheme->Color0);
                #endif    
                }
            }
            
            state = DRAW_CHECK_RUN;
            
        case DRAW_CHECK_RUN:
            checkIndent = (pRb->bottom-pRb->top)>>2;
            if(!FillCircle(x,y,radius-checkIndent))
            	return 0;

            state = DRAW_FOC;

        case DRAW_FOC:

	        if(GetState(pRb,RB_DRAW|RB_DRAW_FOCUS)){
                if(IsDeviceBusy())
                    return 0;

    	        if(GetState(pRb,RB_FOCUSED)){
		            SetColor(pRb->pGolScheme->TextColor0);
                }else{
                    SetColor(pRb->pGolScheme->CommonBkColor);
                }
    	        SetLineType(FOCUS_LINE);
		        Rectangle(pRb->left, pRb->top,
                          pRb->right, pRb->bottom);
		        SetLineType(SOLID_LINE);
	        }

            state = REMOVE;
            return 1;
    }
    return 1;
}

#endif // USE_RADIOBUTTON
