/***************************************************************************************************
 *
 * Beeper.
 *
 *****************************************************************************
 * FileName:        Beep.c
 * Dependencies:    MainDemo.h
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
 * SOFTWARE AND DOCUMENTATION ARE PROVIDED 揂S IS?WITHOUT WARRANTY OF ANY
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

#include "MainDemo.h"

volatile WORD pulseCounter;
//WORD	Temp_adc;
XCHAR Test_Result[]={0x0030,0x0030,0x0030,0x002e,0x0030,0x0000};
/*********************************************************************
* Function: Timer2 ISR
*
* PreCondition: none
*
* Input: none
*
* Output: none
*
* Side Effects: none
*
* Overview: counts beeper pulses and turns off it after BEEP_TIME
*
* Note: none
*
********************************************************************/
#ifdef __PIC32MX__
#define __T2_ISR    __ISR(_TIMER_2_VECTOR, ipl1)
#else
#define __T2_ISR    __attribute__((interrupt, shadow, no_auto_psv))
#endif

void  __T2_ISR _T2Interrupt(void)
{
    OC_LAT_BIT ^= 1;
    if(BEEP_TIME < pulseCounter++){
        // clear pulse counter
        pulseCounter = 0;
    	// stop counter
        T2CONbits.TON = 0;
        // clear timer
        TMR2 = 0;
        PR2 =  TIMER_BASE;
        OC_LAT_BIT = 0;
    }
  //  PR2 += 1;
    //clear flag
#ifdef __PIC32MX
    mT2ClearIntFlag();
#else
    IFS0bits.T2IF = 0;    
#endif
}

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
void BeepInit(){
    // clear pulse counter
    pulseCounter = 0;

    // init Timer2
    TMR2 = 0;
    PR2 =  TIMER_BASE;
    //set prescale to 1:8
    T2CONbits.TCKPS = 1;
    IFS0bits.T2IF = 0; //clear flag
    IEC0bits.T2IE = 1; //enable interrupt

    OC_LAT_BIT = 0;     
    OC_TRIS_BIT = 0;
    _TRISA14=0;
   PORTAbits.RA14 = 1; // add by Spring.chen
   _TRISC14=0;
   PORTCbits.RC14 = 1; // add by Spring.chen
//    _TRISA15=0;
//	Nop();
//    PORTAbits.RA15 = 1;
}



//variable_definition
#define AVCC                3300
#define SYSCLK              8000000
#define Compare_time_1        90
#define Compare_time_2        70
#define BOARD_VERSION4
#define sclk_adc      PORTFbits.RF13
#define cs_adc        PORTAbits.RA1
#define spi_sdi_adc   PORTFbits.RF12
#define pump_pwr      PORTFbits.RF0
#define pump_on       PORTFbits.RF1
#define pump_reset       PORTGbits.RG1
#define N  7
#define ScreenMask 15
#define TestMask  42
#define AlarmMask  200
//#define MarkDensity  800;
volatile unsigned int  flag=0,press=0;
unsigned int ppp,pppp,Temp=0,Temp2=0,Temp3=0,a=0,t,SampleCount=0,max_num=0,charge_flag,MarkDensity=0; 
unsigned char Markvalue[30]= {0x00};// ,Markvaluetemp[30]= {0x00};
unsigned int AD_press,initial_value=0,j=0,i=0,T=0,T0=0,T1=0,T2=0,Temsign_flag=0,test_result=0,in=0,a_flag=0;
unsigned int Tem_AD,Cons,Kt;
unsigned long sum=0,temp_sum,sum_cons,befpeak_count;
unsigned int pp1,pp2,pp3,pp4,suoitn2,MarkAdc,ok_pp,nAdc,cons_t;
unsigned char Lw,Hi,MarkTem,nflag;
unsigned int	after_peak_count,peak_sample_Count,sample_stop_value,peak_value,peak_compare_time;  //added--chen090604
unsigned char	peak_got;	//added--chen090604
//unsigned int ticknum2=0;
unsigned int ticknum=0;
extern WORD Temp_adc;
extern WORD epPress;
extern unsigned int Demarcate_tem;
extern volatile BYTE DryorWet;
extern volatile BYTE Masklevel;
extern BYTE TestMode,PeakScreen;
extern BYTE Settingposition;
unsigned long  calculate_Temp;
unsigned int Decade[20]	= {0x0000};     // 显示;
/************************************************************************
* Function: void delayus(unsigned char us)             
*                                                                       
* Overview: this function delays  times of microsecond  //延时       
*                                                                       
* Input: us                                                     
*                                                                       
* Output: void
*                                                                       
************************************************************************/
void delayus(unsigned char us)
{
  unsigned char i;

  for(i=0;i<us;i++)
      {
		asm("nop");
	  }
}

/************************************************************************
* Function: void delayms(uint ms)            
*                                                                       
* Overview: this function delays time of   millisecond      延时 
*                                                                       
* Input: us                                                     
*                                                                       
* Output: void
*                                                                       
************************************************************************/

void delayms(uint ms)
{
  uint mm,pp;
  for(mm=0;mm<ms;mm++)
  {
   for(pp=0;pp<=10;pp++)//
      {
	   asm("nop");
	  } 
  }
}

void pumpinit()
{


TRISFbits.TRISF0   = 0 ;
TRISFbits.TRISF1   = 0 ;
TRISGbits.TRISG1   = 0 ;
pump_reset=0;
pump_on=0;
}

/************************************************************************
* Function: void adcinit()           
*                                                                       
* Overview: This function initiates ADC and state mashine.  AD初始化  
*                                                                       
* Input: void                                                    
*                                                                       
* Output: void
*                                                                       
************************************************************************/
void adcinit()
{
TRISBbits.TRISB9   = 1 ; //adc_press port
TRISBbits.TRISB8   = 1;
TRISBbits.TRISB7   = 1;
//TRISBbits.TRISB6   = 1;
AD1CON1 = 0x80E0;//Turn on, auto sample start, auto-convert0x80E4
AD1CON1bits.ADSIDL=0;	
AD1CON2 = 0;//AVdd, AVss, int every conversion（转换）, MUXA only	
AD1CON3 = 0x1F80;//31 Tad auto-sample, Tad = 5*Tcy
AD1PCFGbits.PCFG9 = 0;
AD1PCFGbits.PCFG8 = 0;
AD1PCFGbits.PCFG7 = 0;
//AD1PCFGbits.PCFG6 = 1;//Disable digital input on AN6 
AD1CSSL = 0;

}
void inti_time5(void)
{    
    T5CON = 0x0020;  ///1:64的预分频比(时间单位是16us)	
    IFS1bits.T5IF = 0;	
    IPC7bits.T5IP = 5;	
    IEC1bits.T5IE = 0;
    TMR5 =0x0000; 	
}

