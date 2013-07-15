/*****************************************************************************
 *
 * Real Time Clock Calender
 *
 *****************************************************************************
 * FileName:        rtcc.c
 * Dependencies:    
 * Processor:       PIC24, PIC32
 * Compiler:       	C30 xx.xx or higher, C32
 * Linker:          MPLINK 03.20.01 or higher, MPLINK32
 * Company:         Microchip Technology Incorporated
 *
 * Software License Agreement
 *
 * The software supplied herewith by Microchip Technology Incorporated
 * (the "Company") is intended and supplied to you, the Company's
 * customer, for use solely and exclusively with products manufactured
 * by the Company. 
 *
 * The software is owned by the Company and/or its supplier, and is 
 * protected under applicable copyright laws. All rights are reserved. 
 * Any use in violation of the foregoing restrictions may subject the 
 * user to criminal sanctions under applicable laws, as well as to 
 * civil liability for the breach of the terms and conditions of this 
 * license.
 *
 * THIS SOFTWARE IS PROVIDED IN AN "AS IS" CONDITION. NO WARRANTIES, 
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED 
 * TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A 
 * PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE COMPANY SHALL NOT, 
 * IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR 
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 *
 * Author               Date        Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Chris Valenti		05-26-05	...	
 * Ross Fosler			06-06-2005	Several changes
 * Anton Alkhimenok     10-21-2005  Get/Set functions
 * Anton Alkhimenok     02-07-2008  PIC32 support
 *****************************************************************************/
#include "MainDemo.h"

/*****************************************************************************
 * Structures: _time and _time_chk
 *
 * Overview: These structures contain the time and date.
 * RTCCProcessEvents updates them. _time_chk is used as input/output for
 * get/set operations.
 *
 *****************************************************************************/
RTCC _time;
RTCC _time_chk;
/*****************************************************************************
 * Arrays: _time_str and _date_str
 *
 * Overview: These arrays contain the time and date strings. 
 * RTCCProcessEvents updates them.
 *
 *****************************************************************************/
unsigned char _time_str[16] = "                ";		// Sat 10:01:15
unsigned char _date_str[16] = "                ";		// Sep 30, 2005

// The flag stops updating time and date and used for get/set operations.
unsigned char _rtcc_flag;
extern XCHAR Mon[]; //
extern XCHAR Tue[];    //
extern XCHAR Wed[];    //
extern XCHAR Thu[];    //
extern XCHAR Fri[];    //
extern XCHAR Sat[];    //
extern XCHAR Sun[];    //

#ifdef	useI2CRTCC

extern delay_ms(unsigned int cctt);
unsigned char b2h(unsigned char x);
void error_i2(void);
extern volatile BOOL AutooffSwitch;
extern volatile unsigned int offcount;
void __attribute__((interrupt, shadow, no_auto_psv)) _T4Interrupt(void)
{

	tick++; 
	
    // Clear flag
    IFS1bits.T4IF = 0;

if((AutooffSwitch == TRUE)&&(offcount<1000))
		offcount++;

}
unsigned int save_delayt;
/*void delay_ms(unsigned int cctt)
{
	
	save_delayt = sys_clk;
	while((sys_clk-save_delayt) < cctt) 
	{;}
	Nop();
	Nop();
	Nop();
}*/

extern void error_i2(void);
#if	0
struct data_famte1
{
	unsigned int TypeLimit;
	unsigned char ans_flag;
	unsigned char TimeLimit;
	unsigned long TStart;
	unsigned long TStop;
	unsigned long RCD;
};
extern struct data_famte1 read_recode;
union tagWORD1
{
	unsigned char ch[4];
	unsigned int wVal[2];
	unsigned long lVal;
};
extern union tagWORD1 wwc,wwd;
struct tagUARTFLAG
{
	unsigned URT1IN  :1;
    	unsigned URT2IN  :1;
	unsigned URT3IN  :1;
    	unsigned URT4IN  :1;
	unsigned BUSY1   :1;
	unsigned BUSY2   :1;
    	unsigned BUSY3   :1;
    	unsigned BUSY4   :1;
    	unsigned CALDJ   :1;
    	unsigned GPRSLEEP :1;
    	unsigned GPRSRDSMS :1;
    	unsigned GPRSAPNS  :1;
        unsigned SMS0OK  :1;
        unsigned SMS1OK  :1;
        unsigned SMS2OK  :1;
        unsigned GPRSBUSY :1;
};
extern struct tagUARTFLAG UARTFLAG;
#define _URT1IN UARTFLAG.URT1IN
#define _URT2IN UARTFLAG.URT2IN
#define _URT3IN UARTFLAG.URT3IN
#define _URT4IN UARTFLAG.URT4IN
#define _BUSY1 UARTFLAG.BUSY1
#define _BUSY2 UARTFLAG.BUSY2
#define _BUSY3 UARTFLAG.BUSY3
#define _BUSY4 UARTFLAG.BUSY4
#define _CALDJ UARTFLAG.CALDJ
#define _GPRSLEEP UARTFLAG.GPRSLEEP
#define _GPRSRDSMS UARTFLAG.GPRSRDSMS
#define _GPRSAPNS UARTFLAG.GPRSAPNS
#define _SMS0OK UARTFLAG.SMS0OK
#define _SMS1OK UARTFLAG.SMS1OK
#define _SMS2OK UARTFLAG.SMS2OK
#define _GPRSBUSY UARTFLAG.GPRSBUSY

