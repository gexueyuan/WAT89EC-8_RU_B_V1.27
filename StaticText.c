/*****************************************************************************
 *  Module for Microchip Graphics Library 
 *  GOL Layer 
 *  Static Text
 *****************************************************************************
 * FileName:        StaticText.c
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
 * Paolo A. Tamayo		11/12/07	Version 1.0 release
*****************************************************************************/
#include "Graphics.h"

#ifdef USE_STATICTEXT

/*********************************************************************
* Function: STATICTEXT  *StCreate(WORD ID, SHORT left, SHORT top, SHORT right, SHORT bottom, 
*								  WORD state , XCHAR *pText, GOL_SCHEME *pScheme)
*
* Notes: Creates a STATICTEXT object and adds it to the current active list.
*        If the creation is successful, the pointer to the created Object 
*        is returned. If not successful, NULL is returned.
*
********************************************************************/
STATICTEXT *StCreate(WORD ID, SHORT left, SHORT top, SHORT right, SHORT bottom, 
			         WORD state, XCHAR *pText, GOL_SCHEME *pScheme)
{
	STATICTEXT *pSt = NULL;
	
	pSt = malloc(sizeof(STATICTEXT));
	if (pSt == NULL)
		return pSt;
	
	pSt->ID      	= ID;					// unique id assigned for referencing
	pSt->pNxtObj 	= NULL;					// initialize pointer to NULL
	pSt->type    	= OBJ_STATICTEXT;		// set object type
	pSt->left    	= left;       	    	// left,top corner
	pSt->top     	= top;
	pSt->right   	= right;     	    	// right buttom corner
	pSt->bottom  	= bottom;
	pSt->pText   	= pText;				// location of the text
	pSt->state   	= state; 	

	// Set the color scheme to be used
	if (pScheme == NULL)
		pSt->pGolScheme = _pDefaultGolScheme; 
	else 	
		pSt->pGolScheme = (GOL_SCHEME *)pScheme; 	

	pSt->textHeight 	= 0;
	if (pSt->pText != NULL) {
		// Set the text height  
		pSt->textHeight 	= GetTextHeight(pSt->pGolScheme->pFont);
	}	
	
    GOLAddObject((OBJ_HEADER*) pSt);
	return pSt;
}

/*********************************************************************
* Function: StSetText(STATICTEXT *pSt, XCHAR *pText)
*
* Notes: Sets the string that will be used.
*
********************************************************************/
void StSetText(STATICTEXT *pSt, XCHAR *pText)
{
	pSt->pText = pText;
	pSt->textHeight = GetTextHeight(pSt->pGolScheme->pFont);
}

/*********************************************************************
* Function: WORD StTranslateMsg(STATICTEXT *pSt, GOL_MSG *pMsg)
*
* Notes: Evaluates the message if the object will be affected by the 
*		 message or not.
*
********************************************************************/
WORD StTranslateMsg(STATICTEXT *pSt, GOL_MSG *pMsg)
{
	// Evaluate if the message is for the static text
    // Check if disabled first
	if (GetState(pSt, ST_DISABLED))
        return OBJ_MSG_INVALID;

#ifdef  USE_TOUCHSCREEN
    if(pMsg->type == TYPE_TOUCHSCREEN) {
		// Check if it falls in static text control borders
		if( (pSt->left     < pMsg->param1) &&
	  	    (pSt->right    > pMsg->param1) &&
	   	    (pSt->top      < pMsg->param2) &&
	   	    (pSt->bottom   > pMsg->param2) ) {
			       	
		    	if(pMsg->uiEvent == EVENT_PRESS) 
		    		return ST_MSG_SELECTED;
        }
	}
#endif	

	return OBJ_MSG_INVALID;
}