/*****************************************************************************
* Function: uint ADC(uchar port)
*
* Preconditions: ADCInit must be called before.
*
* Overview: This is a state mashine to grab analog data from potentiometer
* and temperature sensor.  AD采样程序
*
* Input: port.
*
* Output: analog data of ADC Value.
*
******************************************************************************/
uint ADC(uchar port){
	
unsigned long Result;

unsigned int Ad_Result;
 //   AD1CON3 = 0x1F80;
//	delayus(35);
 switch(port){

		case 3:         

               ClrWdt();  
               TRISBbits.TRISB3   = 1 ;
	           AD1PCFGbits.PCFG3  = 0;                   
               AD1CHS0 = 3;//大板TC1047温度
               delayus(10);
               AD1CHS0 = 3;
               asm("nop");
               DISICNT = 0x3FFF; /* disable interrupts */          
               AD1CON1bits.SAMP = 1;        
               delayus(20);          
               while(!AD1CON1bits.DONE)
                    {
          	          Nop();
        	          Nop();
        	          Nop();
                    }
               DISICNT = 0x0000; 
         
               Result = (long) ADC1BUF0; 
        
               Ad_Result = (int)Result;////////
        
               // Ad_Result = (Result*AVCC)/1024;  
                          
                          
                break;

        case 4:          

		       ClrWdt();  
               TRISBbits.TRISB4   = 1 ;
	           AD1PCFGbits.PCFG4  = 0;                    
               AD1CHS0 = port;//电池电压
               delayus(10);
               AD1CHS0 = port;
               asm("nop");
               DISICNT = 0x3FFF; /* disable interrupts */          
               AD1CON1bits.SAMP = 1;        
               delayus(20);
          
               while(!AD1CON1bits.DONE)
                    {
          	         Nop();
        	         Nop();
        	         Nop();
          	         }
                    DISICNT = 0x0000; 
         
               Result = (long) ADC1BUF0; 
        
               Ad_Result = (int)Result;////////
        
              Ad_Result = (Result*AVCC)/1024;  
                          

               break;

	    case 6:          

		       TRISBbits.TRISB6   = 1;
		       AD1PCFGbits.PCFG6  = 0;
               ClrWdt();            
               AD1CHS0 = 6;//
               delayus(10);
               AD1CHS0 = 6;
               asm("nop");
               DISICNT = 0x3FFF; /* disable interrupts */          
               AD1CON1bits.SAMP = 1;        
               delayus(20);
          
               while(!AD1CON1bits.DONE)
                    {
          	          Nop();
        	          Nop();
        	          Nop();
        	
          	         }

		       DISICNT = 0x0000; /* enable interrupts */ // AD1CON1bits.SAMP = 0; 

               Result = (long) ADC1BUF0; 
        
               Ad_Result = (int)Result;////////
        
               //Ad_Result = (Result*AVCC)/1024;  
                          
          break;

		 case 7:          // Convert result for potentiometer

			   ClrWdt();  
               TRISBbits.TRISB7   = 1;
	           AD1PCFGbits.PCFG7  = 0;                   
               AD1CHS0 = 7;//小板温度
               delayus(10);
               AD1CHS0 = 7;
               asm("nop");
               DISICNT = 0x3FFF; /* disable interrupts */          
               AD1CON1bits.SAMP = 1;        
               delayus(20);
          
               while(!AD1CON1bits.DONE)
                    {
          	           Nop();
        	           Nop();
        	           Nop();
        	
          	         }
               DISICNT = 0x0000; /* enable interrupts */ // AD1CON1bits.SAMP = 0; 

               Result = (long) ADC1BUF0; 
        
               Ad_Result = (int)Result;////////
        
              //  Ad_Result = (Result*AVCC)/1024;  
                          
               break;

	  
        case 8: 

		       ClrWdt();
               TRISBbits.TRISB8   = 1;
	           AD1PCFGbits.PCFG8  = 0;		
               AD1CHS0 =  8 ; //泵电压     
         
               asm("nop");
         
               DISICNT = 0x3FFF; /* disable interrupts */
         
               AD1CON1bits.SAMP = 1;
        
               delayus(35);//5 
        
               delayus(15); 
         
               while(!AD1CON1bits.DONE)
                    {
         	            Nop();
        	            Nop();
        	            Nop();
                     } 
        
                DISICNT = 0x0000;   /* enable interrupts */
        
               //  AD1CON1bits.DONE = 0;
       
               Ad_Result = 0;
         
               Result    = 0;
         
               Result = (long)ADC1BUF0;
         
               Ad_Result = (int)Result;
         
               Ad_Result = (Result*AVCC)/1024; 
        
        
               break; 

		case 9:   


		       ClrWdt(); 
               TRISBbits.TRISB9   = 1;
	           AD1PCFGbits.PCFG9  = 0;

               AD1CHS0 =   9 ;  //压力
        
               DISICNT = 0x3FFF; /* disable interrupts */      
        
               asm("nop");
      
               AD1CON1bits.SAMP = 1;
       
        
               while(!AD1CON1bits.DONE)
                    {
        	           Nop();
        	        }
        
      	      

               Result = (long) ADC1BUF0; 
        
               Ad_Result = (int)Result;////////
        
              //   Ad_Result = (Result*AVCC)/1024;  
               DISICNT = 0x0000; /* enable interrupts */

			   break;


		default: break;    
            
    }
// AD1CON3 = 0x1FD0;
// delayus(35);
 return (Ad_Result);
}

/*****************************************************************************
* Function: unsigned int  Adc_Count(uchar Type,uchar Count)
*
* Preconditions: ADCInit must be called before.
*
* Overview: This is a state mashine to grab analog data from potentiometer
* and temperature sensor.  多次采样某AD端口，并求平均值
*
* Input: Type of port,counts of sample.
*
* Output: average of ADC Value.
*
******************************************************************************/

unsigned int  Adc_Count(uchar Type,uchar Count)
{
	 uchar i;
	 unsigned  int Sum=0;//long
	 uint Summax=0,Summin=0,SumTemp=0;
	 if((Type == 0)||(Type >=19)||(Count >=64)||(Count == 0))
	 {
	 	return(0);
	 }
	 for(i=0 ; i<Count + 2 ; i++)
	 {
	 	SumTemp = ADC(Type);
	 	if(i==0)
	 	{
	 		Summax = SumTemp;
	 		Summin = SumTemp;//get the first 
	 	}
	 	if(Summax<SumTemp)
	 		Summax = SumTemp;//get the Max adc
	 	if(Summin>SumTemp)
	 		Summin = SumTemp;//get the Min adc
	 	Sum += SumTemp;
//	 	delayms(10);	 	
	 }
    SumTemp = Sum - Summax - Summin;
	Sum     = SumTemp/(uint)(Count);// del the Max and Min
	return(Sum);
}

unsigned int  Filter(uchar Type)
   {
      unsigned int value_buf[N];
   unsigned int count,i,j,temp;
   for ( count=0;count<N;count++)
   {
      value_buf[count] = ADC(Type);//Adc_Count(Type,5);
      delay();
   }
   for (j=0;j<N-1;j++)
   {
      for (i=0;i<N-j;i++)
      {
         if ( value_buf[i]>value_buf[i+1] )
         {
            temp = value_buf[i];
            value_buf[i] = value_buf[i+1]; 
             value_buf[i+1] = temp;
         }
      }
   }
   return value_buf[(N-1)/2];


     }
unsigned char Get_char(unsigned char tempH)
{
	unsigned char result;
	switch(tempH)
		{
			case 0:
			default:
				result = "0";
				break;
			case 1:
				result = "1";
				break;
			case 2:
				result = "2";
				break;
			case 3:
				result = "3";
				break;
			case 4:
				result = "4";
				break;
			case 5:
				result = "5";
				break;
			case 6:
				result = "6";
				break;
			case 7:
				result = "7";
				break;
			case 8:
				result = "8";
				break;
			case 9:
				result = "9";
				break;
		}
	return result;
}

/*---------------------------------------------------------------------
  Function Name: unsigned char *hex_to_char(unsigned int x)
  Description:   *hex_to_char  
  Inputs:        hex
  Returns:       result
-----------------------------------------------------------------------*/
unsigned char *hex_to_char(unsigned int x)
{
	uchar result[2];
	unsigned long result_temp;
	uch tempH,tempL;
	tempH = x/255;
	tempL = x%255;

	result_temp = (tempH/16)*1000+(tempH%16)*100+(tempL/16)*10+tempL%16;// BCD码（常用的10进制数）
	tempH = result_temp/10+'0';
	tempL = result_temp%10+'0';

	result[0] = 0x38;//Get_char(tempH);
	result[1] = 0x35;//Get_char(tempL);
	return result;
	
}


/*---------------------------------------------------------------------
  Function Name: XCHAR HexChartoChar(char c)
  Description:   hex_to_ASC
  Inputs:        hex
  Returns:       i
-----------------------------------------------------------------------*/
XCHAR HexChartoChar(char c)
{
 unsigned char i= 0;
 if ((c >= 0) &&(c <=9) )
  i= (c +'0');
 else if ( (c >= 'a') && (c <= 'f') ) 
  i= 10+(c -'a');
 else if ((c >= 'A') && (c <= 'F') )
  i= 10+(c -'A');
 return i;
}

