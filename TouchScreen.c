/*****************************************************************************
 *
 * Simple 4 wire touch screen driver
 *
 *****************************************************************************
 * FileName:        TouchScreen.c
 * Dependencies:    MainDemo.h
 * Processor:       PIC24, PIC32
 * Compiler:       	MPLAB C30, MPLAB C32
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
 * Anton Alkhimenok		01/08/07	...
 * Anton Alkhimenok		06/06/07	Basic calibration and GOL messaging are added
 * Anton Alkhimenok     02/05/08    new PICtail support, portrait orientation is added
 * Sean Justice         02/07/08    PIC32 support
 *****************************************************************************/

#include "MainDemo.h"

//////////////////////// LOCAL PROTOTYPES ////////////////////////////
void TouchGetCalPoints(WORD* ax, WORD* ay);

//////////////////////// GLOBAL VARIABLES ////////////////////////////
#define  DEBOUNCE           2
#define  TOUCH_DELAY_PRESS  1000/DEBOUNCE	// changed by Spring.chen
#define  TOUCH_DELAY_MOVE   100/DEBOUNCE	// changed by Spring.chen
#define  CAL_DELTA          0x60
#define  PEN_UP             0
#define  PEN_DOWN           1

// Max/Min ADC values for each derection
volatile WORD _calXMin = XMINCAL;
volatile WORD _calXMax = XMAXCAL;
volatile WORD _calYMin = YMINCAL;
volatile WORD _calYMax = YMAXCAL;

volatile WORD _calAn;
volatile WORD _calBn;
volatile WORD _calCn;
volatile WORD _calDn;
volatile WORD _calEn;
volatile WORD _calFn;
volatile WORD _calDivider;


// Current ADC values for X and Y channels
volatile SHORT adcX     = -1;
volatile SHORT adcY     = -1;
volatile SHORT adcPot   = 0;
volatile char  debounce_counter = 0;
volatile char  calibration = 0;
volatile unsigned int  PenStatus = 0;
volatile POINT Touchstr[3] = {
                               { 0, 0 },
					           { 0, 0 },
                               { 0, 0 }
	                          } ;

volatile POINT Screenstr[3] = { 
	                            { 120, 288},                       
					            { 216, 160},
                                { 24, 32}
	                          } ;
volatile  MATRIX  MATRIXstr;
extern WORD g_rtc_counter;	//add by Spring.Chen
extern WORD g_standby_time;	//add by Spring.Chen


WriteCharToADS7843(unsigned char  data) ;//写命令
unsigned int ReadFromCharFromADS7843() ;// 读数据

delay7843(unsigned int i)
{
while(i--);
}
void ADS7843_start()
{
ADS7843_CS_TRIS = 0;
ADS7843_DCLK=0;
Nop();
ADS7843_CS=1;
Nop();

Nop();
ADS7843_DIN=1;
Nop();
ADS7843_DCLK=1;
Nop();
ADS7843_CS=0;
}
void InitializeINT2()
{
	ADS7843_INT_TRIS = 1;
	AD1PCFGbits.PCFG8= 1;
 //  _INT2IE = 0;
   _INT2EP = 1;
   _INT2IF = 0;
   _INT2IP = 5;
   _INT2IE = 1;
   Nop();
}
void inti_time3(void)
{    
    // Initialize Timer3
    TMR3 = 0;
    PR3 = 8000;//800;
    T3CONbits.TCKPS = 2;         //Set prescale to 1:8
    IFS0bits.T3IF = 0;           //Clear flag
    IEC0bits.T3IE = 0;           //Enable interrupt
    T3CONbits.TON = 0;           //Run timer   	
}
void InitializeADS7843(void)
{  
unsigned int tempx,tempy,x[8],y[8];
unsigned char i;

   AD1PCFGbits.PCFG10= 1;
    Nop();
   AD1PCFGbits.PCFG11= 1;
    Nop();
   AD1PCFGbits.PCFG12= 1;
    Nop();
   AD1PCFGbits.PCFG13= 1;
    Nop();
   AD1PCFGbits.PCFG14= 1;
    Nop();
   AD1PCFGbits.PCFG15= 1;
   ADS7843_CS_TRIS = 0;
   Nop();
  // InitializeSPI();
   ADS7843_DCLK_TRIS = 0;
   Nop();
   ADS7843_DOUT_TRIS= 1;
   Nop();
   ADS7843_DIN_TRIS = 0;
   Nop();
   ADS7843_BUSY_TRIS= 1;
   InitializeINT2();
   ADS7843_CS=1;
    Nop();
	
		ADS7843_start();
		// while(ADS7843_BUSY); //如果BUSY信号不好使可以删除不用
		delay7843(20);
		WriteCharToADS7843(0x90); //送控制字 10010000 即用差分方式读X坐标 详细请见有关资料
		 while(ADS7843_BUSY); //如果BUSY信号不好使可以删除不用
		delay7843(2);
		ADS7843_DCLK=1; 
		delay7843(2);
		Nop();
		Nop();
		ADS7843_DCLK=0; 
		Nop();
		Nop();
		y[i] = ReadFromCharFromADS7843(); //读X轴坐标
		Nop();
		Nop();
		//if( (adcY<(YMINCAL-CAL_DELTA)) ||(adcY>(YMAXCAL+CAL_DELTA)) )
		     // adcY=-1;
		    				 
		while(ADS7843_BUSY);
		Nop();
		WriteCharToADS7843(0xD0); //送控制字 11010000 即用差分方式读Y坐标 详细请见有关资料
		ADS7843_DCLK=1; 
		delay7843(2);
		Nop();
		Nop();
		ADS7843_DCLK=0; 
		Nop();
		Nop();
		x[i] = ReadFromCharFromADS7843(); //读Y轴坐标
		//if( (adcX<(XMINCAL-CAL_DELTA)) ||(adcX>(XMAXCAL+CAL_DELTA)) )
		      //adcX=-1;
		ADS7843_CS = 1;
		delay7843(100);

}