struct tagLOGET2
{
	unsigned ZDBDF  :1;
	unsigned ANSZF  :1;
	unsigned ANSAF  :1;
	unsigned soeF   :1;
	unsigned FP2SAVE :1;
	unsigned FPCHECK :1;
	unsigned CALFLAG :1;
	unsigned TASKFLAG :1;
	unsigned SOESENDF :2;
	unsigned CALDBXLF :1;
	unsigned ZDCBR    :1;
	unsigned DCDFAULT  :1;
	unsigned GBUSY :1;
	unsigned GPRSAPN  :1;
	unsigned CALCSQ   :1;
};
extern struct tagLOGET2 LOGETFLAG2;
#define _ZDBDF LOGETFLAG2.ZDBDF
#define _ANSZF LOGETFLAG2.ANSZF
#define _ANSAF LOGETFLAG2.ANSAF
#define _soeF LOGETFLAG2.soeF
#define _FP2SAVE LOGETFLAG2.FP2SAVE
#define _FPCHECK LOGETFLAG2.FPCHECK
#define _CALFLAG LOGETFLAG2.CALFLAG
#define _TASKFLAG LOGETFLAG2.TASKFLAG
#define _SOESENDF LOGETFLAG2.SOESENDF
#define _CALDBXLF LOGETFLAG2.CALDBXLF
#define _ZDCBR LOGETFLAG2.ZDCBR
#define _DCDFAULT LOGETFLAG2.DCDFAULT
#define _GBUSY LOGETFLAG2.GBUSY
#define _GPRSAPN LOGETFLAG2.GPRSAPN
#define _CALCSQ LOGETFLAG2.CALCSQ
#endif
/**********************************************************************
* init I2 wang
* it indicate the i2c bus operation error when I2C_ERROR=0x55 *
* it means that the result is not avail                       
* extern unsigned char I2C_ERROR;
**********************************************************************/
void initI2 ( void )
{   
     unsigned int i,j,k;
     _RA2 = 0;//SCL2
     _TRISA2 = 0;
     _RA3  = 0;//SDA2
     _TRISA3 = 0;
     delay_ms(10);
//     DelayMs(10);
     _RA2 = 1;
     delay_ms(10);
//     DelayMs(10);
     _RA3 = 1;
     delay_ms(10);
//     DelayMs(10);
     //I2CCONbits.I2CEN=0,turn off the I2C module
     //I2CCONbits.I2CSIDL=0,continute module operation in  idle mode
     //I2CCONbits.SCLREL=1,release the clock
     //I2CCONbits.IPMIEN=0,disable the IPMI mode
     //I2CCONbits.A10M=0,I2CADD is a 7-bit slave address
     //I2CCONbits.DISSLW=1,disable the slew rate
     //I2CCONbits.SMEN=0,disable the SMbus
     //I2CCONbits.GCEN=0,disable the general call interrupt
     //I2CCONbits.STREN=0,disable the clock stretch
     //I2CCONbits.ACKDT=0,sending the ACK during acknowledge
     //I2CCONbits.ACKEN=0,disable the ACK
     //I2CCONbits.RCEN=0,disable the receiven the data for I2C bus
     //I2CCONbits.PEN=0,disable initiate the stop condition on the SDA and SCL
     //I2CCONbits.RSEN=0,disable repeat the start conditon
     //I2CCONbits.SEN=0,disable start condition on the SDA and SCL bus
     I2C2CONbits.I2CEN = 1;
     I2C2CON = 0x9240;
     //transmission rate is 400kbps(Fscl),I2CBRG=INT(Fcy/Fscl)-1==159
     I2C2BRG = 0x12;//0x13;////13
     //I2CSTAT
     //I2CSTAT=0X8030;
     //turn on the I2C module
     I2C2CONbits.I2CEN = 1;
     I2C_ERROR = 0xaa;

}

void error_i2(void)
{
	if(I2C2STATbits.BCL == 1)
	{
		//_TRISG2 = 0;
		I2C2CONbits.I2CEN = 0;
	    //_TRISG3 = 0;
		//_RG2 = 1;
		I2C2STATbits.BCL = 0;
		//_RG3 = 1;

		initI2();
	}
}

/* generate the start condition            *
 *  it is called by I2C_read and I2C_write */
void I2C_start(void)
{
     //clear the collision flag
     I2C2STATbits.IWCOL = 0;

     //initiate the stop condition
     I2C2CONbits.PEN = 1;
     jhm98 = 0;
     //ensure the stop condition is complete
     while (I2C2CONbits.PEN == 1)//=0 after STOP xulie
     {
	jhm98++;
	if(jhm98 > 0x320) break;//0xc80=0x320*4
     }
     //initiate the START condition
     I2C2CONbits.SEN = 1;
     jhm98 = 0;
     //ensure the START condition is completion
     while (I2C2CONbits.SEN == 1) //=0 after START xulie
     {
     	jhm98++;
     	if(jhm98 > 0x320) break;
     }
     I2C_ERROR = 0xaa;         
}

//extern unsigned char softMin,softSec,timeSet[7];
//extern unsigned int I2C_COUNTER,softMS;
unsigned int I2cWriteA(unsigned char Addr, unsigned char *Data, unsigned int NByte)
{
	//initiate the start condition
	I2C_start();
	
	//transmission the slave address
	I2C2TRN = Addr&0xfe;
	
	while (I2C2STATbits.TBF == 1)
	{
		I2C_COUNTER++;
		if(I2C_COUNTER > 0x320)
		{
			I2C_ERROR = 0x55;
			break;
		}
	}
	
	I2C_COUNTER = 0;
	
	//wait for the acknowledge
	while (I2C2STATbits.ACKSTAT == 1)
	{
		I2C_COUNTER++;
		if(I2C_COUNTER > 0x320)
		{
			I2C_ERROR = 0x55;
			break;
		}
	}
	
	while(NByte != 0)
	{
		if(I2C_ERROR == 0xaa)
		{
			//ensure the not acknowledge is complete
			I2C_COUNTER = 0;
			while (I2C2STATbits.TRSTAT == 1)
			{
				I2C_COUNTER++;
				if(I2C_COUNTER > 0x320)
				{
					I2C_ERROR = 0x55;
					break;
				}
			}
			//clear the I2C_COUNTER
			I2C_COUNTER = 0;
			//transmission the data
			I2C2TRN = *Data++;
			NByte--;
			//wait for the slave data's transmission was complete
			while (I2C2STATbits.TBF == 1)
			{
				I2C_COUNTER++;
				if(I2C_COUNTER > 0x320)
				{
					I2C_ERROR = 0x55;
					break;
				}
			}
			I2C_COUNTER = 0;
			//wait for the acknowledge
			while (I2C2STATbits.ACKSTAT == 1)
			{
				I2C_COUNTER++;
				if(I2C_COUNTER > 0x320)
				{
					I2C_ERROR = 0x55;
					break;
				}
			}
		}
		else break;
	}
	return (NByte);
	
}
unsigned int I2cWrite(unsigned char Addr, unsigned char *Data, unsigned int NByte)
{
	unsigned int lengthW;
	lengthW = I2cWriteA(Addr, Data, NByte);
	
	
	//initiate the stop condition
	I2C2CONbits.PEN = 1;
	I2C_COUNTER = 0;
	//ensure the stop condition is complete
	while (I2C2CONbits.PEN == 1)
	{
		I2C_COUNTER++;
		if(I2C_COUNTER > 0x320) break;
	}
	//check the operation condition
	if(I2C2STATbits.IWCOL == 1) 
	{
		I2C_ERROR = 0x55;
		//clear the collision flag
		I2C2STATbits.IWCOL = 0;
	}
	return (lengthW);
}