XCHAR HexChartoInt(char c)
{
 unsigned char i= 0;
 if ((c >= '0') &&(c <='9') )
  i= (c -'0');
 else if ( (c >= 'a') && (c <= 'f') ) 
  i= 10+(c -'a');
 else if ((c >= 'A') && (c <= 'F') )
  i= 10+(c -'A');
 return i;
} 


/*---------------------------------------------------------------------
  Function Name: hex_bcd2
  Description:   hex to bcd2  16进制转BCD码
  Inputs:        None
  Returns:       res1
-----------------------------------------------------------------------*/
unsigned int hex_bcd2(unsigned int x)
{

	uchar ss=0;
	
	uint resl=0;

	if(x > 9999) x = 9999;
	
	ss = x/1000;
	
	resl |= ss;
	
	x = x - ss*1000;
	
	ss = x/100;
	
	resl = resl << 4;
	
	resl |= ss;
	
	x = x - ss*100;
	
	ss = x/10;
	
	resl = resl << 4;
	
	resl |= ss;
	
	x = x - ss*10;
	
	resl = resl << 4;
	
	resl |= x;
	
	return(resl);

}


/************************************************************************
* Function: void  inti_ads7822(void)           
*                                                                       
* Overview: This function initiates ads7822.    ads7822初始化
*                                                                       
* Input: void                                                    
*                                                                       
* Output: void
*                                                                       
************************************************************************/
void  inti_ads7822(void)
{
	
     TRISFbits.TRISF13    = 0 ;
     
     TRISFbits.TRISF12    = 1 ;
     
     TRISAbits.TRISA1   = 0 ;       
}

/************************************************************************
* Function: void  adc_delay(uchar i)            
*                                                                       
* Overview: this function delays time of   second   延时     
*                                                                       
* Input: i                                                    
*                                                                       
* Output: void
*                                                                       
************************************************************************/
void  adc_delay(uchar i)
{
   uchar j;
  
   for(j=0;j<i;j++)
      {
       asm("nop");
      }
} 
unsigned int Ad_Result=0;

/************************************************************************
* Function: unsigned int press_test(void)            
*                                                                       
* Overview: this function detects  the press  and   judges whether it overs       
*                                                                       
* Input: void         AD进行  压力检测                                         
*                                                                       
* Output: press
*                                                                       
************************************************************************/
unsigned int press_test(BYTE type)
{  

 AD_press=Adc_Count(9,5);
 
if(type==0xaa)
{
if(AD_press>Temp_adc )//0x015e	//
  { 
    press=1;
   
   }
else
	press=0;


return  press;
}
else if(type==0xbb)

{
if(AD_press<=epPress )
    press=0;
else     press=1;

return  press;
}
}



/************************************************************************
* Function: unsigned int ads7822(void)            
*                                                                       
* Overview: this function grabs analog data  and  transforms to digital data 
            by ads7822.    外置ads7822对燃料电池输出采样 转化  
*                                                                       
* Input: void                                                     
*                                                                       
* Output: pppp
*                                                                       
************************************************************************/

unsigned int ads7822(void)
{
   uint pppp,tttt;
   uchar i;
   tttt =0x0800;
   pppp=0;
    
   sclk_adc = 0 ;
    
   adc_delay(4);////////
   
   cs_adc = 0  ;
   
   adc_delay(4);////////
   
   sclk_adc = 0 ;
   
   adc_delay(4);
   
   sclk_adc = 1 ;
   
   adc_delay(4);
   
   sclk_adc = 0 ;
   
   adc_delay(4);
   
   sclk_adc = 1 ;
   
   adc_delay(4);
   
   sclk_adc = 0 ;
   
   adc_delay(4);
   
   sclk_adc = 1 ; 
   
   for(i=0;i<=11;i++)
      { 
      	
       sclk_adc = 0 ;
       
       asm("nop");
       
	   adc_delay(4);
	   
       if(spi_sdi_adc)//&0x80)== 0x80
         {
         	
	      pppp |=tttt;
	      
	     }
	     
	   tttt >>=1;     
	   
      sclk_adc = 1 ;	   	
       
      }  
      
        asm("nop");
    
   cs_adc = 1; 
   
   return(pppp);              
}

/*********************************************************************
* Function: Timer5 ISR
*
* PreCondition: none
*
* Input: none
*
* Output: none
*
* Side Effects: none
*
* Overview: Timer5 ISR provides delay between two ads7822 conversions,

            and    定时器5中断  控制采样频率  并进行中断采样
*           
*
* Note: none
*
********************************************************************/

void __attribute__((interrupt, shadow, no_auto_psv)) _T5Interrupt(void)
{

	DISICNT = 0x3FFF; // disable interrupts 
    IFS1bits.T5IF = 0;//clear timer5 int flag
   


//start_of_integration
        if(!flag)
	  {
			a = 0;	//a is sum of 8 time sample value;
            for(i=0;i<=7;i++)
               a += ads7822();	//modified--chen090604
            sum+=(unsigned long)a; //added--chen090604
            SampleCount++;

		if(( SampleCount==5*pp1)&&(pp1<=20))

          	{
             Decade[pp1-1]=a;

			 pp1++;

		     }//added--chen090604
            if((Temp2<a) && (peak_got==0))	//added--chen090604
		        {
				  Temp2=a;
                  after_peak_count = 0;	//added--chen090604
				  peak_sample_Count = SampleCount; //added--chen090604            				 
                 }
            if((Temp2>=a) && (peak_got==0))	//added--chen090604 
				{
				  after_peak_count++;	//--chen090604
				  if(after_peak_count>=peak_compare_time)
					{	peak_got = 1;
						peak_value = Temp2;
						sample_stop_value = Temp2>>4;
						if(sample_stop_value<16)  sample_stop_value = 16;	
				  	} 
				  if((Temp2>=256) && (a<=16))
					{
						peak_value = Temp2;
						flag=1;
				  	}
				}
			if((peak_got==1) && ((a<=sample_stop_value)||(SampleCount>=peak_sample_Count*16)))
					flag=1;		//added--chen090604

      }// end of if((pump_on)&&(!flag))
    DISICNT = 0x0000 ;
	 IFS1bits.T5IF = 0;//clear timer5 int flag
     }
 
/************************************************************************
* Function: unsigned int prepare_and_Charge(void)            
*                                                                       
* Overview: this function charges the pump  and opens the pump.  
*                测试准备   泵充电 进行零点的计算                                                       
* Input: void                                                     
*                                                                       
* Output: charge_flag
*                                                                       
************************************************************************/
unsigned int prepare_and_Charge(void)
{

  unsigned int charge_Temp1=0,charge_Temp2=0,ChargeCount;
  unsigned int i=0,j=0;
  Temp2=0;
  a=0;
  TRISFbits.TRISF0=0;
  pump_pwr=1;
  DISICNT = 0x3FFF;
  charge_Temp1 =  Adc_Count(8,3);
  ChargeCount  = 0x0000;
  charge_flag  =0;
  if(charge_Temp1 <= 0x7d0)//960,960 old is 960
    { 
        charge_Temp2 = 0; 
		
        while((charge_Temp2<=0x06b2)&&(ChargeCount <= 5000))// ,0x0960
             {                                   //2380 ,0x0900
               
                       
               //ClrWdt(); 
                                            //0x0834 - 2880                   
               charge_Temp2 =  Adc_Count( 8,3 ) ;
             //   if(charge_Temp2>=0x0900)       
               ChargeCount++;      
             }
   
       DISICNT = 0x0000 ;
     //   ChargeCount  = 0x0000;
    }
  charge_Temp1=0;
  charge_Temp2=0;
  charge_flag=1; 
/*  for(i=0;i<=2;i++)
     {
       a=ads7822();
       initial_value+=a;

     }
  initial_value/=3;*/
  delayms(100);
  pump_pwr=0;
  return(charge_flag);
}