WriteCharToADS7843(unsigned char  data) //写命令
{
unsigned char count=0;
ADS7843_DCLK_TRIS=0;
ADS7843_DOUT_TRIS= 1;
ADS7843_DCLK=0;
for(count=0;count<8;count++)
{

if(data&0x80)
{
ADS7843_DIN=1;
Nop();
}
else
{
ADS7843_DIN=0;	
Nop();
}
data<<=1;
Nop();
ADS7843_DCLK=0;
delay7843(4);
Nop();
Nop();
ADS7843_DCLK=1; 

Nop();
}
}


unsigned int ReadFromCharFromADS7843() // 读数据
{
unsigned char count=0;
unsigned int data=0;
ADS7843_DCLK_TRIS = 0;
Nop();
ADS7843_DOUT_TRIS= 1;
Nop();
ADS7843_DIN_TRIS = 0;
for(count=0;count<12;count++)
{
data<<=1;
ADS7843_DCLK=1; 
delay7843(4);
Nop(); //下降沿有效
Nop();
Nop();
ADS7843_DCLK=0; 
Nop();
Nop();
Nop();
if(ADS7843_DOUT) data++;
}
return(data);
}
/*********************************************************************
* Function: ADC7843 INT1ISR
*
* PreCondition: none
*
* Input: none
*
* Output: none
*
* Side Effects: none
*
* Overview: ADC ISR processes result of conversions.
*           If 2 conversions are equal the result is stable and it can be
*           used for position calculation.
*
* Note: none
*
********************************************************************/
unsigned char FlagTouchBit=0x00;
void __attribute__ ((__interrupt__)) _INT2Interrupt(void) //外部中断1 用来接受键盘发来的数据
{
    
	T3CONbits.TON = 1;
	IEC0bits.T3IE = 1;
	IFS0bits.T3IF = 0;
	_INT2IF = 0;
    _INT2IE = 0;

}
void __attribute__((interrupt, shadow, no_auto_psv)) _T3Interrupt(void)
{

unsigned int tempx,tempy,x[8],y[8],avx,avy;
unsigned char i;
IFS0bits.T3IF = 0;	
DISICNT = 0x3FFF;
//delay7843(800); //中断后延时以消除抖动，使得采样数据更准确

if(!ADS7843_INT)
{
	for(i=0;i<8;i++)
		{
		ADS7843_start();
		// while(ADS7843_BUSY); //如果BUSY信号不好使可以删除不用
		delay7843(20);
		WriteCharToADS7843(0x90); //送控制字 10010000 即用差分方式读X坐标 详细请见有关资料
		 while(ADS7843_BUSY); //如果BUSY信号不好使可以删除不用
		delay7843(2);
		ADS7843_DCLK=1; 
		delay7843(2);
		Nop();
		Nop();
		ADS7843_DCLK=0; 
		Nop();
		Nop();
		y[i] = ReadFromCharFromADS7843(); //读X轴坐标
		Nop();
		Nop();
		//if( (adcY<(YMINCAL-CAL_DELTA)) ||(adcY>(YMAXCAL+CAL_DELTA)) )
		     // adcY=-1;
		    				 
		while(ADS7843_BUSY);
		Nop();
		WriteCharToADS7843(0xD0); //送控制字 11010000 即用差分方式读Y坐标 详细请见有关资料
		ADS7843_DCLK=1; 
		delay7843(2);
		Nop();
		Nop();
		ADS7843_DCLK=0; 
		Nop();
		Nop();
		x[i] = ReadFromCharFromADS7843(); //读Y轴坐标
		//if( (adcX<(XMINCAL-CAL_DELTA)) ||(adcX>(XMAXCAL+CAL_DELTA)) )
		      //adcX=-1;
		ADS7843_CS = 1;
		delay7843(100);
		}
	avx=0x00;
	avy=0x00;
	for(i=0;i<8;i++)
		{
		avx=avx+x[i];
		avy=avy+y[i];
		}
	adcX=avx/8;
	adcY=avy/8;
	FlagTouchBit=0xaa;

}
else 
{

	T3CONbits.TON = 0;
	IEC0bits.T3IE = 0;
	IFS0bits.T3IF = 0;
	_INT2IF = 0;
    _INT2IE = 1;

}
//delay7843(500);
DISICNT = 0x0000;
//_INT2IF = 0;



}

