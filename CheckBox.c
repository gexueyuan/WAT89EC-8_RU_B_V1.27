/*****************************************************************************
 *  Module for Microchip Graphics Library
 *  GOL Layer 
 *  Check Box
 *****************************************************************************
 * FileName:        CheckBox.c
 * Dependencies:    None 
 * Processor:       PIC24, PIC32
 * Compiler:       	MPLAB C30 V3.00, C32
 * Linker:          MPLAB LINK30, LINK32
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
 * Anton Alkhimenok     11/12/07	Version 1.0 release
 *****************************************************************************/

#include "Graphics.h"
#include "DefineXY.h"
#ifdef USE_CHECKBOX

/*********************************************************************
* Function: CHECKBOX  *CbCreate(WORD ID, SHORT left, SHORT top, SHORT right, 
*                              SHORT bottom, WORD state, XCHAR *pText, 
*                              GOL_SCHEME *pScheme)
*
* Overview: Creates the check box.
*
********************************************************************/
CHECKBOX *CbCreate(WORD ID, SHORT left, SHORT top, SHORT right, SHORT bottom, 
			       WORD state, XCHAR *pText, GOL_SCHEME *pScheme)
{
	CHECKBOX *pCb = NULL;
	
	pCb = malloc(sizeof(CHECKBOX));
	if (pCb == NULL)
		return pCb;

	pCb->ID      	= ID;
	pCb->pNxtObj 	= NULL;
	pCb->type    	= OBJ_CHECKBOX;
	pCb->left    	= left;
	pCb->top     	= top;
	pCb->right   	= right;
	pCb->bottom  	= bottom;
	pCb->pText   	= pText;
	pCb->state   	= state;

	// Set the style scheme
	if (pScheme == NULL)
		pCb->pGolScheme = _pDefaultGolScheme; 
	else 	
		pCb->pGolScheme = (GOL_SCHEME *)pScheme; 	

	// Set the text height  
    pCb->textHeight = 0;
    if(pText != NULL){
	    pCb->textHeight = GetTextHeight(pCb->pGolScheme->pFont);
    }

    GOLAddObject((OBJ_HEADER*) pCb);
	
	return pCb;
}

/*********************************************************************
* Function: CbSetText(CHECKBOX *pCb, char *pText)
*
* Overview: Sets the text.
*
********************************************************************/
void CbSetText(CHECKBOX *pCb, XCHAR *pText)
{
	pCb->pText = pText;
	pCb->textHeight = GetTextHeight((BYTE *)pCb->pGolScheme->pFont);
}

/*********************************************************************
* Function: CbMsgDefault(WORD translatedMsg, CHECKBOX *pCb, GOL_MSG* pMsg)
*
* Overview: Changes the state of the check box by default.
*
********************************************************************/
void CbMsgDefault(WORD translatedMsg, CHECKBOX* pCb, GOL_MSG* pMsg){

#ifdef  USE_FOCUS
#ifdef  USE_TOUCHSCREEN
    if(pMsg->type == TYPE_TOUCHSCREEN){
        if(!GetState(pCb,CB_FOCUSED)){
                GOLSetFocus((OBJ_HEADER*)pCb);
        }
    }
#endif
#endif

    switch(translatedMsg){
        case CB_MSG_CHECKED:
#ifdef	SaveState
        	EEPROMWriteByte(1, (ID_CHECKBOXADDRESS + pCb->ID - 0));//20 is ID_CHECKBOX1  add by Spring.Chen
#endif
            SetState(pCb, CB_CHECKED|CB_DRAW_CHECK); // Set checked and redraw
            break;

        case CB_MSG_UNCHECKED:
#ifdef	SaveState
        	EEPROMWriteByte(0, (ID_CHECKBOXADDRESS + pCb->ID - 0));//20 is ID_CHECKBOX1 add by Spring.Chen
#endif
            ClrState(pCb, CB_CHECKED);        // Reset check   
            SetState(pCb, CB_DRAW_CHECK);     // Redraw
            break;
    }

}