/************************************************************************
* Function: unsigned int Temperature(void)            
*                                                                       
* Overview: this function gets the current temperature  when makes a  marking.  
*                                                                       
* Input: void            温度测量                                         
*                                                                       
* Output: T
*                                                                       
************************************************************************/
unsigned int Temperature(void)

{


   unsigned long Tem_Tem=0;
   Tem_AD = Adc_Count(7,5);
   Tem_Tem=(unsigned long)Tem_AD;
   Tem_Tem=(Tem_Tem*3300)/1024;
   Tem_AD=(unsigned int)Tem_Tem;

   if(Tem_AD>500)
     {

	   T2 =Tem_AD - 500;
       T2 /=10;
       T=T2;
       Kt=3;
      }
   if(Tem_AD==500)
     {

	   T1=0;
       T=T1;
       Kt=2;
     }

   if(Tem_AD<500)
     {

	   T0=500-Tem_AD;
       T0/=10;
       T=T0;
       Kt=1;
      }

   return(T);

}



unsigned int test_result;

/************************************************************************
* Function: Void  Pump_action(void)            
*                                                                       
* Overview: 泵动作，抽气
            
*                                                                       
* Input: void                                                     
*                                                                       
* Output: void
*                                                                       
************************************************************************/
void  Pump_action(void)
{
   unsigned int charge_Temp1=0,charge_Temp2=0,ChargeCount;
   TRISFbits.TRISF1=1;
   TRISFbits.TRISF0=1;
   pump_on =0;
   pump_pwr=1;
   TRISFbits.TRISF1=0;
   TRISFbits.TRISF0=0;
   ChargeCount  = 0x0000;
   charge_flag  =0;
   charge_Temp1 =  Adc_Count(8,3);
   DISICNT = 0x3FFF;
   if(charge_Temp1 <= 0x0960)
     { 
    
       while((charge_Temp2<=0x07D0)&&(ChargeCount <= 3000))
            {                                   
               pump_pwr=1;  
                       
               //ClrWdt(); 
                                                              
               charge_Temp2 =  Adc_Count( 8,3 ) ;
                       
               ChargeCount++;      
            }
       // pump_pwr=0;
       charge_flag  =1;    
       ChargeCount  = 0x0000;
       TRISFbits.TRISF1=1; 
      }

 pump_on =1;	
 TRISFbits.TRISF1=0;
// while(pump_on!=1){pump_on =1;}
delayms(10000);

   charge_Temp1 =  Adc_Count(8,3);

   if(charge_Temp1 <= 0x0960)
     { 
    
       while((charge_Temp2<=0x07D0)&&(ChargeCount <= 3000))
            {                                   
               pump_pwr=1;  
                       
               //ClrWdt(); 
                                                              
               charge_Temp2 =  Adc_Count( 8,3 ) ;
                       
               ChargeCount++;      
            }
       // pump_pwr=0;
       charge_flag  =1;    
       ChargeCount  = 0x0000;
       TRISFbits.TRISF1=1;
      }
   
pump_on =0;
TRISFbits.TRISF1=0;
DISICNT = 0x0000;
}
/************************************************************************
* Function: unsigned int caiyang(void)            
*                                                                       
* Overview: when the voltage of pump beyonds 0x0960,open the pump and begin to
            sampling.  开关定时器  并进行采样计算
*                                                                       
* Input: void                                                     
*                                                                       
* Output: temp_sum
*                                                                       
************************************************************************/
unsigned int caiyang(void)

{

   unsigned int charge_Temp1=0,charge_Temp2=0,ChargeCount;//Temp2=0,SampleCount=0,Temp=0,sum=0;
   unsigned int i=0,j=0;
   pp1=1;
  for(i=0;i<20;i++)
   { 
     Decade[i]	= 0; 
    }

   SampleCount=0;

   
   TRISFbits.TRISF1=0;
   TRISFbits.TRISF0=0;
   pump_pwr=1;
   charge_Temp1 =  Adc_Count(8,3);
   ChargeCount  = 0x0000;
   charge_flag  =0;
 //  delayms(100);
   charge_Temp1 =  Adc_Count(8,3);

   if(charge_Temp1 <= 0x7d0)//2000=7D0
     { 
    
       while((charge_Temp2<=0x06b2)&&(ChargeCount <= 3000))
            {                                   
               pump_pwr=1;  
                       
               //ClrWdt(); 
                                                              
               charge_Temp2 =  Adc_Count( 8,3 ) ;
                       
               ChargeCount++;      
            }
       // pump_pwr=0;
       charge_flag  =1;    
       ChargeCount  = 0x0000;

      }
   
 //  delayms(100);
   pump_pwr=0;
   DISICNT = 0x3FFF;
   peak_compare_time=0;
	Temp2=0;		//added--chen090604
    sum = 0; //added--chen090604
    SampleCount = 0;		//added--chen090604
	peak_got = 0;	//added--chen090604
    after_peak_count = 0;	//added--chen090604
// setting peak_compare_time    added--chen090604
//according to temperature sample
// <2度，peak_compare_time = 100
// >2度，<10度 peak_compare_time = 80
// >10度，<20度 peak_compare_time = 60
// >20度， peak_compare_time = 48
//
//

Tem_AD = Adc_Count(7,5);

if(Tem_AD<=0x00a1)
  { peak_compare_time = 90;   }
else if((Tem_AD>0x00a1)&&(Tem_AD<=0x00ba))
  {peak_compare_time = 70;}
else if((Tem_AD>0x00ba)&&(Tem_AD<=0x00d9))
  { peak_compare_time = 50;}
else  { peak_compare_time = 39;}

EEPROMWriteWord(peak_compare_time,ID_JIQIADDRESS+5);

   pump_on =0;		
   pump_reset = 1;
   DISICNT = 0x0000;
   delayms(1000);
   pump_on =0;		
   pump_reset = 0;
  // pump_pwr=0;
   charge_Temp1=0;
   charge_Temp2=0;
//   delayms(100);
   IFS1bits.T5IF = 0;
   IEC1bits.T5IE = 1;
   delayms(10);
   TMR5 = 0;
   PR5 =0x0FA0;


   //pump_pwr=0;
   //sum=0;
   T5CONbits.TON = 1;
   delayms(10);
   while(!flag){delayms(20);}



   IEC1bits.T5IE = 0;
   T5CONbits.TON = 0;
   IEC1bits.T5IE = 0;

   if(flag)
     {

	    T5CONbits.TON = 0;
		prepare_and_Charge();
       pump_on =1;		//breath_sample_start
       pump_reset = 0;
	   delayms(1000);
       pump_on =0;		
       pump_reset = 0;
		//sum >>= 1;	//added--chen090604
        temp_sum=sum;
        sum=0;
		EEPROMWriteWord(peak_sample_Count,ID_JIQIADDRESS+13);
		EEPROMWriteWord(peak_value,ID_JIQIADDRESS+9);
		EEPROMWriteWord(SampleCount,ID_JIQIADDRESS+11);
		//EEPROMWriteWord(flag,ID_JIQIADDRESS+25);
        temp_sum >>= 5;

		if(temp_sum>=0x0000ffff)
		   temp_sum=0x0000ffff ;

		flag=0;
        temp_sum &=0x0000ffff;
		EEPROMWriteWord((unsigned int)temp_sum,ID_JIQIADDRESS+7);

        return ((unsigned int)temp_sum);

     }



}

/************************************************************************
* Function: unsigned int Manual_Analyze(void)            
*                                                                       
* Overview:  calculate  the  result of test,and transform the  results  of test 
             to  ASC which be in different units  and can be show in screen. 
*                    获取浓度值  并转化BCD码                                                  
* Input: void                                                     
*                                                                       
* Output: ADCResult
*                                                                       
************************************************************************/

unsigned int Manual_Analyze(void)