unsigned int I2cRead(unsigned char Addr, unsigned char *Ret, unsigned char *Eaddr, unsigned int EaddrNByte, unsigned int ReadNbyte)
{
	unsigned char i;
	if (EaddrNByte > 0)
    	{
        	if (I2cWriteA(Addr, Eaddr, EaddrNByte) != 0)
        	{
            		return 0;
        	}
    	}
    
    //initiate the start condition
	I2C_start();
	//transmission the slave address
	I2C2TRN = Addr|0x01;
	while (I2C2STATbits.TBF == 1)
	{
		I2C_COUNTER++;
		if(I2C_COUNTER > 0x320)
		{
			I2C_ERROR = 0x55;
			break;
		}
	}
	I2C_COUNTER = 0;
	
	//wait for the acknowledge
	while (I2C2STATbits.ACKSTAT == 1)
	{
		I2C_COUNTER++;
		if(I2C_COUNTER > 0x320)
		{
			I2C_ERROR = 0x55;
			break;
		}
	}
	//ensure the not acknowledge is complete
	I2C_COUNTER = 0;
	while (I2C2STATbits.TRSTAT == 1)
	{
		I2C_COUNTER++;
		if(I2C_COUNTER > 0x320)
		{
			I2C_ERROR = 0x55;
			break;
		}
	}
	while(ReadNbyte != 0)
	{
		if(I2C_ERROR == 0xaa)
		{
			//enable the receive
			I2C2CONbits.RCEN = 1;
			//clear the I2C_COUNTER
			I2C_COUNTER = 0;
			//wait for data
			while (I2C2STATbits.RBF == 0)
			{
				I2C_COUNTER++;
				if (I2C_COUNTER > 0x0320)
				{
					I2C_ERROR = 0x55;
					break;
				}	
			}
			if (I2C_ERROR == 0xaa)
			{
				//get the data from slave
				i = I2C2RCV;
				*Ret++ = i;
				ReadNbyte--;
			}
			if(ReadNbyte != 0)
			{
				I2C2CONbits.ACKDT = 0;
			}
			else
			{
				I2C2CONbits.ACKDT = 1;
			}
			I2C2CONbits.ACKEN = 1;
			//ensure the acknowledge is complete
			I2C_COUNTER = 0;
			while (I2C2CONbits.ACKEN == 1)
			{
				I2C_COUNTER++;
				if(I2C_COUNTER > 0x320)
				{
					I2C_ERROR = 0x55;
					break;
				}
			}
		}
		else break;
	}
	
	//clear the I2CCON<4:0>
	I2C2CON &= 0xffe0;
	//initiate the stop condition
	I2C2CONbits.PEN = 1;
	I2C_COUNTER = 0;
	//ensure the stop condition is complete
	while (I2C2CONbits.PEN == 1)
	{
		I2C_COUNTER++;
		if(I2C_COUNTER > 0x320) break;
	}
	return (ReadNbyte);
}



/*************************************************************
** : RTCRead 
** : read RTC
** : INPUT RTCAddr
**   [BCD7] = sec,min,hour,day,mon,year,week
**
** : pmj
** : 2006-9-20
**************************************************************/
void RTCRead(unsigned char *RTCAddr)
{
	unsigned char buff[7];
	unsigned char buffr = 0x02;
	unsigned char err;
	
	buff[0] = 0;
	buff[1] = 0;
	buff[2] = 0;
	DISICNT = 0x3FFF; /* disable interrupts */
	while(1)
	{
		if(I2cWrite(0xa2, &buff[0], 3) != 0) {error_i2();}
		else break;
	}
	
	while(1)
	{
		if(I2cRead(0xa2, buff, &buffr, 1, 7) != 0) {error_i2();}
		else break;
	}
	DISICNT = 0x0000; /* enable interrupts */
	buff[4] = buff[4]&0x07;
//	if(buff[4] == 0) buff[4] = 0x07;
	buff[4] = buff[4] << 5;
	*RTCAddr = buff[0]&0x7f;     //sec
	*(RTCAddr+1) = buff[1]&0x7f; //min
	*(RTCAddr+2) = buff[2]&0x3f; //hour
	*(RTCAddr+3) = buff[3]&0x3f; //day
	*(RTCAddr+4) = (buff[5]&0x1f)|buff[4]; //week-mon
	*(RTCAddr+5) = buff[6];      //year
	
// soft rtc
	sys_date.year   = *(RTCAddr+5);
	sys_date.month  = *(RTCAddr+4);
	sys_date.day    = *(RTCAddr+3);
	sys_date.hour   = *(RTCAddr+2);
	sys_date.minute = *(RTCAddr+1);
	sys_date.second = *RTCAddr;
	softMin  = b2h(*(RTCAddr+1));
	softSec = b2h(*RTCAddr);
	softMS = 0;
// soft rtc end	
}


/*************************************************************
** : RTCWrite 
** : set RTC
** : INPUT RTCAddr
**	 [BCD7] = sec,min,hour,day,mon,year,week
**
** : pmj
** : 2006-9-20
**************************************************************/
void RTCWrite(unsigned char *RTCAddr)
{
	unsigned char buff[20];
	unsigned char i,bbc[2];
	unsigned char err;
#if 0
	// soft rtc start
	sys_date.year = *(RTCAddr+5);
	sys_date.month = *(RTCAddr+4);
	sys_date.day = *(RTCAddr+3);
	sys_date.hour = *(RTCAddr+2);
	sys_date.minute = *(RTCAddr+1);
	sys_date.second = *RTCAddr;
	softMin  = b2h(*(RTCAddr+1));
	softSec = b2h(*RTCAddr);
#endif
#if 1
	sys_date.year = 0x08;
	sys_date.month = 0xb0;
	sys_date.day = 0x17;
	sys_date.hour = 0x16;
	sys_date.minute = 0x35;
	sys_date.second = 0x16;
	softMin  = b2h(0x35);
	softSec = b2h(0x16);
#endif
	softMS = 0;
	// soft rtc end	
	buff[0] = 0;
	buff[2] = 0;
	buff[1] = 0x20;
#if 0
	for(i = 0; i < 4; i++)
	{
		buff[i+3] = *(RTCAddr+i);
	}
	buff[7] = (*(RTCAddr+4)) >> 5;
	if(buff[7] == 7) buff[7] = 0;
	buff[8] = (*(RTCAddr+4))&0x1f;
	buff[9] = *(RTCAddr+5);
#endif
#if 1
	buff[3] = _time_chk.sec;//0x18;
	buff[4] = _time_chk.min;//0x35;	// 分
	buff[5] = _time_chk.hr;//0x16;	//16时
	buff[6] = _time_chk.day;//0x31;	//31日
	buff[7] = _time_chk.wkd;//(0x72) >> 5;	// 星期三
//	if(buff[7] == 7) buff[7] = 0;
	buff[8] = _time_chk.mth;//0x72&0x1f;	// 12月
	buff[9] = _time_chk.yr;//0x08;// year
#else
	buff[3] = 0x18;
	buff[4] = 0x35;			// 分
	buff[5] = 0x16;			//16时
	buff[6] = 0x31;			//31日
	buff[7] = (0x72) >> 5;		// 星期三
	if(buff[7] == 7) buff[7] = 0;
	buff[8] = 0x72&0x1f;		// 12月
	buff[9] = 0x08;			// year
#endif
	for(i = 10; i < 14; i++)
	{
		buff[i] = 0x80;
	}
	buff[14] = 0x80;
	buff[15] = 0;
	buff[16] = 0x00;
	
	while(1)
	{
		if(I2cWrite(0xa2, &buff[0], 17) != 0) {error_i2();}
		else break;
	}
	buff[0] = 0;
	buff[1] = 0;
	buff[2] = 0;

	while(1)
	{
		if(I2cWrite(0xa2, &buff[0], 3) != 0) {error_i2();}
		else break;
	}
}


