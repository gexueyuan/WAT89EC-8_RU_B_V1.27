 /**********************************************************************
 *    Filename:       pgps.c  for car    		                       *
 *                                                                     *
 *                                                                     *
 *    Date:           04/20/2007                                       *
 *    File Version:   1.30                                             *
 *    Tools used: MPLAB     -> 7.50                                    *
 *                Compiler  -> 1.30                                    *
 *                Assembler -> 1.30                                    *
 *                Linker    -> 1.30                                    *
 *    Linker File:    p24FJ128GA010.gld                                *
 ***********************************************************************/
#include "MainDemo.h"

/*#include "p24FJ256GB110.h"
#include "math.h"
#include <stdio.h>
#include <string.h>
#include "GenericTypeDefs.h"
#include "DefineXY.h"*/




 DWORD_VAL dval;
volatile XCHAR sLongitude[14+1];
volatile XCHAR sLatitude[13+1];

extern BYTE PrintSec;
extern unsigned int PrintCount;
extern BOOL  ChoicePrint;



unsigned char hex_to_bcd(unsigned char x)
{
	unsigned char x1;
	x1 = __builtin_divud(x,10);
	x1 = x1<< 4;
	x1 |= (x%10);
	return (x1);
}
unsigned char bcd_to_hex(unsigned char x)
{
	x = (x >> 4)*10 + (x & 0x0f);
	return (x);
}
tagUARTFLAG UARTFLAG;
#define _URT1IN UARTFLAG.URT1IN
#define _URT2IN UARTFLAG.URT2IN
#define _URT3IN UARTFLAG.URT3IN
#define _URT4IN UARTFLAG.URT4IN
#define _BUSY1 UARTFLAG.BUSY1
#define _BUSY2 UARTFLAG.BUSY2
#define _BUSY3 UARTFLAG.BUSY3
#define _BUSY4 UARTFLAG.BUSY4
#define _CALDJ UARTFLAG.CALDJ
#define _GPRSRDSMS UARTFLAG.GPRSRDSMS
#define _GPRSAPNS UARTFLAG.GPRSAPNS

void __attribute__((interrupt, shadow, no_auto_psv)) _T1Interrupt(void)
{
	DISICNT = 0x3FFF; /* disable interrupts */
//	unsigned char x;
	 _T1IF = 0;			//clear timer2 int flag
	sys_clk++;
    if(uart4_rtimer > 0)
    {
    	uart4_rtimer--;
    	if(uart4_rtimer == 0) _URT4IN = 1;
    }
	/*
  	if(ChoicePrint)
 	   PrintCount++;
 	if(PrintCount==1000)
 		{
         	PrintSec++;
            PrintCount=0;
	    }
	    */
	DISICNT = 0x0000; /* enable interrupts */
}


//uart4 send interrupt 
void __attribute__((interrupt, shadow, no_auto_psv)) _U4TXInterrupt(void)
{
	DISICNT = 0x3FFF; 
	_U4TXIF = 0;
	if((t4len != 0) && (tx4_ptr < &tx4_buff[TX4_BUFFLEN])) 
	{
		U4TXREG = *tx4_ptr++;
		t4len--;
	}
	else 
	{
		_U4TXIE = 0;
//		_BUSY4 = 0;
	}
	DISICNT = 0x0000; 
}
//uart1 receive interrupt 
void __attribute__((interrupt, shadow, no_auto_psv)) _U4RXInterrupt(void)
{
	DISICNT = 0x3FFF; 
	unsigned int i;
	_U4RXIF = 0;
	while(U4STAbits.URXDA == 1) 
	{
		i = U4RXREG;
		 
		if(rx4_ptr < &rx4_buff[RX4_BUFFLEN])
		{
			*rx4_ptr++ = i;
			*rx4_ptr = 0;
		}
	}
	uart4_rtimer = U4R_TOUT;
	DISICNT = 0x0000; 
}