{

    unsigned int charge_Temp1=0,charge_Temp2=0,ChargeCount, Temp1;
    unsigned int i=0,j=0,K_Value;
    unsigned char AdcTemp,ADCResult[4]="8039",Data_Gas;
   unsigned long result_multi;	
	
	test_result= caiyang();//8000
		
	EEPROMWriteWord(Tem_AD,ID_JIQIADDRESS+3);
	cons_t=Cons_Aduj(Tem_AD);
	EEPROMWriteWord(cons_t,ID_JIQIADDRESS+15);
	DISICNT = 0x3FFF;  /* disable interrupts */
	       
	Cons   = cons_calculate(cons_t);//Big_Value2
	       
	DISICNT = 0x0000;  /* enable interrupts */

	if(peak_compare_time == Compare_time_1 )//&&(Cons<200)
       {

	    if ((Cons<500)&&(Cons>100))//just for ru
    	      Cons  =  Cons+70;//70   
	   else if((Cons>=500)&&(Cons<=700))
	   	    Cons  =  Cons+60;
	   else if((Cons>700)&&(Cons<=1200))
	   	    Cons  =  Cons+50;
	   else Cons  =  Cons+35;

		}
	else  //if(peak_compare_time == Compare_time_2 )//&&(Cons<200)
		{
#ifdef Low_Compensation
       if((Cons<500)&&(Cons>100))
	   	#ifdef Compensation_low_plus
	      Cons  =  Cons+50;//Cons+20;--2012..1.13+50
	    #else
		  Cons  =  Cons+17;//minus-015
		#endif
#endif
	}

	/*
   if(Cons<=0x0800)
	if((Cons&0x000f)>=4)
		{
		if((Cons&0x00f0)<=0x0080)
			{
		  	Cons=Cons+0x0010;

            Cons=Cons&0xfff0;
			}
	else if ((Cons&0x00f0)==0x0090)

		{
           Cons=Cons+0x0100;

		   Cons=Cons&0xff00; 

	}
			
		}	
		*/ //20100731

	if(Masklevel == 0xaa)
		{

       if(Cons<ScreenMask)
			 alocholdetecet=FALSE;

	   else 
	         alocholdetecet=TRUE;

	   }//2010.12.13  by gxy
	else{
     if(Cons<TestMask)
     	{
           //if(Cons<ScreenMask)//2012.05.22
			 Cons =0;
			 alocholdetecet=FALSE;
     	}
	 else 
	 	{


	    if((Cons<200)&&(Cons>=100))
           Cons=Cons+10;
	
	      alocholdetecet=TRUE;
	 	}
		}
	K_Value=EEPROMReadWord(KSelectDataAddress);

	if(Cons>AlarmMask)
	alarmON = TRUE;


//Cons=6000;
if(0x33 == EEPROMReadByte(updateVision))
	if((ID_RADIOBUTTONStar + 8) == EEPROMReadByte(RButtonAddress+4))
		if(Cons>=5250)
	  PeakScreen=0xaa;

    switch(EEPROMReadByte(RButtonAddress+4)){


				  case (ID_RADIOBUTTONStar + 7) :
          
		                  break;
		  
                  case (ID_RADIOBUTTONStar + 8) :


		                  Unit_Select = TRUE;
		                  sum_cons=(unsigned long)Cons;
	                      sum_cons  =sum_cons*1000/K_Value;// mg/L
	                      Cons=(unsigned int)sum_cons;	

		                  break;

	              case (ID_RADIOBUTTONStar + 9) :
         
                          sum_cons=(unsigned long)Cons;
	                      sum_cons  =sum_cons/10;// %
	                      Cons=(unsigned int)sum_cons;	

		                  break;

		  
                  case (ID_RADIOBUTTONStar + 10) :


		
		                  sum_cons=(unsigned long)Cons;
	                      sum_cons  =sum_cons/10;// %
	                      Cons=(unsigned int)sum_cons;	

		                  break;	  

                  case (ID_RADIOBUTTONStar + 11) :	

      
		                  break;	  //%o
	

                  case (ID_RADIOBUTTONStar + 12) :	

                          sum_cons=(unsigned long)Cons;
	                      sum_cons  =sum_cons*1000/K_Value;// ug/mL
	                      Cons=(unsigned int)sum_cons;	

		                  break; 	  

	
                   case (ID_RADIOBUTTONStar + 13) :	
	

		                  break;                         //   mg/mL

	               case (ID_RADIOBUTTONStar + 14) :
                       

		                  break;
	               case (ID_RADIOBUTTONStar + 15) ://ug/l
   		                  sum_cons=(unsigned long)Cons;
	                      sum_cons  = sum_cons*1000/K_Value;// mg/L
	                      Cons=(unsigned int)sum_cons;                    

		                  break;


		           default: break;

   	     }


	 Cons=hex_bcd2(Cons);//hex_to_char(sum);
	 Nop();
	 /*
	if(Cons>0x0020)
	alarmON = TRUE;
	*/
	/*
	if(test_result>120)
	alarmON = TRUE;
	*/
     switch(EEPROMReadByte(RButtonAddress+4)){



				   case (ID_RADIOBUTTONStar + 7) ://mg/100mL

                         
                           //  if(Cons<0x0050)
		                    //        Cons =0;
                         
                         
							 Flashbuff[10] = (Cons>>8) & 0x00ff;
	                         Flashbuff[11] = Cons & 0x00ff;
	
		                     AdcTemp = Cons&0x000f;
		                     Test_Result[4] = AdcTemp+'0';//HexChartoInt('7');
		                     Cons=Cons>>4;

		                     Test_Result[3] = 0x002e;
		
		                     AdcTemp = Cons&0x000f;
		                     Test_Result[2] = AdcTemp+'0';//HexChartoInt('7');
		                     Cons=Cons>>4;

		                     AdcTemp = Cons&0x000f;
		                     Test_Result[1] = AdcTemp+'0';//HexChartoInt('7');
		                     Cons=Cons>>4;

		                     AdcTemp = Cons&0x000f;
		                     Test_Result[0] = AdcTemp+'0';//HexChartoInt('7');
		                     Cons=Cons>>4;

		                     break;
		  
                   case (ID_RADIOBUTTONStar + 8) ://mg/L


		                     if(Cons<0x0015)
			                       Cons = 0;

							 Flashbuff[10] = (Cons>>8) & 0x00ff;
		                     Flashbuff[11] = Cons & 0x00ff;
	
		                     AdcTemp = Cons&0x000f;
		                     Test_Result[4] = AdcTemp+'0';//HexChartoInt('7');
		                     Cons=Cons>>4;

		                     AdcTemp = Cons&0x000f;
		                     Test_Result[3] = AdcTemp+'0';
		                     Cons=Cons>>4;
		
		                     AdcTemp = Cons&0x000f;
		                     Test_Result[2] = AdcTemp+'0';//HexChartoInt('7');
		                     Cons=Cons>>4;

		                     Test_Result[1] = 0x002e;

		                     AdcTemp = Cons&0x000f;
		                     Test_Result[0] = AdcTemp+'0';//HexChartoInt('7');
		                     Cons=Cons>>4;	

		                     break;


	               case (ID_RADIOBUTTONStar + 9) ://%
                             //  if(Cons<0x0010)
		                      //      Cons =0;

	                         Flashbuff[10] = (Cons>>8) & 0x00ff;
	                         Flashbuff[11] = Cons & 0x00ff; 
		                     AdcTemp = Cons&0x000f;
		                     Test_Result[4] = AdcTemp+'0';//HexChartoInt('7');
		                     Cons=Cons>>4;

		                     AdcTemp = Cons&0x000f;
		                     Test_Result[3] = AdcTemp+'0';
		                     Cons=Cons>>4;
		
		                     AdcTemp = Cons&0x000f;
		                     Test_Result[2] = AdcTemp+'0';//HexChartoInt('7');
		                     Cons=Cons>>4;

		                     Test_Result[1] = 0x002e;

		                     AdcTemp = Cons&0x000f;
		                     Test_Result[0] = AdcTemp+'0';//HexChartoInt('7');
		                     Cons=Cons>>4;	
	

		                     break;

		  
                   case (ID_RADIOBUTTONStar + 10) ://g/100mL
                              // if(Cons<0x0010)
		                      //      Cons =0;

	                         Flashbuff[10] = (Cons>>8) & 0x00ff;
	                         Flashbuff[11] = Cons & 0x00ff;


		                     AdcTemp = Cons&0x000f;
		                     Test_Result[4] = AdcTemp+'0';//HexChartoInt('7');
		                     Cons=Cons>>4;

		                     AdcTemp = Cons&0x000f;
		                     Test_Result[3] = AdcTemp+'0';
		                     Cons=Cons>>4;
		
		                     AdcTemp = Cons&0x000f;
		                     Test_Result[2] = AdcTemp+'0';//HexChartoInt('7');
		                     Cons=Cons>>4;

		                     Test_Result[1] = 0x002e;

		                     AdcTemp = Cons&0x000f;
		                     Test_Result[0] = AdcTemp+'0';//HexChartoInt('7');
		                     Cons=Cons>>4;	
	

		                     break;	  


                   case (ID_RADIOBUTTONStar + 11) ://%.	

                              // if(Cons<0x0010)
		                      //      Cons =0;
			                 Flashbuff[10] = (Cons>>8) & 0x00ff;
	                         Flashbuff[11] = Cons & 0x00ff;


		                     AdcTemp = Cons&0x000f;
		                     Test_Result[4] = AdcTemp+'0';//HexChartoInt('7');
		                     Cons=Cons>>4;

		                     AdcTemp = Cons&0x000f;
		                     Test_Result[3] = AdcTemp+'0';
		                     Cons=Cons>>4;
		
		                     AdcTemp = Cons&0x000f;
		                     Test_Result[2] = AdcTemp+'0';//HexChartoInt('7');
		                     Cons=Cons>>4;

		                     Test_Result[1] = 0x002e;

		                     AdcTemp = Cons&0x000f;
		                     Test_Result[0] = AdcTemp+'0';//HexChartoInt('7');
		                     Cons=Cons>>4;	
	

		                     break;
      
	  
	

                   case (ID_RADIOBUTTONStar + 12) :	//ug/100mL
		                     //if(Cons<0x0050)
			                  //      Cons = 0;

			                 Flashbuff[10] = (Cons>>8) & 0x00ff;
	                         Flashbuff[11] = Cons & 0x00ff;

							 
		                     AdcTemp = Cons&0x000f;
		                     Test_Result[4] = AdcTemp+'0';//HexChartoInt('7');
		                     Cons=Cons>>4;

		                     Test_Result[3] = 0x002e;
		
		                     AdcTemp = Cons&0x000f;
		                     Test_Result[2] = AdcTemp+'0';//HexChartoInt('7');
		                     Cons=Cons>>4;

		                     AdcTemp = Cons&0x000f;
		                     Test_Result[1] = AdcTemp+'0';//HexChartoInt('7');
		                     Cons=Cons>>4;

		                     AdcTemp = Cons&0x000f;
		                     Test_Result[0] = AdcTemp+'0';//HexChartoInt('7');
		                     Cons=Cons>>4;

		                     break; 	  

	
                   case (ID_RADIOBUTTONStar + 13) ://mg/mL	

							//if(Cons<0x0010)
		                     //       Cons =0;

		                     Flashbuff[10] = (Cons>>8) & 0x00ff;
	                         Flashbuff[11] = Cons & 0x00ff;


							 AdcTemp = Cons&0x000f;
		                     Test_Result[4] = AdcTemp+'0';//HexChartoInt('7');
		                     Cons=Cons>>4;

		                     AdcTemp = Cons&0x000f;
		                     Test_Result[3] = AdcTemp+'0';
		                     Cons=Cons>>4;
		
		                     AdcTemp = Cons&0x000f;
		                     Test_Result[2] = AdcTemp+'0';//HexChartoInt('7');
		                     Cons=Cons>>4;

		                     Test_Result[1] = 0x002e;

		                     AdcTemp = Cons&0x000f;
		                     Test_Result[0] = AdcTemp+'0';//HexChartoInt('7');
		                     Cons=Cons>>4;	
	

		                     break;

	 

	               case (ID_RADIOBUTTONStar + 14) ://g/L

						    // if(Cons<0x0010)
		                     //       Cons =0;

		                     Flashbuff[10] = (Cons>>8) & 0x00ff;
	                         Flashbuff[11] = Cons & 0x00ff;


							 AdcTemp = Cons&0x000f;
		                     Test_Result[4] = AdcTemp+'0';//HexChartoInt('7');
		                     Cons=Cons>>4;

		                     AdcTemp = Cons&0x000f;
		                     Test_Result[3] = AdcTemp+'0';
		                     Cons=Cons>>4;
		
		                     AdcTemp = Cons&0x000f;
		                     Test_Result[2] = AdcTemp+'0';//HexChartoInt('7');
		                     Cons=Cons>>4;

		                     Test_Result[1] = 0x002e;

		                     AdcTemp = Cons&0x000f;
		                     Test_Result[0] = AdcTemp+'0';//HexChartoInt('7');
		                     Cons=Cons>>4;
		                     break;

                   case (ID_RADIOBUTTONStar + 15) :	//ug/L


			                 Flashbuff[10] = (Cons>>8) & 0x00ff;
	                         Flashbuff[11] = Cons & 0x00ff;

					         Test_Result[4] = 0;
				 


		                     AdcTemp = Cons&0x000f;
		                     Test_Result[3] = AdcTemp+'0';//HexChartoInt('7');
		                     Cons=Cons>>4;							 
		
		                     AdcTemp = Cons&0x000f;
		                     Test_Result[2] = AdcTemp+'0';//HexChartoInt('7');
		                     Cons=Cons>>4;

		                     AdcTemp = Cons&0x000f;
		                     Test_Result[1] = AdcTemp+'0';//HexChartoInt('7');
		                     Cons=Cons>>4;

		                     AdcTemp = Cons&0x000f;
		                     Test_Result[0] = AdcTemp+'0';//HexChartoInt('7');
		                     Cons=Cons>>4;

		                     break;

		           default: break;

   	}

     sum = 0;

     return(ADCResult);
}