/*********************************************************************
* Function: SHORT TouchGetX()
*
* PreCondition: none
*
* Input: none
*
* Output: x coordinate
*
* Side Effects: none
*
* Overview: returns x coordinate if touch screen is pressed
*           and -1 if not
*
* Note: none
*
********************************************************************/
SHORT TouchGetX(){
long result;


    result = ADCGetX();


    if(result>=0){
        result = (GetMaxX()*(result - _calXMin))/(_calXMax - _calXMin);

    }
    return result;
}

/*********************************************************************
* Function: SHORT TouchGetY()
*
* PreCondition: none
*
* Input: none
*
* Output: y coordinate
*
* Side Effects: none
*
* Overview: returns y coordinate if touch screen is pressed
*           and -1 if not
*
* Note: none
*
********************************************************************/
SHORT TouchGetY(){
long result;


    result = ADCGetY();


    if(result>=0){

        result = GetMaxY()-(GetMaxY()*(result- _calYMin))/(_calYMax - _calYMin);

    }
    
    return result;
}

/*********************************************************************
* Function: void TouchGetMsg(GOL_MSG* pMsg)
*
* PreCondition: none
*
* Input: pointer to the message structure to be populated
*
* Output: none
*
* Side Effects: none
*
* Overview: populates GOL message structure
*
* Note: none
*
********************************************************************/
void TouchGetMsg(GOL_MSG* pMsg){

static SHORT prevX = -1;
static SHORT prevY = -1;
SHORT x,y,_x,_y;

   Nop();
   
	if(!ADS7843_INT){//if(FlagTouchBit==0xaa){
	   Nop();	
    x = TouchGetX();
	   Nop();
	y = TouchGetY();
	   Nop();
	FlagTouchBit=0x00;
	//adcX=-1;
	//adcY=-1;
	_x = ( (MATRIXstr.An*x)+(MATRIXstr.Bn*y)+MATRIXstr.Cn)/ MATRIXstr.Divider ;                                                                              
    _y = ( (MATRIXstr.Dn*x)+(MATRIXstr.En*y)+MATRIXstr.Fn)/ MATRIXstr.Divider ;
	  }
	else
		{
    Nop();
	x=-1;
	adcX=-1;
	Nop();
	y=-1;
	adcY=-1;
	_x=-1;
	_y=-1;
		}
	
 
                                                                              
    pMsg->type    = TYPE_TOUCHSCREEN;
    pMsg->uiEvent = EVENT_INVALID;

    if( x == -1 ){
        y = -1;
	   _x = -1;
	   _y = -1;
    }else{
        if( y == -1 )
          {  x = -1;
		   _x = -1;
		   _y = -1;
        	}
    }

    if( (prevX == _x) && (prevY == _y) )
        return;

    if( (prevX != -1) || (prevY != -1) ){

        if( (x != -1) && (y != -1) ){
            // Move
            pMsg->uiEvent = EVENT_MOVE;
        }else{
            // Released
            x=_x;
			y=_y;
            pMsg->uiEvent = EVENT_RELEASE;
            pMsg->param1 = prevX;
            pMsg->param2 = prevY;
            prevX = _x;
            prevY = _y;
            return;
        }

    }else{

        if( (x != -1) && (y != -1) ){
            // Pressed
            pMsg->uiEvent = EVENT_PRESS;

		    g_rtc_counter = 0;

            
        }else{
            // No message
            pMsg->uiEvent = EVENT_INVALID;
        }

    }
    x=_x;
    y=_y;
    pMsg->param1 = x;
    pMsg->param2 = y;
    prevX = x;
    prevY = y;


}