unsigned char b2h(unsigned char x)
{
	x = (x >> 4)*10 + (x & 0x0f);
	return (x);
}
#endif

/*****************************************************************************
 * Function: RTCCProcessEvents
 *
 * Preconditions: RTCCInit must be called before.
 *
 * Overview: The function grabs the current time from the RTCC and translate
 * it into strings.
 *
 * Input: None.
 *
 * Output: It update time and date strings  _time_str, _date_str,
 * and _time, _time_chk structures.
 *
 *****************************************************************************/
BYTE TimeTemp[7];
void RTCCProcessEvents(void)
{
	// Process time object only if time is not being set
	while (!_rtcc_flag) {


#ifdef __PIC32MX
		// Grab the time
		_time.sec = *((BYTE*)&RTCTIME + 1); // SEC
		_time.min = *((BYTE*)&RTCTIME + 2); // MIN
		_time.hr = *((BYTE*)&RTCTIME + 3);  // HR
		_time.wkd = RTCDATEbits.WDAY01;     // WDAY
        _time.day = *((BYTE*)&RTCDATE + 1); // DAY;
        _time.mth = *((BYTE*)&RTCDATE + 2); // MONTH;
        _time.yr = *((BYTE*)&RTCDATE + 3);  // YEAR
		
		// Grab the time again 
		_time_chk.sec = *((BYTE*)&RTCTIME + 1); // SEC
		_time_chk.min = *((BYTE*)&RTCTIME + 2); // MIN
		_time_chk.hr = *((BYTE*)&RTCTIME + 3);  // HR
		_time_chk.wkd = RTCDATEbits.WDAY01;     // WDAY
        _time_chk.day = *((BYTE*)&RTCDATE + 1); // DAY;
        _time_chk.mth = *((BYTE*)&RTCDATE + 2); // MONTH;
        _time_chk.yr = *((BYTE*)&RTCDATE + 3);  // YEAR
#else

#ifdef	useI2CRTCC
RTCRead(TimeTemp);
//RTCCRead(TimeTemp);
_time.sec = TimeTemp[0];
_time.min= TimeTemp[1];
_time.hr = TimeTemp[2];
_time.day = TimeTemp[3];
_time.mth = TimeTemp[4]&0x1f;
_time.wkd= (TimeTemp[4]>>5);
_time.yr = TimeTemp[5];

RTCRead(TimeTemp);
_time_chk.sec = TimeTemp[0];
_time_chk.min= TimeTemp[1];
_time_chk.hr = TimeTemp[2];
_time_chk.day = TimeTemp[3];
_time_chk.mth = TimeTemp[4]&0x1f;
_time_chk.wkd= (TimeTemp[4]>>5);
_time_chk.yr = TimeTemp[5];
#else
		// Grab the time
		RCFGCALbits.RTCPTR = 0;			
		_time.prt00 = RTCVAL;
		RCFGCALbits.RTCPTR = 1;			
		_time.prt01 = RTCVAL;
		RCFGCALbits.RTCPTR = 2;			
		_time.prt10 = RTCVAL;
		RCFGCALbits.RTCPTR = 3;			
		_time.prt11 = RTCVAL;

		// Grab the time again 
		RCFGCALbits.RTCPTR = 0;			
		_time_chk.prt00 = RTCVAL;
		RCFGCALbits.RTCPTR = 1;			
		_time_chk.prt01 = RTCVAL;
		RCFGCALbits.RTCPTR = 2;			
		_time_chk.prt10 = RTCVAL;
		RCFGCALbits.RTCPTR = 3;			
		_time_chk.prt11 = RTCVAL;
#endif
#endif
		// Verify there is no roll-over
		if ((_time.prt00 == _time_chk.prt00) &&
			(_time.prt01 == _time_chk.prt01) &&
			(_time.prt10 == _time_chk.prt10) &&
			(_time.prt11 == _time_chk.prt11))
		{
		/*
				switch (_time.mth) {

				default:
				case 0x01: _date_str[3] = 0x009F; _date_str[4] = 0x008D; _date_str[5] = 0x0082; break; 
				case 0x02: _date_str[3] = 0x0094; _date_str[4] = 0x0085; _date_str[5] = 0x0082; break; 
				case 0x03: _date_str[3] = 0x008C; _date_str[4] = 0x0080; _date_str[5] = 0x0090; break; 
				case 0x04: _date_str[3] = 0x0080; _date_str[4] = 0x008F; _date_str[5] = 0x0090; break; 
				case 0x05: _date_str[3] = 0x008C; _date_str[4] = 0x0080; _date_str[5] = 0x0089; break; 
				case 0x06: _date_str[3] = 0x0088; _date_str[4] = 0x009E; _date_str[5] = 0x008D; break; 
				case 0x07: _date_str[3] = 0x0088; _date_str[4] = 0x009E; _date_str[5] = 0x008B; break; 
				case 0x08: _date_str[3] = 0x0080; _date_str[4] = 0x0082; _date_str[5] = 0x0083; break; 
				case 0x09: _date_str[3] = 0x0091; _date_str[4] = 0x0085; _date_str[5] = 0x008D; break; 
				case 0x10: _date_str[3] = 0x008E; _date_str[4] = 0x008A; _date_str[5] = 0x0092; break; 
				case 0x11: _date_str[3] = 0x008D; _date_str[4] = 0x008E; _date_str[5] = 0x009F; break; 
				case 0x12: _date_str[3] = 0x0082; _date_str[4] = 0x0085; _date_str[5] = 0x008A; break; 


			}
			*/

			_date_str[0] = (_time.day >> 4) + '0';
			_date_str[1] = (_time.day & 0xF) + '0';
			_date_str[2] = '/';
			_date_str[3] = (_time.mth >> 4) + '0';
			_date_str[4] = (_time.mth & 0xF) + '0';
			_date_str[5] = '/';
			_date_str[6] = ' ';
			_date_str[7] = '2';
			_date_str[8] = '0';
			_date_str[9] = (_time.yr >> 4) + '0';
			_date_str[10] = (_time.yr & 0xF) + '0';

			switch (_time.wkd) {
				default:
				case 0x00: _time_str[0] = Sun[0]; _time_str[1] = Sun[1]; _time_str[2] = Sun[2]; break; 
				case 0x01: _time_str[0] = Mon[0]; _time_str[1] = Mon[1]; _time_str[2] = Mon[2]; break; 
				case 0x02: _time_str[0] = Tue[0]; _time_str[1] = Tue[1]; _time_str[2] = Tue[2]; break; 
				case 0x03: _time_str[0] = Wed[0]; _time_str[1] = Wed[1]; _time_str[2] = Wed[2]; break; 
				case 0x04: _time_str[0] = Thu[0]; _time_str[1] = Thu[1]; _time_str[2] = Thu[2]; break; 
				case 0x05: _time_str[0] = Fri[0]; _time_str[1] = Fri[1]; _time_str[2] = Fri[2]; break; 
				case 0x06: _time_str[0] = Sat[0]; _time_str[1] = Sat[1]; _time_str[2] = Sat[2]; break;  
			}
			
			_time_str[3] = ' ';
			_time_str[6] = ':';
			_time_str[9] = ':';

			_time_str[4] = (_time.hr >> 4) + '0';
			_time_str[5] = (_time.hr & 0xF) + '0';

			_time_str[7] = (_time.min >> 4) + '0';
			_time_str[8] = (_time.min & 0xF) + '0';

			_time_str[10] = (_time.sec >> 4) + '0';
			_time_str[11] = (_time.sec & 0xF) + '0';

			break;
		}	
	}
}