/************************************************************************
* Function: unsigned int  cons_calculate(unsigned int AdujAdc)            
*                                                                       
* Overview:  calculate  
*                                                                       
* Input: AdujAdc                                                     
*                                                                       
* Output: MarkDensityNow
*                                                                       
************************************************************************/
unsigned int  cons_calculate(unsigned int AdujAdc)

{
	 unsigned char  Data_Gas;
	unsigned long result_multi;

  
     unsigned long   MarkDensityNow;

     MarkDensity= EEPROMReadWord(0x6020);
     MarkDensityNow = (unsigned long)MarkDensity;
     MarkAdc= EEPROMReadWord(0x6022);

	//if( Drygas == EEPROMReadByte(DryorWetAddress)) 干气校准
		//if(0 != EEPROMReadByte(DrydataAddress))
			if(((TestMode == 0xdd)&&(DryorWet == Drygas))||((TestMode == 0xdd)&&(Settingposition==0xaa)))//干气cal  check
		{
		
            Data_Gas = EEPROMReadByte(DrydataAddress);
			
            result_multi = (unsigned long)MarkAdc;
		 
	             result_multi = result_multi-result_multi*Data_Gas/100;

                    MarkAdc = (unsigned int)result_multi;	

			calculate_Temp  = MarkDensityNow*(unsigned long)test_result; 
			    calculate_Temp  = calculate_Temp/(unsigned long)MarkAdc;
			    MarkDensityNow = calculate_Temp;	

			return((unsigned int)MarkDensityNow);	

	}
/*
	if( Drygas == EEPROMReadByte(DryorWetAddress))
		if(0 != EEPROMReadByte(DrydataAddress))
			if((TestMode == 0xdd)&&(DryorWet == Wetgas))
		{
		
		   Data_Gas = EEPROMReadByte(DrydataAddress);
		//	if(signtemp == 0x2B)
		result_multi = (unsigned long)MarkAdc;
		
				result_multi = result_multi+2*result_multi*Data_Gas/100;
		
				   MarkAdc = (unsigned int)result_multi;

	}	 
*/
     nAdc= EEPROMReadWord(0x6014);

     if((test_result != 0)&&(MarkAdc != 0)){

          if(nflag==0xee)

        	 {


			    calculate_Temp  = MarkDensityNow*(unsigned long)test_result; 
			    calculate_Temp  = calculate_Temp/(unsigned long)MarkAdc;
			    MarkDensityNow = calculate_Temp;
				EEPROMWriteWord(MarkAdc,ID_JIQIADDRESS+15);
			    nflag=0;

		     }

		  else

	         {	
                
					   calculate_Temp  = MarkDensityNow*(unsigned long)test_result;
    
                       calculate_Temp  =calculate_Temp/(unsigned long)MarkAdc;

	                   calculate_Temp  =calculate_Temp*(unsigned long)nAdc;

	                   calculate_Temp  = calculate_Temp/(unsigned long)AdujAdc;	

					   MarkDensityNow = calculate_Temp;

    	    }
       }
	else

	{

	    MarkDensityNow = 0;

	}

        //MarkDensityNow=(unsigned int)MarkDensityNow;
	
   return((unsigned int)MarkDensityNow);

}