/*********************************************************************
* Function: void TouchStoreCalibration(void)
*
* PreCondition: EEPROMInit() must be called before
*
* Input: none
*
* Output: none
*
* Side Effects: none
*
* Overview: stores calibration parameters into EEPROM
*
* Note: none
*
********************************************************************/
void TouchStoreCalibration(void){
    EEPROMWriteWord(_calXMin, EEPROM_XMIN);
    EEPROMWriteWord(_calXMax, EEPROM_XMAX);
    EEPROMWriteWord(_calYMin, EEPROM_YMIN);
    EEPROMWriteWord(_calYMax, EEPROM_YMAX);
	EEPROMWriteWord(GRAPHICS_LIBRARY_VERSION,EEPROM_VERSION);
	_calAn=(WORD)((MATRIXstr.An)&0x0000FFFF);
	EEPROMWriteWord(_calAn, EEPROM_An);//78a0
	_calAn=(WORD)((MATRIXstr.An)>>16);
	EEPROMWriteWord(_calAn, EEPROM_An+2);//ffff
	
	_calBn=(WORD)((MATRIXstr.Bn)&0x0000FFFF);
	EEPROMWriteWord(_calBn, EEPROM_Bn);//fbe0
	_calBn=(WORD)((MATRIXstr.Bn)>>16);
	EEPROMWriteWord(_calBn, EEPROM_Bn+2);//ffff
	
	_calCn=(WORD)((MATRIXstr.Cn)&0x0000FFFF);
	EEPROMWriteWord(_calCn, EEPROM_Cn);
	_calCn=(WORD)((MATRIXstr.Cn)>>16);
	EEPROMWriteWord(_calCn, EEPROM_Cn+2);
	
	_calDn=(WORD)((MATRIXstr.Dn)&0x0000FFFF);
	EEPROMWriteWord(_calDn, EEPROM_Dn);
	_calDn=(WORD)((MATRIXstr.Dn)>>16);
	EEPROMWriteWord(_calDn, EEPROM_Dn+2);
	
	_calEn=(WORD)((MATRIXstr.En)&0x0000FFFF);
	EEPROMWriteWord(_calEn, EEPROM_En);
	_calEn=(WORD)((MATRIXstr.En)>>16);
	EEPROMWriteWord(_calEn, EEPROM_En+2);
	
	_calFn=(WORD)((MATRIXstr.Fn)&0x0000FFFF);
	EEPROMWriteWord(_calFn, EEPROM_Fn);
	_calFn=(WORD)((MATRIXstr.Fn)>>16);
	EEPROMWriteWord(_calFn, EEPROM_Fn+2);
	
	_calDivider=(WORD)((MATRIXstr.Divider)&0x0000FFFF);
	EEPROMWriteWord(_calDivider, EEPROM_Divider);
	_calDivider=(WORD)((MATRIXstr.Divider)>>16);
	EEPROMWriteWord(_calDivider, EEPROM_Divider+2);
	
}

void TouchStoreCalibrationDefault(void){
    EEPROMWriteWord(XMINCAL, EEPROM_XMIN);
    EEPROMWriteWord(XMAXCAL, EEPROM_XMAX);
    EEPROMWriteWord(YMINCAL, EEPROM_YMIN);
    EEPROMWriteWord(YMAXCAL, EEPROM_YMAX);
}

/*********************************************************************
* Function: void TouchLoadCalibration(void)
*
* PreCondition: EEPROMInit() must be called before
*
* Input: none
*
* Output: none
*
* Side Effects: none
*
* Overview: loads calibration parameters from EEPROM
*
* Note: none
*
********************************************************************/
void TouchLoadCalibration(void){
    _calXMin = EEPROMReadWord(EEPROM_XMIN);
    _calXMax = EEPROMReadWord(EEPROM_XMAX);
    _calYMin = EEPROMReadWord(EEPROM_YMIN);
    _calYMax = EEPROMReadWord(EEPROM_YMAX);
   g_standby_time = EEPROMReadWord(BackLightTimeAddress);

	MATRIXstr.An=0;
	MATRIXstr.Bn=0;
	MATRIXstr.Cn=0;
	MATRIXstr.Dn=0;
	MATRIXstr.En=0;
	MATRIXstr.Fn=0;
	MATRIXstr.Divider=0;
	
	MATRIXstr.An = EEPROMReadWord(EEPROM_An+2);
	MATRIXstr.An = MATRIXstr.An<<16;
	MATRIXstr.An += EEPROMReadWord(EEPROM_An);
	
    MATRIXstr.Bn = EEPROMReadWord(EEPROM_Bn+2);
	MATRIXstr.Bn = MATRIXstr.Bn<<16;
	MATRIXstr.Bn += EEPROMReadWord(EEPROM_Bn);
	
	MATRIXstr.Cn = EEPROMReadWord(EEPROM_Cn+2);
    MATRIXstr.Cn = MATRIXstr.Cn<<16; 
	MATRIXstr.Cn += EEPROMReadWord(EEPROM_Cn);

	MATRIXstr.Dn = EEPROMReadWord(EEPROM_Dn+2);
    MATRIXstr.Dn = MATRIXstr.Dn<<16;
	MATRIXstr.Dn += EEPROMReadWord(EEPROM_Dn);
	
	MATRIXstr.En = EEPROMReadWord(EEPROM_En+2);
    MATRIXstr.En = MATRIXstr.En<<16;
	MATRIXstr.En += EEPROMReadWord(EEPROM_En);
		
	MATRIXstr.Fn = EEPROMReadWord(EEPROM_Fn+2);
    MATRIXstr.Fn = MATRIXstr.Fn<<16;
	MATRIXstr.Fn += EEPROMReadWord(EEPROM_Fn);
	
	MATRIXstr.Divider = EEPROMReadWord(EEPROM_Divider+2);
    MATRIXstr.Divider = MATRIXstr.Divider<<16; 
	MATRIXstr.Divider += EEPROMReadWord(EEPROM_Divider);
	
}