/*****************************************************************************
 * Function: RTCCInit
 *
 * Preconditions: RTCCInit must be called before.
 *
 * Overview: Enable the oscillator for the RTCC
 *
 * Input: None.
 *
 * Output: None.
 *****************************************************************************/
void RTCCInit(void)
{return 0;
    // Enables the LP OSC for RTCC operation
#ifdef __PIC32MX
    OSCCONbits.SOSCEN = 1;
#else
	asm("mov #OSCCON,W1");
	asm("mov.b	#0x02, W0");
	asm("mov.b	#0x46, W2");
	asm("mov.b	#0x57, W3");
	asm("mov.b	W2, [W1]");
	asm("mov.b	W3, [W1]");
	asm("mov.b	W0, [W1]");
#endif

    // Unlock sequence must take place for RTCEN to be written
#ifdef __PIC32MX
    RTCCON = 0;
#else
	RCFGCAL	= 0x0000;			    
#endif
    mRTCCUnlock();
#ifdef __PIC32MX
    RTCCONbits.ON = 1;
#else
    RCFGCALbits.RTCEN = 1;
#endif
	mRTCCOn();
	mRTCCSetSec(0x01);
	mRTCCSetMin(0x10);
	mRTCCSetHour(0x10);
	mRTCCSetWkDay(0x5);
	mRTCCSetDay(0x15);
	mRTCCSetMonth(0x08);// Changed By Spring.Chen
	mRTCCSetYear(0x08);	
	mRTCCSet();
}

/*****************************************************************************
 * Function: RTCCSet
 *
 * Preconditions: None.
 *
 * Overview: The function upload time and date from _time_chk into clock.
 *
 * Input: _time_chk - structure containing time and date.
 *
 * Output: None.
 *
 *****************************************************************************/
void RTCCSet(void)
{
	mRTCCUnlock();			// Unlock the RTCC
	// Set the time
#ifdef __PIC32MX
	*((BYTE*)&RTCTIME + 1) = _time_chk.sec; // SEC
	*((BYTE*)&RTCTIME + 2) = _time_chk.min; // MIN
	*((BYTE*)&RTCTIME + 3) = _time_chk.hr;  // HR
	RTCDATEbits.WDAY01 = _time_chk.wkd;     // WDAY
    *((BYTE*)&RTCDATE + 1) = _time_chk.day; // DAY;
    *((BYTE*)&RTCDATE + 2) = _time_chk.mth; // MONTH;
    *((BYTE*)&RTCDATE + 3) = _time_chk.yr;  // YEAR
#else
#if	0
	RCFGCALbits.RTCPTR = 0;			
	RTCVAL = _time_chk.prt00;
	RCFGCALbits.RTCPTR = 1;			
	RTCVAL = _time_chk.prt01;
	RCFGCALbits.RTCPTR = 2;			
	RTCVAL = _time_chk.prt10;
	RCFGCALbits.RTCPTR = 3;			
	RTCVAL = _time_chk.prt11;
#else
	RTCWrite(0);

#endif

#endif
	mRTCCLock();			// Lock the RTCC
	_rtcc_flag = 0;			// Release the lock on the time
}

/*****************************************************************************
 * Function: RTCCUnlock
 *
 * Preconditions: None.
 *
 * Overview: The function allows a writing into the clock registers.
 *
 * Input: None.
 *
 * Output: None.
 *
 *****************************************************************************/
void RTCCUnlock(void){
#ifdef __PIC32MX
    SYSKEY = 0xaa996655; // write first unlock key to SYSKEY
    SYSKEY = 0x556699aa; // write second unlock key to SYSKEY
    RTCCONSET = 0x8;     // set RTCWREN in RTCCONSET
#else
	asm volatile("disi	#5");
	asm volatile("mov	#0x55, w7");
	asm volatile("mov	w7, _NVMKEY");
	asm volatile("mov	#0xAA, w8");
	asm volatile("mov	w8, _NVMKEY");
//	asm volatile("bset	_NVMCON, #15");
    asm volatile("bset	_RCFGCAL, #13");
	asm volatile("nop");
	asm volatile("nop");
#endif
//	EECON2 = 0x55; 
//	EECON2 = 0xAA; 
//	RCFGCALbits.RTCWREN = 1;
}

/*****************************************************************************
 * Function: RTCCSetBinSec
 *
 * Preconditions: None.
 *
 * Overview: The function verifies setting seconds range, translates it into
 * BCD format and writes into _time_chk structure. To write the structure into
 * clock RTCCSet must be called.
 *
 * Input: Seconds binary value.
 *
 * Output: Checked BCD value in _time_chk structure.
 *
 *****************************************************************************/
void RTCCSetBinSec(unsigned char Sec){
    if(Sec == 0xff)  Sec = 59;
    if(Sec == 60)  Sec = 0;
    mRTCCSetSec(mRTCCBin2Dec(Sec));
}

/*****************************************************************************
 * Function: RTCCSetBinMin
 *
 * Preconditions: None.
 *
 * Overview: The function verifies a setting minutes range, translates it into
 * BCD format and writes into _time_chk structure. To write the structure into
 * clock RTCCSet must be called.
 *
 * Input: Minutes binary value.
 *
 * Output: Checked BCD value in _time_chk structure.
 *
 *****************************************************************************/
void RTCCSetBinMin(unsigned char Min){
    if(Min == 0xff)   Min = 59;
    if(Min == 60)  Min = 0;
    mRTCCSetMin(mRTCCBin2Dec(Min));
}

/*****************************************************************************
 * Function: RTCCSetBinHour
 *
 * Preconditions: None.
 *
 * Overview: The function verifies a setting hours range, translates it into
 * BCD format and writes into _time_chk structure. To write the structure into
 * clock RTCCSet must be called.
 *
 * Input: Hours binary value.
 *
 * Output: Checked BCD value in _time_chk structure.
 *
 *****************************************************************************/
void RTCCSetBinHour(unsigned char Hour){
    if(Hour == 0xff)  Hour = 23;
    if(Hour == 24) Hour = 0;
    mRTCCSetHour(mRTCCBin2Dec(Hour));
}