/*********************************************************************
* Function: WORD CbTranslateMsg(CHECKBOX *pCb, GOL_MSG *pMsg)
*
* Overview: Checks if the check box will be affected by the message
*           and returns translated message.
*
********************************************************************/
WORD CbTranslateMsg(CHECKBOX *pCb, GOL_MSG *pMsg)
{
	// Evaluate if the message is for the check box
    // Check if disabled first
	if ( GetState(pCb,CB_DISABLED) )
		return OBJ_MSG_INVALID;

#ifdef USE_TOUCHSCREEN
    if(pMsg->type == TYPE_TOUCHSCREEN){
    	// Check if it falls in the check box borders
	    if( (pCb->left   < pMsg->param1) &&
   	        (pCb->right  > pMsg->param1) &&
            (pCb->top    < pMsg->param2) &&
            (pCb->bottom > pMsg->param2) ){

            if(pMsg->uiEvent == EVENT_PRESS){
                if(GetState(pCb,CB_CHECKED))
                    return CB_MSG_UNCHECKED;
                else
                    return CB_MSG_CHECKED;
            }
        }

	    return OBJ_MSG_INVALID;	
    }
#endif

#ifdef USE_KEYBOARD

    if(pMsg->type == TYPE_KEYBOARD){

	    if(pMsg->param1 == pCb->ID){

	        if(pMsg->uiEvent == EVENT_KEYSCAN){

                if( (pMsg->param2 == SCAN_SPACE_PRESSED) ||
                    (pMsg->param2 == SCAN_CR_PRESSED) ){

                    if(GetState(pCb,CB_CHECKED))
                        return CB_MSG_UNCHECKED;
                    else
                        return CB_MSG_CHECKED;
                }
            }
        }
	    return OBJ_MSG_INVALID;	
    }

#endif

	return OBJ_MSG_INVALID;	
}