const unsigned char dayd[12] = {31,28,31,30,31,30,31,31,30,31,30,31};
unsigned char dxdate(unsigned char gyear,unsigned char gmouth)
{
	unsigned char zi,zj,zk;
	PSVPAG = __builtin_psvpage(&dayd);
	CORCONbits.PSV = 1;
	zj = bcd_to_hex(gmouth);
	if((zj == 0)||(zj > 11)) zj = 1;
	zk = dayd[zj-1];
	zj = bcd_to_hex(gyear);
	if(((zj%4) == 0) && (gmouth == 0x02)) zk += 1;
	return (zk);
}
const char RES_GPRMC[] = "$GPRMC,";
const char RES_GPGGA[] = "$GPGGA,";
//uart4 gprs port receive handle
void u4_hand(void)
{
	unsigned char buff[10],buff1[8];
	unsigned int i,j,x,k;
	unsigned long y;
	unsigned char *ptr,*ptrd;
	_U4RXIE = 0;  // disable u4rx interrupt
	PSVPAG = __builtin_psvpage(&RES_GPRMC);
	CORCONbits.PSV = 1;
	ptr = strstr((const char *)rx4_buff,RES_GPRMC);
	if (ptr != NULL)
	{
		ptr += 7;
		ptrd = ptr;
		for(; (ptrd < rx4_ptr)&&(*ptrd != '*'); ptrd++)
		{
			;
		}
		if(ptrd >= rx4_ptr) goto gpsnolink;
		if(*(ptrd-1) == 'N')
		{
			satellite = 'N';
			goto gpsnolink;
		}
		//hh mm ss
		for(i = 0;(ptr < rx4_ptr)&&(*ptr != ',');ptr++)
		{
			buff[i] = *ptr-0x30;
			i++;
		}
		buff1[3] = (buff[0]<<4)+buff[1];
		buff1[4] = (buff[2]<<4)+buff[3];
		buff1[5] = (buff[4]<<4)+buff[5];
		ptr += 1;
		satellite = *ptr;
		if(*ptr != 'A') goto gpsnolink;
		ptr += 2;
		//wei du
	#if 0
		x = 0;
		y = 0;
		for(i = 0; (ptr < rx4_ptr); ptr++)
		{
			if(*(ptr+2) == '.') break;
			x = x*10 + ((*ptr) - 0x30);
		}
		for(i = 0; (ptr < rx4_ptr); ptr++)
		{
			if(*(ptr+0) == '.') continue;
			if(*(ptr+0) == ',') break;
			y = y*10 + ((*ptr) - 0x30);
		}
		ptr++;
		k = *ptr;
		latitude[0] = k;
		dval.Val = x;
		latitude[1] = dval.v[2];
		latitude[2] = dval.v[1];
		latitude[3] = dval.v[0];
		dval.Val = y;
		latitude[4] = dval.v[3];
		latitude[5] = dval.v[2];
		latitude[6] = dval.v[1];
		latitude[7] = dval.v[0];
	#endif 
 
	#if 1   //vivian
		i = 0;
		for(; (ptr < rx4_ptr); ptr++,i++)
		{
			if(*(ptr+2) == '.') 
			{
				sLatitude[i+1] = 0x007B; //du
				i++;
				break;
			}
			//x = x*10 + ((*ptr) - 0x30);
			sLatitude[i+1] = 0x0000|(*ptr);
		}
		for(; (ptr < rx4_ptr); ptr++,i++)
		{
			if(*(ptr+0) == '.') 
			{
				sLatitude[i+1] = 0x002e;  //.
				continue;
			}
			if(*(ptr+0) == ',') 
			{
				sLatitude[i+1] = 0x0027;//'
				break;
			}
			//y = y*10 + ((*ptr) - 0x30);
			sLatitude[i+1] = 0x0000|(*ptr);
		}
		ptr++;
		k = *ptr;
		//latitude[0] = k;
		sLatitude[0] = 0x0000|(*ptr);
		sLatitude[13] = 0x0000;
	#endif
		ptr += 2;
		//jing du
	#if 0
		x = 0;
		y = 0;
		for(i = 0; (ptr < rx4_ptr); ptr++)
		{
			if(*(ptr+2) == '.') break;
			x = x*10 + ((*ptr) - 0x30);
		}
		for(i = 0; (ptr < rx4_ptr); ptr++)
		{
			if(*(ptr+0) == '.') continue;
			if(*(ptr+0) == ',') break;
			y = y*10 + ((*ptr) - 0x30);
		}
		ptr++;
		k = *ptr;
		longitude[0] = k;
		dval.Val = x;
		longitude[1] = dval.v[2];
		longitude[2] = dval.v[1];
		longitude[3] = dval.v[0];
		dval.Val = y;
		longitude[4] = dval.v[3];
		longitude[5] = dval.v[2];
		longitude[6] = dval.v[1];
		longitude[7] = dval.v[0];
	#endif

	#if 1  //vivian
		j = 0;
		for(; (ptr < rx4_ptr); ptr++,j++)
		{
			if(*(ptr+2) == '.') 
			{
				sLongitude[j+1] = 0x007B;	//du
				j++;
				break;
			}
			//x = x*10 + ((*ptr) - 0x30);
			sLongitude[j+1] = 0x0000|(*ptr);
		}
		for(; (ptr < rx4_ptr); ptr++,j++)
		{
			if(*(ptr+0) == '.') 
			{
				sLongitude[j+1] = 0x002E;
				continue;
			}
			if(*(ptr+0) == ',') 
			{
				sLongitude[j+1] = 0x0027;//'
				break;
			}
			//y = y*10 + ((*ptr) - 0x30);
			sLongitude[j+1] = 0x0000|(*ptr);
		}
		ptr++;
		k = *ptr;
		//longitude[0] = k;
		sLongitude[0] = 0x0000|(*ptr);
		sLongitude[14] = 0x0000;
	#endif
		ptr += 2;
		for(i = 0;(ptr < rx4_ptr)&&(*ptr != ',');ptr++) {;}
		ptr++;
		for(i = 0;(ptr < rx4_ptr)&&(*ptr != ',');ptr++) {;}
		ptr++;
		//yy mm dd
		for(i = 0;(ptr < rx4_ptr)&&(*ptr != ',');ptr++)
		{
			buff[i] = *ptr-0x30;
			i++;
			if(i > 5) break;
		}
		buff1[2] = (buff[0]<<4)+buff[1];
		buff1[1] = (buff[2]<<4)+buff[3];
		buff1[0] = (buff[4]<<4)+buff[5];
		if((eeset[TIME_ZONE]&0x7f) < 24)
		{
			x = eeset[TIME_ZONE]&0x7f;
			x = bcd_to_hex(x);
			k = bcd_to_hex(buff1[3]);
			i = 0;
			if(eeset[TIME_ZONE]&0x80)
			{
				if(k >= x) k = k-x;
				else
				{
					k = (k+24)-x;
					i = 2;
				}
			}
			else
			{
				k = k+x;
				if(k > 23)
				{
					k = k-24;
					i = 1;
				}
			}
			buff1[3] = hex_to_bcd(k);
			if(i == 1)
			{//day +1
				x = bcd_to_hex(buff1[2])+1;
				if(x > dxdate(buff1[0],buff1[1]))
				{
					buff1[2] = 1;
					x = bcd_to_hex(buff1[1])+1;
					if(x > 12)
					{
						buff1[1] = 1;
						x = bcd_to_hex(buff1[0])+1;
						buff1[0] = hex_to_bcd(x);
					}
					else buff1[1] = hex_to_bcd(x);
				}
				else buff1[2] = hex_to_bcd(x);
			}
			if(i == 2)
			{//day -1
				if(buff1[2] == 1)
				{
					x = dxdate(buff1[0],buff1[1]);
					buff1[2] = hex_to_bcd(x);
					if(buff1[1] == 1)
					{
						buff1[1] = 0x12;
						x = bcd_to_hex(buff1[0])-1;
						buff1[0] = hex_to_bcd(x);
					}
					else
					{
						x = bcd_to_hex(buff1[1])-1;
						buff1[1] = hex_to_bcd(x);
					}
				}
				else
				{
					x = bcd_to_hex(buff1[2])-1;
					buff1[2] = hex_to_bcd(x);
				}
			}
		}
		gps_date.year = buff1[0];
		gps_date.month = buff1[1];
		gps_date.day = buff1[2];
		gps_date.hour = buff1[3];
		gps_date.minute = buff1[4];
		gps_date.second = buff1[5];
	}
	
gpsnolink:	
	PSVPAG = __builtin_psvpage(&RES_GPGGA);
	CORCONbits.PSV = 1;
	ptr = strstr((const char *)rx4_buff,RES_GPGGA);
	if (ptr != NULL)
	{
		ptr += 7;
		ptrd = ptr;
		for(i = 0; i < 6; i++)
		{
			for(; (ptrd < rx4_ptr)&&(*ptrd != ','); ptrd++)
			{//$GPGGA,,,,,,,7,,,M,,M,0000*hh
				;
			}
			ptrd++;
		}
		y = 0;
		for(; (ptrd < rx4_ptr)&&(*ptrd != ','); ptrd++)
		{
			y = y*10 + ((*ptrd) - 0x30);
		}
		satellite_cnt = y;
	}
	
	_URT4IN = 0;
	rx4_ptr = &rx4_buff[0];
	memset(&rx4_buff[0],0,255);
	Nop();
	_U4RXIE = 1;  // enable u4rx interrupt
	return;
}
/*void SATERTC(void)
{
	unsigned char zbuff[7];
	zbuff[0] = gps_date.second;
	zbuff[1] = gps_date.minute;
	zbuff[2] = gps_date.hour;
	zbuff[3] = gps_date.day;
	zbuff[4] = (gps_date.month&0x1f)|(sys_date.month&0xe0);
	zbuff[5] = gps_date.year;
	RTCWrite((unsigned char *)&zbuff);
}*/