/************************************************************************
* Function: unsigned int  Cons_Aduj(unsigned int app)            
*                                                                       
* Overview:  adjust  the result  in different temperature
*                                                                       
* Input: app                                                     
*                                                                       
* Output: rr
*                                                                       
************************************************************************/
unsigned int  Cons_Aduj(unsigned int app)

{  

     unsigned char i , zone, tt ;
   
     unsigned int  rr ;
   
     int yyt;
   
     //   unsigned char Markvalue[30]= {0x00} ;//26
     unsigned char   value;
     uchar A[4] = {0x00}; 
   
     unsigned int  sum1  = 0,sum2  = 0;
   
     unsigned long sum3   = 0;
   
     // suoitn = 0x04f0;
     rr = app;//suoitn;
   
     if((rr>=0x00df)&&(rr<=0x00f3))
         {

		     //    nAdc = EEPROMReadWord(DemarcateAddress+20);
   
	         rr=nAdc;

	         nflag=0xee;

	         return(0xee);
         }
     else
   	        nflag=0;

     if(rr >=0)
       {

		 for(i = 0;i <= 27;i++)//23
             {   

	           Markvalue[i]= EEPROMReadByte(0x6000+i) ;
   
             }
     
         for(i=0;i<=27;i+=2)

		    {

              value=Markvalue[i];
	          Markvalue[i]=Markvalue[i+1];
	          Markvalue[i+1]=value;
            } 
  
       }
  
   
     if((rr >= 0)&&(rr <= 0x0900))

	     zone  = tem_interzone(Markvalue,rr) ;//确定温度区间，共八个区间，返回区间号count

     if(zone<= 8)//7

	     Data_Calculate1(Markvalue,zone,rr);//进入温度区间，计算pp1=t高-t低，pp2=t当前-t低，pp3=ad高-ad低，pp4=pp2*pp3
   
         i   =  0; 

		 // tbr =  0x0320;

		 yyt =  Data_Calculate4(Markvalue,zone);//kk2=pp4/pp1,yyt=suotin2-kk2
   
         rr  = (unsigned int)yyt;
         ticknum=rr;
         i   =  0; 
         nflag=0;
         return(rr);  
}


/************************************************************************
* Function: void Hexshow(unsigned int Dem[])            
*                                                                       
* Overview:  Data of Hex  be showed  on screen
*                                                                       
* Input: unsigned int Dem[]                                                    
*                                                                       
* Output: void
*                                                                       
************************************************************************/
void Hexshow(XCHAR Dem[],unsigned int tem)

{

   	for(i=5;i>1;i--)

	   {

          Demarcate_tem= tem&0x000f; 
		  if(Demarcate_tem>9)
			{
               Demarcate_tem-=0x000a;
			   Dem[i]=(XCHAR)(Demarcate_tem+'A'); 
		    }
		  else
		    Dem[i]=(XCHAR)(Demarcate_tem+'0');
		    tem=tem>>4;

        }

}
/************************************************************************
* Function: unsigned int ByteToInt(uchar Byte1,uchar Byte2)            
*                                                                       
* Overview:  combine two variables whose type is Byte to  int
*                                                                       
* Input: uchar Byte1,uchar Byte2                                                     
*                                                                       
* Output: Temp1
*                                                                       
************************************************************************/
unsigned int ByteToInt(uchar Byte1,uchar Byte2)
{

	unsigned int Temp1;
    
    Temp1        = 0;
    
    Temp1       |= (unsigned int)Byte1;
    
    Temp1      <<= 8;
    
    Temp1       |= (unsigned int)Byte2;
    
    return(Temp1);	

}



/************************************************************************
* Function: unsigned char  tem_interzone(uchar y[],uint t)            
*                                                                       
* Overview:  locate the zone which the current temperature  belongs to
*                                                                       
* Input: uchar y[],uint t                                                     
*                                                                       
* Output: Count
*                                                                       
************************************************************************/
unsigned char  tem_interzone(uchar y[],uint t)
{
   uchar Count = 0;
   
   uchar S[2]  ={0};
   
   uchar i;
   
   uint  tt;
   
   int   tt2;
   
   int   t2;
   
   t2   = (int)t;
   
   for(i=0;i<=13;i+=2)//11
   {
   	
     S[0]  = y[i];//wender_show(y[i]);
     
     S[1]  = y[i+1];//wender_show(y[i+1]);
     
     tt    = ByteToInt(S[0],S[1]);
     
     tt2   = (int)tt;  //取标定温度，一共七个   
     
     Count++;
     
     if(Count == 7)//6
     {
      if(t2-tt2 > 0)//大于最高温度，count=8
      {
       Count++;
      }
     }
     if(t2-tt2 <= 0)
     {
      break;
     }     
   }
   i= 4;
   
   ok_pp = 0;
   
   S[0]  = y[i];
   
   S[1]  =y[i+1];
   
   ok_pp = ByteToInt(S[0],S[1]);//常温
   
   return(Count);
}