/*****************************************************************************
 * Function: RTCCCalculateWeekDay
 *
 * Preconditions: Valid values of day, month and year must be presented in 
 * _time_chk structure.
 *
 * Overview: The function reads day, month and year from _time_chk and 
 * calculates week day. Than It writes result into _time_chk. To write
 * the structure into clock RTCCSet must be called.
 *
 * Input: _time_chk with valid values of day, month and year.
 *
 * Output: Zero based week day in _time_chk structure.
 *
 *****************************************************************************/
void RTCCCalculateWeekDay(){
const char MonthOffset[] =
//jan feb mar apr may jun jul aug sep oct nov dec
{   0,  3,  3,  6,  1,  4,  6,  2,  5,  0,  3,  5 };
unsigned Year;
unsigned Month;
unsigned Day;
unsigned Offset;
    // calculate week day 
    Year  = mRTCCGetBinYear();
    Month = mRTCCGetBinMonth();
    Day  = mRTCCGetBinDay();
    
    // 2000s century offset = 6 +
    // every year 365%7 = 1 day shift +
    // every leap year adds 1 day
    Offset = 6 + Year + Year/4;
    // Add month offset from table
    Offset += MonthOffset[Month-1];
    // Add day
    Offset += Day;

    // If it's a leap year and before March there's no additional day yet
    if((Year%4) == 0)
        if(Month < 3)
            Offset -= 1;
    
    // Week day is
    Offset %= 7;

    mRTCCSetWkDay(Offset);
}

/*****************************************************************************
 * Function: RTCCSetBinDay
 *
 * Preconditions: None.
 *
 * Overview: The function verifies a setting day range, translates it into
 * BCD format and writes into _time_chk structure. To write the structure into
 * clock RTCCSet must be called.
 *
 * Input: Day binary value.
 *
 * Output: Checked BCD value in _time_chk structure.
 *
 *****************************************************************************/