/*********************************************************************
* Function: WORD CbDraw(CHECKBOX *pCb)
*
* Output: returns the status of the drawing
*		  0 - not complete
*         1 - done
*
* Overview: Draws check box.
*
********************************************************************/
WORD CbDraw(CHECKBOX *pCb)
{
typedef enum {
	REMOVE,
	BOX_DRAW,
	RUN_DRAW,
	TEXT_DRAW,
    TEXT_DRAW_RUN,
    CHECK_DRAW,
    FOCUS_DRAW,
} CB_DRAW_STATES;

static CB_DRAW_STATES state = REMOVE;

SHORT checkIndent;

    if(IsDeviceBusy())
        return 0;

    switch(state){

        case REMOVE:
            if(GetState(pCb,CB_HIDE|CB_DRAW)){

                if(IsDeviceBusy())
                    return 0;
                SetColor(pCb->pGolScheme->CommonBkColor);
                Bar(pCb->left,pCb->top,pCb->right,pCb->bottom);
                if(GetState(pCb,CB_HIDE))
                    return 1;
            }
            state = BOX_DRAW;

        case BOX_DRAW:

            if(GetState(pCb,CB_DRAW)){

                if(!GetState(pCb,CB_DISABLED)){

                    GOLPanelDraw(pCb->left+CB_INDENT,pCb->top+CB_INDENT,
                             pCb->left+(pCb->bottom-pCb->top)-CB_INDENT,
                             pCb->bottom-CB_INDENT, 0,
                             pCb->pGolScheme->Color0,
                             pCb->pGolScheme->EmbossDkColor,
                             pCb->pGolScheme->EmbossLtColor,
                             NULL, GOL_EMBOSS_SIZE);


                }else{

                    GOLPanelDraw(pCb->left+CB_INDENT,pCb->top+CB_INDENT,
                             pCb->left+(pCb->bottom-pCb->top)-CB_INDENT,
                             pCb->bottom-CB_INDENT, 0,
                             pCb->pGolScheme->ColorDisabled,
                             pCb->pGolScheme->EmbossDkColor,
                             pCb->pGolScheme->EmbossLtColor,
                             NULL, GOL_EMBOSS_SIZE);

                }

                state = RUN_DRAW;

                case RUN_DRAW:
                    if(!GOLPanelDrawTsk())
                        return 0;
                    state = TEXT_DRAW;
                    

                case TEXT_DRAW:
                	if(pCb->pText != NULL){

                        SetFont(pCb->pGolScheme->pFont);

                        if (!GetState(pCb,CB_DISABLED)){
	                        SetColor(pCb->pGolScheme->TextColor0);
                        }else{
		                    SetColor(pCb->pGolScheme->TextColorDisabled);
                        }

                        MoveTo(pCb->left+pCb->bottom-pCb->top+CB_INDENT,
                              (pCb->bottom+pCb->top-pCb->textHeight)>>1);

                        state = TEXT_DRAW_RUN;

                        case TEXT_DRAW_RUN:
                            if(!OutText(pCb->pText))
                                return 0;	
                    }

            }
            state = CHECK_DRAW;

        case CHECK_DRAW:

            if(GetState(pCb,RB_DRAW|RB_DRAW_CHECK)){
                if(IsDeviceBusy())
                    return 0;
#ifdef	SaveState
                	if(EEPROMReadByte(ID_CHECKBOXADDRESS + pCb->ID - 0))//20 is ID_CHECKBOX1; add by Spring.Chen
			{
//				ClrState(pCb, CB_CHECKED);        // Reset check   
            			SetState(pCb, CB_CHECKED|CB_DRAW_CHECK);     // Redraw // add 2 line by Spring.Chen
                	}
                	else
                	{
				ClrState(pCb, CB_CHECKED);        // Reset check   
            			SetState(pCb, CB_DRAW_CHECK);     // Redraw // add 2 line by Spring.Chen
                	}
       #endif
                if(!GetState(pCb,CB_DISABLED)){
       #if	0//def	SaveState
                	if(EEPROMReadByte(0x7F00 + pCb->ID - 20 + 1))//20 is ID_CHECKBOX1; add by Spring.Chen
			{
//				ClrState(pCb, CB_CHECKED);        // Reset check   
            			SetState(pCb, CB_CHECKED|CB_DRAW_CHECK);     // Redraw // add 2 line by Spring.Chen
                	}
       #endif
                    if(GetState(pCb,CB_CHECKED)){
                        SetColor(pCb->pGolScheme->TextColor0);
                    }else{
                        SetColor(pCb->pGolScheme->Color0);
                    }
                }else{
       #if	0//def	SaveState
                if(EEPROMReadByte(0x7F00 + pCb->ID - 20 + 1))//20 is ID_CHECKBOX1; add by Spring.Chen
			{
				ClrState(pCb, CB_CHECKED);        // Reset check   
            			SetState(pCb, CB_DRAW_CHECK);     // Redraw // add 2 line by Spring.Chen
                	}
       #endif
                    if(GetState(pCb,CB_CHECKED)){
                        SetColor(pCb->pGolScheme->TextColorDisabled);
                    }else{
                        SetColor(pCb->pGolScheme->ColorDisabled);
                    }
                }

                checkIndent = (pCb->bottom-pCb->top)>>2;

                Bar(pCb->left+checkIndent+GOL_EMBOSS_SIZE,
                    pCb->top+checkIndent+GOL_EMBOSS_SIZE,
                    pCb->left+(pCb->bottom-pCb->top)-checkIndent-GOL_EMBOSS_SIZE,
                    pCb->bottom-checkIndent-GOL_EMBOSS_SIZE);
            }
            state = FOCUS_DRAW;

        case FOCUS_DRAW:
	        if(GetState(pCb,CB_DRAW|CB_DRAW_FOCUS)){
                if(IsDeviceBusy())
                    return 0;
	            if(GetState(pCb,CB_FOCUSED)){
		            SetColor(pCb->pGolScheme->TextColor0);
                }else{
                    SetColor(pCb->pGolScheme->CommonBkColor);
                }
    	        SetLineType(FOCUS_LINE);
		        Rectangle(pCb->left, pCb->top,
                          pCb->right, pCb->bottom);
		        SetLineType(SOLID_LINE);
	        }
            state = REMOVE;
            return 1;
    }
    return 1;
}

#endif // USE_CHECKBOX