void TouchLoadCalibrationDefault(void){
    _calXMin = XMINCAL;
    _calXMax = XMAXCAL;
    _calYMin = YMINCAL;
    _calYMax = YMAXCAL;
   g_standby_time = EEPROMReadWord(RButtonAddress+2);
}

/*********************************************************************
* Function:  void TouchCalibration()
*
* PreCondition: InitGraph() must be called before
*
* Input: none
*
* Output: none
*
* Side Effects: none
*
* Overview: calibrates touch screen
*
* Note: none
*
********************************************************************/
void TouchCalibration(){
SHORT counter;
SHORT x,y;
WORD  ax[3],ay[3];
// 重点。你可以校准触摸屏
 XCHAR Important[] 	 = {'I','M','P','O','R','T','A','N','T', 0x0000};
//使得触笔更加准确
XCHAR Instruction1[] = {'N','o','w',0x0020,'t','o','u','c','h',0x0020,'s','c','r','e','e','n',0x0020,'c','a','l','i','b','r','a','t','i','o','n',0x0000};
//请点击画面开始进行校准
 XCHAR Instruction2[] = {'w','i','l','l',0x0020,'b','e', 0x0020,'p','e','r','f','o','m','e','d','.','T','o','u','c','h',0x0020,'p','o','i','n','t','s', 0x0000};
 XCHAR Instruction3[] = {'E','X','A','C','T','L','Y',0x0020,'a','t',0x0020,'t','h','e',0x0020,'p','o','s','i','t','i','o','n','s', 0x0000};
 XCHAR Instruction4[] = {'s','h','o','w','n',0x0020,'b','y',0x0020,'a','r','r','o','w','s','.', 0x0000};
//请点击触摸屏任意位置继续
 XCHAR CalibInstr5[]  = {'T','o','u','c','h',0x0020,'s','c','r','e','e','n',' ','t','o',0x0020,'c','o','n','t','i','n','u','e','.', 0x0000};

SHORT textHeight;

    // Set touch screen timing for calibration
    calibration = 1;
	
	SetFont((void*)&FONTMEDIUM);
    textHeight = GetTextHeight((void*)&FONTMEDIUM);

    SetColor(WHITE);
    ClearDevice();

    SetColor(BRIGHTRED);
    OutTextXY(0,0*textHeight,Important);
    SetColor(BLACK);
    OutTextXY(0,1*textHeight,Instruction1);
    OutTextXY(0,2*textHeight,Instruction2);
    OutTextXY(0,3*textHeight,Instruction3);
    OutTextXY(0,4*textHeight,Instruction4);
    SetColor(BRIGHTRED);
    OutTextXY(0,6*textHeight,CalibInstr5);

    // Wait for touch
    do{
        x=ADCGetX(); y=ADCGetY();
    }while(ADS7843_INT==1);

    Beep();

    DelayMs(500);

    SetColor(WHITE);
    ClearDevice();

    SetColor(BRIGHTRED);

#ifdef USE_PORTRAIT

    Line(GetMaxX()-5,5,GetMaxX()-5,15);
    Line(GetMaxX()-4,5,GetMaxX()-4,15);
    Line(GetMaxX()-6,5,GetMaxX()-6,15);

    Line(GetMaxX()-5,5,GetMaxX()-15,5);
    Line(GetMaxX()-5,4,GetMaxX()-15,4);
    Line(GetMaxX()-5,6,GetMaxX()-15,6);

    Line(GetMaxX()-5,6,GetMaxX()-15,16);
    Line(GetMaxX()-5,4,GetMaxX()-15,14);
    Line(GetMaxX()-5,5,GetMaxX()-15,15);

#else

    Line(5,5,5,15);
    Line(4,5,4,15);
    Line(6,5,6,15);

    Line(5,5,15,5);
    Line(5,4,15,4);
    Line(5,6,15,6);

    Line(5,6,15,16);
    Line(5,4,15,14);
    Line(5,5,15,15);

#endif

    TouchGetCalPoints(ax, ay);

    for(counter=1; counter<3; counter++){
        if(_calXMax < ax[counter])
            _calXMax = ax[counter];

        if(_calYMin > ay[counter])
            _calYMin = ay[counter];

        if(_calYMax < ay[counter])
            _calYMax = ay[counter];

        if(_calXMin > ax[counter])
            _calXMin = ax[counter];
    }


    SetColor(WHITE);
    ClearDevice();

    SetColor(BRIGHTRED);

#ifdef USE_PORTRAIT

    Line(5,5,5,15);
    Line(4,5,4,15);
    Line(6,5,6,15);

    Line(5,5,15,5);
    Line(5,4,15,4);
    Line(5,6,15,6);

    Line(5,6,15,16);
    Line(5,4,15,14);
    Line(5,5,15,15);

#else

    Line(5,GetMaxY()-5,5,GetMaxY()-15);
    Line(4,GetMaxY()-5,4,GetMaxY()-15);
    Line(6,GetMaxY()-5,6,GetMaxY()-15);

    Line(5,GetMaxY()-5,15,GetMaxY()-5);
    Line(5,GetMaxY()-4,15,GetMaxY()-4);
    Line(5,GetMaxY()-6,15,GetMaxY()-6);

    Line(5,GetMaxY()-6,15,GetMaxY()-16);
    Line(5,GetMaxY()-4,15,GetMaxY()-14);
    Line(5,GetMaxY()-5,15,GetMaxY()-15);

#endif

    TouchGetCalPoints(ax, ay);

    for(counter=1; counter<3; counter++){
        if(_calXMax < ax[counter])
            _calXMax = ax[counter];

        if(_calYMin > ay[counter])
            _calYMin = ay[counter];

        if(_calYMax < ay[counter])
            _calYMax = ay[counter];

        if(_calXMin > ax[counter])
            _calXMin = ax[counter];
    }


    SetColor(WHITE);
    ClearDevice();

    SetColor(BRIGHTRED);    


#ifdef USE_PORTRAIT

    Line(GetMaxX()/2-5,GetMaxY()-5,GetMaxX()/2-5,GetMaxY()-15);
    Line(GetMaxX()/2-4,GetMaxY()-5,GetMaxX()/2-4,GetMaxY()-15);
    Line(GetMaxX()/2-6,GetMaxY()-5,GetMaxX()/2-6,GetMaxY()-15);

    Line(GetMaxX()/2-5,GetMaxY()-5,GetMaxX()/2-15,GetMaxY()-5);
    Line(GetMaxX()/2-5,GetMaxY()-4,GetMaxX()/2-15,GetMaxY()-4);
    Line(GetMaxX()/2-5,GetMaxY()-6,GetMaxX()/2-15,GetMaxY()-6);

    Line(GetMaxX()/2-5,GetMaxY()-6,GetMaxX()/2-15,GetMaxY()-16);
    Line(GetMaxX()/2-5,GetMaxY()-4,GetMaxX()/2-15,GetMaxY()-14);
    Line(GetMaxX()/2-5,GetMaxY()-5,GetMaxX()/2-15,GetMaxY()-15);

#else

    Line(GetMaxX()-5,GetMaxY()/2-5,GetMaxX()-5,GetMaxY()/2-15);
    Line(GetMaxX()-4,GetMaxY()/2-5,GetMaxX()-4,GetMaxY()/2-15);
    Line(GetMaxX()-6,GetMaxY()/2-5,GetMaxX()-6,GetMaxY()/2-15);

    Line(GetMaxX()-5,GetMaxY()/2-5,GetMaxX()-15,GetMaxY()/2-5);
    Line(GetMaxX()-5,GetMaxY()/2-4,GetMaxX()-15,GetMaxY()/2-4);
    Line(GetMaxX()-5,GetMaxY()/2-6,GetMaxX()-15,GetMaxY()/2-6);

    Line(GetMaxX()-5,GetMaxY()/2-6,GetMaxX()-15,GetMaxY()/2-16);
    Line(GetMaxX()-5,GetMaxY()/2-4,GetMaxX()-15,GetMaxY()/2-14);
    Line(GetMaxX()-5,GetMaxY()/2-5,GetMaxX()-15,GetMaxY()/2-15);

#endif

    TouchGetCalPoints(ax, ay);

    for(counter=1; counter<3; counter++){
        if(_calXMax < ax[counter])
            _calXMax = ax[counter];

        if(_calYMin > ay[counter])
            _calYMin = ay[counter];

        if(_calYMax < ay[counter])
            _calYMax = ay[counter];

        if(_calXMin > ax[counter])
            _calXMin = ax[counter];
    }

   SetColor(WHITE);
    ClearDevice();

    SetColor(BRIGHTRED);

    Line(23,26,23,38);
    Line(24,26,24,38);
    Line(25,26,25,38);

    Line(18,31,30,31);
    Line(18,32,30,32);
    Line(18,33,30,33);

TouchGetCalPoints(ax, ay);	

for(counter=1; counter<3; counter++){
	 Touchstr[2].x+=ax[counter];
	 Touchstr[2].y+=ay[counter];
 	}
 Touchstr[2].x=Touchstr[2].x/2;
 Touchstr[2].x =  (GetMaxX()*(Touchstr[2].x - _calXMin))/(_calXMax - _calXMin);
 Touchstr[2].y=Touchstr[2].y/2;
 Touchstr[2].y =  GetMaxY()-(GetMaxY()*(Touchstr[2].y- _calYMin))/(_calYMax - _calYMin);
 

    SetColor(WHITE);
    ClearDevice();

    SetColor(BRIGHTRED);

    Line(210,159,222,159);
    Line(210,160,222,160);
    Line(210,161,222,161);

    Line(215,154,215,166);
    Line(216,154,216,166);
    Line(217,154,217,166);

TouchGetCalPoints(ax, ay);	

 for(counter=1; counter<3; counter++){
	 Touchstr[1].x+=ax[counter];
	 Touchstr[1].y+=ay[counter];
 	}
 Touchstr[1].x=Touchstr[1].x/2;
 Touchstr[1].x =  (GetMaxX()*(Touchstr[1].x - _calXMin))/(_calXMax - _calXMin);
 Touchstr[1].y=Touchstr[1].y/2;
 Touchstr[1].y =  GetMaxY()-(GetMaxY()*(Touchstr[1].y- _calYMin))/(_calYMax - _calYMin);

    SetColor(WHITE);
    ClearDevice();

    SetColor(BRIGHTRED);

    Line(119,282,119,294);
    Line(120,282,120,294);
    Line(121,282,121,294);

    Line(114,287,126,287);
    Line(114,288,126,288);
    Line(114,289,126,289);

TouchGetCalPoints(ax, ay);	

for(counter=1; counter<3; counter++){
	 Touchstr[0].x+=ax[counter];
	 Touchstr[0].y+=ay[counter];
 	}
 Touchstr[0].x=Touchstr[0].x/2;
 Touchstr[0].x =  (GetMaxX()*(Touchstr[0].x - _calXMin))/(_calXMax - _calXMin);
 Touchstr[0].y=Touchstr[0].y/2;
 Touchstr[0].y =  GetMaxY()-(GetMaxY()*(Touchstr[0].y- _calYMin))/(_calYMax - _calYMin);

setCalibrationMatrix( &Screenstr[0],&Touchstr[0],&MATRIXstr);
                          
                          
#ifndef	USE_SSD1289
//    SetFont((void*)&GOLFontDefault);
//    textHeight = GetTextHeight((void*)&GOLFontDefault);
#else
	SetFont((void*)&FONTMEDIUM);
    textHeight = GetTextHeight((void*)&FONTMEDIUM);
#endif

#ifdef	USE_MULTIBYTECHAR
    SetColor(WHITE);
    ClearDevice();

    SetColor(BRIGHTRED);
    OutTextXY(10,6*textHeight,CalibInstr5);

#else
    SetColor(WHITE);
    ClearDevice();

    SetColor(BLACK);
    OutTextXY(10,1*textHeight,"Hold S3 button and");
    OutTextXY(10,2*textHeight,"press MCLR reset(S1)");
    OutTextXY(10,3*textHeight,"to REPEAT the calibration");
    OutTextXY(10,4*textHeight,"procedure.");
    SetColor(BRIGHTRED);
    OutTextXY(10,6*textHeight,"Touch screen to continue.");
#endif

    // Wait for touch
    do{
        x=ADCGetX(); y=ADCGetY();
    }while(ADS7843_INT==1);

    Beep();

    DelayMs(500);

    SetColor(BLACK);
    ClearDevice();

    // Set touch screen timing for work mode
    calibration = 0;
}