/************************************************************************
* Function: void  Data_Calculate1(uchar y3[],uchar Count3,uint Du2)            
*                                                                       
* Overview:  calculate  pp1,pp2,pp3,pp4
*                                                                       
* Input: uchar y3[],uchar Count3,uint Du2                                                    
*                                                                       
* Output: void 
*                                                                       
************************************************************************/
void  Data_Calculate1(uchar y3[],uchar Count3,uint Du2)
{
   uchar Count = 0;
   uchar i;
   int  tt1,tt2,tt3,tt4;   
   int   du2;
   uchar y[28] = {0x00};//24
   du2   = (int)Du2;
   for(i=0;i<=27;i++)//23
   {
     y[i] = y3[i];
   } 
   switch(Count3)
   {

    case 0x01:

    tt1  =  (int)ByteToInt(y[0],y[1]);
    tt2  =  (int)ByteToInt(y[2],y[3]);
    Data_Calculate2(tt1,du2,tt2);
    tt1  =  (int)ByteToInt(y[14],y[15]);//12,13
    suoitn2  = tt1;
    tt2  =  (int)ByteToInt(y[16],y[17]);//14,15
    Data_Calculate3(tt1,tt2);
    break;

	case 0x02:

    tt1  =  (int)ByteToInt(y[0],y[1]);
    tt2  =  (int)ByteToInt(y[2],y[3]);
    Data_Calculate2(tt1,du2,tt2);
    tt1  =  (int)ByteToInt(y[14],y[15]);//12,13
    suoitn2  = tt1;
    tt2  =  (int)ByteToInt(y[16],y[17]);//14,15
    Data_Calculate3(tt1,tt2);
    break;

	case 0x03:

    tt1  =  (int)ByteToInt(y[2],y[3]);
    tt2  =  (int)ByteToInt(y[4],y[5]);
    Data_Calculate2(tt1,du2,tt2);
    tt1  =  (int)ByteToInt(y[16],y[17]);//14,15
    suoitn2  = tt1;
    tt2  =  (int)ByteToInt(y[18],y[19]);//16,17
    Data_Calculate3(tt1,tt2);
    break;

	case 0x04:

    tt1  =  (int)ByteToInt(y[4],y[5]);
    tt2  =  (int)ByteToInt(y[6],y[7]);
    Data_Calculate2(tt1,du2,tt2);
    tt1  =  (int)ByteToInt(y[18],y[19]);//16,17
    suoitn2  = tt1;
    tt2  =  (int)ByteToInt(y[20],y[21]);//18,19
    Data_Calculate3(tt1,tt2);
    break;

	case 0x05:


    tt1  =  (int)ByteToInt(y[6],y[7]);
    tt2  =  (int)ByteToInt(y[8],y[9]);
    Data_Calculate2(tt1,du2,tt2);
    tt1  =  (int)ByteToInt(y[20],y[21]);//18,19
    suoitn2  = tt1;
	       //nAdc  = tt1;
    tt2  =  (int)ByteToInt(y[22],y[23]);//20,21
    Data_Calculate3(tt1,tt2);

    break;

	case 0x06:

    tt1  =  (int)ByteToInt(y[8],y[9]);
    tt2  =  (int)ByteToInt(y[10],y[11]);
    Data_Calculate2(tt1,du2,tt2);
    tt1  =  (int)ByteToInt(y[22],y[23]);//20,21
    suoitn2  = tt1;
    tt2  =  (int)ByteToInt(y[24],y[25]);//22,23
    Data_Calculate3(tt1,tt2);
    break;

	case 0x07:

    tt1  =  (int)ByteToInt(y[10],y[11]);//8,9
    tt2  =  (int)ByteToInt(y[12],y[13]);//10,11
    Data_Calculate2(tt1,du2,tt2);
    tt1  =  (int)ByteToInt(y[24],y[25]);//20,21
    suoitn2  = tt1;
    tt2  =  (int)ByteToInt(y[26],y[27]);//22,23
    Data_Calculate3(tt1,tt2);
    break;

	case 0x08://jiade

    tt1  =  (int)ByteToInt(y[10],y[11]);//8,9
    tt2  =  (int)ByteToInt(y[12],y[13]);//10,11
    Data_Calculate2(tt1,du2,tt2);
    tt1  =  (int)ByteToInt(y[24],y[25]);//20,21
    suoitn2  = tt1;
    tt2  =  (int)ByteToInt(y[26],y[27]);//22,23
    Data_Calculate3(tt1,tt2);
    break;

	default: pp1=0;pp2=0; break;
   }
}


/************************************************************************
* Function: void Data_Calculate2(int ee1,int rr2,int rr3)            
*                                                                       
* Overview:  calculate  pp1,pp2
*                                                                       
* Input: int ee1,int rr2,int rr3                                                     
*                                                                       
* Output: void
*                                                                       
************************************************************************/
void Data_Calculate2(int ee1,int rr2,int rr3)
{
    
    pp1 = 0;
    pp2 = 0;
    Lw  = 0;
    if(rr3 > ee1)
    {
     pp1 = rr3 - ee1;
    }
    if(rr2 > ee1)
    {
     pp2 = rr2 - ee1;
     Lw  = 0x03;
    } 
    else
    {
     pp2 = ee1 - rr2;
     Lw  = 0xee;
    } 
    
}


/************************************************************************
* Function: void Data_Calculate3(int rr1,int rr2)            
*                                                                       
* Overview:  calculate  pp3,pp4
*                                                                       
* Input: int rr1,int rr2                                                    
*                                                                       
* Output: void
*                                                                       
************************************************************************/
void Data_Calculate3(int rr1,int rr2)
{
    
   pp3 = 0;
    
   if(rr2 > rr1)
   {
     pp3 = rr2 - rr1;
     Hi  = 0x03;
   } 
   else
   {    
     pp3 = rr1 - rr2;
     Hi  = 0xee;     
   } 
   pp4 = pp2*pp3;  
}




/************************************************************************
* Function: int  Data_Calculate4(unsigned char b[],uchar yo)           
*                                                                       
* Overview:  calculate  the final result of adjust
*                                                                       
* Input: unsigned char b[],uchar yo                                                    
*                                                                       
* Output: sum
*                                                                       
************************************************************************/
int  Data_Calculate4(unsigned char b[],uchar yo)
{

     uchar i,j;
     unsigned int  kk2;
     unsigned int sum=0;

	 if((yo == 0)||(yo >= 9)||(pp4==0)||(pp1==0)||(suoitn2 == 0))//||8
        {   

		    return((int)suoitn2);//tbr
        }

	 kk2 = pp4/pp1;


     if((suoitn2 < kk2)&&(yo == 1)&&(Hi == 0x03))//fan zhi fan jian
       {

		 return((int)suoitn2);
       }

     if((suoitn2 < kk2)&&(Hi == 0xee))           //fan zhi fan jian
       {
         return((int)suoitn2);
       }

	 if(yo == 1)
        {

           Nop();
           Nop();
           Nop();
           if(Hi == 0x03)
              {
  
                 sum = suoitn2 - kk2;

		      }
           else
		       if(Hi == 0xee)
                  { 

			             
                     sum = suoitn2 + kk2;

				   }

           return(sum);
        }
     else
         if(yo == 2)
           {
             Nop();
             Nop();
             Nop();
             if(Hi == 0x03)
                {

                      
                   sum = suoitn2 + kk2;
                }
             else
                if(Hi == 0xee)
                  { 

				       
                    sum = suoitn2 - kk2;
                  }
             return(sum);
           }
     else
         if(yo == 3)
           {
              Nop();
              Nop();
              Nop();
              if(Hi == 0x03)
                {
                        
                    sum = suoitn2 + kk2;
                }
              else
                 if(Hi == 0xee)
                    { 
                           
                        sum = suoitn2 - kk2;
                     }
              return(sum);
           }
     else
         if(yo == 4)
           {
              Nop();
              Nop();
              Nop();
              if(Hi == 0x03)
                {

				       
                    sum = suoitn2 +kk2;
                 }
              else
                 if(Hi == 0xee)
                    { 
                          
                       sum = suoitn2 -kk2;
                     }
              return(sum);
            }
     else
          if(yo == 5)
            {

			  Nop();
              Nop();
              Nop();
              if(Hi == 0x03)
                 {

                        
                   sum = suoitn2 +kk2;
                 }
              else
                 if(Hi == 0xee)
                   { 

					   
                     sum = suoitn2 -kk2;
                   }
              return(sum);
            }
     else
         if(yo == 6)
           {
              Nop();
              Nop();
              Nop();
              if(Hi == 0x03)
                {
                     
                    sum = suoitn2 + kk2;
                }
              else
                 if(Hi == 0xee)
                    { 
                         
                      sum = suoitn2 -kk2;
                     }
              return(sum);
           }
     else
         if(yo == 7)
           {
              Nop();
              Nop();
              Nop();
              if(Hi == 0x03)
                {
                       
                   sum = suoitn2 + kk2;
                }
              else
                if(Hi == 0xee)
                  { 
                         
                     sum = suoitn2 -kk2;
                   }
              return(sum);
            }
      else
          if(yo == 8)
            {
               Nop();
               Nop();
               Nop();
               if(Hi == 0x03)
                 {
                     
                    sum = suoitn2 + kk2;//+
                 }
               else
                  if(Hi == 0xee)
                    { 
                          
                       sum = suoitn2 - kk2;//+
                     }
               return(sum);
            }
}



