/*****************************************************************************
 *
 * Beeper.
 *
 *****************************************************************************
 * FileName:        Beep.h
 * Dependencies:    None
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
 * Anton Alkhimenok		07/10/07	...
 * Anton Alkhimenok     02/07/08    PIC32 support
 *****************************************************************************/

#ifndef _BEEP_H
#define _BEEP_H

#define OC_TRIS_BIT     TRISDbits.TRISD0
#define OC_LAT_BIT      LATDbits.LATD0
extern WORD BEEP_TIME,TIMER_BASE;
extern BOOL alocholdetecet;
/*
#define BEEP_TIME       200

#ifdef __PIC32MX
#define TIMER_BASE      4400
#else
#define TIMER_BASE      400
#endif
*/
/*********************************************************************
* Function:  void BeepInit(void)
*
* PreCondition: none
*
* Input: none
*
* Output: none
*
* Side Effects: none
*
* Overview: initializes beeper
*
* Note: none
*
********************************************************************/
void BeepInit();

/*********************************************************************
* Macro:  Beep()
*
* PreCondition: none
*
* Input: none
*
* Output: none
*
* Side Effects: none
*
* Overview: starts beeper
*
* Note: none
*
********************************************************************/
#define     Beep() T2CONbits.TON = 1

#define uchar unsigned char
#define uint unsigned int
#define pump_pwr      PORTFbits.RF0
#define pump_on       PORTFbits.RF1
#define pump_reset       PORTGbits.RG1

void adcinit();
void delayus(unsigned char us);
void delayms(uint ms);
unsigned int  Adc_Count(uchar Type,uchar Count);
unsigned int ADC(uchar nm);
unsigned int Filter(uchar Type);
void  inti_ads7822(void);
void  adc_delay(uchar i);
unsigned int press_test(BYTE type);//WORD Temp_adc);
unsigned int ads7822(void);
unsigned int Blow_And_Test(void);
void inti_time5(void);
extern unsigned int prepare_and_Charge(void);
extern unsigned int caiyang(void);
extern void pumpinit(void);
extern void  Pump_action(void);
extern unsigned int Decade[20];
extern unsigned char  tem_interzone(uchar y[],uint t);

extern unsigned char wender_show(unsigned char shu);

extern void  Data_Calculate1(uchar y3[],uchar Count3,uint Du2);

extern uchar  ByteToByte(uchar Temp1,uchar Temp2);

extern uint ok_pp;
volatile extern unsigned int  flag;
extern void Data_Calculate2(int ee1,int rr2,int rr3);

extern void Data_Calculate3(int rr1,int rr2);

extern int  Data_Calculate4(unsigned char b[],uchar yo);
extern unsigned int  cons_calculate(unsigned int adt);
extern void Hexshow(XCHAR Dem[],unsigned int tem);

extern unsigned int  Cons_Aduj(uint app);

#endif // _BEEP_H