void RTCCSetBinDay(unsigned char Day){
const char MonthDaymax[] =
//jan feb mar apr may jun jul aug sep oct nov dec
{  31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
unsigned Daymax;
unsigned Month;
unsigned Year;

    Month = mRTCCGetBinMonth();
    Year = mRTCCGetBinYear();

    Daymax = MonthDaymax[Month-1];

    // February has one day more for a leap year
    if(Month == 2)
    if( (Year%4) == 0)
        Daymax++;

    if(Day == 0) Day = Daymax;
    if(Day > Daymax) Day = 1;
    mRTCCSetDay(mRTCCBin2Dec(Day));
}

/*****************************************************************************
 * Function: RTCCSetBinMonth
 *
 * Preconditions: None.
 *
 * Overview: The function verifies a setting month range, translates it into
 * BCD format and writes into _time_chk structure. To write the structure into
 * clock RTCCSet must be called.
 *
 * Input: Month binary value.
 *
 * Output: Checked BCD value in _time_chk structure.
 *
 *****************************************************************************/
void RTCCSetBinMonth(unsigned char Month){
    if(Month < 1) Month = 12;
    if(Month > 12) Month = 1;
    mRTCCSetMonth(mRTCCBin2Dec(Month));
}

/*****************************************************************************
 * Function: RTCCSetBinYear
 *
 * Preconditions: None.
 *
 * Overview: The function verifies a setting year range, translates it into
 * BCD format and writes into _time_chk structure. To write the structure into
 * clock RTCCSet must be called.
 *
 * Input: Year binary value.
 *
 * Output: Checked BCD value in _time_chk structure.
 *
 *****************************************************************************/
void RTCCSetBinYear(unsigned char Year){
   if(Year == 0xff) Year = 99;
   if(Year == 100)  Year = 0;
    mRTCCSetYear(mRTCCBin2Dec(Year));
    // Recheck day. Leap year influences to Feb 28/29.
    RTCCSetBinDay(mRTCCGetBinDay());
}



#if	0
void I2cInit()
{
	_RA2 = 0;
     _TRISA2 = 0;
     _RA3  = 0;
     _TRISA3 = 0;
     delay(10);
     _RA2 = 1;
     delay(10);
     _RA3 = 1;
     delay(10);
     //I2CCONbits.I2CEN=0,turn off the I2C module
     //I2CCONbits.I2CSIDL=0,continute module operation in  idle mode
     //I2CCONbits.SCLREL=1,release the clock
     //I2CCONbits.IPMIEN=0,disable the IPMI mode
     //I2CCONbits.A10M=0,I2CADD is a 7-bit slave address
     //I2CCONbits.DISSLW=1,disable the slew rate
     //I2CCONbits.SMEN=0,disable the SMbus
     //I2CCONbits.GCEN=0,disable the general call interrupt
     //I2CCONbits.STREN=0,disable the clock stretch
     //I2CCONbits.ACKDT=0,sending the ACK during acknowledge
     //I2CCONbits.ACKEN=0,disable the ACK
     //I2CCONbits.RCEN=0,disable the receiven the data for I2C bus
     //I2CCONbits.PEN=0,disable initiate the stop condition on the SDA and SCL
     //I2CCONbits.RSEN=0,disable repeat the start conditon
     //I2CCONbits.SEN=0,disable start condition on the SDA and SCL bus
/*     I2C2CONbits.I2CEN = 1;
     I2C2CON = 0x9240;
     //transmission rate is 100kbps(Fscl),I2CBRG=INT(Fcy/Fscl)-1==159
     I2C2BRG = 0x13;//13
     //I2CSTAT
     //I2CSTAT=0X8030;
     //turn on the I2C module
     I2C2CONbits.I2CEN = 1;
     I2C_ERROR = 0xaa;
*/

//     SRbits.IPL = 7;				// 关闭所有中断
I2C2CON = 0x9240;
     I2C2CONbits.I2CEN = 1;		// 启动I2C模块
     I2C2BRG = 0X0003;			// I2C波特率
     
}

BYTE p[7]={0x30,0x59,0x15,0x20,0x02,0x06,0x00}; //2000 年6 月20 日星期3 15 点 59 分30 秒<倒序>
void SetRtccDefault(void)
{
	unsigned int i;
	I2cRtccWriteByte(0x00,0x00);
	for(i = 0; i<7;i++)
		I2cRtccWriteByte(p+i,PCF8563Addr+i);

}

void I2C_Stop(void)
{
	I2C2CONbits.PEN = 1;						//发送停止位
	while(I2C2CONbits.PEN == 1);				//等待停止操作完成
}
void SetEEPROMRead(void)
{
	I2C2CONbits.RSEN = 1;						//发送重起始位
	while(I2C2CONbits.RSEN == 1);				// 等待重起动操作完成
	I2C2TRN = ControlByteR;					// 发送控制字:读准备
	while(I2C2STATbits.TRSTAT == 1);			//等待发送完成
}

void SetRtccAddress(WORD address)
{
	I2C2CONbits.SEN = 1;						//发送起始位
	while(I2C2CONbits.SEN == 1);				//等待启动操作完成
	I2C2TRN = ControlByteW;					//发送控制字:写准备
	while(I2C2STATbits.TRSTAT == 1);			//等待发送完成
	I2C2TRN = (((WORD_VAL)address).v[1]);		// AddressH;
	while(I2C2STATbits.TRSTAT == 1);			//等待发送完成
	I2C2TRN = (((WORD_VAL)address).v[0]);		// AddressL;
	while(I2C2STATbits.TRSTAT == 1);			//等待发送完成
}

void I2cRtccWriteByte(BYTE Data, WORD address)
{
//	_AD1IE = 0;
	SetRtccAddress(address);
	I2C2TRN = Data;
	while(I2C2STATbits.TRSTAT == 1);			//等待发送完成
	I2C_Stop();
//	_AD1IE = 1;	//开中断
}

BYTE I2CRtccReadByte(WORD address)
{
	BYTE temp;
//	_AD1IE = 0;
	SetRtccAddress(address);
	I2C2CONbits.RSEN = 1;						//发送重起始位
	while(I2C2CONbits.RSEN == 1);				// 等待重起动操作完成
	I2C2TRN = ControlByteR;					// 发送控制字:读准备
	while(I2C2STATbits.TRSTAT == 1);			//等待发送完成
	I2C2CONbits.RCEN = 1;						//接收数据使能
	while(I2C2STATbits.RBF == 0);				//
	temp = I2C2RCV;
	I2C2CONbits.ACKDT = 1;					// 最后一字节则不发ACK
	I2C2CONbits.ACKEN = 1;
	while(I2C2CONbits.ACKEN == 1);
	I2C_Stop();
//	_AD1IE = 1;	//开中断
	return temp;

}

BYTE RtccWriteArray(DWORD address, BYTE* pData, WORD nCount)
{
	DWORD_VAL addr;
	BYTE*     pD;
	WORD      counter;

    addr.Val = address;
    pD       = pData;
	
	SetRtccAddress(addr.Val);
	for(counter= 0; counter<nCount; counter++)
		{
			I2C2TRN =*pD++;		// DataH;
			while(I2C2STATbits.TRSTAT == 1);			//等待发送完成

			addr.Val++;

		        // check for page rollover
		        if((addr.v[0]&0x7f) == 0)
		        	{
		        		I2C2CONbits.PEN = 1;						//发送停止位
					while(I2C2CONbits.PEN == 1);				//等待停止操作完成
					delay(10);
					SetRtccAddress(addr.Val);
		        	}
		}
	I2C_Stop();
}


void I2cRtccReadArray(WORD address, BYTE* pData, WORD nCount)
{
	unsigned int i;
	SetRtccAddress(address);
	SetEEPROMRead();
	for(i = 0; i<nCount; i++)
		{
			I2C2CONbits.RCEN = 1;				//接收数据使能
			while(I2C2STATbits.RBF == 0);		//
			*pData++ = I2C2RCV;
			I2C2CONbits.ACKDT = 0;
			if(i == (nCount - 1))
				I2C2CONbits.ACKDT = 1;		// 最后一字节则不发ACK
			I2C2CONbits.ACKEN = 1;
			while(I2C2CONbits.ACKEN == 1);
		}
	I2C_Stop();
}

void RTCCRead(BYTE *TempBuff)
{
	BYTE BUF[7];
	BUF[0] = 0;
	BUF[1] = 0;
	BUF[2] = 0;
	_AD1IE = 0;
#if	1
	I2cRtccWriteByte(BUF[0], 0x00);DelayMs(3);
	I2cRtccWriteByte(BUF[1], 0x01);DelayMs(3);
	I2cRtccWriteByte(BUF[2], 0x02);DelayMs(3);
	DelayMs(30);
	TimeTemp[0] = I2CRtccReadByte(0x02);
	DelayMs(3);
	TimeTemp[1] = I2CRtccReadByte(0x03);
	DelayMs(3);
	TimeTemp[2] = I2CRtccReadByte(0x04);
	DelayMs(3);
	TimeTemp[3] = I2CRtccReadByte(0x05);
	DelayMs(3);
	TimeTemp[4] = I2CRtccReadByte(0x06);
	DelayMs(3);
	TimeTemp[5] = I2CRtccReadByte(0x07);
	DelayMs(3);
	TimeTemp[6] = I2CRtccReadByte(0x08);
/*	*TempBuff = I2CRtccReadByte(0x02);
	DelayMs(3);TempBuff++;
	*TempBuff = I2CRtccReadByte(0x03);
	DelayMs(3);TempBuff++;
	*TempBuff = I2CRtccReadByte(0x04);
	DelayMs(3);TempBuff++;
	*TempBuff = I2CRtccReadByte(0x05);
	DelayMs(3);TempBuff++;
	*TempBuff = I2CRtccReadByte(0x06);
	DelayMs(3);TempBuff++;
	*TempBuff = I2CRtccReadByte(0x07);
	DelayMs(3);TempBuff++;
	*TempBuff = I2CRtccReadByte(0x08);*/
	DelayMs(30);
#else
	RtccWriteArray(0,BUF,3);
	I2cRtccReadArray(0,TempBuff,7);
#endif
	_AD1IE = 1;

}


/*****************************************************************************
 * EOF
 *****************************************************************************/
#else

#define SDA PORTAbits.RA3	//RC4
#define SCL PORTAbits.RA2	//RC3
uch ACK;

#if	0
#define	WriteCtrolByte	0xa0
#define	ReadCtrolByte	0xa1
#else
#define	WriteCtrolByte	0xa2
#define	ReadCtrolByte	0xa3
#endif

void I2cInit()
{
//	PORTA=0XFF;
//    TRISA=0;
	TRISAbits.TRISA2=0;         //置输出
   TRISAbits.TRISA3=0;
}



void start (void)    //  IIC开始
{
   TRISAbits.TRISA2=0;         //置输出
   TRISAbits.TRISA3=0;
   SDA=1;        
   asm("nop");
   SCL=1;            //在至少4u秒期间SDA从高电平到低电平
   asm("nop");
   asm("nop");
   asm("nop");
   SDA=0;
   asm("nop");
   asm("nop");
   asm("nop");
   SCL=0;
   asm("nop");
   asm("nop");
  }

void stop(void)      // IIC结束
{  TRISAbits.TRISA2=0;
    TRISAbits.TRISA3=0;         //置输出
    SDA=0;
    asm("nop");
    SCL=1;           //在至少4u秒期间SDA从低电平到高电平
    asm("nop");
    asm("nop");
    asm("nop");
    SDA=1;
    asm("nop");
    asm("nop");
    asm("nop");
    SCL=0;
    asm("nop");
    asm("nop");
  }
  
  uch  check(void)       //  检查应答信号
  {  
     TRISAbits.TRISA3=1;
     SCL=0;
     SCL=1;
   
     if (SDA == 1)
     {
         ACK = 1;
     }
     else
     {
         ACK = 0;
     }
     SCL=0;
     return(ACK);
     }
     

  void send(uch data)     //  发送一个字节
{
  uch bitcount=8;       //发送8位
        do
          { 
           TRISAbits.TRISA2=0;     //置输出
           TRISAbits.TRISA3=0;
           if((data&0x80)==0x80)   
             {
                SDA=1;       //发送 1
             }  
             else 
             {
                SDA=0;      //发送 0
             }
             
          SCL=0;        // 在时钟大于4u秒期间写数据
          SCL=1;
          asm("nop");
          asm("nop");
          asm("nop");
          asm("nop");
          asm("nop");
          SCL=0; 
          data=data<<1;
          bitcount--;
          } while(bitcount);
           TRISAbits.TRISA3=1 ;                  //释放总线等待应答
           asm("nop");
           asm("nop");
           asm("nop");
           asm("nop");
           asm("nop");   
}   

  uch recive(void)      //接受一个字节
  {
   uch temp1=0;
   uch bitcount1=8;
   TRISAbits.TRISA3=1;         //置输入
   TRISAbits.TRISA2=0;
   do
   { SCL=0;              //在时钟大于4u秒期间读数据
     SCL=1;
     asm("nop");
     asm("nop");   
   if(SDA)                  //读 1
      { 
      temp1=temp1|0x01;
      }
    else                    //读 0
    {
      temp1=temp1&0xfe;
    }
       SCL=0;
       if(bitcount1-1)
       {
         temp1=temp1<<1;
       }
       bitcount1--;  
      }while(bitcount1);
       return(temp1);
  }       
void ack(void)     //发送继续读信号
{ 
    SDA=0;
    SCL=0;
    SCL=1;
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
    SCL=0;
}
    
  void nack(void)      //停止继续读
  { 
    SDA=1;
    SCL=0;
    SCL=1;
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
    SCL=0;
  }  
  
   
void wrtoROM(uch *data,unint address,uch num)  //给24LC256写数据
{ unint i;
   unint adrl=address%256;       //低8位地址
   unint adrh=address/256;       //高8位地址
         start();               //起始信号
         send(WriteCtrolByte);          //写主控器识别地址，本人是a0
         
     do{;
        }while(check());    //等待应答
     
     send(adrh);           //送数据高8位地址

   do{;
      }while(check());     //等待应答
      send(adrl);           //送数据低8位地址
    do
    {
      ;
      }while(check());          //等待应答
     for(i=0;i<num;i++,data++)
    {
       send(*data);            //发送数据
     do{;
        }while(check());       //等待应答
        } 
     stop();                 //停止
    delay();                //延时，下次发数据
     
}

//void EEPROMWriteByte(WORD data, WORD address)
void I2cRtccWriteByte(BYTE data, WORD address)
{
//	unint i;
   unint adrl=address%256;       //低8位地址
   unint adrh=address/256;       //高8位地址
         start();               //起始信号
         send(WriteCtrolByte);          //写主控器识别地址，本人是a0
         
     do{;
        }while(check());    //等待应答
     
     send(adrh);           //送数据高8位地址

   do{;
      }while(check());     //等待应答
      send(adrl);           //送数据低8位地址
    do
    {
      ;
      }while(check());          //等待应答
      send(data);            //发送数据
     do{;
        }while(check());       //等待应答

     stop();                 //停止
     delay();                //延时，下次发数据

}

BYTE p[7]={0x30,0x59,0x15,0x20,0x02,0x06,0x00}; //2000 年6 月20 日星期3 15 点 59 分30 秒<倒序>

#if 0
void SetRtccDefault(void)
{
	unsigned int i;
	I2cRtccWriteByte(0x00,0x00);
//	I2cRtccWriteByte(0x01,0x11);


	//设置时钟状态 
	I2cRtccWriteByte(0x00,0x00);
//	I2cRtccWriteByte(0x81,0x0d);
	I2cRtccWriteByte(0x82,0x0e);
//	I2cRtccWriteByte(1,0x0f);
	for(i = 0; i<7;i++)
		I2cRtccWriteByte(p+i,PCF8563Addr+i);
	I2cRtccWriteByte(0x81,0x01);
		


}
#endif
#if	0
void EEPROMWriteWord(WORD data, WORD address)
{
    EEPROMWriteByte(((WORD_VAL)data).v[0],address);
    EEPROMWriteByte(((WORD_VAL)data).v[1],address+1);
/*
    unint adrl=address%256;       //低8位地址
   unint adrh=address/256;       //高8位地址
   BYTE Byte0=((WORD_VAL)data).v[0];
   BYTE Byte1=((WORD_VAL)data).v[1];
         start();               //起始信号
         send(0xa0);          //写主控器识别地址，本人是a0
         
     do{;
        }while(check());    //等待应答
     
     send(adrh);           //送数据高8位地址

   do{;
      }while(check());     //等待应答
      send(adrl);           //送数据低8位地址
    do
    {
      ;
      }while(check());          //等待应答
      send(Byte1);            //发送数据
      do{;
        }while(check());       //等待应答
      send(Byte0);
     do{;
        }while(check());       //等待应答

     stop();                 //停止
     delay();                //延时，下次发数据
     */
}

BYTE EEPROMWriteArray(DWORD address, BYTE* pData, WORD nCount)
{
	wrtoROM(pData,address,nCount);

}

void rdfromROM(uch *pdata,unint address,uch num2)   //从24LC256中读数据
{  unint adrl;      
    unint adrh;
    uch j;
    for(j=0;j<num2;j++,pdata++) 
     { 
      adrl=address%256;      //地址低位
      adrh=address/256;      //地址高位
      start();               //开始
    send(WriteCtrolByte);              //写主控器识别地址（写），本人是a0
    do{
       ;
      }while(check());        //等待应答
    send(adrh);               //送高位
    do
      { 
      ;
      }while(check());         //等待应答
    send(adrl);              //送低位
    do
     {
       ;
     }while(check());          //等待应答
    start();                  //开始读数据
    send(ReadCtrolByte);               //写主控器识别地址（读）本程序是0XA1
     do
     {
       ;
      }while(check());       //等待应答
     
      *pdata=recive();
      nack();
      stop(); 
      address=address+1;       //指向下一个地址
    
    
     }   
} 
#endif
BYTE I2CRtccReadByte(WORD address)
{
	unint adrl;      
    unint adrh;
    BYTE pdata;

    adrl=address%256;      //地址低位
      adrh=address/256;      //地址高位
      start();               //开始
    send(WriteCtrolByte);              //写主控器识别地址（写），本人是a0
    do{
       ;
      }while(check());        //等待应答
    send(adrh);               //送高位
    do
      { 
      ;
      }while(check());         //等待应答
    send(adrl);              //送低位
    do
     {
       ;
     }while(check());          //等待应答
    start();                  //开始读数据
    send(ReadCtrolByte);               //写主控器识别地址（读）本程序是0XA1
     do
     {
       ;
      }while(check());       //等待应答
     
      pdata=recive();
      nack();
      stop(); 
      address=address+1;       //指向下一个地址
      return pdata;

}

#endif