/*********************************************************************
* Function: void TouchGetCalPoints(WORD* ax, WORD* ay)
*
* PreCondition: InitGraph() must be called before
*
* Input: ax - pointer to array receiving 3 X touch positions
*        ay - pointer to array receiving 3 Y touch positions
*
* Output: none
*
* Side Effects: none
*
* Overview: gets values for 3 touches
*
* Note: none
*
********************************************************************/
void TouchGetCalPoints(WORD* ax, WORD* ay){
#ifdef	USE_MULTIBYTECHAR
XCHAR calStr[] = {'C','A','L','I','B','R','A','T','I','O','N',0x00};
XCHAR calTouchLeft[] = {' ',' ','3',' ','t','o','u','c','h','e','s',' ','l','e','f','t',0x00};
#else
static const char calStr[] = "CALIBRATION";
char calTouchLeft[] = "3 touches left";
#endif

SHORT counter;
SHORT x,y;

    SetFont((void*)&GOLFontDefault);

    SetColor(BRIGHTRED);

#ifdef	USE_MULTIBYTECHAR
    OutTextXY((GetMaxX()-GetTextWidth((XCHAR*)calStr,(void*)&GOLFontDefault))>>1,
              (GetMaxY()-GetTextHeight((void*)&GOLFontDefault))>>1,
              (XCHAR*)calStr);
#else
    OutTextXY((GetMaxX()-GetTextWidth((char*)calStr,(void*)&GOLFontDefault))>>1,
              (GetMaxY()-GetTextHeight((void*)&GOLFontDefault))>>1,
              (char*)calStr);
#endif

    for(counter=0; counter<3; counter++){

        SetColor(BRIGHTRED);

        calTouchLeft[2] = '3' - counter;

        OutTextXY((GetMaxX()-GetTextWidth(calTouchLeft,(void*)&GOLFontDefault))>>1,
                  (GetMaxY()+GetTextHeight((void*)&GOLFontDefault))>>1,
                   calTouchLeft);

        // Wait for press
        do{
            x=ADCGetX();
			y=ADCGetY();
        }while(ADS7843_INT==1);
            x=ADCGetX();
			y=ADCGetY();
        Beep();

        *(ax+counter) = x; *(ay+counter) = y;
     
        // Wait for release
        do{
           Nop();
        }while(ADS7843_INT==0);

        SetColor(WHITE);

        OutTextXY((GetMaxX()-GetTextWidth(calTouchLeft,(void*)&GOLFontDefault))>>1,
                  (GetMaxY()+GetTextHeight((void*)&GOLFontDefault))>>1,
                   calTouchLeft);

        DelayMs(500);
    }
}
int setCalibrationMatrix( POINT * displayPtr,
                          POINT * touchPtr,
                          MATRIX * matrixPtr)
{
    int  retValue = OK ;
    matrixPtr->Divider = ((touchPtr[0].x - touchPtr[2].x) * (touchPtr[1].y - touchPtr[2].y)) - 
                         ((touchPtr[1].x - touchPtr[2].x) * (touchPtr[0].y - touchPtr[2].y)) ;

    if( matrixPtr->Divider == 0 )
    {
        retValue = NOT_OK ;
    }
    else
    {
        matrixPtr->An = ((displayPtr[0].x - displayPtr[2].x) * (touchPtr[1].y - touchPtr[2].y)) - 
                        ((displayPtr[1].x - displayPtr[2].x) * (touchPtr[0].y - touchPtr[2].y)) ;

        matrixPtr->Bn = ((touchPtr[0].x - touchPtr[2].x) * (displayPtr[1].x - displayPtr[2].x)) - 
                        ((displayPtr[0].x - displayPtr[2].x) * (touchPtr[1].x - touchPtr[2].x)) ;

        matrixPtr->Cn = (touchPtr[2].x * displayPtr[1].x - touchPtr[1].x * displayPtr[2].x) * touchPtr[0].y +
                        (touchPtr[0].x * displayPtr[2].x - touchPtr[2].x * displayPtr[0].x) * touchPtr[1].y +
                        (touchPtr[1].x * displayPtr[0].x - touchPtr[0].x * displayPtr[1].x) * touchPtr[2].y ;

        matrixPtr->Dn = ((displayPtr[0].y - displayPtr[2].y) * (touchPtr[1].y - touchPtr[2].y)) - 
                        ((displayPtr[1].y - displayPtr[2].y) * (touchPtr[0].y - touchPtr[2].y)) ;
    
        matrixPtr->En = ((touchPtr[0].x - touchPtr[2].x) * (displayPtr[1].y - displayPtr[2].y)) - 
                        ((displayPtr[0].y - displayPtr[2].y) * (touchPtr[1].x - touchPtr[2].x)) ;

        matrixPtr->Fn = (touchPtr[2].x * displayPtr[1].y - touchPtr[1].x * displayPtr[2].y) * touchPtr[0].y +
                        (touchPtr[0].x * displayPtr[2].y - touchPtr[2].x * displayPtr[0].y) * touchPtr[1].y +
                        (touchPtr[1].x * displayPtr[0].y - touchPtr[0].x * displayPtr[1].y) * touchPtr[2].y ;
    }
 
    return( retValue ) ;
} /* end of setCalibrationMatrix() */

int getDisplayPoint( POINT * displayPtr,
                     POINT * screenPtr,
                     MATRIX * matrixPtr )
{
    int  retValue = OK ;
    if( matrixPtr->Divider != 0 )
    {
            /* Operation order is important since we are doing integer */
            /*  math. Make sure you add all terms together before      */
            /*  dividing, so that the remainder is not rounded off     */
            /*  prematurely.                                           */
        displayPtr->x = ( (matrixPtr->An * screenPtr->x) + 
                          (matrixPtr->Bn * screenPtr->y) + 
                           matrixPtr->Cn 
                        ) / matrixPtr->Divider ;
        displayPtr->y = ( (matrixPtr->Dn * screenPtr->x) + 
                          (matrixPtr->En * screenPtr->y) + 
                           matrixPtr->Fn 
                        ) / matrixPtr->Divider ;
    }
    else
    {
        retValue = NOT_OK ;
    }
    return( retValue ) ;
} /* end of getDisplayPoint() */