/*********************************************************************
* Function: WORD StDraw(STATICTEXT *pSt)
*
* Notes: This is the state machine to draw the static text.
*
********************************************************************/
WORD StDraw(STATICTEXT *pSt)
{
typedef enum {
	ST_STATE_IDLE,
	ST_STATE_CLEANAREA,
	ST_STATE_INIT,
	ST_STATE_SETALIGN,
	ST_STATE_DRAWTEXT,
} ST_DRAW_STATES;

static ST_DRAW_STATES state = ST_STATE_IDLE;
static SHORT charCtr = 0, lineCtr = 0;
static XCHAR *pCurLine = NULL;
SHORT textWidth;
XCHAR   ch = 0;


    if(IsDeviceBusy())
        return 0;
        
    switch(state){

        case ST_STATE_IDLE:
        
            SetClip(CLIP_DISABLE);

           	if (GetState(pSt, ST_HIDE)) {
   	   	        SetColor(pSt->pGolScheme->CommonBkColor);
    	        Bar(pSt->left,pSt->top,pSt->right,pSt->bottom);
    	        // State is still IDLE STATE so no need to set
    	        return 1;
    	    }    
	           	
	       	if (GetState(pSt, ST_FRAME)) {
		       	// show frame if specified to be shown
	            if(!GetState(pSt,ST_DISABLED)){
		            // show enabled color
    	   	        SetColor(pSt->pGolScheme->Color1);
	    	        Rectangle(pSt->left,pSt->top,pSt->right,pSt->bottom);

            	}
	            else {
       	        	// show disabled color
    	   	        SetColor(pSt->pGolScheme->ColorDisabled);
		            Rectangle(pSt->left,pSt->top,pSt->right,pSt->bottom);
    	        }
    	    }
    	    // set clipping area, text will only appear inside the static text area.    
            SetClip(CLIP_ENABLE);
            SetClipRgn(pSt->left+ST_INDENT, pSt->top,   		\
                       pSt->right-ST_INDENT, pSt->bottom);    
            state = ST_STATE_CLEANAREA;

        case ST_STATE_CLEANAREA:
        
            if(IsDeviceBusy())
                return 0;

			// clean area where text will be placed.
			SetColor(pSt->pGolScheme->CommonBkColor);
    	    Bar(pSt->left+1,pSt->top + 2,pSt->right+1,pSt->bottom-1);
            state = ST_STATE_INIT;
    	    
        case ST_STATE_INIT:
        
            if(IsDeviceBusy())
                return 0;

			// set the text color
	        if(!GetState(pSt,ST_DISABLED)){
	   	        SetColor(pSt->pGolScheme->TextColor0);
	        }    
	        else {
	   	        SetColor(pSt->pGolScheme->TextColorDisabled);
	        }    
	        // use the font specified in the object
	        SetFont(pSt->pGolScheme->pFont);
			pCurLine = pSt->pText;						// get first line of text
			state = ST_STATE_SETALIGN;					// go to drawing of text

        case ST_STATE_SETALIGN:
          
st_state_alignment:

			if (!charCtr) {
				// set position of the next character (based on alignment and next character)
			    textWidth = GetTextWidth(pCurLine, pSt->pGolScheme->pFont);  
			
				// Display text with center alignment
				if (GetState(pSt, (ST_CENTER_ALIGN))) { 			
					MoveTo((pSt->left+pSt->right-textWidth) >> 1,	\
					       pSt->top+(lineCtr * pSt->textHeight));
				}	
				// Display text with right alignment
				else if (GetState(pSt, (ST_RIGHT_ALIGN))) {     	
					MoveTo((pSt->right-textWidth-ST_INDENT), 		\
					        pSt->top+(lineCtr * pSt->textHeight));
				}	
				// Display text with left alignment
				else {									
					MoveTo(pSt->left+ST_INDENT, 					\
					       pSt->top+(lineCtr * pSt->textHeight));
				}	
			}	
			state = ST_STATE_DRAWTEXT;
			
        case ST_STATE_DRAWTEXT:

			ch = *(pCurLine + charCtr);
			// output one character at time until a newline character or a NULL character is sampled
		    while((0x0000 != ch) && (0x000A != ch )) {
		        if(IsDeviceBusy()) {
			        return 0;								// device is busy return 
		        }	
		        OutChar(ch);								// render the character
		        charCtr++;									// update to next character
				ch = *(pCurLine + charCtr);
		    }
	    
			// pCurText is updated for the next line 
			if (ch == 0x000A) {								// new line character
				pCurLine = pCurLine + charCtr + 1;			// go to first char of next line
				lineCtr++;									// update line counter
			    charCtr = 0;          						// reset char counter
				goto st_state_alignment;					// continue to next line
			}	
			// end of text string is reached no more lines to display
			else {
				pCurLine = NULL;							// reset static variables
				lineCtr = 0;
				charCtr = 0;
	            SetClip(CLIP_DISABLE);						// remove clipping
				state = ST_STATE_IDLE;						// go back to IDLE state
				return 1;
			}	

    }
    return 1;
}

#endif // USE_STATICTEXT
