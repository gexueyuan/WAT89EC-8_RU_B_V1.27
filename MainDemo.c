/*****************************************************************************
 * Microchip Graphics Library Demo Application
 * This program shows how to use the Graphics Objects Layer.
 *****************************************************************************
 * FileName:        MainDemo.c
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
 * Anton Alkhimenok		30/05/07	...
 * Paolo Tamayo         09/20/07    Round buttons demo screen is added
 * Paolo Tamayo			10/05/07    Modified navigation, added Meter, Dial, 
 *									RTCC demos
 * Paolo Tamayo			01/21/08    Updated for v1.1
 * Sean Justice         02/07/08    PIC32 support 
 *****************************************************************************/

#include "MainDemo.h"
#include "MainDemoStrings.h"
/** INCLUDES *******************************************************/
#include "GenericTypeDefs.h"
//#include "Compiler.h"
/** CONFIGURATION **************************************************/
// Configuration bits

_CONFIG1( JTAGEN_OFF & GCP_OFF & GWRP_OFF & COE_OFF & FWDTEN_OFF & ICS_PGx1) 
_CONFIG2( 0xF7FF & IESO_OFF & FCKSM_CSDCMD & OSCIOFNC_OFF & POSCMOD_HS & FNOSC_PRIPLL & PLLDIV_DIV2 & IOL1WAY_ON)// IOL1WAY_OFF

/** V A R I A B L E S ********************************************************/
#pragma udata

unsigned char Calibrate_Value[28];
unsigned int i_chk_paper;
BOOL stringPrinted;

//usb
unsigned char ReceivedDataBuffer[64];
unsigned char ToSendDataBuffer[64];
USB_HANDLE USBOutHandle = 0;
USB_HANDLE USBInHandle = 0;
BOOL blinkStatusValid = TRUE;

/** PRIVATE PROTOTYPES *********************************************/
void BlinkUSBStatus(void);
BOOL Switch2IsPressed(void);
BOOL Switch3IsPressed(void);
static void InitializeSystem(void);
void ProcessIO(void);
void UserInit(void);
void YourHighPriorityISRCode();
void YourLowPriorityISRCode();
WORD_VAL ReadPOT(void);


/** P R I V A T E  P R O T O T Y P E S ***************************************/
void testing(void);
//static void InitializeSystem(void);
//void USBDeviceTasks(void);

void YourHighPriorityISRCode(void);
void YourLowPriorityISRCode(void);

/** VECTOR REMAPPING ***********************************************/

void __attribute__((interrupt, shadow, no_auto_psv)) _T1Interrupt(void);
void  __attribute__((interrupt, shadow, no_auto_psv)) _T2Interrupt(void);
void __attribute__((interrupt, shadow, no_auto_psv)) _T3Interrupt(void);
//void __attribute__((interrupt, shadow, no_auto_psv)) _ADC1Interrupt(void);
void __attribute__((interrupt, shadow, no_auto_psv)) _T4Interrupt(void);
void __attribute__((interrupt, shadow, no_auto_psv)) _T5Interrupt(void);
void __attribute__((interrupt, shadow, no_auto_psv)) _U4RXInterrupt(void);
void __attribute__((interrupt, shadow, no_auto_psv)) _U4TXInterrupt(void);

void __attribute__ ((address(0x2800))) ISRTable(){
	asm("reset"); //reset instruction to prevent runaway code
	asm("goto %0"::"i"(&_T1Interrupt));  //T2Interrupt's address
	asm("goto %0"::"i"(&_T2Interrupt));  //T2Interrupt's address
	asm("goto %0"::"i"(&_T3Interrupt));  //T2Interrupt's address
//	asm("goto %0"::"i"(&_ADC1Interrupt));  //T2Interrupt's address
	asm("goto %0"::"i"(&_T4Interrupt));  //T2Interrupt's address
	asm("goto %0"::"i"(&_T5Interrupt));  //T2Interrupt's address
	asm("goto %0"::"i"(&_U4RXInterrupt));  //T2Interrupt's address
	asm("goto %0"::"i"(&_U4TXInterrupt));  //T2Interrupt's addre
}

#if defined(__18CXX)
	
	#if defined(PROGRAMMABLE_WITH_USB_HID_BOOTLOADER)
		#define REMAPPED_RESET_VECTOR_ADDRESS			0x1000
		#define REMAPPED_HIGH_INTERRUPT_VECTOR_ADDRESS	0x1008
		#define REMAPPED_LOW_INTERRUPT_VECTOR_ADDRESS	0x1018
	#elif defined(PROGRAMMABLE_WITH_USB_MCHPUSB_BOOTLOADER)	
		#define REMAPPED_RESET_VECTOR_ADDRESS			0x800
		#define REMAPPED_HIGH_INTERRUPT_VECTOR_ADDRESS	0x808
		#define REMAPPED_LOW_INTERRUPT_VECTOR_ADDRESS	0x818
	#else	
		#define REMAPPED_RESET_VECTOR_ADDRESS			0x00
		#define REMAPPED_HIGH_INTERRUPT_VECTOR_ADDRESS	0x08
		#define REMAPPED_LOW_INTERRUPT_VECTOR_ADDRESS	0x18
	#endif
	
	#if defined(PROGRAMMABLE_WITH_USB_HID_BOOTLOADER)||defined(PROGRAMMABLE_WITH_USB_MCHPUSB_BOOTLOADER)
	extern void _startup (void);        // See c018i.c in your C18 compiler dir
	#pragma code REMAPPED_RESET_VECTOR = REMAPPED_RESET_VECTOR_ADDRESS
	void _reset (void)
	{
	    _asm goto _startup _endasm
	}
	#endif
	#pragma code REMAPPED_HIGH_INTERRUPT_VECTOR = REMAPPED_HIGH_INTERRUPT_VECTOR_ADDRESS
	void Remapped_High_ISR (void)
	{
	     _asm goto YourHighPriorityISRCode _endasm
	}
	#pragma code REMAPPED_LOW_INTERRUPT_VECTOR = REMAPPED_LOW_INTERRUPT_VECTOR_ADDRESS
	void Remapped_Low_ISR (void)
	{
	     _asm goto YourLowPriorityISRCode _endasm
	}
	
	#if defined(PROGRAMMABLE_WITH_USB_HID_BOOTLOADER)||defined(PROGRAMMABLE_WITH_USB_MCHPUSB_BOOTLOADER)
	#pragma code HIGH_INTERRUPT_VECTOR = 0x08
	void High_ISR (void)
	{
	     _asm goto REMAPPED_HIGH_INTERRUPT_VECTOR_ADDRESS _endasm
	}
	#pragma code LOW_INTERRUPT_VECTOR = 0x18
	void Low_ISR (void)
	{
	     _asm goto REMAPPED_LOW_INTERRUPT_VECTOR_ADDRESS _endasm
	}
	#endif	//end of "#if defined(PROGRAMMABLE_WITH_USB_HID_BOOTLOADER)||defined(PROGRAMMABLE_WITH_USB_LEGACY_CUSTOM_CLASS_BOOTLOADER)"

	#pragma code
	
	
	//These are your actual interrupt handling routines.
	#pragma interrupt YourHighPriorityISRCode
	void YourHighPriorityISRCode()
	{
		//Check which interrupt flag caused the interrupt.
		//Service the interrupt
		//Clear the interrupt flag
		//Etc.
		#if defined(USB_INTERRUPT)
	        USBDeviceTasks();
        #endif

	}	//This return will be a "retfie fast", since this is in a #pragma interrupt section 
	#pragma interruptlow YourLowPriorityISRCode
	void YourLowPriorityISRCode()
	{
		//Check which interrupt flag caused the interrupt.
		//Service the interrupt
		//Clear the interrupt flag
		//Etc.
	
	}	//This return will be a "retfie", since this is in a #pragma interruptlow section 

#elif defined(__C30__)
    #if defined(PROGRAMMABLE_WITH_USB_HID_BOOTLOADER)
        /*
         *	ISR JUMP TABLE
         *
         *	It is necessary to define jump table as a function because C30 will
         *	not store 24-bit wide values in program memory as variables.
         *
         *	This function should be stored at an address where the goto instructions 
         *	line up with the remapped vectors from the bootloader's linker script.
         *  
         *  For more information about how to remap the interrupt vectors,
         *  please refer to AN1157.  An example is provided below for the T2
         *  interrupt with a bootloader ending at address 0x1400
         */
//        void __attribute__ ((address(0x1404))) ISRTable(){
//        
//        	asm("reset"); //reset instruction to prevent runaway code
//        	asm("goto %0"::"i"(&_T2Interrupt));  //T2Interrupt's address
//        }
    #endif
#endif
/////////////////////////////////////////////////////////////////////////////
//                              OBJECT'S IDs
/////////////////////////////////////////////////////////////////////////////
#define ID_WINDOW1      10

#define ID_BUTTON1      11
#define ID_BUTTON2      12
#define ID_BUTTON3      13
#define ID_BUTTON4      14
#define ID_BUTTON5      15
#define ID_BUTTON6      16
#define ID_BUTTON7      17

#define ID_BUTTON8      25
#define ID_BUTTON9      26
#define ID_BUTTON10    27
#define ID_BUTTON11    28
#define ID_BUTTON12    29

#define ID_BUTTON_BACK  18
#define ID_BUTTON_NEXT  19
#define ID_BUTTON_HOME  24

#define ID_CHECKBOX1    20
#define ID_CHECKBOX2    21
#define ID_CHECKBOX3    22
#define ID_CHECKBOX4    23
#define ID_CHECKBOX5    24
#define ID_CHECKBOX6    25
#define ID_CHECKBOX7    26
#define ID_CHECKBOX8    27
#define ID_CHECKBOX9    28
#define ID_CHECKBOX10    0
#define ID_CHECKBOX11    1
#define ID_CHECKBOX12    2
#define ID_CHECKBOX13    3
#define ID_CHECKBOX14    4
#define ID_CHECKBOX15    5
#define ID_CHECKBOX16    6
#define ID_CHECKBOX17    7
#define ID_CHECKBOX18    8
#define ID_CHECKBOX19    9
#define ID_CHECKBOX20    10
#define ID_CHECKBOX21    11
#define ID_CHECKBOX22    12
#define ID_CHECKBOX23    13
#define ID_CHECKBOX24    14
#define ID_CHECKBOX25    15
#define ID_CHECKBOX26    16
#define ID_CHECKBOX27    17
#define ID_CHECKBOX28    18
#define ID_CHECKBOX29    19

#define ID_RADIOBUTTON1 30
#define ID_RADIOBUTTON2 31
#define ID_RADIOBUTTON3 32
#define ID_RADIOBUTTON4 33
#define ID_RADIOBUTTON5 34
#define ID_RADIOBUTTON6 35
#define ID_RADIOBUTTON7 36
#define ID_RADIOBUTTON8 37
#define ID_RADIOBUTTON9 38
#define ID_RADIOBUTTON10 39

#define ID_SLIDER1      40
#define ID_SLIDER2      41
#define ID_SLIDER3      42
#define ID_SLIDER4      44

#define ID_STATICTEXT0  49 
#define ID_GROUPBOX1    50
#define ID_GROUPBOX2    51
#define ID_STATICTEXT1  52
#define ID_STATICTEXT2  53
#define ID_STATICTEXT3  54
#define ID_STATICTEXT4  55
#define ID_STATICTEXTLAT   56
#define ID_STATICTEXTLON   57
#define ID_STATICTEXT7  58
#define ID_STATICTEXT8  59
#define ID_STATICTEXT9  60
#define ID_STATICTEXT10  61
#define ID_STATICTEXT11  62
#define ID_STATICTEXT12  63
#define ID_STATICTEXT13  64
#define ID_STATICTEXT14  65
#define ID_STATICTEXT15  66
#define ID_STATICTEXT16  67
#define ID_STATICTEXT17  68
#define ID_STATICTEXT18  69
#define ID_STATICTEXT19  70
#define ID_STATICTEXTID  74

#define ID_PICTURE1     71

#define ID_PROGRESSBAR1 72
#define ID_CUSTOM1      73

#define ID_LISTBOX1     80
#define ID_LISTBOX2     81
#define ID_LISTBOX3     82
#define ID_EDITBOX1     83
#define ID_LISTBOX4     84

#define ID_CALL         91
#define ID_CHANGE       92
#define ID_HOLD			93
#define ID_BACKSPACE    94
#define ID_POUND		95
#define ID_ASTERISK		96
#define ID_KEYPADPOID   99
#define ID_KEYPAD       100	/* uses 100 to 110 for phone key pad demo */
#define ID_KEYCHINAPAD       110	/* uses 110 to 109 for China key pad demo */

#define ID_ROUNDDIAL    120
#define ID_METER1	    130
#define ID_METER2	    131
#define ID_BUTTONChina_P   140
#define ID_BUTTONChina_M   141
#define ID_BUTTONHR_P   150
#define ID_BUTTONHR_M   151
#define ID_BUTTONMN_P   152
#define ID_BUTTONMN_M   153
#define ID_BUTTONSC_P   154
#define ID_BUTTONSC_M   155
#define ID_BUTTON1_P   156
#define ID_BUTTON1_M   157
#define ID_BUTTON2_P   158
#define ID_BUTTON2_M   159
#define ID_BUTTON3_P   160
#define ID_BUTTON3_M   161

#define ID_BUTTON_DATE_UP 180
#define ID_BUTTON_DATE_DN 181

#define ID_BUTTON_UNIT_UP 182
#define ID_BUTTON_UNIT_DN 183

#define ID_BUTTON_UNIT    185

#define ID_BUTTON_MO    186
#define ID_BUTTON_DY    187
#define ID_BUTTON_YR    188
#define ID_BUTTON_RET   189

#define ID_EB_UNIT		199

#define ID_EB_MONTH		200
#define ID_EB_DAY		201
#define ID_EB_YEAR		202

#define ID_EB_HOUR      203
#define ID_EB_MINUTE    204
#define ID_EB_SECOND    205

#define ID_EB_RECORD1    206
#define ID_EB_RECORD2    207
#define ID_EB_RECORD3    208

#define ID_EB_DryGas    209
#define ID_EB_WetGas    210


#define ID_ABC		97
#define ID_ABCPAD       211	/* uses 210 to 235 for ABC key pad demo */

//batt type
#define BATT_TYPE_ALKALINE		0
#define BATT_TYPE_NIH			1
#define BATT_TYPE_LITHIUM		2


/////////////////////////////////////////////////////////////////////////////
//                            LOCAL PROTOTYPES
/////////////////////////////////////////////////////////////////////////////
void StartScreen();								// draws intro screen
void CreatePage(XCHAR *pText); 					// creates the navigation buttons and window for each screen
void CreatePage0(XCHAR *pText); 

void CreateDateTime();							// creates date time demo screen
WORD MsgDateTime(WORD objMsg, OBJ_HEADER* pObj);//  processes messages for date and time demo screen
void UpdateRTCCDates(LISTBOX* pLb); 			// update the date edit boxes 
WORD MsgSetDate(WORD objMsg, OBJ_HEADER* pObj, GOL_MSG* pMsg); // processes messages for setting dates
void ShowPullDownMenu();						// create objects for pull down menu and draw 
WORD RemovePullDownMenu();						// removes pop out pull down menu from screen
void updateDateTimeEb();						// update edit boxes with current date and time values 


void ErrorTrap(XCHAR* message);              	// outputs text message on screen and stop execution

void TickInit(void);                        	// starts tick counter  

void CreatePullDown(); 
WORD MsgPullDown(WORD objMsg, OBJ_HEADER* pObj, GOL_MSG* pMsg); // processes screen pull down menu

void	CreateMain();
WORD MsgMain(WORD objMsg, OBJ_HEADER* pObj);
void CreateAlcoholdetect();
WORD MsgAlcoholdetect(WORD objMsg, OBJ_HEADER* pObj);
void Aircheck();
void ResidentAlcoholdetected();
WORD MsgAlcoholdetected(WORD objMsg, OBJ_HEADER* pObj);
void Check_Screen();
void CreateEditTest(XCHAR *EditTestStr,WORD CharMax);
WORD MsgEditTest(WORD objMsg, OBJ_HEADER* pObj, GOL_MSG* pMsg); 
WORD MsgEditMark(WORD objMsg, OBJ_HEADER* pObj, GOL_MSG* pMsg);
WORD MsgPassword(WORD objMsg, OBJ_HEADER* pObj, GOL_MSG* pMsg);
WORD MsgScreenPassword(WORD objMsg, OBJ_HEADER* pObj, GOL_MSG* pMsg);
WORD MsgMasterPassword(WORD objMsg, OBJ_HEADER* pObj, GOL_MSG* pMsg);
WORD MsgEditInspectDate(WORD objMsg, OBJ_HEADER* pObj, GOL_MSG* pMsg);
void CreateEditABC(XCHAR *EditTestStr,WORD CharMax);
WORD MsgEditABC(WORD objMsg, OBJ_HEADER* pObj, GOL_MSG* pMsg);
void CreateRecord();
WORD MsgRecord(WORD objMsg, OBJ_HEADER* pObj, GOL_MSG* pMsg); 
void CreateJumptox(void);
void updateJumptoxEb(void);
WORD MsgJumptox(WORD objMsg, OBJ_HEADER* pObj, GOL_MSG* pMsg);
void	CreateSetting();
void CreateSoundControl();
void SoundControlupdate();
WORD MsgSoundControl(WORD objMsg, OBJ_HEADER * pObj, GOL_MSG * pMsg);
void CreateEditeeprom(XCHAR *EditTestStr,WORD CharMax);
WORD MsgEditeeprom(WORD objMsg, OBJ_HEADER * pObj, GOL_MSG * pMsg);
WORD MsgEditDemarcate(WORD objMsg, OBJ_HEADER* pObj, GOL_MSG* pMsg);
WORD MsgEditMarkValue(WORD objMsg, OBJ_HEADER* pObj, GOL_MSG* pMsg);
WORD MsgEditID(WORD objMsg, OBJ_HEADER* pObj, GOL_MSG* pMsg);
void CreateDisplayMachineID(void);
WORD MsgDisplayMachineID(WORD objMsg, OBJ_HEADER* pObj);	
void  CreateBlowPlease();
WORD MsgBlowPlease(WORD objMsg, OBJ_HEADER* pObj);

void  CreateDatadeal();
WORD  MsgDatadeal(WORD objMsg, OBJ_HEADER* pObj);

void CreateDISCONTINUED();
WORD MsgDISCONTINUED(WORD objMsg, OBJ_HEADER* pObj, GOL_MSG* pMsg);

WORD MsgSetting(WORD objMsg, OBJ_HEADER* pObj);
void  CreateGPS();
void	ShowBattery(int value, char batt_type);
void	CreateSetBackLight();
void  CreateUnitPDmenu();
WORD MsgUnitPDment(WORD objMsg, OBJ_HEADER* pObj, GOL_MSG* pMsg);
void ShowUnitPullDownMeny();
void CreateConnectToPC();
WORD MsgToPC(WORD objMsg, OBJ_HEADER* pObj);
WORD MsgSetBackLight(WORD objMsg, OBJ_HEADER* pObj);
void CreateResult(void);
WORD MsgResult(WORD objMsg, OBJ_HEADER* pObj);
void CreateThickness();
WORD MsgThickness(WORD objMsg, OBJ_HEADER* pObj); 
void CreatePrintSetting();
WORD MsgPrintSetting(WORD objMsg, OBJ_HEADER* pObj); 
void CreateDemarcate();
WORD MsgDemarcate(WORD objMsg, OBJ_HEADER* pObj);
void CreateDemarcate2();
WORD MsgDemarcate2(WORD objMsg, OBJ_HEADER* pObj);
void CreateChoosegas();
WORD MsgChoosegas(WORD objMsg, OBJ_HEADER * pObj);
void CreategasCal();
WORD MsggasCal(WORD objMsg, OBJ_HEADER * pObj);
void CreateCalTestresult(void);
WORD MsgCalTestresult(WORD objMsg, OBJ_HEADER * pObj);
void CreateMark();
WORD MsgMark(WORD objMsg, OBJ_HEADER* pObj);
void Createcaladjust();
WORD Msgcaladjust(WORD objMsg, OBJ_HEADER* pObj);
void CreateWait();
WORD MsgWait(WORD objMsg, OBJ_HEADER* pObj);
void  CreatePowerOff();
WORD MsgPowerOFF(WORD objMsg, OBJ_HEADER* pObj);
void  CreateRefuse();
WORD MsgRefuse(WORD objMsg, OBJ_HEADER* pObj);
void  CreateBlowPress();
WORD MsgBlowPress(WORD objMsg, OBJ_HEADER* pObj);
void  CreateItem();
WORD MsgItem(WORD objMsg, OBJ_HEADER* pObj, GOL_MSG* pMsg);
WORD MsgItemEditABC(WORD objMsg, OBJ_HEADER* pObj, GOL_MSG* pMsg);
void CreateAgent(void);
WORD MsgAgent(WORD objMsg, OBJ_HEADER* pObj);
void CreateMaster(void);
WORD MsgMaster(WORD objMsg, OBJ_HEADER* pObj);
void CreateSetpassword(void);
WORD MsgSetpassword(WORD objMsg, OBJ_HEADER* pObj);
WORD MsgEditPassword(WORD objMsg, OBJ_HEADER* pObj, GOL_MSG* pMsg);
void CreateOutcomeSetpassword(void);
WORD MsgOutcomeSetpassword(WORD objMsg, OBJ_HEADER* pObj);
void CreateYesNo(XCHAR* YesOrNo);
WORD MsgYesNo(WORD objMsg, OBJ_HEADER* pObj);
void CreateCalibrate();
WORD MsgCalibrate(WORD objMsg, OBJ_HEADER* pObj);
void CreateChooseCal();
WORD MsgChooseCal(WORD objMsg, OBJ_HEADER * pObj);
void CreateCalLock();
WORD MsgCalLock(WORD objMsg, OBJ_HEADER* pObj);
void SaveReadItem(BOOL SaveRead);
void CreateDebug();
void CreateDebug2();
void CreateDemarcateshow();
WORD MsgDemarcateshow(WORD objMsg, OBJ_HEADER* pObj);
void Createautuooff(void);
void CreateBlueTooth();
WORD MsgBlueTooth(WORD objMsg, OBJ_HEADER* pObj);
/////////////////////////////////////////////////////////////////////////////
//                            IMAGES USED
/////////////////////////////////////////////////////////////////////////////

extern const BITMAP_FLASH PicGPS;
extern const BITMAP_FLASH Picrecord;
extern const BITMAP_FLASH Test3;
extern const BITMAP_FLASH Picsetting;
extern const BITMAP_FLASH Test1;
extern const BITMAP_FLASH Test2;
extern const BITMAP_FLASH set2;
extern const BITMAP_FLASH PC_copy;
extern const BITMAP_FLASH Pictime;
extern const BITMAP_FLASH arrowDown;
extern const BITMAP_FLASH arrowUp;
extern const BITMAP_FLASH mainp;
extern const BITMAP_FLASH HOME;
extern const BITMAP_FLASH LEFT;
extern const BITMAP_FLASH RIGHT;
extern const BITMAP_FLASH gpslogo;
extern const BITMAP_FLASH ag3;
extern const BITMAP_FLASH printerbutton;
extern const BITMAP_FLASH bluetooth;
//extern BITMAP_EXTERNAL E;
/////////////////////////////////////////////////////////////////////////////
//                             FONTS USED
/////////////////////////////////////////////////////////////////////////////
extern const FONT_FLASH GOLFontDefault; 	// default GOL font
extern const FONT_FLASH GOLMediumFont; 		// medium font
extern const FONT_FLASH GOLSmallFont; 		// small font
extern const FONT_FLASH BigArial;			// only for the TestResult Display
//#ifdef	USE_MULTIBYTECHAR
extern const FONT_FLASH CHINESE_LARGE; 			// Japanese large font
extern const FONT_FLASH CHINESE_SMALL; 			// Japanese small font

//FONT_FLASH *ptrLargeAsianFont = &CHINESE_LARGE;
const FONT_FLASH *ptrLargeAsianFont = &GOLMediumFont;//&GOLFontDefault;
const FONT_FLASH *ptrSmallAsianFont = &GOLMediumFont;//&CHINESE_SMALL;// &GOLFontDefault;
const FONT_FLASH *ptrBigArialFont = &BigArial;
//#endif
/////////////////////////////////////////////////////////////////////////////
//                            COLORS USED
/////////////////////////////////////////////////////////////////////////////
#define GRAY20             RGB565CONVERT( 51,  51,  51)
#define GRAY40             RGB565CONVERT(102, 102, 102)
#define GRAY80             RGB565CONVERT(204, 204, 204)
#define GRAY90             RGB565CONVERT(229, 229, 229)
#define GRAY95             RGB565CONVERT(242, 242, 242)

#define RED4 			   RGB565CONVERT(139,   0,   0)
#define FIREBRICK1 		   RGB565CONVERT(255,  48,  48)
#define DARKGREEN          RGB565CONVERT(  0, 100,   0)
#define PALEGREEN          RGB565CONVERT(152, 251, 152)
#define LIGHTYELLOW        RGB565CONVERT(238, 221, 130)
#define GOLD               RGB565CONVERT(255, 215,   0)
#define DARKORANGE		   RGB565CONVERT(255, 140,   0)

/////////////////////////////////////////////////////////////////////////////
//                            DEMO STATES      
/////////////////////////////////////////////////////////////////////////////
typedef enum {
	CREATE_MAIN=0,
    DISPLAY_MAIN,    
    CREATE_Alcoholdetect,
    DISPLAY_Alcoholdetect,
    CREATE_Aircheck,
    DISPLAY_Aircheck,
    CREATE_ResidentAlcoholdetected,
    DISPLAY_ResidentAlcoholdetected,
  /* CREATE_SUBJECT_NAME,
    DISPLAY_SUBJECT_NAME,
    CREATE_PLACE,
   DISPLAY_PLACE,
    CREATE_VEHICLENO,	// 车牌号
    DISPLAY_VEHICLENO,
    CREATE_BADGENO,		// 驾驶证号
    DISPLAY_BADGENO,
    CREATE_OPERATORNAME,	//警官号
    DISPLAY_OPERATORNAME,
    CREATE_DEPARTMENT,
    DISPLAY_DEPARTMENT,
 */   
    CREATE_PREPROCESS,		// 测试准备
    DISPLAY_PREPROCESS,
    CREATE_BLOW,			// 请吹气
    DISPLAY_BLOW,

    
	CREATE_PROGRESSBAR,
    DISPLAY_PROGRESSBAR,


    
	CREATE_SIGNATURE,
    DISPLAY_SIGNATURE,
    BOX_DRAW_SIGNATURE,

	CREATE_RECORD,
	DISPLAY_RECORD,
	CREATE_JUMP,
	DISPLAY_JUMP,
	CREATE_SETTING,
	DISPLAY_SETTING,
	CREATE_EDITEEPROM,
	DISPLAY_EDITEEPROM,
	CREATE_EDITID,	
	DISPLAY_EDITID,
	CREATE_DISPLAYID,
	DISPLAY_DISPLAYID,
	CREATE_GPS,
	DISPLAY_GPS,

	CREATE_SETBACKLIGHT,
	DISPLAY_SETBACKLIGHT,


	CREATE_TO_PC,
	DISPLAY_TO_PC,
	
    CREATE_Datadeal,			//
    DISPLAY_Datadeal,
    
	CREATE_RESULT,
	DISPLAY_RESULT,

	CREATE_UNIT_SCREEN,
	DISPLAY_UNIT_SCREEN,
	DISPLAY_UNIT_PDMENU,
	SHOW_UNIT_PDMENU,

	CREATE_THICKNESS,
	DISPLAY_THICKNESS,
	CREATE_PRINTSETTING,
	DISPLAY_PRINTSETTING,
	CREATE_SoundControl,
	DISPLAY_SoundControl,
	CREATE_DEMARCATE,
	DISPLAY_DEMARCATE,
	CREATE_DEMARCATE2,
	DISPLAY_DEMARCATE2,
    CREATE_EDITDEMARCATE1,
	DISPLAY_EDITDEMARCATE1,
	CREATE_EDITDEMARCATE2,
	DISPLAY_EDITDEMARCATE2,
	CREATE_EDITDEMARCATE3,
	DISPLAY_EDITDEMARCATE3,
	CREATE_EDITDEMARCATE4,
	DISPLAY_EDITDEMARCATE4,
	CREATE_EDITDEMARCATE5,
	DISPLAY_EDITDEMARCATE5,
	CREATE_EDITDEMARCATE6,
	DISPLAY_EDITDEMARCATE6,	
	CREATE_EDITMARK,
	DISPLAY_EDITMARK,	
	
    CREATE_Choosegas,
	DISPLAY_Choosegas,
    CREATE_Gascal,
	DISPLAY_Gascal,
    CREATE_Calresult,
	DISPLAY_Calresult,
	
    CREATE_INPUT,
	DISPLAY_INPUT,
    CREATE_Mark,
	DISPLAY_Mark,
	CREATE_Wait,
	DISPLAY_Wait,
	CREATE_Waittest,
	DISPLAY_Waittest,
	CREATE_Waitdem,
	DISPLAY_Waitdem,
	CREATE_Waitdem2,
	DISPLAY_Waitdem2,
	CREATE_Debug,
	DISPLAY_Debug,
	CREATE_Debug2,
	DISPLAY_Debug2,
	CREATE_DEMARCATEshow,
	DISPLAY_DEMARCATEshow,
	DISPLAY_passivity,
	CREATE_REFUSE,
	DISPLAY_REFUSE,
	CREATE_DISCONTINUED,
	DISPLAY_DISCONTINUED,	
	CREATE_BLOW_PRESS,
	DISPLAY_BLOW_PRESS,



CREATE_LISTBOX,
    DISPLAY_LISTBOX,
    CREATE_CUSTOMCONTROL,
    DISPLAY_CUSTOMCONTROL,
    CREATE_Caladjust,
    DISPLAY_Caladjust,
    CREATE_PASSWORD,
    DISPLAY_PASSWORD,
    CREATE_ScreenPASSWORD,
    DISPLAY_ScreenPASSWORD,
    CREATE_MASTERPASSWORD,
    DISPLAY_MASTERPASSWORD,
    CREATE_POWEROFF,
    DISPLAY_POWEROFF,
    CREATE_ITEM,
    DISPLAY_ITEM,
    CREATE_ITEM0,
    DISPLAY_ITEM0,
    CREATE_ITEM1,
    DISPLAY_ITEM1,
    CREATE_ITEM2,
    DISPLAY_ITEM2,
    CREATE_ITEM3,
    DISPLAY_ITEM3,
    CREATE_ITEM4,
    DISPLAY_ITEM4,
    CREATE_ITEM5,
    DISPLAY_ITEM5,
    CREATE_ITEM6,
    DISPLAY_ITEM6,
    CREATE_ITEM7,
    DISPLAY_ITEM7,
    CREATE_ITEM8,
    DISPLAY_ITEM8,
    CREATE_ITEM9,
    DISPLAY_ITEM9,
    CREATE_AGENT,
    DISPLAY_AGENT,
    CREATE_Master,
    DISPLAY_Master,
    CREATE_Setpassword,
    DISPLAY_Setpassword,
    CREATE_InputSetpassword1,
    DISPLAY_InputSetpassword1,
    CREATE_InputSetpassword2,
    DISPLAY_InputSetpassword2,
    CREATE_OutcomeSetpassword,
    DISPLAY_OutcomeSetpassword,
    CREATE_RESET,
    DISPLAY_RESET,
    CREATE_ChooseCal,
    DISPLAY_ChooseCal,    
    CREATE_CYC,
    DISPLAY_CYC,
    CREATE_CALLOCK,
    DISPLAY_CALLOCK,    
    CREATE_DELETE,
    DISPLAY_DELETE,
    CREATE_YESNO,
    DISPLAY_YESNO,
    CREATE_AUTOOFF,
    DISPLAY_AUTOOFF,
    CREATE_BlueTooth,
    DISPLAY_BlueTooth,    

    
    
    
    // these states are for time and date settings 0xF3xx is used here as a 
    // state ID to check when date and time are to be updated or not.
    CREATE_DATETIME = 0xF300,		// creates the date and time setting menu		
    DISPLAY_DATETIME = 0xF301,		// displays the menu for the date and time setting
    DISPLAY_DATE_PDMENU = 0xF302,	// displays the pulldown menu to set one of the date items (month, day or year)
    SHOW_DATE_PDMENU = 0xF303,		// creates the pulldown menu to set one of the date items (month, day or year)
    HIDE_DATE_PDMENU = 0xF304		// hides the pulldown menu to set one of the date items (month, day or year)

} SCREEN_STATES;

/////////////////////////////////////////////////////////////////////////////
//                       GLOBAL VARIABLES FOR DEMO
/////////////////////////////////////////////////////////////////////////////

SCREEN_STATES  screenState = CREATE_MAIN;
SCREEN_STATES  PrescreenState = CREATE_MAIN;
GOL_SCHEME*    altScheme;					 // alternative style scheme
GOL_SCHEME*    alt2Scheme;					 // alternative 2 style scheme
GOL_SCHEME*    alt3Scheme;					 // alternative 3 style scheme
GOL_SCHEME*    alt4Scheme;					 // alternative 4 style scheme
GOL_SCHEME*    alt5Scheme;					 // alternative 5 style scheme
GOL_SCHEME*	   timeScheme;	
GOL_SCHEME*    meterScheme;					 // meter scheme
GOL_SCHEME*    navScheme;		 			 // style scheme for the navigation
GOL_SCHEME*    failScheme;
GOL_SCHEME*    redScheme;                    // alternative red style scheme
GOL_SCHEME*    greenScheme;                  // alternative green style scheme
GOL_SCHEME*    yellowScheme;                 // alternative yellow style scheme
GOL_SCHEME*    mainScheme;
GOL_SCHEME*	   timemainScheme;
GOL_SCHEME*    blackScheme;


volatile DWORD tick = 0;                     // tick counter
volatile unsigned int offcount=0,PrintCount=0;
volatile BYTE PrintSec=0,Pressflag=0;
volatile BOOL ChoicePrint = FALSE,AutooffSwitch = FALSE;

OBJ_HEADER*	   pGenObj;						 // pointer to a general purpose object

PROGRESSBAR*   pProgressBar;                 // pointer to progress bar object for progress bar demo

SLIDER*		   pSlider;						 // pointer to the slider controlling the animation speed 


SHORT x;                                     // cursor X position
SHORT y;                                     // cursor Y position
// Variables for the pulldown menus
SHORT pDwnLeft, pDwnTop, pDwnRight, pDwnBottom;


SCREEN_STATES prevState = CREATE_MAIN;//CREATE_BUTTONS;	// used to mark state where time setting was called	

OBJ_HEADER *pListSaved;			// temporary storage for object list
WORD 		DateItemID;			// ID of date item being modified (ID_EB_MONTH(DAY)(YEAR))

XCHAR 		dateTimeStr[26];		// string variable for date and time display
XCHAR 		DisplaydateTimeStr[30]={0,0,0,0x0020,0,0,0x003A,0,0,0x003A,0,0,0x000A,0,0,0x0020,'/',0x0020,0,0,0x0020,'/',0x0020,0x0032,0x0030,0,0,0};		//spring add
XCHAR OperatorNameTempStr[19]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
CHAR		MAXCHAROld=0;
WORD		AlcTypeaddress=0X38;

XCHAR CHK_PAPER_VAL=0;
BOOL status_pc = FALSE;
static BOOL status_Press = TRUE;
BOOL BoolBatteryOFF = FALSE;
WORD g_backlight_time=0;    //关背光时间,1秒为单位
//WORD g_sleep_time=0;    //睡眠时间,1秒为单位
WORD g_standby_time=0;    //自动关机时间,1秒为单位
WORD g_rtc_counter=0;    //rtc 消息计?
WORD g_blow_time = 0;	// 吹气时间
BYTE TestMode=0xaa;
volatile BYTE Masklevel=0;  //测试类型aa=1,bb=2,cc=3
BOOL Unit_Select = FALSE;	// 浓度单位选择,默认为mg/100ml
BOOL FirstIn = FALSE;
BOOL alarmON = FALSE;
BOOL Calibrate_Flag = FALSE;
static char YesNoNumber = 0;
extern XCHAR Test_Result[];
volatile extern XCHAR sLongitude[];
volatile extern XCHAR sLatitude[];
volatile XCHAR	PasswordBuffer[7];
WORD	Temp_adc,initialpress;
WORD    epPress;
WORD ItemID = 0;
extern WORD	Temp_adc;
unsigned int ST_IRstatus = 0;
extern  unsigned int  Tem_AD;
extern  unsigned int  AD_press;
extern  unsigned int  MarkDensity;
extern  unsigned int   nAdc;
extern unsigned int MarkAdc;
extern unsigned int MarkTem;
extern unsigned int WriteTable;
extern unsigned int ReadTable;
extern unsigned int test_result;
extern  unsigned int Kt;
BOOL alocholdetecet= FALSE;
//WORD_VAL zyq_temp;
//WORD	ZYQ_REALLY;
//BYTE	zyq_tempbyte;
//unsigned int   ZYQ1[] = {0x00,0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x99,0xaa,0xbb,0xcc,0xdd,0xee,0xff};//{0x38};
extern XCHAR TestMiMaStr[3];
XCHAR DemarcateAD[7]	= {0x0030,0x0058,0x0030,0x0030,0x0030,0x0030,0x0000};    // 判定
XCHAR DemarcateAD1[7]	= {0x0030,0x0058,0x0030,0x0030,0x0030,0x0030,0x0000};    // 判定;
XCHAR DemarcateAD2[7]	= {0x0030,0x0058,0x0030,0x0030,0x0030,0x0030,0x0000};     // 判定;
XCHAR DemarcateAD3[7]	= {0x0030,0x0058,0x0030,0x0030,0x0030,0x0030,0x0000};     // 判定;
XCHAR DemarcateAD4[7]	= {0x0030,0x0058,0x0030,0x0030,0x0030,0x0030,0x0000};    // 判定;
XCHAR DemarcateAD5[7]	= {0x0030,0x0058,0x0030,0x0030,0x0030,0x0030,0x0000};    // 判定;
XCHAR DemarcateAD6[7]	= {0x0030,0x0058,0x0030,0x0030,0x0030,0x0030,0x0000};     // 判定;
XCHAR DemarcateAD7[7]	= {0x0030,0x0058,0x0030,0x0030,0x0030,0x0030,0x0000};     // 判定;
XCHAR DemarcateAD8[7]	= {0x0030,0x0058,0x0030,0x0030,0x0030,0x0030,0x0000};     // 判定;
XCHAR DemarcateAD9[7]	= {0x0030,0x0058,0x0030,0x0030,0x0030,0x0030,0x0000};     // 判定;
XCHAR DemarcateAD10[7]	= {0x0030,0x0058,0x0030,0x0030,0x0030,0x0030,0x0000};   // 判定;
XCHAR DemarcateAD11[7]	= {0x0030,0x0058,0x0030,0x0030,0x0030,0x0030,0x0000};     // 判定;
XCHAR DemarcateAD12[7]	= {0x0030,0x0058,0x0030,0x0030,0x0030,0x0030,0x0000};    // 判定;
XCHAR DemarcateAD13[7]	= {0x0030,0x0058,0x0030,0x0030,0x0030,0x0030,0x0000};     // 判定;
XCHAR Marktem[5]	= {0,0,0,0x00C3,0x00};
XCHAR MarkAD2[7]	= {0x30,0x58,0,0,0,0,0x00};
XCHAR DemarcateAD14[7]	= {0x0030,0x0058,0x0030,0x0030,0x0030,0x0030,0x0000};     // 判定;
XCHAR DemarcateAD15[7]	= {0x0030,0x0058,0x0030,0x0030,0x0030,0x0030,0x0000};     // 判定;
XCHAR DemarcateAD16[7]	= {0x0030,0x0058,0x0030,0x0030,0x0030,0x0030,0x0000};     // 判定;
XCHAR DemarcateAD17[7]	= {0x0030,0x0058,0x0030,0x0030,0x0030,0x0030,0x0000};     // 判定;
XCHAR DemarcateAD18[7]	= {0x0030,0x0058,0x0030,0x0030,0x0030,0x0030,0x0000};     // 判定;
XCHAR DemarcateAD19[7]	= {0x0030,0x0058,0x0030,0x0030,0x0030,0x0030,0x0000};     // 判定;
XCHAR DemarcateAD20[7]	= {0x0030,0x0058,0x0030,0x0030,0x0030,0x0030,0x0000};     // 判定;
//WORD MarkAD3[7]	= {0x0030,0x0058,0x0030,0x0030,0x0030,0x0030,0x0000};
//WORD MarkAD4[7]	= {0x0030,0x0058,0x0030,0x0030,0x0030,0x0030,0x0000};
unsigned int Adj_adc=0,Adj_tem=0,Demarcate_tem,Demarcate_adc, Mark_adc,Marktemporary;
XCHAR T1Str[3]={0x002d,0x0032,0x0000};
XCHAR T2Str[3]={0x0020,0x0035,0x0000};
XCHAR T3Str[3]={0x0031,0x0035,0x0000};
XCHAR T4Str[3]={0x0032,0x0032,0x0000};
XCHAR T5Str[3]={0x0032,0x0038,0x0000};
XCHAR T6Str[3]={0x0033,0x0035,0x0000};
XCHAR T7Str[3]={0x0034,0x0035,0x0000};
XCHAR T8Str[3]={0x0032,0x0035,0x0000};
XCHAR Mark1Str[3]={0x0032,0x0030,0x0000};
XCHAR Mark2Str[3]={0x0033,0x0030,0x0000};
XCHAR Mark3Str[3]={0x0035,0x0030,0x0000};
XCHAR Mark4Str[3]={0x0038,0x0030,0x0000};

XCHAR serialnumberStr[] = {0x0030,0x0030,0x0030,0x0030,0x0030,0x0000};// 最大是serialnumber == 65535;
volatile XCHAR	Flashbuff[RecordSizeMax] = {0, 0, 0, 0,0x00};	// 往FLASH 中写数据时候暂时保存地方
volatile XCHAR	TempbuffStr[19] = {0, 0, 0, 0,0, 0, 0, 0,0, 0, 0, 0,0, 0, 0, 0,0, 0, 0};	// 临时保存地址
//XCHAR sFlashbuff[256]={0x0091,0x0092};
volatile XCHAR	consbuffStr[19] = {0, 0, 0, 0,0, 0, 0, 0,0, 0, 0, 0,0, 0, 0, 0,0, 0, 0};
DATETIMES sys_datetime;
WORD_VAL wordval;
volatile unsigned int serialnumber = 1,serialnumberRecord = 1,Marktimes,Record_Over_Flag;
USERTITLE user_title;
static XCHAR IDumberStr[7]={0,0,0,0,0,0,0};
unsigned char longitude[8],latitude[8];
unsigned char timeSet[7];
static int S4_count = 1;

unsigned int sys_clk,USB_sys_clk,MarkTemp=0;
unsigned int dif;
unsigned int jhm98;
unsigned char I2C_ERROR,i2busy,usbcmd,usbcount;
unsigned int I2C_COUNTER,softMS;
unsigned char softMin,softSec,timeSet[7];
DATETIMES sys_date,old_date,gps_date;
unsigned char rx4_buff[RX4_BUFFLEN],*rx4_ptr;
unsigned char tx4_buff[TX4_BUFFLEN],*tx4_ptr,*tx4_ptrend;
unsigned int uart4_rtimer,t4len; 
unsigned char satellite,satellite_cnt;
WORD Valnum; 
unsigned char eeset[100];
DATETIMES gps_date,sys_date,old_date;
WORD BatteryValue;

OBJ_HEADER*      pbtn;
BUTTON*        pbutton;
STATICTEXT*   PST;
WINDOW*       pWin;

SHORT done;
unsigned int EETemp0,EETemp1;
unsigned int  TTTemp0,TTTemp1;
BOOL Demarcate=FALSE;
BOOL PDset=FALSE;
BYTE Settingposition;
WORD      update1 = 0,update2 = 0; 
WORD BEEP_TIME,TIMER_BASE;
SLIDER                      *pSld1;      // pointer to the slider object
SLIDER                      *pSld2;      // pointer to the slider object
BYTE Setpasswordposition;

BYTE    BnumberStr[2]  = {0x30,0x00};
BYTE    UnumberStr[2]  = {0x30,0x00};

XCHAR    DrygasadustStr[4]  = {0x0030,0x0030,'%',0x00};
XCHAR    WetgasadustStr[4]  = {'+',0x0030,0x0030,'%',0x00};

volatile BYTE DryorWet;

unsigned char Drygas_D=0,Wetgas_d=0,savetemp=0;

XCHAR  Jumptox[8]={0x30,0,0x30,0x30,0,0x30,0x30,0};
static XCHAR  PDread[6] = {0};
static XCHAR  PDread2[6] = {0};


BYTE ADLIST;
BYTE PeakScreen=0;
unsigned int Tem_Detect;

unsigned int BlueToothcount = 0;

extern volatile unsigned char Printbuff[];
extern volatile BOOL SPrint,endprint;
extern volatile unsigned int Sprinnum;

const UARTInitParameter UART4p = {(unsigned int *)0x020c,9600,'n',8,1};
extern tagUARTFLAG UARTFLAG;
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
#define OutPio4 _TRISG0 = 0
#define InPio4 _TRISG0 = 1
//XCHAR pTempABC[][2] = 	{  {'A',0},

XCHAR Countdown[][2] = {{0x0039,0},
                        {0x0038,0},
                        {0x0037,0},
                        {0x0036,0},
                        {0x0035,0},
                        {0x0034,0},
                        {0x0033,0},
                        {0x0032,0},
                        {0x0031,0},
                        {0x0030,0}};
                    
//XCHAR Countdown[] = {0x0036,0};
void initialUART4(void)
{
	unsigned int brg;
	unsigned int mode;
	unsigned int sta;
	PSVPAG = __builtin_psvpage(&UART4p);
	CORCONbits.PSV = 1;
	switch (UART4p.BaudRate)
	{
		case 38400: brg = 8;
			    break;
		case 19200: brg = 12;
			    break;
		case 9600:  brg = 103;//25;
	            	    break;
		case 4800:  brg = 51; 
	                    break;
		case 2400:  brg = 103;
			    break;
		case 300:   brg = 832;
			    break;
		default:    brg = 207;  // 1200bps
	}
	//p = x.comaddress;
	U4BRG = brg;               //configure the baud rate register
	mode = 0xa000;
	//UxMODEBITS.UARTEN=1            //UART is enabled
	//UxMODEBITS.USIDL=1             //Discontinue operation when device enters IDLE mode
	//UxMODEBITS.ALTIO=0             //UART communicates using UxTX and UxRX I/O pins
	//UxMODEBITS.WAKE=0              //Wake-up disabled
	//UxMODEBITS.LPBACK=0            //Loopback mode is disabled
	//UxMODEBITS.ABAUD=0             //Input to Capture module from ICx pin 
	//UxMODEBITS.PDSEL=0             //8-bit data, no parity
	//UxMODEBITS.STSEL=0             //1 STOP bit                                   
	sta = 0x8510;
	//UxSTABITS.UTXISL=1             //Interrupt when a character is transferred to the Transmit Shift register and as result, the transmit buffer becomes empty
	//UxSTABITS.UTXBRK=0             //UxTX pin operates normally
	//UxSTABITS.UTXEN=1              //UART transmitter enabled, UxTX pin controlled by UART (if UARTEN = 1)
	//UxSTABITS.UTXBF=0              //Transmit buffer is not full, at least one more data word can be written
	//UxSTABITS.TRMT=1               //Transmit shift register is empty and transmit buffer is empty (the last transmission has completed)
	//UxSTABITS.URXISEL=0            //Interrupt flag bit is set when Receive Buffer is 1/4 full (i.e., has 3 data characters)
	//UxSTABITS.ADDEN=0              //Address Detect mode disabled
	//UxSTABITS.RIDLE=1              //Receiver is IDLE
	//UxSTABITS.PERR=0               //Parity error has not been detected
	//UxSTABITS.FERR=0               //Framing Error has not been detected
	//UxSTABITS.OERR=0               //Receive buffer has not overflowed
	//UxSTABITS.URXDA=0              //Receive buffer is empty
	if ((UART4p.WordLength == 9) && (UART4p.check == 'n'))
	{
		mode |= 0x0006;          //9-bit data, no parity
		sta |= 0x0020;           //Address Detect mode enabled
		sta &= 0xff7f;           //Interrupt flag bit is set when a character is received
	}
	if ((UART4p.WordLength == 8) && (UART4p.check == 'o')) mode |= 0x0004;   //8-bit data, odd parity
	if ((UART4p.WordLength == 8) && (UART4p.check == 'e')) mode |= 0x0002;   //8-bit data, even parity
	if (UART4p.StopBitLength == 2) mode |= 0x0001;                      //2 STOP bits	
	U4MODE = mode;             //configure the mode register
	U4STA = sta;               //configure the Status and Control register
	
	
        _U4RXIF = 0;             //clear the receive interrupt flag
        _U4TXIF = 0;             //clear the transmit interrupt flag
        _U4RXIP = 4;             //set the receive priority level 5
        _U4TXIP = 4;             //set the transmit priority level 5
  #ifdef  USE_GPS
        _U4RXIE = 1;             //enable the receive interrupt
  #else
        _U4RXIE = 0; 
  #endif
        _U4TXIE = 0;             //disable the transmit interrupt
        uart4_rtimer = 0;
        rx4_ptr = &rx4_buff[0];
        tx4_ptr = &tx4_buff[0];
        tx4_ptrend = tx4_ptr;


	_TRISF4 = 1;//u4rx input
   Nop();
	_TRISF5 = 0;//u4tx output
    Nop();
}

 void delay_ms(unsigned int cctt)
{
	unsigned int save_delayt=6000;
	unsigned long aa;
	//aa = save_delayt*cctt;
	while(save_delayt!=0) 
	{
		save_delayt--;
	}
} 

#define spi2_data_wait 180
#define SPIGet() SPI2BUF
void initSPI2(void)
{
	SPI2STAT = 0;
    SPI2CON1 = 0x001b;
    SPI2CON1bits.MSTEN = 1; 
    SPI2CON2 = 0;
    SPI2CON1bits.MODE16 = 0;
    SPI2CON1bits.CKE = 0;
    SPI2CON1bits.CKP = 1;
    SPI2CON1bits.SMP = 1;
    SPI2STATbits.SPIEN = 1;
	_TRISC13 = 0;//flash1 /cs control output
    Nop();
	_RC13 = 1;
	Nop();
	_TRISD11 = 0;//flash2 /cs control output
    Nop();
	_RD11 = 1;
	Nop();
	_TRISD12 = 0;//eeprom /cs control output
    Nop();
	//_RD12 = 1;
	Nop();
	_TRISG6 = 0;//SPI SCK2 output
    Nop();
	_TRISG8 = 0;//SPI SDO2 output
    Nop();
	_TRISG7 = 1;//SPI SDI2 input
}
//extern void delay_ms(unsigned int cctt);

void SPI2_WAIT(void)
{	
	//unsigned char k=0;
	unsigned int k=0;
	while (SPI2STATbits.SPITBF)
	{
		if (k < 180) k++; 
		//if (k < 720) k++;
		else 
		{
			initSPI2();
			return;         
		}
	}
}		
/*DATA FLASH read*/
void FLASH_RD(unsigned int page_address, unsigned int byte_address, unsigned int length, unsigned char *rdptr,unsigned char cs)
{
	unsigned char x;   
	unsigned int k = 0,i;
	ClrWdt();
	if(cs)	_RD11 = 0;//enable CS FLASH2
	else _RC13 = 0;//enable CS FLASH1
	Nop();
	Nop();
	Nop();
	Nop();

	SPI2_WAIT();    /*if spi2txb is full,wait*/
	SPI2BUF = 0xe8;                 /*transfer the opcode e8h*/
	SPI2_WAIT();
	SPI2BUF = (page_address >> 6) & 0xff;  /*one reserved bits(0) and PA12 - PA6*/
	SPI2_WAIT();
	SPI2BUF = ((page_address << 2) | (byte_address>>8))& 0xff;  /*PA5 - PA0,BA9,BA8*/ 
	SPI2_WAIT();
	SPI2BUF = byte_address & 0xff;  /*BA7 - BA0*/ 
	SPI2_WAIT();
	SPI2BUF = 0;                    /*32 donot care bits*/
	SPI2_WAIT();
	SPI2BUF = 0;
	SPI2_WAIT();
	SPI2BUF = 0;
	SPI2_WAIT();
	SPI2BUF = 0;
 
	for (i = 0; i < 20; i++)
	{
		Nop();
	}
	k = SPI2BUF;
	SPI2STATbits.SPIROV = 0;        /*clear receive overflow flag bit*/
	while (length)                      /*the length of data expected*/
	{

		SPI2_WAIT();
		SPI2BUF = 0;                    /*generate clock pulses*/


		k = 0;
		while (!SPI2STATbits.SPIRBF)    /*if spi2rxb is empty,wait*/
		{
			if (k < spi2_data_wait)     /*timeout value*/
			k++;                     /*wait for time out operation*/
			else 
			{

				initSPI2();
				break;         /*Time out,return number of byte not to be read*/
			}
		}       
		*rdptr++ = SPI2BUF;             /*the received data to be recorded to this array*/

		SPI2STATbits.SPIROV = 0;        /*clear receive overflow flag bit*/
		length--;                       /*reduce string length count by 1*/
	}
	Nop();
	Nop();
	Nop();
	Nop();
	if(cs)	_RD11 = 1;//disenable CS FLASH2
	else _RC13 = 1;//disenable CS FLASH1
}

void flash_buffer(unsigned char opcode, unsigned int page_address,unsigned char cs)
{
	unsigned char i;
	if(cs)	_RD11 = 0;//enable CS FLASH2
	else _RC13 = 0;//enable CS FLASH1
	Nop();
	Nop();
	Nop();
	Nop();

	SPI2_WAIT();           /*if spi2txb is full,wait*/
	SPI2BUF = opcode;                      /*53h for buffer1,55h for buffer2*/
	SPI2_WAIT();
	SPI2BUF = (page_address >> 6) & 0xff;  /*one reserved bits(0) and PA12 - PA6*/
	SPI2_WAIT();
	SPI2BUF = (page_address << 2) & 0xff;  /*PA5 - PA0,00*/ 
	SPI2_WAIT();
	SPI2BUF = 0;                           /*8 donot care bits*/
	SPI2_WAIT();

	for (i = 0; i < 20; i++)
	{
		Nop();
	}
	if(cs)	_RD11 = 1;//disenable CS FLASH2
	else _RC13 = 1;//disenable CS FLASH1
	Nop();
	Nop();
	Nop();
	Nop();
	delay_ms(2);
}

void write_bufferstart(unsigned char opcode, unsigned int buffer_address,unsigned char cs)
{
	unsigned char x;	
	ClrWdt();       
	if(cs)	_RD11 = 0;//enable CS FLASH2
	else _RC13 = 0;//enable CS FLASH1
	Nop();
	Nop();
	Nop();
	Nop();

	SPI2_WAIT();
	SPI2BUF = opcode;                          /*84h for buffer1,87h for buffer2*/
	SPI2_WAIT();
	SPI2BUF = 0;                               /*8 donot care bits*/
	SPI2_WAIT();
	SPI2BUF = (buffer_address >> 8) & 0xff;    /*7 donot care bits,BFA8*/ 
	SPI2_WAIT();
	SPI2BUF = buffer_address & 0xff;           /*BFA7 - BFA0*/ 

}

void write_buffer(unsigned char *ptra,unsigned int length)
{
	unsigned int i;
	for(i= 0;i<length;i++)
	{
#if 1
	SPI2_WAIT();
	SPI2BUF = *ptra++; 
#endif
#if 0
	SPIPut(*ptra++);
	SPIGet();
#endif             
	}
}

void write_bufferend(unsigned char cs)
{
	unsigned char i;
	SPI2_WAIT();
	for (i = 0; i < 20; i++)
	{
		Nop();
	}
	if(cs)	_RD11 = 1;//disenable CS FLASH2
	else _RC13 = 1;//disenable CS FLASH1
	Nop();
	Nop();
	Nop();
	Nop();
}

void solid_data(unsigned char opcode, unsigned int page_address,unsigned char cs)
{
	unsigned char i;
	if(cs)	_RD11 = 0;//enable CS FLASH2
	else _RC13 = 0;//enable CS FLASH1
	Nop();
	Nop();
	Nop();
	Nop();
#if 1
	SPI2_WAIT();
	SPI2BUF = opcode;                      /*83h for buffer1,86h for buffer2*/
	SPI2_WAIT();
	SPI2BUF = (page_address >> 6) & 0xff;  /*one reserved bits(0) and PA12 - PA6*/
	SPI2_WAIT();
	SPI2BUF = (page_address << 2) & 0xff;  /*PA5 - PA0,0,0*/ 
	SPI2_WAIT();
	SPI2BUF = 0;                           /*8 donot care bits*/
	SPI2_WAIT();
#endif
#if 0
	SPIPut(opcode);
	SPIGet();
	SPIPut((page_address >> 6) & 0xff);
	SPIGet();
	SPIPut((page_address << 2) & 0xff);
	SPIGet();
	SPIPut(0);
	SPIGet();
#endif
	for (i = 0; i < 20; i++)
	{
		Nop();
	}
	if(cs)	_RD11 = 1;//disenable CS FLASH2
	else _RC13 = 1;//disenable CS FLASH1
	Nop();
	Nop();
	Nop();
	Nop();
	delay_ms(20);
}

/*DATA FLASH AUTO PAGE REWRITE*/
/*i = 0-127*/
void AUTO_REWR(unsigned char i)
{
	unsigned char j,x;
	for (j = 0; j < 64; j++)         /*main memory page address bits PA6 - PA0,each page of the main memory to be rewritten*/
	{
		_RC13 = 0;
		Nop();
		Nop();
		Nop();
		Nop();
		SPI2_WAIT();  /*if spi2txb is full,wait*/
		SPI2BUF = 0x59;               /*auto page rewrite code*/
		SPI2_WAIT();
		SPI2BUF = i;                  /*three reserved bits(000),and five address bits PA11 - PA7*/
		SPI2_WAIT();
		SPI2BUF = (j<<2);             /*seven address bits PA5 - PA0,and two donot care bit*/
		SPI2_WAIT();
		SPI2BUF = 0;                  /*eight donot care bits*/
		SPI2_WAIT();
		for (i = 0; i < 20; i++)
		{
			Nop();
		}
		_RG14 = 1;
		ClrWdt();
	}
}
/*
	write_bufferstart(0x84,0); //0x84 is buff1,0 is buff1 addr
	write_buffer(ptra,33);//ptra is buffer address,33 is write length
	write_bufferend();
	solid_data(0x83,yy);//0x83 is buff1,yy is page no.
	FLASH_RD(page_address,byte_address,length,rdptr);
*/


void testflash(void)
{
	char ebuff[1024],cs=0;
	memset(&ebuff[0],0xdd,256);
	//SRbits.IPL = 6;
	flash_buffer(0x53,1,cs);	//擦除FALSH
	write_bufferstart(0x84,0,cs); //p1 is flash1,0x84 is buff1,93 is buff1 addr
	write_buffer(&ebuff[0],256);//ptra is buffer address,33 is write length	写数据
	write_bufferend(cs);//恢复CS 信号
	solid_data(0x83,1,cs);//0 is flash1,0x83 is buff1,yy is page no.固化
	Nop();
	Nop();
	Nop();
	FLASH_RD(1,0,256,&ebuff[0],cs);
	Nop();
	Nop();
	memset(&ebuff[0],0xCC,256);
	//SRbits.IPL = 6;
	flash_buffer(0x53,1,cs);	//擦除FALSH
	write_bufferstart(0x84,0,cs); //p1 is flash1,0x84 is buff1,93 is buff1 addr
	write_buffer(&ebuff[0],256);//ptra is buffer address,33 is write length	写数据
	write_bufferend(cs);//恢复CS 信号
	solid_data(0x83,1,cs);//0 is flash1,0x83 is buff1,yy is page no.固化
	Nop();
	Nop();
	Nop();
	FLASH_RD(1,0,256,&ebuff[0],cs);
	Nop();
	Nop();	
}


void	CopyChar(XCHAR*Dest, XCHAR *Str, unsigned int uCount)
{
	unsigned int i;
	if(uCount<=0)return 0;
	for(i = 0; i< uCount; i++)
	{
	Dest[i] = Str[i];
	}


}
void	CopyXCHARtoCHAR(unsigned char *Dest, unsigned char *Str, unsigned int uCount)
{
	unsigned int i;
	if(uCount<=0)return 0;
	for(i = 0; i< uCount; i++)
	{
	Dest[i] = Str[i];
	}


}
#if	1

void SaveAdjust(void)//保存当前标定的时间日期
{
	EEPROMWriteByte(_time.yr, MarkYrDataAddress);//实际校准
	EEPROMWriteByte(_time.mth, MarkMthDataAddress);
	EEPROMWriteByte(_time.day, MarkDayDataAddress);
	EEPROMWriteWord(0, TestTimesDataAddress);
}
void InitialSETTING(void)
{
      EEPROMWriteByte(0x00, MarkYrDataAddress);//实际校准
	EEPROMWriteByte(0x01, MarkMthDataAddress);
	EEPROMWriteByte(0x01, MarkDayDataAddress);
	
   EEPROMWriteWord(0, TestTimesDataAddress);

EEPROMWriteByte(0X00,ID_JIQIADDRESS);
EEPROMWriteByte(0X00,ID_JIQIADDRESS+1);
EEPROMWriteByte(0X00,ID_JIQIADDRESS+2);

EEPROMWriteWord(800,0x6020);
EEPROMWriteWord(5200,0x6022);//标定值

       EEPROMWriteWord(0x0092,0x6000);//-2温度
       EEPROMWriteWord(0x0d48,0x600e);//-2 AD

       EEPROMWriteWord(0x00ac,0x6002);//5 tem
       EEPROMWriteWord(0x1105,0x6010);//5 AD

       EEPROMWriteWord(0x00ca,0x6004);//15 tem
       EEPROMWriteWord(0x1367,0x6012);//15 AD

       EEPROMWriteWord(0x00df,0x6006);//22 tem
       EEPROMWriteWord(0x1498,0x6014);//22 AD
       
       EEPROMWriteWord(0x00f3,0x6008);//28 tem
       EEPROMWriteWord(0x1498,0x6016);//28 AD

       EEPROMWriteWord(0x0107,0x600a);//35 tem
       EEPROMWriteWord(0x1546,0x6018);//35 AD

       EEPROMWriteWord(0x0126,0x600c);//45 tem
       EEPROMWriteWord(0x142e,0x601a);//45 AD

	   EEPROMWriteWord(200, Beepsld1Address);
       EEPROMWriteWord(400, Beepsld2Address);

	   BEEP_TIME=200;
       TIMER_BASE=400;


}
void DefaultSETTING(void)
{
unsigned char  Initialee[20]={0};
	
EEPROMWriteByte(1, RButtonAddress);	
EEPROMWriteWord(120, BackLightTimeAddress);//power off

EEPROMWriteByte(ID_RADIOBUTTONStar + 38, BlowTimeAddress);
EEPROMWriteWord(70, BlowTimeDataAddress);//time

EEPROMWriteByte(ID_RADIOBUTTONStar + 46, BlowPressAddress);
EEPROMWriteWord(230, BlowPressDataAddress);//press

EEPROMWriteByte(8, ThicknessAddress);//Unit

EEPROMWriteByte(ID_RADIOBUTTONStar + 28, KSelectAddress);

EEPROMWriteWord(2100, KSelectDataAddress);//K

EEPROMWriteByte(0x32, PrintSelectAddress);//under 0
EEPROMWriteByte(0x32, PrintSelectAddress+1);//beyond 0
EEPROMWriteByte(0, ID_CHECKBOXADDRESS + ID_CHECKBOX7 );//auto print

EEPROMWriteByte(0, ID_CHECKBOXADDRESS + ID_CHECKBOX8 );//Lock1

EEPROMWriteByte(0, ID_CHECKBOXADDRESS + ID_CHECKBOX9 );//GPS

EEPROMWriteByte(1, ID_CHECKBOXADDRESS + ID_CHECKBOX10 );//
EEPROMWriteByte(0, ID_CHECKBOXADDRESS + ID_CHECKBOX20 );//

EEPROMWriteByte(1, ID_CHECKBOXADDRESS + ID_CHECKBOX11 );//
EEPROMWriteByte(1, ID_CHECKBOXADDRESS + ID_CHECKBOX21 );//

EEPROMWriteByte(1, ID_CHECKBOXADDRESS + ID_CHECKBOX12 );//
EEPROMWriteByte(0, ID_CHECKBOXADDRESS + ID_CHECKBOX22 );//

EEPROMWriteByte(1, ID_CHECKBOXADDRESS + ID_CHECKBOX13 );//
EEPROMWriteByte(1, ID_CHECKBOXADDRESS + ID_CHECKBOX23 );//

EEPROMWriteByte(1, ID_CHECKBOXADDRESS + ID_CHECKBOX14 );//
EEPROMWriteByte(1, ID_CHECKBOXADDRESS + ID_CHECKBOX24 );//

EEPROMWriteByte(1, ID_CHECKBOXADDRESS + ID_CHECKBOX15 );//
EEPROMWriteByte(1, ID_CHECKBOXADDRESS + ID_CHECKBOX25 );//
EEPROMWriteByte(0, ID_CHECKBOXADDRESS + ID_CHECKBOX16 );//
EEPROMWriteByte(0, ID_CHECKBOXADDRESS + ID_CHECKBOX26 );//
EEPROMWriteByte(0, ID_CHECKBOXADDRESS + ID_CHECKBOX17 );//
EEPROMWriteByte(0, ID_CHECKBOXADDRESS + ID_CHECKBOX27 );//
EEPROMWriteByte(0, ID_CHECKBOXADDRESS + ID_CHECKBOX18 );//
EEPROMWriteByte(0, ID_CHECKBOXADDRESS + ID_CHECKBOX28 );//
EEPROMWriteByte(0, ID_CHECKBOXADDRESS + ID_CHECKBOX19 );//
EEPROMWriteByte(0, ID_CHECKBOXADDRESS + ID_CHECKBOX29 );//
EEPROMWriteArray(Item0Address, Initialee, 20);
EEPROMWriteArray(Item1Address, Initialee, 20);
EEPROMWriteArray(Item2Address, Initialee, 20);
EEPROMWriteArray(Item3Address, Initialee, 20);
EEPROMWriteArray(Item4Address, Initialee, 20);
EEPROMWriteArray(Item5Address, Initialee, 20);
EEPROMWriteArray(Item6Address, Initialee, 20);
EEPROMWriteArray(Item7Address, Initialee, 20);
EEPROMWriteArray(Item8Address, Initialee, 20);
EEPROMWriteArray(Item9Address, Initialee, 20);
EEPROMWriteArray(Item0Address, DriverName, 32);
EEPROMWriteArray(Item1Address, PlaceStr, 32);
EEPROMWriteArray(Item2Address, VehicleNoStr, 32);
EEPROMWriteArray(Item3Address, BadgeNoStr, 32);
EEPROMWriteArray(Item4Address, OperatorNameStr, 32);
EEPROMWriteArray(Item5Address, DepartmentStr, 32);
//EEPROMWriteArray(Item6Address, print_add1, 32);
//EEPROMWriteArray(Item7Address, print_add2, 32);
EEPROMWriteArray(Item6Address, ItemStr, 5);
EEPROMWriteArray(Item7Address, ItemStr, 5);
EEPROMWriteArray(Item8Address, ItemStr, 5);
EEPROMWriteArray(Item9Address, ItemStr, 5);
EEPROMWriteWord(200, Beepsld1Address);
EEPROMWriteWord(400, Beepsld2Address);
BEEP_TIME = 200;
TIMER_BASE = 400;
/*
EEPROMWriteByte(0x11, SettingPasswordAddress);
EEPROMWriteByte(0x91, SettingPasswordAddress+1);
EEPROMWriteByte(0x19, SettingPasswordAddress+2);
*/
EEPROMWriteByte(0x07, SettingPasswordAddress);
EEPROMWriteByte(0x01, SettingPasswordAddress+1);
EEPROMWriteByte(0x79, SettingPasswordAddress+2);
EEPROMWriteByte(0x11, AdvanceSettingPasswordAddress);
EEPROMWriteByte(0x21, AdvanceSettingPasswordAddress+1);
EEPROMWriteByte(0x12, AdvanceSettingPasswordAddress+2);
EEPROMWriteByte(0x33, updateVision);//俄文版本
EEPROMWriteByte(0, DryorWetAddress);
EEPROMWriteByte(0x2B, DrysignAddress);
EEPROMWriteByte(7, DrydataAddress);
EEPROMWriteByte(0x2B, WetsignAddress);
EEPROMWriteByte(0, WetdataAddress);
}
void Markjudgement(void)
{


unsigned char decyear,decmonth,decday;
// 如果超出标定期限，则先显示请标定界面。
// 当前时间日期 - 标定时时间日期为设定月份
// 或当前记录条数减去标定时条数大于设定条数则提示标定
	/*

    EETemp0 = EEPROMReadWord(AdjustMthDataAddress);
Nop();
Nop();
	if(EEPROMReadByte(MarkYrDataAddress) <_time.yr)
	{
		
		EETemp1 = ((_time.yr-EEPROMReadByte(MarkYrDataAddress)-1)*12+_time.mth-1+13-EEPROMReadByte(MarkMthDataAddress))*30+_time.day-EEPROMReadByte(MarkDayDataAddress);
		if(EETemp1>EETemp0)
			Demarcate = TRUE;
	}
	else if(EEPROMReadByte(MarkYrDataAddress) ==_time.yr)
		{
		  
		if((EEPROMReadByte(MarkMthDataAddress) <_time.mth))
        EETemp1=(_time.mth-EEPROMReadByte(MarkMthDataAddress)-1)*30+_time.day+31-EEPROMReadByte(MarkDayDataAddress);
       		if(EETemp1>EETemp0)
			Demarcate = TRUE;

	}
	// 如果超出标定期限，则先显示请标定界面。

	TTTemp0=EEPROMReadWord(AdjustTimesDataAddress);
	TTTemp1=EEPROMReadWord(TestTimesDataAddress);
	if(TTTemp0<=TTTemp1 )
		Demarcate = TRUE;
	*////次数取消	
	decyear = (_time.yr >> 4)*10+ (_time.yr & 0xF);
	decmonth = (_time.mth >> 4)*10+ (_time.mth & 0xF);
	decday = (_time.day >> 4)*10+ (_time.day & 0xF);

#ifdef TradeMark_Version
	if((EEPROMReadByte(inspectYearAddress)+1) <decyear)
		Demarcate = TRUE;
	else if((EEPROMReadByte(inspectYearAddress)+1) ==decyear)
#else
	if(EEPROMReadByte(inspectYearAddress) <decyear)
		Demarcate = TRUE;
	else if(EEPROMReadByte(inspectYearAddress) ==decyear)
#endif	
		{
		if(EEPROMReadByte(inspectMonthAddress) <decmonth)
			Demarcate = TRUE;

		else if(EEPROMReadByte(inspectMonthAddress) ==decmonth)
			{
                             
			 if(EEPROMReadByte(inspectDayAddress) <decday)
					 Demarcate = TRUE;

		}
	    }



	if(Demarcate)
	{
	
		SetColor(BLACK); //WHITE
		ClearDevice(); 


		MoveTo(25,100);
		SetColor(BRIGHTRED);
		SetFont((void*)&FONTDEFAULT);// FONTMEDIUM
	    while(!OutText((XCHAR*)PeriodsText));

		MoveTo(48,150);
		SetColor(BRIGHTRED);
		SetFont((void*)&FONTDEFAULT);// FONTMEDIUM
	    while(!OutText((XCHAR*)PeriodsText2));

		MoveTo(30,180);
		SetColor(BRIGHTRED);
		SetFont((void*)&FONTDEFAULT);// FONTMEDIUM
	    while(!OutText((XCHAR*)PeriodsText3));
		
		
	    while(TouchGetX() == -1)
			{
			Beep();
			delay(500);
	    	}
	    delay(500);
	}



}
XCHAR DisplayUnitStr[10];
void DisplayThickness(XCHAR* ThicknessTemp)
{
//	XCHAR ThicknessTemp;
	switch(Flashbuff[9])//EEPROMReadByte(ThicknessAddress))
	{
		case 0:	//实际为(ID_RADIOBUTTONStar + 7)
		ThicknessTemp[0] = mg_100mlStr[0];//'m';//mg_100mlStr;	// alcohol_unit;// 浓度单位0:mg/100ml
		ThicknessTemp[1] = mg_100mlStr[1];//'g';
		ThicknessTemp[2] = mg_100mlStr[2];//'/';
		ThicknessTemp[3] = mg_100mlStr[3];//'1';
		ThicknessTemp[4] = mg_100mlStr[4];//'0';
		ThicknessTemp[5] = mg_100mlStr[5];//'0';
		ThicknessTemp[6] = mg_100mlStr[6];//'m';
		ThicknessTemp[7] = mg_100mlStr[7];//'L';
		ThicknessTemp[8] = 0;
		break;
		case 1:	//实际为(ID_RADIOBUTTONStar + 8)
		ThicknessTemp[0] = 0x00AC;//mg_lStr;	// alcohol_unit;// 浓度单位0:mg/100ml
		ThicknessTemp[1] = 0x00A3;
		ThicknessTemp[2] = 0x002F;
		ThicknessTemp[3] = 0x00AB;
		ThicknessTemp[4] = 0;
		break;
		case 2:	//实际为(ID_RADIOBUTTONStar + 9)
		ThicknessTemp[0] = '%';//mg_100Str;	// alcohol_unit;// 浓度单位0:mg/100ml
		ThicknessTemp[1] = 0;
		break;
		case 3:	//实际为(ID_RADIOBUTTONStar + 10)
//		ThicknessTemp = g_100mlStr;	// alcohol_unit;// 浓度单位0:mg/100ml
		ThicknessTemp[0] = g_100mlStr[0];//'g';
		ThicknessTemp[1] = g_100mlStr[1];//'/';
		ThicknessTemp[2] = g_100mlStr[2];//'1';
		ThicknessTemp[3] = g_100mlStr[3];//'0';
		ThicknessTemp[4] = g_100mlStr[4];//'0';
		ThicknessTemp[5] = g_100mlStr[5];//'m';
		ThicknessTemp[6] = g_100mlStr[6];//'L';
		ThicknessTemp[7] = 0;
		break;
		case 4:	//实际为(ID_RADIOBUTTONStar + 11)
//		ThicknessTemp = mg_1000mlStr;	// alcohol_unit;// 浓度单位0:mg/100ml
		ThicknessTemp[0] = 0x00C2;//千分号
		ThicknessTemp[1] = 0;
		ThicknessTemp[2] = 0;
		break;
		case 5:	//实际为(ID_RADIOBUTTONStar + 12)
		ThicknessTemp[0] = ug_100mlStr[0];//'u';//ug_100mlStr;	// alcohol_unit;// 浓度单位0:mg/100ml
		ThicknessTemp[1] = ug_100mlStr[1];//'g';
		ThicknessTemp[2] = ug_100mlStr[2];//'/';
		ThicknessTemp[3] = ug_100mlStr[3];//'1';
		ThicknessTemp[4] = ug_100mlStr[4];//'0';
		ThicknessTemp[5] = ug_100mlStr[5];//'0';
		ThicknessTemp[6] = ug_100mlStr[6];//'m';
		ThicknessTemp[7] = ug_100mlStr[7];//'L';
		ThicknessTemp[8] = ug_100mlStr[8];//'L';
		ThicknessTemp[9] = 0;
		break;
		case 6:	//实际为(ID_RADIOBUTTONStar + 13)
		ThicknessTemp[0] = 'm';//mg_mlStr;	// alcohol_unit;// 浓度单位0:mg/100ml
		ThicknessTemp[1] = 'g';
		ThicknessTemp[2] = '/';
		ThicknessTemp[3] = 'm';
		ThicknessTemp[4] = 'L';
		ThicknessTemp[5] = 0;
		break;
		case 7:	//实际为(ID_RADIOBUTTONStar + 14)
		ThicknessTemp[0] = 'g';//mg_mg100Str;	// alcohol_unit;// 浓度单位0:mg/100ml
		ThicknessTemp[1] = '/';
		ThicknessTemp[2] = 'L';
		ThicknessTemp[3] = 0;
		break;
		case 8:	//实际为(ID_RADIOBUTTONStar + 15)
//		ThicknessTemp = ug_lStr;	// alcohol_unit;// 浓度单位0:ug_lStr
		ThicknessTemp[0] = ug_lStr[0];//'g';
		ThicknessTemp[1] = ug_lStr[1];//'/';
		ThicknessTemp[2] = ug_lStr[2];//'2';
		ThicknessTemp[3] = ug_lStr[3];//'1';
		ThicknessTemp[4] = ug_lStr[4];//'0';
		ThicknessTemp[5] = 0;
		break;
/*		case 16:	//实际为(ID_RADIOBUTTONStar + 16)
		0;	// alcohol_unit;// 浓度单位0:mg/100ml
		break;*/
		default:
			break;
	}
//	Flashbuff[9] = ThicknessTemp;// 保存对应的单位

}

void GetThickness(void)
{
	XCHAR ThicknessTemp;
	switch(EEPROMReadByte(ThicknessAddress))
	{
		case 7:	//实际为(ID_RADIOBUTTONStar + 7)
		ThicknessTemp = 0;	// alcohol_unit;// 浓度单位0:mg/100ml
		break;
		case 8:	//实际为(ID_RADIOBUTTONStar + 8)
		ThicknessTemp = 1;	// alcohol_unit;// 浓度单位0:mg/100ml
		break;
		case 9:	//实际为(ID_RADIOBUTTONStar + 9)
		ThicknessTemp = 2;	// alcohol_unit;// 浓度单位0:mg/100ml
		break;
		case 10:	//实际为(ID_RADIOBUTTONStar + 10)
		ThicknessTemp = 3;	// alcohol_unit;// 浓度单位0:mg/100ml
		break;
		case 11:	//实际为(ID_RADIOBUTTONStar + 11)
		ThicknessTemp = 4;	// alcohol_unit;// 浓度单位0:mg/100ml
		break;
		case 12:	//实际为(ID_RADIOBUTTONStar + 12)
		ThicknessTemp = 5;	// alcohol_unit;// 浓度单位0:mg/100ml
		break;
		case 13:	//实际为(ID_RADIOBUTTONStar + 13)
		ThicknessTemp = 6;	// alcohol_unit;// 浓度单位0:mg/100ml
		break;
		case 14:	//实际为(ID_RADIOBUTTONStar + 14)
		ThicknessTemp = 7;	// alcohol_unit;// 浓度单位0:mg/100ml
		break;
		case 15:	//实际为(ID_RADIOBUTTONStar + 15)
		ThicknessTemp = 8;	// alcohol_unit;// 浓度单位0:mg/100ml
		break;
/*		case 16:	//实际为(ID_RADIOBUTTONStar + 16)
		0;	// alcohol_unit;// 浓度单位0:mg/100ml
		break;*/
		default:
			break;
	}
	Flashbuff[9] = ThicknessTemp;// 保存对应的单位
	DisplayThickness(DisplayUnitStr);

}

void writerecord(void)
{
	unsigned char j,k,f,cs=0;
	unsigned char recordbuff[256];
	unsigned int page,addr,i;

	serialnumber = EEPROMReadWord(EESERIAL_NUMBER);
	wordval.Val = (serialnumber);  //hex_bcd2(serialnumber);	record serial number ,type is unsigned int
	Flashbuff[0] = 0x00;
	Flashbuff[1] = wordval.v[1];
	Flashbuff[2] = wordval.v[0];
	Flashbuff[3] = _time.yr;// sys_datetime.year;// dateTimeStr
	Flashbuff[4] = _time.mth;// sys_datetime.month;
	Flashbuff[5] = _time.day;// sys_datetime.day;
	Flashbuff[6] = _time.hr;// sys_datetime.hour;
	Flashbuff[7] = _time.min;// sys_datetime.minute;
	Flashbuff[8] = _time.sec;// sys_datetime.second;	

	GetThickness();//Flashbuff[9]单位
//Flashbuff[10],Flashbuff[11]  测试结果
//Flashbuff[12]测试结果类型?吹起中断，拒绝测试，数值结果
    
    Flashbuff[13] = Temperature();
    Flashbuff[14] = Kt;

if(PeakScreen==0xaa)
   Flashbuff[15] = 0xaa;
else 
   Flashbuff[15] = 0;
	Flashbuff[16] = ST_IRstatus;// 测试模式
	
    Flashbuff[46]=EEPROMReadByte(MarkYrDataAddress);

    Flashbuff[47]=EEPROMReadByte(MarkMthDataAddress);

    Flashbuff[48]=EEPROMReadByte(MarkDayDataAddress);

	Flashbuff[253]=EEPROMReadByte(inspectYearAddress);

    Flashbuff[254]=EEPROMReadByte(inspectMonthAddress);

    Flashbuff[255]=EEPROMReadByte(inspectDayAddress);

    for(i=0;i<=255;i++)
	recordbuff[i]=(char)Flashbuff[i];
	if(serialnumber>0x4000) cs=1; //cs flash2
	page = (serialnumber-1)/2;	
	addr = ((serialnumber-1)%2)*256;//in fact,only write 240bytes 
//	memset(&Flashbuff[0],0x01,RecordSizeMax);
	flash_buffer(0x53,page,cs);
	write_bufferstart(0x84,addr,cs); //p1 is flash1,0x84 is buff1,93 is buff1 addr
	write_buffer(&recordbuff[0],RecordSizeMax);//ptra is buffer address,33 is write length
	write_bufferend(cs);
	solid_data(0x83,page,cs);//0 is flash1,0x83 is buff1,yy is page no.
	Nop();
	serialnumberRecord = serialnumber;
	serialnumber++;
	FLASH_RD(page,addr,RecordSizeMax,&Flashbuff[0],cs);
	Nop();
	Nop();
	//if(serialnumber > 0x8000) serialnumber = 0;
	if(serialnumber > 0x7530)//if >30000  record to be 1
		{
		  serialnumber = 1;
		  EEPROMWriteWord(Code_OVER,Record_over_30000);

		}
	EEPROMWriteWord(serialnumber,EESERIAL_NUMBER);
}

/******************************************************************************
 * Function:        BYTE readrecord(char *inbuffer, char *outbuffer)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        readrecord from flash,the record content lenth have240bytes
 *
 * Note:            
 *
 *****************************************************************************/
void readrecord(void)
{
	unsigned char recordbuff[256]={0},cs=0;
	unsigned int page,addr,i;
	page = (serialnumberRecord-1)/2;
	addr = ((serialnumberRecord-1)%2)*256;
	FLASH_RD(page,addr,RecordSizeMax,&recordbuff[0],cs);
	 for(i=0;i<=255;i++)
	Flashbuff[i]=(XCHAR)recordbuff[i];
	 
}//end read record

#endif
void initimer1(void)
{
	//init TIMER2,timer2 interrupt is for system 1ms clk
	T1CON = 0;
	TMR1 = 0;
	PR1 = 0x3e80; //fcy=16MHZ
	_T1IP = 4;
	_T1IF = 0;
	T1CON = 0x8000;
#ifdef  USE_GPS
	_T1IE = 1;
#else
    _T1IE = 0;
#endif
	//timer2 interrupt prority is level 7
	//enable timer2 interrupt
}

#if 0
void initPrint(void)
{
//	unsigned int j;
     inti_port();    //初始化端口
     //SPIMPolInit();  //初始化SPI模块
     
     //ADCInit();       //初始化ADC
//     inti_time4();  //初始化Timer4
     inti_time5();  //初始化Timer5
/*     VH_ON = 1;      //开VS和VH
     MOTOR_IN1=0;
     asm("nop");
     MOTOR_IN2=0;
     asm("nop");
     MOTOR_IN3=0;
     asm("nop");
     MOTOR_IN4=0;

     for(j = 0; j < 8; j++)
     {
         SPI_PRN_FeedPaper_DotLine();
     }

     VH_ON = 0;      //关VS和VH
     */
}
#endif

/////////////////////////////////////////////////////////////////////////////
//                                  MAIN
/////////////////////////////////////////////////////////////////////////////
int main(void){
//uch str2[]={"Wellcome to shen zhen well co.LTD"};
/******************************************/
//new
// Configure Input Functions (Table 9-1))
// Assign SDI To Pin RP26
RPINR22bits.SDI2R = 26;
// Assign SCK To Pin RP21
RPOR10bits.RP21R = 11;
// Configure Output Functions (Table 9-2)
// Assign SDO To Pin RP23
RPOR9bits.RP19R = 10;

//uart4
// Assign U4RX To Pin RP10
RPINR27bits.U4RXR = 10;
// Assign U4TX To Pin RP17
RPOR8bits.RP17R = 30;

//uart1 ===PRINTER
// Assign U1RX To Pin RP6
RPINR18bits.U1RXR = 6;
// Assign U1TX To Pin RP18
RPOR9bits.RP18R = 3;

//int2 RP29
RPINR1bits.INT2R = 29;



// Lock Registers
asm volatile ( "MOV #OSCCON, w1 \n"
"MOV #0x46, w2 \n"
"MOV #0x57, w3 \n"
"MOV.b w2, [w1] \n"
"MOV.b w3, [w1] \n"
"BSET OSCCON, #6" );
/**********************************************/
    InitializeSystem();//use for USB
#if defined(USB_INTERRUPT)
        USBDeviceAttach();
#endif
  //SR=0x0080;    
unsigned long int i;
unsigned int j_print;
DelayMs(300);


GOL_MSG msg;        			// GOL message structure to interact with GOL
//PORTFbits.RF3 = 0;
/////////////////////////////////////////////////////////////////////////////
// ADC Explorer 16 Development Board Errata (work around 2)
// RB15 should be output
/////////////////////////////////////////////////////////////////////////////
//	LATDbits.LATD9 = 0;
//	TRISDbits.TRISD9 = 0;//changed by zyq
/////////////////////////////////////////////////////////////////////////////

#ifdef __PIC32MX__
    INTEnableSystemMultiVectoredInt();
    SYSTEMConfigPerformance(GetSystemClock());	
#endif

    EEPROMInit();   			// Initialize EEPROM
   	TickInit();     			// Start tick counter    
    GOLInit();      			// Initialize graphics library and crete default style scheme for GOL
    BEEP_TIME = EEPROMReadWord(Beepsld1Address);
    TIMER_BASE = EEPROMReadWord(Beepsld2Address);
	BeepInit();     			// Initialize beeper
     adcinit();
    InitializeADS7843();
    InitializeINT2();
initialUART1();
//initialUART3();
   inti_time3();

#ifdef  USE_GPS
	initimer1( );

	initialUART4();
#endif	
	initI2();
	Nop();
	Nop();
	Nop();
	Nop();
	error_i2();
//	while(1)
	{
	RTCRead(&timeSet[0]);
	Nop();
	Nop();
	
	Nop();
	Nop();
		}

    RTCCProcessEvents();
//            Temp_adc=Adc_Count(9,5);
//            Temp_adc+=150;
    //adcinit();
inti_time5();
//  initimer2();
inti_ads7822();
TRISFbits.TRISF1=0;
TRISFbits.TRISF0=0;
pumpinit();
TRISAbits.TRISA6=0;
PORTAbits.RA6 = 0;

delay(200);
//serialnumber = EEPROMReadWord(EESERIAL_NUMBER);
PORTAbits.RA6 = 1;

    // If S3 button on Explorer 16 board is pressed calibrate touch screen
    if(PORTDbits.RD6 == 0)
    	{
        TouchCalibration();
        TouchStoreCalibration();
    }

    // If it's a new board (EEPROM_VERSION byte is not programed) calibrate touch screen
    if(GRAPHICS_LIBRARY_VERSION != EEPROMReadWord(EEPROM_VERSION))
{ 
       InitialSETTING();
	DefaultSETTING();
       TouchCalibration();
       TouchStoreCalibration();
       EEPROMWriteWord(GRAPHICS_LIBRARY_VERSION,EEPROM_VERSION);
       EEPROMWriteWord(1,EESERIAL_NUMBER);// 写一作为记录的条数开始
    }

if(0x33!=EEPROMReadByte(updateVision))
{
      
	   DefaultSETTING();

}
    // Load touch screen calibration parameters from EEPROM
    TouchLoadCalibration();
    serialnumber = EEPROMReadWord(EESERIAL_NUMBER);//该写的记录条目
	Record_Over_Flag = EEPROMReadWord(Record_over_30000);
	Marktimes=EEPROMReadWord(TestTimesDataAddress);			
    serialnumberRecord = serialnumber - 1;// 保持记录一致，最后一条记录的序号，显示用
    if((Record_Over_Flag == 0xaaaa)&&(serialnumber == 1))
		serialnumberRecord = 1;
    memset(&Flashbuff[0],0,256);
    POWER_CONTROL = 1;	//for the Power On, add by Spring.chen 


   #ifdef CHANGE4800
    _TRISG0 = 0;
	PORTGbits.RG0=1;//2011.04.19
   #endif
	//ChangeBRG();
#ifndef CER_MODE
     Tem_Detect = Adc_Count(7,5);
	if(Tem_Detect<=0x0088)
	{
    SetColor(BLACK);
    ClearDevice();
		MoveTo(30,100);
		SetColor(BRIGHTRED);
		SetFont((void*)&FONTMEDIUM);// FONTMEDIUM
	       while(!OutText((XCHAR*)TemperatureExtremelow));
		    DelayMs(2000);
			POWER_CONTROL = 0;//开关机
		}
	else if(Tem_Detect>=0x0129)
		{
		SetColor(BLACK);
		ClearDevice();
	MoveTo(30,100);
	SetColor(BRIGHTRED);
	SetFont((void*)&FONTMEDIUM);// FONTMEDIUM
	   while(!OutText((XCHAR*)TemperatureExtremehigh));
	    DelayMs(2000);
		POWER_CONTROL = 0;//开关机
	}
 #endif   
	Markjudgement();
    StartScreen();  // Show intro screen and wait for touch
    prepare_and_Charge();// PORTAbits.RA6 = 1;
    pump_reset=0;
    pump_on=1;
	delayms(1000);
	pump_reset=0;
    pump_on=0;

	// create the alternate schemes
	navScheme = GOLCreateScheme(); 		// alternative scheme for the navigate buttons
    altScheme = GOLCreateScheme(); 		// create alternative 1 style scheme
    alt2Scheme = GOLCreateScheme(); 	// create alternative 2 style scheme
    alt3Scheme = GOLCreateScheme(); 	// create alternative 3 style scheme
    alt4Scheme = GOLCreateScheme(); 	// create alternative 4 style scheme
    alt5Scheme = GOLCreateScheme();
    timeScheme = GOLCreateScheme();
    meterScheme = GOLCreateScheme(); 	// create meter scheme
    failScheme = GOLCreateScheme();
    redScheme = GOLCreateScheme();   	// create red style scheme
    greenScheme = GOLCreateScheme(); 	// create green style scheme
    yellowScheme = GOLCreateScheme(); 	// create yellow style scheme
    mainScheme = GOLCreateScheme(); 		// create alternative 1 style scheme
    timemainScheme = GOLCreateScheme();
	blackScheme= GOLCreateScheme();
#if (GRAPHICS_PICTAIL_VERSION == 1)
	/* for Microtips display */
	altScheme->Color0 = RGB565CONVERT(0x4C, 0x7E, 0xFF);
    altScheme->Color1 = RGB565CONVERT(0xFF, 0xCB, 0x3C);
	altScheme->EmbossDkColor = RGB565CONVERT(0x1E, 0x00, 0xE5);
	altScheme->EmbossLtColor = RGB565CONVERT(0xA9, 0xDB, 0xEF);
    altScheme->ColorDisabled = RGB565CONVERT(0xD4, 0xE1, 0xF7);
    altScheme->TextColor1 = RGB565CONVERT(0x4C, 0x7E, 0xFF);
    altScheme->TextColor0 = RGB565CONVERT(0xFF, 0xCB, 0x3C);
    altScheme->TextColorDisabled = RGB565CONVERT(0xB8, 0xB9, 0xBC);
#elif (GRAPHICS_PICTAIL_VERSION == 2)
	/* for Truly display */
	altScheme->Color0 = RGB565CONVERT(0x4C, 0x8E, 0xFF);
    altScheme->Color1 = RGB565CONVERT(0xFF, 0xBB, 0x4C);
	altScheme->EmbossDkColor = RGB565CONVERT(0x1E, 0x00, 0xE5);
	altScheme->EmbossLtColor = RGB565CONVERT(0xA9, 0xDB, 0xEF);
    altScheme->ColorDisabled = RGB565CONVERT(0xD4, 0xE1, 0xF7);

    altScheme->TextColor1 = BRIGHTBLUE;
    altScheme->TextColor0 = RGB565CONVERT(0xFF, 0xBB, 0x4C);
    altScheme->TextColorDisabled = RGB565CONVERT(0xB8, 0xB9, 0xBC);
#endif // GRAPHICS_PICTAIL_VERSION

#ifdef	USE_MULTIBYTECHAR
   altScheme->pFont = (void*)&GOLFontDefault;
	navScheme->pFont = (void*)ptrLargeAsianFont;
#else
//	altScheme->pFont = (void*)&GOLArial;
#endif

    alt2Scheme->TextColor1 = BRIGHTRED;
    alt2Scheme->TextColor0 = BRIGHTBLUE;
//    alt2Scheme->CommonBkColor = BLACK;
    blackScheme->EmbossDkColor=RGB565CONVERT(0xEF, 0xFE, 0xFF);
	blackScheme->EmbossLtColor=RGB565CONVERT(0xEF, 0xFE, 0xFF);
#ifndef	USE_MULTIBYTECHAR
	alt2Scheme->pFont = (void*)&GOLMediumFont;
#else
 	alt2Scheme->pFont = (void*)ptrSmallAsianFont;
#endif

//	mainScheme->TextColor1 = BLACK;//BRIGHTRED;
//	mainScheme->TextColor0 = BLACK;//WHITE;//BRIGHTRED;//BRIGHTBLUE;
//	mainScheme->CommonBkColor = BLACK;
	mainScheme->EmbossDkColor = BLACK;//RGB565CONVERT(0x1E, 0x00, 0xE5);
	mainScheme->EmbossLtColor = BLACK;//RGB565CONVERT(0xA9, 0xDB, 0xEF);

	mainScheme->pFont = (void*)&GOLMediumFont;



    alt3Scheme->Color0 = LIGHTBLUE; 		
    alt3Scheme->Color1 = BRIGHTGREEN;
    alt3Scheme->TextColor0 = BLACK;
    alt3Scheme->TextColor1 = WHITE;
#ifdef	USE_MULTIBYTECHAR
	alt3Scheme->pFont = (void*)ptrSmallAsianFont;
#else
	alt3Scheme->pFont = (void*)&GOLMediumFont;

#endif
    alt4Scheme->Color0 = LIGHTBLUE; 		
    alt4Scheme->Color1 = BRIGHTGREEN;
    alt4Scheme->TextColor0 = BLACK;
    alt4Scheme->TextColor1 = WHITE;
#ifdef USE_MULTIBYTECHAR
	alt4Scheme->pFont = (void*)ptrSmallAsianFont;
#else
	alt4Scheme->pFont = (void*)&FONTMEDIUM;	// GOLSmallFont;
#endif



	failScheme->Color0 = RGB565CONVERT(0xCC, 0x00, 0x00); 
    failScheme->Color1 = BRIGHTRED;
    failScheme->EmbossDkColor = RED4;
    failScheme->EmbossLtColor = FIREBRICK1;
    failScheme->TextColor0 = BRIGHTRED;//RGB565CONVERT(0xC8, 0xD5, 0x85); //显示字体的颜色
    failScheme->TextColor1 = BLACK;
    
    redScheme->Color0 = RGB565CONVERT(0xCC, 0x00, 0x00); 
    redScheme->Color1 = BRIGHTRED;
    redScheme->EmbossDkColor = RED4;
    redScheme->EmbossLtColor = FIREBRICK1;
    redScheme->TextColor0 = BRIGHTRED;//RGB565CONVERT(0xC8, 0xD5, 0x85); //显示字体的颜色
    redScheme->TextColor1 = BLACK;
    redScheme->pFont = (void*)&BigArial;

	greenScheme->Color0 = RGB565CONVERT(0x23, 0x9E, 0x0A); 
    greenScheme->Color1 = BRIGHTGREEN;
    greenScheme->EmbossDkColor = DARKGREEN;
    greenScheme->EmbossLtColor = PALEGREEN;
    greenScheme->TextColor0 = DARKGREEN;//BRIGHTGREEN;//RGB565CONVERT(0xDF, 0xAC, 0x83); 
    greenScheme->TextColor1 = BLACK; 
    greenScheme->pFont = (void*)&BigArial;//ptrBigArialFont;//
    /*
    greenScheme->Color0 = RGB565CONVERT(0x23, 0x9E, 0x0A); 
    greenScheme->Color1 = WHITE;
    greenScheme->EmbossDkColor = WHITE;
    greenScheme->EmbossLtColor = WHITE;
    greenScheme->TextColor0 = WHITE;//RGB565CONVERT(0xDF, 0xAC, 0x83); 
    greenScheme->TextColor1 = WHITE; 
    greenScheme->pFont = (void*)&BigArial;//ptrBigArialFont;//
*/
    yellowScheme->Color0 = BRIGHTYELLOW; 
    yellowScheme->Color1 = YELLOW;
    yellowScheme->EmbossDkColor = RGB565CONVERT(0xFF, 0x94, 0x4C);
    yellowScheme->EmbossLtColor = RGB565CONVERT(0xFD, 0xFF, 0xB2);
    yellowScheme->TextColor0 = RGB565CONVERT(0xAF, 0x34, 0xF3);
    yellowScheme->TextColor1 = RED;

    timeScheme->Color0 = BLACK; 		
    timeScheme->Color1 = WHITE;
    timeScheme->TextColor0 = BRIGHTBLUE;
    timeScheme->TextColor1 = WHITE;
    timeScheme->EmbossDkColor = GRAY20; 		
    timeScheme->EmbossLtColor = GRAY80;
//    timeScheme->CommonBkColor = BLACK;
    timeScheme->pFont = (void*)&GOLSmallFont;    

    meterScheme->Color0 = BLACK; 		
    meterScheme->Color1 = WHITE;
    meterScheme->TextColor0 = BRIGHTBLUE;
    meterScheme->TextColor1 = WHITE;
    meterScheme->EmbossDkColor = GRAY20; 		
    meterScheme->EmbossLtColor = GRAY80;    
    meterScheme->pFont = (void*)&GOLSmallFont;    

    timemainScheme->Color0 = BLACK; 		
    timemainScheme->Color1 = BLACK;//WHITE;
    timemainScheme->TextColor0 = WHITE;//BRIGHTBLUE;
    timemainScheme->TextColor1 = BLACK;//WHITE;
    timemainScheme->EmbossDkColor = GRAY20; 		
    timemainScheme->EmbossLtColor = GRAY80;
    timemainScheme->CommonBkColor = BLACK;
    timemainScheme->pFont = (void*)&GOLSmallFont;

//    alt5Scheme->Color0 = BLACK; 		
//    alt5Scheme->Color1 = BLACK;//WHITE;
    alt5Scheme->TextColor0 = BRIGHTBLUE;
    alt5Scheme->TextColor1 = WHITE;
    alt5Scheme->EmbossDkColor = GRAY20; 		
    alt5Scheme->EmbossLtColor = GRAY80;
//    alt5Scheme->pFont = (void*)&GOLSmallFont;
	unsigned char tt;//use for USB

//AD1PCFGbits.PCFG5 = 1;
  /*     
       CHK_PAPER_TRIS = 1;
       VH_ON_TRIS = 0;
       VH_ON = 1;
       CHK_PAPER=0;
       
       if(CHK_PAPER==1)
       	{
       	 DelayMs(100);
       	 if(CHK_PAPER==1&&CHK_PAPER_VAL==0)
       	 	{
       	 	CHK_PAPER_VAL=1;
           screenState = CREATE_MAIN;
           ErrorTrap(NoPaper);}
        }
       VH_ON = 0;// 关打印机
*/
       Masklevel = Maskleveltrigger;
#ifndef  TradeMark_Version
	   Manual_Analyze();
	   
	if(alocholdetecet) 

	   ErrorTrap(Alcoholdetected); 

	else screenState = CREATE_MAIN;
#else
       screenState = CREATE_MAIN;
#endif
	Masklevel = 0;
/*		
	EEPROMWriteByte(0x10, MarkYrDataAddress);//实际校准
	EEPROMWriteByte(0x02, MarkMthDataAddress);
	EEPROMWriteByte(0x08, MarkDayDataAddress);

EEPROMWriteByte(0X86,ID_JIQIADDRESS);
EEPROMWriteByte(0X00,ID_JIQIADDRESS+1);
EEPROMWriteByte(0X05,ID_JIQIADDRESS+2);

       EEPROMWriteWord(0x0092,0x6000);//-2温度
       EEPROMWriteWord(0x0d48,0x600e);//-2 AD

       EEPROMWriteWord(0x00ac,0x6002);//5 tem
       EEPROMWriteWord(0x1105,0x6010);//5 AD

       EEPROMWriteWord(0x00ca,0x6004);//15 tem
       EEPROMWriteWord(0x1367,0x6012);//15 AD

       EEPROMWriteWord(0x1498,0x6014);//22 AD
       EEPROMWriteWord(0x1498,0x6016);//28 AD

       EEPROMWriteWord(0x0107,0x600a);//35 tem
       EEPROMWriteWord(0x1546,0x6018);//35 AD

       EEPROMWriteWord(0x0126,0x600c);//45 tem
       EEPROMWriteWord(0x142e,0x601a);//45 AD

       */
       
if(EEPROMReadByte(PasswordsettedAddress)!=0xaa)
{


	EEPROMWriteByte(0x07, SettingPasswordAddress);
	EEPROMWriteByte(0x01, SettingPasswordAddress+1);
	EEPROMWriteByte(0x79, SettingPasswordAddress+2);
	EEPROMWriteByte(0x11, AdvanceSettingPasswordAddress);
	EEPROMWriteByte(0x21, AdvanceSettingPasswordAddress+1);
	EEPROMWriteByte(0x12, AdvanceSettingPasswordAddress+2);
	EEPROMWriteByte(0xaa, PasswordsettedAddress);	


} 

if(EEPROMReadByte(inspectdateSettedAddress)!=0xaa)	
{

EEPROMWriteByte(99, inspectYearAddress);//实际校准
EEPROMWriteByte(1, inspectMonthAddress);
EEPROMWriteByte(1, inspectDayAddress);

EEPROMWriteByte(0xaa, inspectdateSettedAddress);


}


    while(1){
		  
			if((sys_clk-USB_sys_clk)>1)
			{
       			USB_sys_clk = sys_clk;
			    ProcessIO();
			}      
		if (_URT4IN == 1)
		{
			_URT4IN = 0;
			u4_hand();
		}
	if((USBDeviceState ==POWERED_STATE)||(USBDeviceState == DEFAULT_STATE)||(USBSuspendControl==1)||(USBDeviceState==ATTACHED_STATE))
       {
       if(GOLDraw()){             // Draw GOL objects
       
            // Drawing is done here, process messages
            TouchGetMsg(&msg);     // Get message from touch screen
            GOLMsg(&msg);          // Process message
            SideButtonsMsg(&msg);  // Get message from side buttons
            GOLMsg(&msg);          // Process message
        }
       if((FirstIn)&&(tt>250)) FirstIn = FALSE;
        	}
    else if(!FirstIn)
		{
			screenState = CREATE_TO_PC;
			GOLDraw();
			FirstIn = TRUE;
			tt=0;
		}
		tt++;

    } 

}
/********************************************************************
 * Function:        static void InitializeSystem(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        InitializeSystem is a centralize initialization
 *                  routine. All required USB initialization routines
 *                  are called from here.
 *
 *                  User application initialization routine should
 *                  also be called from here.                  
 *
 * Note:            None
 *******************************************************************/
 static void InitializeSystem(void)
{

    
    UserInit();

    USBDeviceInit();	//usb_device.c.  Initializes USB module SFRs and firmware
    					//variables to known states.
}//end InitializeSystem

/******************************************************************************
 * Function:        void UserInit(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        This routine should take care of all of the demo code
 *                  initialization that is required.
 *
 * Note:            
 *
 *****************************************************************************/
void UserInit(void)
{
    //initialize the variable holding the handle for the last
    // transmission
    USBOutHandle = 0;
    USBInHandle = 0;
}//end UserInit
/********************************************************************
 * Function:        void ProcessIO(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        This function is a place holder for other user
 *                  routines. It is a mixture of both USB and
 *                  non-USB tasks.
 *
 * Note:            None
 *******************************************************************/
void ProcessIO(void)
{ 
  
	static unsigned char packetcnt;
    unsigned char pz;
//////////////////////////////////////////
	unsigned long j;
	static unsigned long startrecordnmb;
	WORD page,addr,x;
	static unsigned int length;
	static unsigned char buff[255];
	unsigned char cs=0;
	WORD_VAL wordval;
	unsigned char cmd1,cmd2,idlen,itemlen,itemnamelen;
	
//////////////////////////////////////////
    // User Application USB tasks
    if((USBDeviceState < CONFIGURED_STATE)||(USBSuspendControl==1)) return;

    if(!HIDRxHandleBusy(USBOutHandle))//Check if data was received from the host.
    {   
    	cmd1 = ReceivedDataBuffer[0];
		cmd2 = ReceivedDataBuffer[1];
		idlen = ReceivedDataBuffer[2];
		itemlen = ReceivedDataBuffer[3];
		itemnamelen = ReceivedDataBuffer[4];
        switch(cmd1)				//Look at the data the host sent, to see what kind of application specific command it sent.
        {
        case 0x01:
        	{
        	ToSendDataBuffer[0] = 0x01;
        	switch(cmd2)				//Look at the data the host sent, to see what kind of application specific command it sent.
        	{

        	case 0x01:
			
        			ToSendDataBuffer[1] = 0x01;
        			//RD_MAX_RECORD 2bytes
					wordval.Val = serialnumber-1; 
					ToSendDataBuffer[2] = wordval.v[1];
					ToSendDataBuffer[3] = wordval.v[0];
					//DEVICE ID    3btyes
                	EEPROMReadArray(0x6a00, &(ToSendDataBuffer[4]), idlen);
                	//CHECKITEM   10bytes
                	EEPROMReadArray(0x7e00, &(ToSendDataBuffer[4+idlen]), itemlen);//ITEM
                	//CALIBRATE DATE 3bytes
                	EEPROMReadArray(0x7f3d, &(ToSendDataBuffer[4+idlen+itemlen]), 3);//CALIBRATE DATE
            		//ITEM1 16bytes
            		EEPROMReadArray(0x7040, &(ToSendDataBuffer[7+idlen+itemlen]), itemnamelen);
				
            		break;

            case 0x02:
            		ToSendDataBuffer[1] = 0x02;
            		// ITEM2-3-4   48bytes
            		EEPROMReadArray(0x7040+itemnamelen, &(ToSendDataBuffer[2]), 62);
      				break;

      		case 0x03:
            		ToSendDataBuffer[1] = 0x03;
            		// ITEM5-6-7   48bytes
            		EEPROMReadArray(0x7040+(cmd2-2)*62+itemnamelen, &(ToSendDataBuffer[2]), 62);
      				break;
	
      		case 0x04:
            		ToSendDataBuffer[1] = 0x04;
            		// ITEM8-9-10   48bytes
            		EEPROMReadArray(0x7040+(cmd2-2)*62+itemnamelen, &(ToSendDataBuffer[2]), 62);
      			break;

			case 0x05:
            		ToSendDataBuffer[1] = 0x05;
            		// ITEM8-9-10   48bytes
            		EEPROMReadArray(0x7040+(cmd2-2)*62+itemnamelen, &(ToSendDataBuffer[2]), 62);
      			break;

			case 0x06:
            		ToSendDataBuffer[1] = 0x06;
            		// ITEM8-9-10   48bytes
            		EEPROMReadArray(0x7040+(cmd2-2)*62+itemnamelen, &(ToSendDataBuffer[2]), 62);
      			break;

			case 0x07:
            		ToSendDataBuffer[1] = 0x07;
            		// ITEM8-9-10   48bytes
            		EEPROMReadArray(0x7040+(cmd2-2)*62+itemnamelen, &(ToSendDataBuffer[2]), 62);
      			break;
	
            	}
            	if(!HIDTxHandleBusy(USBInHandle))
                {
                    USBInHandle = HIDTxPacket(HID_EP,(BYTE*)&ToSendDataBuffer[0],64);
                }

                break;
                }

	case 0x02://RD_RECORD
		{
        	ToSendDataBuffer[0] = 0x02;
        	switch(ReceivedDataBuffer[1])				//Look at the data the host sent, to see what kind of application specific command it sent.
        	{
        	case 0x01:
        		ToSendDataBuffer[1] = 0x01;
        		wordval.v[1] = ReceivedDataBuffer[2];
			wordval.v[0] = ReceivedDataBuffer[3];
			x = wordval.Val;
			if(Record_Over_Flag != Code_OVER)
				{
			if(x < serialnumber) 
			{
			page = (x-1)/2;	
			addr = ((x-1)%2)*256;//in fact,only write 240bytes 
			if(serialnumber>0x4000) cs=1; //cs flash2
			FLASH_RD(page,addr,240,buff,cs);
			memcpy(&ToSendDataBuffer[2],&buff[0],62);
			}
				}
			else
			{
            page = (x-1)/2;	
			addr = ((x-1)%2)*256;//in fact,only write 240bytes 
			if(serialnumber>0x4000) cs=1; //cs flash2
			FLASH_RD(page,addr,240,buff,cs);
			memcpy(&ToSendDataBuffer[2],&buff[0],62);  

			}
            		break;
            	case 0x02:
            		ToSendDataBuffer[1] = 0x02;
            		memcpy(&ToSendDataBuffer[2],&buff[62],62);
      			break;
      		case 0x03:
            		ToSendDataBuffer[1] = 0x03;
            		memcpy(&ToSendDataBuffer[2],&buff[124],62);
      			break;	
      		case 0x04:
            		ToSendDataBuffer[1] = 0x04;
            		memcpy(&ToSendDataBuffer[2],&buff[186],62);
      			break;
      		}
      		if(!HIDTxHandleBusy(USBInHandle))
                {
                    USBInHandle = HIDTxPacket(HID_EP,(BYTE*)&ToSendDataBuffer[0],64);
                }
                break;
        	}
        case 0x03://set device id
        	{
        	ToSendDataBuffer[1] = 0x03;	
        	//DEVICE ID    3btyes
                EEPROMWriteArray(0x6a00, &(ReceivedDataBuffer[2]), 3);
                if(!HIDTxHandleBusy(USBInHandle))
                {
                    USBInHandle = HIDTxPacket(HID_EP,(BYTE*)&ToSendDataBuffer[0],64);
                }	
        	break;	
        	}
        case 0x04://clear record
        	{
        	ToSendDataBuffer[1] = 0x04;
        	EEPROMWriteWord(1,EESERIAL_NUMBER);// 写一作为记录的条数开始
        	EEPROMWriteWord(0,Record_over_30000);//
		serialnumber = 1;//EEPROMReadWord(EESERIAL_NUMBER);
		serialnumberRecord = serialnumber - 1;// 保持记录一致
		if(!HIDTxHandleBusy(USBInHandle))
                {
                    USBInHandle = HIDTxPacket(HID_EP,(BYTE*)&ToSendDataBuffer[0],64);
                }
        	break;	
        	}
        }  
        //Re-arm the OUT endpoint for the next packet
        USBOutHandle = HIDRxPacket(HID_EP,(BYTE*)&ReceivedDataBuffer,64);
    }    

}//end ProcessIO


// ******************************************************************************************************
// ************** USB Callback Functions ****************************************************************
// ******************************************************************************************************
// The USB firmware stack will call the callback functions USBCBxxx() in response to certain USB related
// events.  For example, if the host PC is powering down, it will stop sending out Start of Frame (SOF)
// packets to your device.  In response to this, all USB devices are supposed to decrease their power
// consumption from the USB Vbus to <2.5mA each.  The USB module detects this condition (which according
// to the USB specifications is 3+ms of no bus activity/SOF packets) and then calls the USBCBSuspend()
// function.  You should modify these callback functions to take appropriate actions for each of these
// conditions.  For example, in the USBCBSuspend(), you may wish to add code that will decrease power
// consumption from Vbus to <2.5mA (such as by clock switching, turning off LEDs, putting the
// microcontroller to sleep, etc.).  Then, in the USBCBWakeFromSuspend() function, you may then wish to
// add code that undoes the power saving things done in the USBCBSuspend() function.

// The USBCBSendResume() function is special, in that the USB stack will not automatically call this
// function.  This function is meant to be called from the application firmware instead.  See the
// additional comments near the function.

/******************************************************************************
 * Function:        void USBCBSuspend(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Call back that is invoked when a USB suspend is detected
 *
 * Note:            None
 *****************************************************************************/
void USBCBSuspend(void)
{
	//Example power saving code.  Insert appropriate code here for the desired
	//application behavior.  If the microcontroller will be put to sleep, a
	//process similar to that shown below may be used:
	
	//ConfigureIOPinsForLowPower();
	//SaveStateOfAllInterruptEnableBits();
	//DisableAllInterruptEnableBits();
	//EnableOnlyTheInterruptsWhichWillBeUsedToWakeTheMicro();	//should enable at least USBActivityIF as a wake source
	//Sleep();
	//RestoreStateOfAllPreviouslySavedInterruptEnableBits();	//Preferrably, this should be done in the USBCBWakeFromSuspend() function instead.
	//RestoreIOPinsToNormal();									//Preferrably, this should be done in the USBCBWakeFromSuspend() function instead.

	//IMPORTANT NOTE: Do not clear the USBActivityIF (ACTVIF) bit here.  This bit is 
	//cleared inside the usb_device.c file.  Clearing USBActivityIF here will cause 
	//things to not work as intended.	
	

    #if defined(__C30__)
    #if 0
        U1EIR = 0xFFFF;
        U1IR = 0xFFFF;
        U1OTGIR = 0xFFFF;
        IFS5bits.USB1IF = 0;
        IEC5bits.USB1IE = 1;
        U1OTGIEbits.ACTVIE = 1;
        U1OTGIRbits.ACTVIF = 1;
        Sleep();
    #endif
    #endif
}



/******************************************************************************
 * Function:        void USBCBWakeFromSuspend(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        The host may put USB peripheral devices in low power
 *					suspend mode (by "sending" 3+ms of idle).  Once in suspend
 *					mode, the host may wake the device back up by sending non-
 *					idle state signalling.
 *					
 *					This call back is invoked when a wakeup from USB suspend 
 *					is detected.
 *
 * Note:            None
 *****************************************************************************/
void USBCBWakeFromSuspend(void)
{
	// If clock switching or other power savings measures were taken when
	// executing the USBCBSuspend() function, now would be a good time to
	// switch back to normal full power run mode conditions.  The host allows
	// a few milliseconds of wakeup time, after which the device must be 
	// fully back to normal, and capable of receiving and processing USB
	// packets.  In order to do this, the USB module must receive proper
	// clocking (IE: 48MHz clock must be available to SIE for full speed USB
	// operation).
}

/********************************************************************
 * Function:        void USBCB_SOF_Handler(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        The USB host sends out a SOF packet to full-speed
 *                  devices every 1 ms. This interrupt may be useful
 *                  for isochronous pipes. End designers should
 *                  implement callback routine as necessary.
 *
 * Note:            None
 *******************************************************************/
void USBCB_SOF_Handler(void)
{
    // No need to clear UIRbits.SOFIF to 0 here.
    // Callback caller is already doing that.
}

/*******************************************************************
 * Function:        void USBCBErrorHandler(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        The purpose of this callback is mainly for
 *                  debugging during development. Check UEIR to see
 *                  which error causes the interrupt.
 *
 * Note:            None
 *******************************************************************/
void USBCBErrorHandler(void)
{
    // No need to clear UEIR to 0 here.
    // Callback caller is already doing that.

	// Typically, user firmware does not need to do anything special
	// if a USB error occurs.  For example, if the host sends an OUT
	// packet to your device, but the packet gets corrupted (ex:
	// because of a bad connection, or the user unplugs the
	// USB cable during the transmission) this will typically set
	// one or more USB error interrupt flags.  Nothing specific
	// needs to be done however, since the SIE will automatically
	// send a "NAK" packet to the host.  In response to this, the
	// host will normally retry to send the packet again, and no
	// data loss occurs.  The system will typically recover
	// automatically, without the need for application firmware
	// intervention.
	
	// Nevertheless, this callback function is provided, such as
	// for debugging purposes.
}


/*******************************************************************
 * Function:        void USBCBCheckOtherReq(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        When SETUP packets arrive from the host, some
 * 					firmware must process the request and respond
 *					appropriately to fulfill the request.  Some of
 *					the SETUP packets will be for standard
 *					USB "chapter 9" (as in, fulfilling chapter 9 of
 *					the official USB specifications) requests, while
 *					others may be specific to the USB device class
 *					that is being implemented.  For example, a HID
 *					class device needs to be able to respond to
 *					"GET REPORT" type of requests.  This
 *					is not a standard USB chapter 9 request, and 
 *					therefore not handled by usb_device.c.  Instead
 *					this request should be handled by class specific 
 *					firmware, such as that contained in usb_function_hid.c.
 *
 * Note:            None
  *****************************************************************************/
void USBCBCheckOtherReq(void)
{
    USBCheckHIDRequest();

}//end


/*******************************************************************
 * Function:        void USBCBStdSetDscHandler(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        The USBCBStdSetDscHandler() callback function is
 *					called when a SETUP, bRequest: SET_DESCRIPTOR request
 *					arrives.  Typically SET_DESCRIPTOR requests are
 *					not used in most applications, and it is
 *					optional to support this type of request.
 *
 * Note:            None
 *****************************************************************************/
void USBCBStdSetDscHandler(void)
{
    // Must claim session ownership if supporting this request
}//end


/******************************************************************************
 * Function:        void USBCBInitEP(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        This function is called when the device becomes
 *                  initialized, which occurs after the host sends a
 * 					SET_CONFIGURATION (wValue not = 0) request.  This 
 *					callback function should initialize the endpoints 
 *					for the device's usage according to the current 
 *					configuration.
 *
 * Note:            None
 *****************************************************************************/
void USBCBInitEP(void)
{
    //enable the HID endpoint
    USBEnableEndpoint(HID_EP,USB_IN_ENABLED|USB_OUT_ENABLED|USB_HANDSHAKE_ENABLED|USB_DISALLOW_SETUP);
    //Re-arm the OUT endpoint for the next packet
    USBOutHandle = HIDRxPacket(HID_EP,(BYTE*)&ReceivedDataBuffer,64);

}

/********************************************************************
 * Function:        void USBCBSendResume(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        The USB specifications allow some types of USB
 * 					peripheral devices to wake up a host PC (such
 *					as if it is in a low power suspend to RAM state).
 *					This can be a very useful feature in some
 *					USB applications, such as an Infrared remote
 *					control	receiver.  If a user presses the "power"
 *					button on a remote control, it is nice that the
 *					IR receiver can detect this signalling, and then
 *					send a USB "command" to the PC to wake up.
 *					
 *					The USBCBSendResume() "callback" function is used
 *					to send this special USB signalling which wakes 
 *					up the PC.  This function may be called by
 *					application firmware to wake up the PC.  This
 *					function should only be called when:
 *					
 *					1.  The USB driver used on the host PC supports
 *						the remote wakeup capability.
 *					2.  The USB configuration descriptor indicates
 *						the device is remote wakeup capable in the
 *						bmAttributes field.
 *					3.  The USB host PC is currently sleeping,
 *						and has previously sent your device a SET 
 *						FEATURE setup packet which "armed" the
 *						remote wakeup capability.   
 *
 *					This callback should send a RESUME signal that
 *                  has the period of 1-15ms.
 *
 * Note:            Interrupt vs. Polling
 *                  -Primary clock
 *                  -Secondary clock ***** MAKE NOTES ABOUT THIS *******
 *                   > Can switch to primary first by calling USBCBWakeFromSuspend()
 
 *                  The modifiable section in this routine should be changed
 *                  to meet the application needs. Current implementation
 *                  temporary blocks other functions from executing for a
 *                  period of 1-13 ms depending on the core frequency.
 *
 *                  According to USB 2.0 specification section 7.1.7.7,
 *                  "The remote wakeup device must hold the resume signaling
 *                  for at lest 1 ms but for no more than 15 ms."
 *                  The idea here is to use a delay counter loop, using a
 *                  common value that would work over a wide range of core
 *                  frequencies.
 *                  That value selected is 1800. See table below:
 *                  ==========================================================
 *                  Core Freq(MHz)      MIP         RESUME Signal Period (ms)
 *                  ==========================================================
 *                      48              12          1.05
 *                       4              1           12.6
 *                  ==========================================================
 *                  * These timing could be incorrect when using code
 *                    optimization or extended instruction mode,
 *                    or when having other interrupts enabled.
 *                    Make sure to verify using the MPLAB SIM's Stopwatch
 *                    and verify the actual signal on an oscilloscope.
 *******************************************************************/
void USBCBSendResume(void)
{
    static WORD delay_count;
    
    USBResumeControl = 1;                // Start RESUME signaling
    
    delay_count = 1800U;                // Set RESUME line for 1-13 ms
    do
    {
        delay_count--;
    }while(delay_count);
    USBResumeControl = 0;
}


/*******************************************************************
 * Function:        BOOL USER_USB_CALLBACK_EVENT_HANDLER(
 *                        USB_EVENT event, void *pdata, WORD size)
 *
 * PreCondition:    None
 *
 * Input:           USB_EVENT event - the type of event
 *                  void *pdata - pointer to the event data
 *                  WORD size - size of the event data
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        This function is called from the USB stack to
 *                  notify a user application that a USB event
 *                  occured.  This callback is in interrupt context
 *                  when the USB_INTERRUPT option is selected.
 *
 * Note:            None
 *******************************************************************/
BOOL USER_USB_CALLBACK_EVENT_HANDLER(USB_EVENT event, void *pdata, WORD size)
{
    switch(event)
    {
        case EVENT_CONFIGURED: 
            USBCBInitEP();
            break;
        case EVENT_SET_DESCRIPTOR:
            USBCBStdSetDscHandler();
            break;
        case EVENT_EP0_REQUEST:
            USBCBCheckOtherReq();
            break;
        case EVENT_SOF:
            USBCB_SOF_Handler();
            break;
        case EVENT_SUSPEND:
            USBCBSuspend();
            break;
        case EVENT_RESUME:
            USBCBWakeFromSuspend();
            break;
        case EVENT_BUS_ERROR:
            USBCBErrorHandler();
            break;
        case EVENT_TRANSFER:
            Nop();
            break;
        default:
            break;
    }      
    return TRUE; 
}
/** EOF main.c ***************************************************************/
/////////////////////////////////////////////////////////////////////////////
// Function: WORD GOLMsgCallback(WORD objMsg, OBJ_HEADER* pObj, GOL_MSG* pMsg)
// Input: objMsg - translated message for the object,
//        pObj - pointer to the object,
//        pMsg - pointer to the non-translated, raw GOL message 
// Output: if the function returns non-zero the message will be processed by default
// Overview: this function must be implemented by user. GOLMsg() function calls it each
//           time the valid message for the object received
/////////////////////////////////////////////////////////////////////////////
WORD GOLMsgCallback(WORD objMsg, OBJ_HEADER* pObj, GOL_MSG* pMsg){

    // beep if button is pressed
  //static unsigned int BlueToothcount = 0;  
    if(objMsg == BTN_MSG_PRESSED){
        Beep();
    }else{
        if(GetObjType(pObj) == OBJ_RADIOBUTTON){
            Beep();
        }else{
            if(GetObjType(pObj) == OBJ_CHECKBOX)
                Beep();
        }
    }

	if ((screenState & 0xF300) != 0xF300) {
		// check for time setting press, process only when not setting time and date
		if (objMsg == ST_MSG_SELECTED) {
		    /* note how the states are arranged in the enumeration, the display state is 
		       right after the create state. So at the time the static text box of the 
		       time is selected, the state is in one of the displays. So a minus one is needed
		       to adjust to go back to the create state of the current page.
		       After the date and time adjust screen is exited, the saved create state will
		       be entered as the next state. So we get the effect of going back to the previous
		       screen after date and time settings are done.
		    */ 	
			if ((GetObjID(pObj) == ID_STATICTEXT1) || (GetObjID(pObj) == ID_STATICTEXT2)) {
				prevState = screenState - 1;			// save the current create state
	    	    screenState = CREATE_DATETIME;			// go to date and time setting screen
		    	return 1;	
		    }
			if ((GetObjID(pObj) == ID_STATICTEXT4)|
				(GetObjID(pObj) == ID_STATICTEXT16)|(GetObjID(pObj) == ID_STATICTEXT17)|
				(GetObjID(pObj) == ID_STATICTEXT18)|(GetObjID(pObj) == ID_STATICTEXT19)|
				(GetObjID(pObj) == ID_STATICTEXT10)|(GetObjID(pObj) == ID_STATICTEXT11)|
				(GetObjID(pObj) == ID_STATICTEXT12)|(GetObjID(pObj) == ID_STATICTEXT13)|
				(GetObjID(pObj) == ID_STATICTEXT14)|(GetObjID(pObj) == ID_STATICTEXT15))
				{
				ItemID = GetObjID(pObj);
					prevState = screenState - 1;
					screenState = CREATE_PREPROCESS;
				}
	    } 
    }


// check if pull down menu is called
    if (GetObjID(pObj) == ID_WINDOW1) {
#ifndef	USE_SSD1289
	if ((objMsg == WND_MSG_TITLE) && (screenState != DISPLAY_PULLDOWN))
#endif
	{
	// check area of press
		    if ((pMsg->param1 <= 40) && (pMsg->param2 <= 40)) {
			    switch(screenState) {
					// pull down is disabled when setting date and time
			    	case CREATE_DATETIME: 						
    				case DISPLAY_DATETIME: 		
    				case DISPLAY_DATE_PDMENU: 
    				case SHOW_DATE_PDMENU:
    				case HIDE_DATE_PDMENU:
    					
    				case CREATE_UNIT_SCREEN:
    				case DISPLAY_UNIT_SCREEN:
    				case SHOW_UNIT_PDMENU:
    				case DISPLAY_UNIT_PDMENU:
    					return 0; 
    		    	default:	
		    			prevState = screenState;		// save the current create state
			    	 	break;
			   	}
#ifndef	USE_SSD1289
			    screenState = CREATE_PULLDOWN;			// go to show pulldown menu state 
#endif
			    
			    return 1;
			}
		}
	}
   
    // process messages for demo screens
    switch(screenState){
		

        // date and time settings display     
        case DISPLAY_DATETIME:
            return MsgDateTime(objMsg, pObj);
        case DISPLAY_DATE_PDMENU:
         	return MsgSetDate(objMsg, pObj, pMsg);  
    	case CREATE_DATETIME: 						
		case SHOW_DATE_PDMENU:
    	case HIDE_DATE_PDMENU:
//    		case CREATE_UNIT_SCREEN:
//    		case SHOW_UNIT_PDMENU:
    		return 0;

        case DISPLAY_MAIN:
            return MsgMain(objMsg, pObj);
	case DISPLAY_Alcoholdetect:
            return MsgAlcoholdetect(objMsg, pObj);
	case DISPLAY_ResidentAlcoholdetected:
            return MsgAlcoholdetected(objMsg, pObj);
/*			
       case DISPLAY_SUBJECT_NAME:

return MsgEditABC(objMsg, pObj, pMsg);

	case DISPLAY_PLACE:
		return MsgEditABC(objMsg, pObj, pMsg);

	case DISPLAY_VEHICLENO:
		return MsgEditABC(objMsg, pObj, pMsg);

	case DISPLAY_BADGENO:
		return MsgEditABC(objMsg, pObj, pMsg);

	case DISPLAY_OPERATORNAME:
		return MsgEditABC(objMsg, pObj, pMsg);

	case DISPLAY_DEPARTMENT:
		return MsgEditTest(objMsg, pObj, pMsg);
*/
	case DISPLAY_PREPROCESS:
		return MsgEditTest(objMsg, pObj, pMsg);
		
	case DISPLAY_RECORD:
		return MsgRecord(objMsg, pObj, pMsg);

	case  DISPLAY_JUMP:
		return MsgJumptox(objMsg,pObj,pMsg);	

	case DISPLAY_BLOW:
		return	MsgBlowPlease(objMsg, pObj);
	case DISPLAY_Datadeal:
		return	MsgDatadeal(objMsg, pObj);
	case DISPLAY_DISCONTINUED:
		return     MsgDISCONTINUED(objMsg, pObj, pMsg);

	case DISPLAY_SETTING:
		return MsgSetting(objMsg, pObj);

	case DISPLAY_SoundControl:
		return MsgSoundControl(objMsg, pObj,  pMsg);
		
	case DISPLAY_EDITEEPROM:
		return  MsgEditeeprom(objMsg,  pObj,  pMsg);
		
    case DISPLAY_DISPLAYID:
        return  MsgDisplayMachineID(objMsg, pObj);
		
	case DISPLAY_EDITID:
		return  MsgEditID(objMsg,  pObj,  pMsg);		

	case DISPLAY_GPS:
		return MsgSetting(objMsg, pObj);

#ifdef	USE_BACKLIGHT
	case DISPLAY_SETBACKLIGHT:
		g_standby_time = EEPROMReadWord(BackLightTimeAddress);
		return MsgSetBackLight(objMsg, pObj);
#endif

	case DISPLAY_UNIT_PDMENU:
//         	return MsgSetDate(objMsg, pObj, pMsg); 
         	return MsgUnitPDment(objMsg, pObj, pMsg);
	case DISPLAY_TO_PC:
		return MsgToPC(objMsg, pObj);
	case DISPLAY_RESULT:
		return MsgResult(objMsg, pObj);

	case DISPLAY_THICKNESS:
		return	MsgThickness(objMsg, pObj);
	case DISPLAY_PRINTSETTING:
		return	MsgPrintSetting(objMsg, pObj);
	case DISPLAY_DEMARCATE:
		return	MsgDemarcate(objMsg, pObj);
	case DISPLAY_DEMARCATE2:
		return	MsgDemarcate2(objMsg, pObj);
	case DISPLAY_EDITDEMARCATE1	:
		return MsgEditDemarcate(objMsg,pObj, pMsg);
	case DISPLAY_EDITDEMARCATE2	:
		return MsgEditDemarcate(objMsg,pObj, pMsg);	
	case DISPLAY_EDITDEMARCATE3	:
		return MsgEditDemarcate(objMsg,pObj, pMsg);	
	case DISPLAY_EDITDEMARCATE4	:
		return MsgEditDemarcate(objMsg,pObj, pMsg);	
	case DISPLAY_EDITDEMARCATE5	:
		return MsgEditDemarcate(objMsg,pObj, pMsg);		
	case DISPLAY_EDITDEMARCATE6	:
		return MsgEditDemarcate(objMsg,pObj, pMsg);	
	case DISPLAY_EDITMARK:
		return MsgEditMarkValue(objMsg,pObj, pMsg);	
	case DISPLAY_INPUT:
		return  MsgEditMark(objMsg, pObj, pMsg);
	case DISPLAY_Wait:	
		return  MsgWait(objMsg, pObj);
    case DISPLAY_Mark:
		return	MsgMark(objMsg, pObj);
    case DISPLAY_Choosegas:
		return	MsgChoosegas(objMsg, pObj);
    case DISPLAY_Gascal:
		return	MsggasCal(objMsg, pObj);
    case DISPLAY_Calresult:
		return	MsgCalTestresult(objMsg, pObj);		
    case DISPLAY_Caladjust:
		return	Msgcaladjust(objMsg, pObj);
	case DISPLAY_PASSWORD:
		if((PasswordBuffer[0] == PDread[0])&&(PasswordBuffer[1] == PDread[1])&&(PasswordBuffer[2] == PDread[2])\
            		&&(PasswordBuffer[3] == PDread[3])&&(PasswordBuffer[4] == PDread[4])&&(PasswordBuffer[5] == PDread[5]))


            		{
            		screenState = CREATE_AGENT;//CREATE_Choosegas;//CREATE_SETTING;
            		PasswordBuffer[0] = 0;
					TempbuffStr[0] = 0;
            		}
		return MsgPassword(objMsg, pObj, pMsg);
	case DISPLAY_ScreenPASSWORD:
			PDread[0] = (EEPROMReadByte(SettingPasswordAddress)&0xF0)>>4+0x30;
			PDread[1] = (EEPROMReadByte(SettingPasswordAddress)&0x0F)+0x30;
			PDread[2] = (EEPROMReadByte(SettingPasswordAddress+1)&0xF0)>>4+0x30;
			PDread[3] = (EEPROMReadByte(SettingPasswordAddress+1)&0x0F)+0x30;
			PDread[4] = (EEPROMReadByte(SettingPasswordAddress+2)&0xF0)>>4+0x30;
			PDread[5] = (EEPROMReadByte(SettingPasswordAddress+2)&0x0F)+0x30;
		if((PasswordBuffer[0] == PDread[0])&&(PasswordBuffer[1] == PDread[1])&&(PasswordBuffer[2] == PDread[2])\
            		&&(PasswordBuffer[3] == PDread[3])&&(PasswordBuffer[4] == PDread[4])&&(PasswordBuffer[5] == PDread[5]))
            		{
            			dif=1;
				screenState = CREATE_DELETE;
				YesNoNumber = 2;// 删除记录
            		PasswordBuffer[0] = 0;
					TempbuffStr[0] = 0;
            		}
		return MsgScreenPassword(objMsg, pObj, pMsg);

	case DISPLAY_MASTERPASSWORD:
			
		if((PasswordBuffer[0] == PDread[0])&&(PasswordBuffer[1] == PDread[1])&&(PasswordBuffer[2] == PDread[2])\
            		&&(PasswordBuffer[3] == PDread[3])&&(PasswordBuffer[4] == PDread[4])&&(PasswordBuffer[5] == PDread[5]))
            		{
			while( ADS7843_INT!=1);//wait button released
		
			while( ADS7843_INT!=1);//wait button released
            		screenState = CREATE_Master;//CREATE_SETTING;
            		PasswordBuffer[0] = 0;
					TempbuffStr[0] = 0;
            		}
		else  if((PasswordBuffer[0] == PDread2[0])&&(PasswordBuffer[1] == PDread2[1])&&(PasswordBuffer[2] == PDread2[2])\
            		&&(PasswordBuffer[3] == PDread2[3])&&(PasswordBuffer[4] == PDread2[4])&&(PasswordBuffer[5] == PDread2[5]))

            		{
            		while( ADS7843_INT!=1);//wait button released
            		screenState = CREATE_AGENT;//CREATE_Choosegas;//CREATE_SETTING;
            		PasswordBuffer[0] = 0;
					TempbuffStr[0] = 0;
            		}
            		
		return MsgMasterPassword(objMsg, pObj, pMsg);
		
		case DISPLAY_POWEROFF:
			return MsgPowerOFF(objMsg, pObj);

		case DISPLAY_REFUSE:
			return MsgRefuse(objMsg, pObj);
		
		case DISPLAY_BLOW_PRESS:
			return MsgBlowPress(objMsg, pObj);

		case DISPLAY_ITEM:
			return MsgItem(objMsg, pObj, pMsg);


		case DISPLAY_ITEM0:
			return MsgItemEditABC(objMsg, pObj, pMsg);
		case DISPLAY_ITEM1:
			return MsgItemEditABC(objMsg, pObj, pMsg);
		case DISPLAY_ITEM2:
			return MsgItemEditABC(objMsg, pObj, pMsg);
		case DISPLAY_ITEM3:
			return MsgItemEditABC(objMsg, pObj, pMsg);
		case DISPLAY_ITEM4:
			return MsgItemEditABC(objMsg, pObj, pMsg);
		case DISPLAY_ITEM5:
			return MsgItemEditABC(objMsg, pObj, pMsg);
		case DISPLAY_ITEM6:
			return MsgItemEditABC(objMsg, pObj, pMsg);
		case DISPLAY_ITEM7:
			return MsgItemEditABC(objMsg, pObj, pMsg);
		case DISPLAY_ITEM8:
			return MsgItemEditABC(objMsg, pObj, pMsg);
		case DISPLAY_ITEM9:
			return MsgItemEditABC(objMsg, pObj, pMsg);
		case DISPLAY_AGENT:
			return MsgAgent(objMsg, pObj);
		case DISPLAY_Master:
			return MsgMaster(objMsg, pObj);
		case DISPLAY_Setpassword:
			return MsgSetpassword(objMsg, pObj);
		case DISPLAY_InputSetpassword1:
			return MsgEditPassword(objMsg, pObj, pMsg);
		case DISPLAY_InputSetpassword2:
			return MsgEditPassword(objMsg, pObj, pMsg);
		case DISPLAY_OutcomeSetpassword:
			return MsgOutcomeSetpassword(objMsg, pObj);						
		case DISPLAY_RESET:
			return MsgYesNo(objMsg, pObj);
		case DISPLAY_DELETE:
			return MsgYesNo(objMsg, pObj);
        case DISPLAY_ChooseCal:
			return MsgChooseCal(objMsg, pObj);
		case DISPLAY_CYC:
			//return MsgCalibrate(objMsg, pObj);
			return MsgEditInspectDate( objMsg, pObj, pMsg);
		case DISPLAY_CALLOCK:
			return MsgCalLock(objMsg, pObj);			
		case DISPLAY_YESNO:
			return MsgYesNo(objMsg, pObj);
		case DISPLAY_AUTOOFF:
			return 1;
		case DISPLAY_BlueTooth:				
			return MsgBlueTooth(objMsg, pObj);

        default:
            // process message by default
            return 1;
    }
}
/////////////////////////////////////////////////////////////////////////////
// Function: WORD GOLDrawCallback()
// Output: if the function returns non-zero the draw control will be passed to GOL
// Overview: this function must be implemented by user. GOLDraw() function calls it each
//           time when GOL objects drawing is completed. User drawing should be done here.
//           GOL will not change color, line type and clipping region settings while 
//           this function returns zero.
/////////////////////////////////////////////////////////////////////////////
WORD GOLDrawCallback(){
static DWORD prevTick  = 0;  		// keeps previous value of tick
static DWORD prevTime  = 0;  		// keeps previous value of time tick
static DWORD prevBatteryTime  = 0;
static BYTE  direction = 1; 		// direction switch for progress bar
static BYTE  arrowPos  = 0;  		// arrows pictures position for custom control demo
static BYTE  pBDelay   = 40;  		// progress bar delay variable
OBJ_HEADER 	*pObj;					// used to change text in Window
SLIDER     	*pSld;					// used when updating date and time
LISTBOX    	*pLb;					// used when updating date and time
STATICTEXT  *pSt;
WORD  		i;
static BYTE  directionZYQ = 1;
static unsigned int Aircheckcount=0;
BYTE  printnum,pddata;
static  BYTE POWER_OFF=0;
static  BYTE offnum=0;
static  BYTE Bluetoothflag = 0;


	// update the time display
	if ((screenState & 0x0000F300) != 0x0000F300) {		// process only when NOT setting time and date
		if ((tick-prevTime) > 1000)
			{
			
			RTCCProcessEvents();						// update the date and time string variabes 


		//if((tick-prevBatteryTime) > 2000)
		if((screenState == CREATE_MAIN)|(screenState == DISPLAY_MAIN))
		{	
			BatteryValue = Adc_Count(4,5);
			ShowBattery(BatteryValue,BATT_TYPE_LITHIUM);//add by Spring.Chen
			prevBatteryTime = tick;
		if(sLatitude[11]!=0&&sLongitude[12]!=0)//&&exdisplay!=0xee)
		     { 	                   
			  PutImage(20,3,(void*)&gpslogo,IMAGE_NORMAL);
			  
			}
		if(sLatitude[11]==0&&sLongitude[12]==0)//&&exdisplay==0xee)
			{
			   
			   SetColor(BLACK)
			   Bar(20, 8, 50, 30);			 
			}
		if((PORTEbits.RE8)&&(!Bluetoothflag)&&(!PORTEbits.RE9))
			{
			
              	PutImage(55,3,(void*)&bluetooth,IMAGE_NORMAL);
			    Bluetoothflag = 0xaa;
				//U1BRG=103;


		    }
		else if(((!PORTEbits.RE8)&&(Bluetoothflag))||((PORTEbits.RE9)&&(Bluetoothflag)))
			  {

               
				SetColor(BLACK)
			    Bar(50, 3, 75, 30);
                Bluetoothflag = 0;
				//U1BRG=207;

		     }
		}

		g_rtc_counter++;
/*
        	if(g_backlight_time != 0 && g_rtc_counter == g_backlight_time)    //自动关背光 (g_light_time != 0) && 
	        {
			g_backlight_time = 0;	//恢复不跳转
			//_AD1IE = 1;	//开中断
	            screenState = CREATE_MAIN; 			// switch to next state
            //return 1;
	        }
	        */
        	if(alarmON)
        		Beep();


	        if((g_standby_time != 0 && g_rtc_counter >= g_standby_time) ) //自动关机
	        {
	            //PrescreenState = screenState;
	        	screenState = CREATE_AUTOOFF;//POWER_CONTROL = 0;
	        	AutooffSwitch = TRUE;
				g_standby_time =0;

	        }

			if(BoolBatteryOFF)
				{
				 POWER_CONTROL = 0;
				}
				


	       if(g_blow_time != 0 && g_rtc_counter == g_blow_time) //拒绝测试
	        {
	        	screenState = CREATE_REFUSE;
	        	g_blow_time = 0;
				
	        }

			i = 0;						
			while (i < 12) {
				dateTimeStr[i] = _time_str[i];
				dateTimeStr[i+13] = _date_str[i];
				i++;
			}
			dateTimeStr[12] = 0x000A; 	// (XCHAR)'\n';
			dateTimeStr[25] = 0x0000;	// (XCHAR)'\0';
			
			if (pObj = GOLFindObject(ID_STATICTEXT1)) {		// get the time display obj pointer
				StSetText((STATICTEXT *)pObj, dateTimeStr); // now display the new date & time
				SetState(pObj, ST_DRAW);					// redraw the time display
			}
			if (pObj = GOLFindObject(ID_STATICTEXTLON)) {		// get the time display obj pointer
			StSetText((STATICTEXT *)pObj, sLongitude); // now display the new date & time
			SetState(pObj, ST_DRAW);					// redraw the time display
			}
		if (pObj = GOLFindObject(ID_STATICTEXTLAT)) {		// get the time display obj pointer
			StSetText((STATICTEXT *)pObj, sLatitude); // now display the new date & time
			SetState(pObj, ST_DRAW);					// redraw the time display
			}
			prevTime = tick;							// reset tick timer
		}
	} else {											// process only when setting time and date
		if (screenState != DISPLAY_DATE_PDMENU) {		// do not update when pull down menus are on
			if ((tick-prevTime) > 1000){
			    updateDateTimeEb();						// update edit boxes for date and time settings
				prevTime = tick;						// reset tick timer
			}
		}
	}
	
    switch(screenState){

        case CREATE_DATETIME:
            CreateDateTime(); 							// create date and time demo
            screenState = DISPLAY_DATETIME; 			// switch to next state
            return 1;       							// draw objects created
            
	    case SHOW_DATE_PDMENU:
	    	ShowPullDownMenu(); 
	    	screenState = DISPLAY_DATE_PDMENU;
	    	return 1;    
	    	
		case HIDE_DATE_PDMENU:
			if (RemovePullDownMenu()) 
		        screenState = DISPLAY_DATETIME; 		// switch to next state
			return 1;

        case DISPLAY_DATE_PDMENU:
        	// this moves the slider and editbox for the date setting to 
        	// move while the up or down arrow buttons are pressed
        	if((tick-prevTick)>100) {
        		pLb = (LISTBOX*)GOLFindObject(ID_LISTBOX1);		
			    pSld = (SLIDER*)GOLFindObject(ID_SLIDER1);
				pObj = GOLFindObject(ID_BUTTON_DATE_UP);
				
            	if(GetState(pObj, BTN_PRESSED)) {
                	LbSetFocusedItem(pLb,LbGetFocusedItem(pLb)-1);
                	SetState(pLb, LB_DRAW_ITEMS);
                	SldSetPos(pSld,SldGetPos(pSld)+1);
                	SetState(pSld, SLD_DRAW_THUMB);
                }	
				pObj = GOLFindObject(ID_BUTTON_DATE_DN);
				
            	if(GetState(pObj, BTN_PRESSED)) {
	                LbSetFocusedItem(pLb,LbGetFocusedItem(pLb)+1);
    	            SetState(pLb, LB_DRAW_ITEMS);
        	        SldSetPos(pSld,SldGetPos(pSld)-1);
            	    SetState(pSld, SLD_DRAW_THUMB);
            	}
            	prevTick = tick; 
            }
            return 1;

       case DISPLAY_DATETIME:
//       	case DISPLAY_UNIT_SCREEN:

        	// Checks if the pull down menus are to be created or not
		    pObj = GOLFindObject(ID_BUTTON_MO);
		    if (GetState(pObj, BTN_PRESSED)) {
                screenState = SHOW_DATE_PDMENU;  	// change state 
		   		return 1;
		   	}
		    pObj = GOLFindObject(ID_BUTTON_YR);
		    if (GetState(pObj, BTN_PRESSED)) {
                screenState = SHOW_DATE_PDMENU;  	// change state 
		   		return 1;
		   	}
		    pObj = GOLFindObject(ID_BUTTON_DY);
		    if (GetState(pObj, BTN_PRESSED)) {
                screenState = SHOW_DATE_PDMENU;  	// change state 
		   		return 1;
		   	}
        	// this increments the values for the time settings
        	// while the + or - buttons are pressed
       		if((tick-prevTick)>200) {
	       		pObj = GOLFindObject(ID_BUTTONHR_P);
            	if(GetState(pObj, BTN_PRESSED)) {
	            	MsgDateTime(BTN_MSG_PRESSED, pObj);
	            }
				pObj = GOLFindObject(ID_BUTTONHR_M);
            	if(GetState(pObj, BTN_PRESSED)) {
	            	MsgDateTime(BTN_MSG_PRESSED, pObj);
	            }
				pObj = GOLFindObject(ID_BUTTONMN_P);
            	if(GetState(pObj, BTN_PRESSED)) {
	            	MsgDateTime(BTN_MSG_PRESSED, pObj);
	            }
				pObj = GOLFindObject(ID_BUTTONMN_M);
            	if(GetState(pObj, BTN_PRESSED)) {
	            	MsgDateTime(BTN_MSG_PRESSED, pObj);
	            }
				pObj = GOLFindObject(ID_BUTTONSC_P);
            	if(GetState(pObj, BTN_PRESSED)) {
	            	MsgDateTime(BTN_MSG_PRESSED, pObj);
	            }
				pObj = GOLFindObject(ID_BUTTONSC_M);
            	if(GetState(pObj, BTN_PRESSED)) {
	            	MsgDateTime(BTN_MSG_PRESSED, pObj);
	            }
	            prevTick = tick; 
	        }
	        return 1;

	case CREATE_MAIN:	
        	CreateMain();
			Bluetoothflag = 0;
        	screenState = DISPLAY_MAIN; 
        	return 1;    
        	
        case DISPLAY_MAIN:
	#ifdef TEST
		
          if(serialnumber>=16384)
			screenState = CREATE_RECORD;
		  else
		  	{
		  	 srand(tick);			 
		  	 Flashbuff[10] = rand()%10+0x10;
			 Flashbuff[11] = Flashbuff[10]+1;
		  	  writerecord();
		  	}
			
	#endif
            return 1; 									// redraw objects if needed
	 case CREATE_Alcoholdetect:
					  CreateAlcoholdetect();
					  screenState = DISPLAY_Alcoholdetect; 
					  return 1;    
					  
				  case DISPLAY_Alcoholdetect:
					  return 1; 
			
			  case CREATE_Aircheck:
					Aircheck();		
					  screenState = DISPLAY_Aircheck;
				       return 1;
			  case DISPLAY_Aircheck:
			  	    Masklevel = Maskleveltrigger;
			        Manual_Analyze();
					 mainScheme->pFont = (void*)&GOLMediumFont;
					 /*
					if(Flashbuff[10]==0&&Flashbuff[11]==0)					
						  screenState =CREATE_Waittest;
				else	  	screenState =CREATE_ResidentAlcoholdetected;
				*/
				    Masklevel = 0;
					if(alocholdetecet)					
						  screenState =CREATE_ResidentAlcoholdetected;
				    else	
						  screenState =CREATE_Waittest;
						 return 1;		  
			
			
			  case CREATE_ResidentAlcoholdetected:
				  ResidentAlcoholdetected();
				  screenState=DISPLAY_ResidentAlcoholdetected;
				return 1;  
				 case	DISPLAY_ResidentAlcoholdetected:
				  return 1;
/*
	case CREATE_SUBJECT_NAME:	
        	CreateEditABC(SubjectNameStr,SubjectNameMax);//MaxChar
        	screenState = DISPLAY_SUBJECT_NAME; 
        	return 1;    
        	
        case DISPLAY_SUBJECT_NAME:
            return 1; 									// redraw objects if needed

        case CREATE_PLACE:	
        	CreateEditABC(PlaceStr,PlaceMax);//MaxChar
        	screenState = DISPLAY_PLACE; 
        	return 1;    
        	
        case DISPLAY_PLACE:
            return 1; 									// redraw objects if needed

	case CREATE_VEHICLENO:	

        	CreateEditABC(VehicleNoStr,VehicleNoMax);//MaxChar

        	screenState = DISPLAY_VEHICLENO; 
        	return 1;    
        	
        case DISPLAY_VEHICLENO:
		//	pump_pwr=1;
			return 1; 									// redraw objects if needed

        case CREATE_BADGENO:	
        	CreateEditTest(BadgeNoStr,BadgeNoMax);//  1); //MaxChar
        	screenState = DISPLAY_BADGENO; 
        	return 1;    
        	
        case DISPLAY_BADGENO:
            return 1; 									// redraw objects if needed

        case CREATE_OPERATORNAME:	
        	CreateEditABC(OperatorNameStr,OperatorNameMax);//MaxChar
        	screenState = DISPLAY_OPERATORNAME; 
        	return 1;    
        	
        case DISPLAY_OPERATORNAME:
            return 1; 									// redraw objects if needed

        case CREATE_DEPARTMENT:	
        	CreateEditTest(DepartmentStr,DepartmentMax);//MaxChar
        	screenState = DISPLAY_DEPARTMENT; 
        	return 1;    
        	
        case DISPLAY_DEPARTMENT:
            return 1; 									// redraw objects if needed
*/    //110930
        case CREATE_PREPROCESS:
#ifdef USE_ITEM
		if((ItemID == ID_STATICTEXT10))
		{	
//			CreateEditTest(Item0Str,ItemMax);
			CreateEditABC(Item0Str,ItemMax);
			ItemID = ID_STATICTEXT10;
			screenState = DISPLAY_ITEM;
		}
		else if((ItemID == ID_STATICTEXT11))
		{
//			CreateEditTest(Item1Str,ItemMax);
			CreateEditABC(Item1Str,ItemMax);
			ItemID = ID_STATICTEXT11;
			screenState = DISPLAY_ITEM;
    		}
		else if((ItemID == ID_STATICTEXT12))
		{	
//			CreateEditTest(Item2Str,ItemMax);
			CreateEditABC(Item2Str,ItemMax);
			ItemID = ID_STATICTEXT12;
			screenState = DISPLAY_ITEM;
		}
		else if((ItemID == ID_STATICTEXT13))
		{
//			CreateEditTest(Item3Str,ItemMax);
			CreateEditABC(Item3Str,ItemMax);
			ItemID = ID_STATICTEXT13;
			screenState = DISPLAY_ITEM;
    		}
		else if((ItemID == ID_STATICTEXT14))
		{	
//			CreateEditTest(Item4Str,ItemMax);
			CreateEditABC(Item4Str,ItemMax);
			ItemID = ID_STATICTEXT14;
			screenState = DISPLAY_ITEM;
		}
		else if((ItemID == ID_STATICTEXT15))
		{
//			CreateEditTest(Item5Str,ItemMax);
			CreateEditABC(Item5Str,ItemMax);
			ItemID = ID_STATICTEXT15;
			screenState = DISPLAY_ITEM;
    		}
		else if((ItemID == ID_STATICTEXT16))
		{	
//			CreateEditTest(Item6Str,ItemMax);
			CreateEditABC(Item6Str,ItemMax);
			ItemID = ID_STATICTEXT16;
			screenState = DISPLAY_ITEM;
		}
		else if((ItemID == ID_STATICTEXT17))
		{
//			CreateEditTest(Item7Str,ItemMax);
			CreateEditABC(Item7Str,ItemMax);
			ItemID = ID_STATICTEXT17;
			screenState = DISPLAY_ITEM;
    		}
		else if((ItemID == ID_STATICTEXT18))
		{	
//			CreateEditTest(Item8Str,ItemMax);
			CreateEditABC(Item8Str,ItemMax);
			ItemID = ID_STATICTEXT18;
			screenState = DISPLAY_ITEM;
		}
		else if((ItemID == ID_STATICTEXT19))
		{
//			CreateEditTest(Item9Str,ItemMax);
			CreateEditABC(Item9Str,ItemMax);
			ItemID = ID_STATICTEXT19;
			screenState = DISPLAY_ITEM;
    		}
        	else
        	{
        		CreateEditTest(PaperNumStr,1);//(PreprocessStr,10);//MaxChar
        		screenState = DISPLAY_PREPROCESS;
        	}
#endif
        	return 1;    
        	
        case DISPLAY_PREPROCESS:
            return 1; 									// redraw objects if needed

	case CREATE_BLOW:

            CreateBlowPlease();
            PbSetPos(pProgressBar,100);
		Temp_adc=Adc_Count(9,5);
		initialpress =Temp_adc;//+ EEPROMReadWord(BlowPressDataAddress)/2;//Temp_adc;
		   if(TestMode==0xcc)
		   	{
		   	  Temp_adc+=30;
			  pBDelay =4;	
		   	} 
		   else
		   	{
				Temp_adc+=EEPROMReadWord(BlowPressDataAddress);//initialpress;
           		pBDelay = EEPROMReadWord(BlowTimeDataAddress);
		   } 
           g_blow_time = DelayRefussTime;
           status_Press=FALSE;
            screenState = DISPLAY_BLOW;
        	return 1;
        	
        case DISPLAY_BLOW:

        	 if((tick-prevTick)>pBDelay){

        	 if(press_test(0xaa))
        	 	{
                if(direction){
                    if(pProgressBar->pos == 0)
                    {
                        	direction = 1;    				// change direction
 //                       	PbSetPos(pProgressBar,100);
                        	directionZYQ = 0;
 				goto goto_test;
                    }
                    else
                    {
                        PbSetPos(pProgressBar,PbGetPos(pProgressBar)-1); // decrease
				if(pProgressBar->pos == 0)
				{
					directionZYQ = 0;
					direction = 1;
					epPress=Adc_Count(9,5);
					epPress=(epPress+initialpress)/2;//Adc_Count(9,5);
				}
				status_Press=TRUE;
				g_blow_time = 0;//吹气过程禁止拒绝测试界面出现
				Beep();
                    }
                    
                }
                SetState(pProgressBar,PB_DRAW_BAR); 	// redraw bar only
                prevTick = tick;
        	 	}
        	 else if(status_Press&&(pProgressBar->pos!=0))
        	 	{
	        	 	PbSetPos(pProgressBar,100);
				SetState(pProgressBar,PB_DRAW_BAR); 	// redraw bar only
				status_Press=FALSE;
				if(TestMode==0xcc)
					screenState = CREATE_BLOW;
				else
        	 	    screenState = CREATE_DISCONTINUED;
        	 		return 1;
        	 	}
        	 directionZYQ = 1;
			 return 1;

            }

goto_test:		if((!directionZYQ))
		{
            while(press_test(0xbb));
                screenState = CREATE_Datadeal;
			status_Press=TRUE;
		}
            return 1; 									// redraw objects if needed
            
        case CREATE_Datadeal:
        	CreateDatadeal();//
        	screenState = DISPLAY_Datadeal; 
        	return 1;    
        	
        case DISPLAY_Datadeal:

            if(TestMode==0xcc)
            Masklevel = Maskleveltrigger;
			
			Manual_Analyze();//调测试酒精浓度程序
            delayus(5);
			if(TestMode==0xaa||TestMode==0xbb)
			{
			Marktimes++;
			EEPROMWriteWord(Marktimes, TestTimesDataAddress);
			}
			delayus(10);// 恢复Turn on

			Masklevel = 0;
			
			if(TestMode == 0xdd)
			  screenState = CREATE_Calresult; 
			else
			screenState = CREATE_RESULT; 
			
            return 1;
       case CREATE_DISCONTINUED:
        	CreateDISCONTINUED();//
        	screenState = DISPLAY_DISCONTINUED; 
        	return 1;    
        	
        case DISPLAY_DISCONTINUED:
			g_blow_time = 0;//吹气过程禁止拒绝测试界面出现
            return 1; 
			
	case CREATE_RECORD:
        	CreateRecord();//MaxChar
        	screenState = DISPLAY_RECORD; 
        	return 1;
        case DISPLAY_RECORD:

#ifndef NO_BLUETOOTH_BUTTON
			if((!PORTEbits.RE8)&&(WndGetText(pWin)==RecordStr))
				{
				  //SetColor(YELLOW);
   				  //SetFont((void*)&BigArial);
				  //BtnSetText(pbutton, Sighstr);
				  WndSetText(pWin, RecordBKStr);

				  done = WndDraw(pWin);
				}
			if((PORTEbits.RE8)&&(WndGetText(pWin)==RecordBKStr))
				{
				  WndSetText(pWin, RecordStr);

				  done = WndDraw(pWin);
			}
#endif			
			if(ChoicePrint)
				{
			      Print_TestRecord();
				  /*
				  if((!PORTEbits.RE8)&&(!endprint))
				  	endprint = TRUE;
				  	*/
				  	
				}
			if((ChoicePrint)&&(GetState(pbutton,BTN_DRAW)))
				{
				SetState(pbutton, BTN_DISABLED);
				//BtnSetBitmap(pbutton, &bluetooth);
			    done = BtnDraw(pbutton);
				}
			if(endprint)
					{
					ClrState(pbutton, BTN_DISABLED);

					ClrState(pbutton, BTN_FOCUSED);
					//BtnSetBitmap(pbutton, &printerbutton);
					SetState(pbutton, BTN_DRAW);
					//memset(&rx1_buff[0],0,200);
                   // rx1_ptr = &rx1_buff[0];
					endprint = FALSE;

					Beep();
					}
			if(PrintSec==10)
				{
                   
                	ClrState(pbutton, BTN_DISABLED);

					ClrState(pbutton, BTN_FOCUSED);
					
					SetState(pbutton, BTN_DRAW);

					PrintSec=0;
					PrintCount = 0;
					ChoicePrint = FALSE;
					SPrint=FALSE;
			    }
			if((BTN_S3 == 0)&&(BTN_S6 == 0))
			{
                         if((BTN_S3 == 0)&&(BTN_S6 == 0))
		                {S4_count ++;}
	                 else
		                 {S4_count = 0;}
	                  if(S4_count >10)	//
	                  {
	                     screenState = CREATE_JUMP;//
                            S4_count = 0;
		            return 1;
					  }

			}
			else if((BTN_S6 == 0)&&(BTN_S3))
				{
		if(BTN_S6 == 0)
		      {S4_count ++;}
	        else
		      {S4_count = 0;}
	        if(S4_count >10)	//
	          {
	          if(Record_Over_Flag != Code_OVER)
	          {
                if((serialnumberRecord<serialnumber-10)&&(serialnumber>10))
            	{
            		serialnumberRecord +=10;//;= serialnumber
            		screenState = CREATE_RECORD;//
            	}
            	else if(serialnumberRecord<serialnumber-1)
            	{
            		serialnumberRecord = serialnumber-1;
            		screenState = CREATE_RECORD; 	// goto list box screen
            	}	
		        else if(serialnumberRecord==serialnumber-1)	
			    {
			            S4_count = 0;
                        return 0;
		        }
	          }
			  else
			  	{
					if(serialnumberRecord<=29990)
						{
                         serialnumberRecord +=10;//;= serialnumber
            		     screenState = CREATE_RECORD;//  
					}
					else if((serialnumberRecord>29990)&&(serialnumberRecord<=30000)){
						 serialnumberRecord -=29990;//;= serialnumber
            		     screenState = CREATE_RECORD;//
					}

			  }
		       S4_count = 0;
		       return 1;
	          }

			}
			else if((BTN_S3 == 0)&&(BTN_S6))
				{
		if(BTN_S3 == 0)
		      {S4_count ++;}
	        else
		      {S4_count = 0;}
	        if(S4_count >10)	//
	          {
	            if(serialnumberRecord>10)
            	{
            		serialnumberRecord-=10;//;= serialnumber
            		screenState = CREATE_RECORD;//
            	}
            	else  if(serialnumberRecord>1)
            	{
            	if(Record_Over_Flag != Code_OVER)
	                {
            		serialnumberRecord = 1;
            		screenState = CREATE_RECORD; 	
            		}
				else{
					serialnumberRecord += 29990;
            		screenState = CREATE_RECORD;
				}
            	}
		else if(serialnumberRecord==1)	
			{
			   if(Record_Over_Flag != Code_OVER)
	                {
			            S4_count = 0;
                        return 0;
			   	}
			   else{
					serialnumberRecord += 29990;
            		screenState = CREATE_RECORD;
			   }
			}
		       S4_count = 0;
		       return 1;
	          }
				}
            return 1; 									// redraw objects if needed
            
    case CREATE_JUMP:
		   CreateJumptox();
		   screenState = DISPLAY_JUMP;
		   return 1;
	case  DISPLAY_JUMP:
		   //updateJumptoxEb();
		   return 1;


	case CREATE_SETTING:	
        	CreateSetting();//MaxChar
        	screenState = DISPLAY_SETTING; 
        	return 1;    
        	
        case DISPLAY_SETTING:
            return 1; 									// redraw objects if needed

	case CREATE_EDITEEPROM:	
        	CreateEditeeprom(EEPROMEDIT,6);
        	screenState = DISPLAY_EDITEEPROM; 
        	return 1;    
        	
        case DISPLAY_EDITEEPROM:
            return 1; 									// redraw objects if needed


	case CREATE_DISPLAYID:	
        	CreateDisplayMachineID();
        	screenState = DISPLAY_DISPLAYID; 
        	return 1;    
        	
        case DISPLAY_DISPLAYID:
            return 1;
			
	case CREATE_EDITID:	
        	CreateEditTest(IDumberStr,6);
        	screenState = DISPLAY_EDITID; 
        	return 1;    
        	
        case DISPLAY_EDITID:
            return 1; 
			
	case CREATE_GPS:	
        	CreateGPS();//MaxChar
        	screenState = DISPLAY_GPS; 
        	return 1;    
        	
        case DISPLAY_GPS:
            return 1; 									// redraw objects if needed

#ifdef	USE_BACKLIGHT
	case CREATE_SETBACKLIGHT:	
        	CreateSetBackLight();//MaxChar
//        	CreatePicture();
        	screenState = DISPLAY_SETBACKLIGHT; 
        	return 1;    
        	
        case DISPLAY_SETBACKLIGHT:
            return 1; 
#endif

	case CREATE_TO_PC:	
        	CreateConnectToPC( );//To the pc
        	screenState = DISPLAY_TO_PC; 
        	return 1;    
        	
        case DISPLAY_TO_PC:
            return 1;

        case CREATE_RESULT:
        	g_backlight_time = 0;	//吹气成功恢复不跳转
        	g_blow_time = 0;		// 结果出来恢复正常
        	CreateResult( );
   /*     	 
	  if(EEPROMReadByte(ID_CHECKBOXADDRESS+ID_CHECKBOX7)&&(TestMode!=0xcc))//&&(!Pressflag))
	  	{
	  			SPrint=TRUE;
            	ChoicePrint = TRUE;
				Sprinnum = 0;
				PrintSec=0;
				PrintCount = 0;
	       		//memset(&Flashbuff[0],0,256);
				//memset(&tx1_buff[0],0,250);
				//memset(&rx1_buff[0],0,250);
				readrecord();
	 			for(i=0;i<=255;i++)
					Printbuff[i]=(unsigned char)Flashbuff[i];
    			Inputdata();
				p_n = 0;

	  }	
	  */
	  screenState = DISPLAY_RESULT;
        	return 1;    
        	
        case DISPLAY_RESULT:

	  if(screenState==DISPLAY_RESULT)

		   { 
		    BTN_S3 = 1;
	        DelayMs(10);
	        if(BTN_S3 == 0)
		      {S4_count ++;}
	        else
		      {S4_count = 0;}
	        BTN_S3 = 1;
	        if(S4_count >80)	// 长按5秒Debug
	          {
		       screenState=CREATE_Debug;
		       S4_count = 0;
		       return 1;
	          }

            }
	  if((!Pressflag)&&(TestMode!=0xcc)&&EEPROMReadByte(ID_CHECKBOXADDRESS+ID_CHECKBOX7))
	  	{
	  			SPrint=TRUE;
            	ChoicePrint = TRUE;
				Sprinnum = 0;
				PrintSec=0;
				PrintCount = 0;
	       		//memset(&Flashbuff[0],0,256);
				//memset(&tx1_buff[0],0,250);
				//memset(&rx1_buff[0],0,250);
				readrecord();
	 			for(i=0;i<=255;i++)
					Printbuff[i]=(unsigned char)Flashbuff[i];
    			Inputdata();
				p_n = 0;

				Pressflag=0xaa;

				SetState(pbutton, BTN_DISABLED);
				//BtnSetBitmap(pbutton, &bluetooth);
			    done = BtnDraw(pbutton);
				

	  }		  
	//if(Pressflag==0xaa)
		//{
			  
	    if(ChoicePrint)
				{
			      Print_TestRecord();
				  /*
				  if((!PORTEbits.RE8)&&(!endprint))
				  	endprint = TRUE;
				  	*/
				  	
				}
			if((ChoicePrint)&&(GetState(pbutton,BTN_DRAW)))
				{
				SetState(pbutton, BTN_DISABLED);
				//BtnSetBitmap(pbutton, &bluetooth);
			    done = BtnDraw(pbutton);
				}
			if(endprint)
					{
					ClrState(pbutton, BTN_DISABLED);

					ClrState(pbutton, BTN_FOCUSED);
					//BtnSetBitmap(pbutton, &printerbutton);
					SetState(pbutton, BTN_DRAW);
					//memset(&rx1_buff[0],0,200);
                   // rx1_ptr = &rx1_buff[0];
					endprint = FALSE;
                   // Pressflag = 0;
					Beep();
					}
		//}
	/*  
	  if(EEPROMReadByte(ID_CHECKBOXADDRESS+ID_CHECKBOX7)&&(TestMode!=0xcc)&&(!Pressflag))
	  	{
	  	readrecord();
	    ChoicePrint = TRUE;
	    initPrint();
	    if(Flashbuff[10]==0&&Flashbuff[11]==0)
                    printnum=EEPROMReadByte(PrintSelectAddress);
		else        printnum=EEPROMReadByte(PrintSelectAddress+1);

		if(printnum>0x39||printnum<0x30)
				 	printnum=0x30;
				 printnum=printnum-0x30;
				  if(printnum==0)
				  	{  
                   IPC6bits.T4IP = 4;	
                   TickInit();
		           initI2();
                   ChoicePrint = FALSE;
                   IEC1bits.T4IE = 1;
			   	      return 1;
				  	}
                 for(;printnum>0;printnum--)
                   Print_TestRecord();
                   IPC6bits.T4IP = 4;	
                   TickInit();
		           initI2();
                   ChoicePrint = FALSE;
                   IEC1bits.T4IE = 1;
				   screenState = CREATE_MAIN;
	  	}
	  	*/
			return 1;


	case CREATE_UNIT_SCREEN:	
        	CreateUnitPDmenu();//MaxChar
        	screenState = DISPLAY_UNIT_SCREEN; 
        	return 1;

        case DISPLAY_UNIT_SCREEN:
        	pObj = GOLFindObject(ID_BUTTON_UNIT);
		    if (GetState(pObj, BTN_PRESSED)) 
                	{
                		screenState = SHOW_UNIT_PDMENU;  	// change state 
		   		return 1;
		    	}
if((tick-prevTick)>200) {
	       		pObj = GOLFindObject(ID_BUTTONHR_P);
            	if(GetState(pObj, BTN_PRESSED)) {
	            	MsgDateTime(BTN_MSG_PRESSED, pObj);
	            }
				pObj = GOLFindObject(ID_BUTTONHR_M);
            	if(GetState(pObj, BTN_PRESSED)) {
	            	MsgDateTime(BTN_MSG_PRESSED, pObj);
	            }
				pObj = GOLFindObject(ID_BUTTONMN_P);
            	if(GetState(pObj, BTN_PRESSED)) {
	            	MsgDateTime(BTN_MSG_PRESSED, pObj);
	            }
				pObj = GOLFindObject(ID_BUTTONMN_M);
            	if(GetState(pObj, BTN_PRESSED)) {
	            	MsgDateTime(BTN_MSG_PRESSED, pObj);
	            }
				pObj = GOLFindObject(ID_BUTTONSC_P);
            	if(GetState(pObj, BTN_PRESSED)) {
	            	MsgDateTime(BTN_MSG_PRESSED, pObj);
	            }
				pObj = GOLFindObject(ID_BUTTONSC_M);
            	if(GetState(pObj, BTN_PRESSED)) {
	            	MsgDateTime(BTN_MSG_PRESSED, pObj);
	            }
	            prevTick = tick; 
	        }
		    return 1;
		   	

        	
        case SHOW_UNIT_PDMENU:
        	ShowUnitPullDownMeny();
        	screenState = DISPLAY_UNIT_PDMENU;  	// change state 
            return 1; 									// redraw objects if needed

        case DISPLAY_UNIT_PDMENU:
        	if((tick-prevTick)>100) {
        		pLb = (LISTBOX*)GOLFindObject(ID_LISTBOX4);		
			    pSld = (SLIDER*)GOLFindObject(ID_SLIDER4);
				pObj = GOLFindObject(ID_BUTTON_DATE_UP);
				
            	if(GetState(pObj, BTN_PRESSED)) {
                	LbSetFocusedItem(pLb,LbGetFocusedItem(pLb)-1);
                	SetState(pLb, LB_DRAW_ITEMS);
                	SldSetPos(pSld,SldGetPos(pSld)+1);
                	SetState(pSld, SLD_DRAW_THUMB);
                }	
				pObj = GOLFindObject(ID_BUTTON_DATE_DN);
				
            	if(GetState(pObj, BTN_PRESSED)) {
	                LbSetFocusedItem(pLb,LbGetFocusedItem(pLb)+1);
    	            SetState(pLb, LB_DRAW_ITEMS);
        	        SldSetPos(pSld,SldGetPos(pSld)-1);
            	    SetState(pSld, SLD_DRAW_THUMB);
            	}
            	prevTick = tick; 
            }
            return 1;
	case CREATE_THICKNESS:
		CreateThickness();// 英文用
//		CreateUnits();// 中文用
		screenState = DISPLAY_THICKNESS;
		return 1;    
	case DISPLAY_THICKNESS:
		return 1;
	case CREATE_PRINTSETTING:

		CreatePrintSetting();
		screenState = DISPLAY_PRINTSETTING;
		return 1;    
	case DISPLAY_PRINTSETTING:
		return 1;
		
	case CREATE_SoundControl:
		CreateSoundControl();
		screenState = DISPLAY_SoundControl;
		return 1;
	case DISPLAY_SoundControl:
		SoundControlupdate();
		return 1;

	case CREATE_DEMARCATE:
		Adj_tem = EEPROMReadWord(DemarcateAddress);
		Adj_adc = EEPROMReadWord(DemarcateAddress+14);
        Hexshow(DemarcateAD, Adj_tem);
		Hexshow(DemarcateAD1, Adj_adc);

        Adj_tem = EEPROMReadWord(DemarcateAddress+2);
		Adj_adc = EEPROMReadWord(DemarcateAddress+16);
        Hexshow(DemarcateAD2, Adj_tem);
		Hexshow(DemarcateAD3, Adj_adc);

        Adj_tem = EEPROMReadWord(DemarcateAddress+4);
		Adj_adc = EEPROMReadWord(DemarcateAddress+18);
        Hexshow(DemarcateAD4, Adj_tem);
		Hexshow(DemarcateAD5, Adj_adc);

        Adj_tem = EEPROMReadWord(DemarcateAddress+6);
		Adj_adc = EEPROMReadWord(DemarcateAddress+20);
        Hexshow(DemarcateAD6, Adj_tem);
		Hexshow(DemarcateAD7, Adj_adc);

        Adj_tem = EEPROMReadWord(DemarcateAddress+8);
		Adj_adc = EEPROMReadWord(DemarcateAddress+22);
        Hexshow(DemarcateAD8, Adj_tem);
		Hexshow(DemarcateAD9, Adj_adc);


        Adj_tem = EEPROMReadWord(DemarcateAddress+10);
		Adj_adc = EEPROMReadWord(DemarcateAddress+24);
        Hexshow(DemarcateAD10, Adj_tem);
		Hexshow(DemarcateAD11, Adj_adc);

		Adj_tem = EEPROMReadWord(DemarcateAddress+12);
		Adj_adc = EEPROMReadWord(DemarcateAddress+26);
        Hexshow(DemarcateAD12, Adj_tem);
		Hexshow(DemarcateAD13, Adj_adc);
		CreateDemarcate();
		screenState = DISPLAY_DEMARCATE;
		return 1;    
	case DISPLAY_DEMARCATE:
		return 1;
	case CREATE_DEMARCATE2:
		Adj_tem = EEPROMReadWord(DemarcateAddress);
		Adj_adc = EEPROMReadWord(DemarcateAddress+14);
        Hexshow(DemarcateAD, Adj_tem);
		Hexshow(DemarcateAD1, Adj_adc);

		Adj_tem = EEPROMReadWord(DemarcateAddress+2);
		Adj_adc = EEPROMReadWord(DemarcateAddress+16);
        Hexshow(DemarcateAD2, Adj_tem);
		Hexshow(DemarcateAD3, Adj_adc);

        Adj_tem = EEPROMReadWord(DemarcateAddress+4);
	    Adj_adc = EEPROMReadWord(DemarcateAddress+18);
        Hexshow(DemarcateAD4, Adj_tem);
		Hexshow(DemarcateAD5, Adj_adc);

        Adj_tem = EEPROMReadWord(DemarcateAddress+6);
	    Adj_adc = EEPROMReadWord(DemarcateAddress+20);
        Hexshow(DemarcateAD6, Adj_tem);
		Hexshow(DemarcateAD7, Adj_adc);

        Adj_tem = EEPROMReadWord(DemarcateAddress+8);
		Adj_adc = EEPROMReadWord(DemarcateAddress+22);
        Hexshow(DemarcateAD8, Adj_tem);
		Hexshow(DemarcateAD9, Adj_adc);


        Adj_tem = EEPROMReadWord(DemarcateAddress+10);
	    Adj_adc = EEPROMReadWord(DemarcateAddress+24);
        Hexshow(DemarcateAD10, Adj_tem);
		Hexshow(DemarcateAD11, Adj_adc);

	    Adj_tem = EEPROMReadWord(DemarcateAddress+12);
		Adj_adc = EEPROMReadWord(DemarcateAddress+26);
        Hexshow(DemarcateAD12, Adj_tem);
		Hexshow(DemarcateAD13, Adj_adc);

		CreateDemarcate2();
		screenState = DISPLAY_DEMARCATE2;
		return 1;
	case DISPLAY_DEMARCATE2:
		return 1;
		
    case CREATE_EDITDEMARCATE1:
		CreateEditeeprom(T1Str, 8);
		screenState = DISPLAY_EDITDEMARCATE1;
		return 1;
	case DISPLAY_EDITDEMARCATE1:
		return 1;

	case CREATE_EDITDEMARCATE2:
		CreateEditeeprom(T2Str, 8);
		screenState = DISPLAY_EDITDEMARCATE2;
		return 1;
	case DISPLAY_EDITDEMARCATE2:
		return 1;	

	case CREATE_EDITDEMARCATE3:
		CreateEditeeprom(T3Str, 8);
		screenState = DISPLAY_EDITDEMARCATE3;
		return 1;
	case DISPLAY_EDITDEMARCATE3:
		return 1;	

	case CREATE_EDITDEMARCATE4:
		CreateEditeeprom(T6Str, 8);
		screenState = DISPLAY_EDITDEMARCATE4;
		return 1;
	case DISPLAY_EDITDEMARCATE4:
		return 1;	

	case CREATE_EDITDEMARCATE5:
		CreateEditeeprom(T7Str, 8);
		screenState = DISPLAY_EDITDEMARCATE5;
		return 1;
	case DISPLAY_EDITDEMARCATE5:
		return 1;	

	case CREATE_EDITDEMARCATE6:
		CreateEditeeprom(T8Str, 8);
		screenState = DISPLAY_EDITDEMARCATE6;
		return 1;
	case DISPLAY_EDITDEMARCATE6:
		return 1;	

	case CREATE_EDITMARK:
		CreateEditeeprom(EDITMarkstr, 4);
		screenState = DISPLAY_EDITMARK;
		return 1;
	case DISPLAY_EDITMARK:
		return 1;		
	case CREATE_INPUT:
		CreateEditTest(INPUTStr,6);
		screenState = DISPLAY_INPUT;
		return 1;
	case DISPLAY_INPUT:
		return 1;
    case CREATE_Wait:
		CreateWait();
		//prepare_and_Charge();
		screenState = DISPLAY_Wait;
		return 1;
	case DISPLAY_Wait:
		prepare_and_Charge();
        screenState =CREATE_Mark;
		 return 1;
    
	case CREATE_Waittest:
		CreateWait();
		//prepare_and_Charge();
		screenState = DISPLAY_Waittest;
		return 1;
	case DISPLAY_Waittest:
		prepare_and_Charge();
        screenState =CREATE_BLOW;
			Nop();
        	Nop();
			pump_on =0;
			Nop();
			Nop();
		return 1;

	case CREATE_Waitdem:
		CreateWait();
		//prepare_and_Charge();
		screenState = DISPLAY_Waitdem;
		return 1;
	case DISPLAY_Waitdem:
		prepare_and_Charge();
        screenState =CREATE_DEMARCATE;
		return 1;	

	case CREATE_Waitdem2:
		CreateWait();
		//prepare_and_Charge();
		screenState = DISPLAY_Waitdem2;
		return 1;
	case DISPLAY_Waitdem2:
		prepare_and_Charge();
        screenState =CREATE_DEMARCATE2;
		return 1;

	
    case CREATE_Choosegas:

	  CreateChoosegas();
	  
      screenState =DISPLAY_Choosegas;
      return 1;
	 
	case  DISPLAY_Choosegas:
		return 1;

    case CREATE_Gascal:

		CreategasCal();

		screenState =DISPLAY_Gascal;
		
	    return 1;
	case  DISPLAY_Gascal:
		return 1;


    case  CREATE_Calresult:
		
          CreateCalTestresult();
		  
		  screenState =DISPLAY_Calresult;

		  return 1;

	case  DISPLAY_Calresult :

		  return 1;
		
    case CREATE_Mark:

        MarkAdc=EEPROMReadWord(0x6022);
		Hexshow(MarkAD2,MarkAdc);

        CreateMark();
		screenState = DISPLAY_Mark;
		return 1;   
	case DISPLAY_Mark:
         return 1;

	case CREATE_Caladjust:
		Createcaladjust();
        savetemp = EEPROMReadByte(DrysignAddress);
        DrygasadustStr[0] = (XCHAR)savetemp;
        Drygas_D = EEPROMReadByte(DrydataAddress);

        savetemp = EEPROMReadByte(WetsignAddress);
        WetgasadustStr[0] = (XCHAR)savetemp;
        Wetgas_d = EEPROMReadByte(WetdataAddress);		
		screenState = DISPLAY_Caladjust;
		return 1;    
	case DISPLAY_Caladjust:
		return 1;


	case CREATE_PASSWORD:
		CreateEditTest(PasswordStr,6);
		screenState = DISPLAY_PASSWORD;
		pddata=EEPROMReadByte(SettingPasswordAddress);
		PDread[0] = (pddata&0xF0)/16+0x30;
		PDread[1] = (pddata&0x0F)+0x30;
		pddata=EEPROMReadByte(SettingPasswordAddress+1);		
		PDread[2] = (pddata&0xF0)/16+0x30;
		PDread[3] = (pddata&0x0F)+0x30;
		pddata=EEPROMReadByte(SettingPasswordAddress+2);		
		PDread[4] = (pddata&0xF0)/16+0x30;
		PDread[5] = (pddata&0x0F)+0x30;		
		return 1;    
	case DISPLAY_PASSWORD:
		return 1;

   	case CREATE_ScreenPASSWORD:
		CreateEditTest(DeleteStr,6);
		screenState = DISPLAY_ScreenPASSWORD;
		return 1;    
	case DISPLAY_ScreenPASSWORD:
		return 1;
		
	case CREATE_MASTERPASSWORD:
		CreateEditTest(PasswordStr,6);
		screenState = DISPLAY_MASTERPASSWORD;
		PDread[0] = (EEPROMReadByte(AdvanceSettingPasswordAddress)&0xF0)/16+0x30;
		PDread[1] = (EEPROMReadByte(AdvanceSettingPasswordAddress)&0x0F)+0x30;
		PDread[2] = (EEPROMReadByte(AdvanceSettingPasswordAddress+1)&0xF0)/16+0x30;
		PDread[3] = (EEPROMReadByte(AdvanceSettingPasswordAddress+1)&0x0F)+0x30;
		PDread[4] = (EEPROMReadByte(AdvanceSettingPasswordAddress+2)&0xF0)/16+0x30;
		PDread[5] = (EEPROMReadByte(AdvanceSettingPasswordAddress+2)&0x0F)+0x30;

		pddata=EEPROMReadByte(SettingPasswordAddress);
		PDread2[0] = (pddata&0xF0)/16+0x30;
		PDread2[1] = (pddata&0x0F)+0x30;
		pddata=EEPROMReadByte(SettingPasswordAddress+1);		
		PDread2[2] = (pddata&0xF0)/16+0x30;
		PDread2[3] = (pddata&0x0F)+0x30;
		pddata=EEPROMReadByte(SettingPasswordAddress+2);		
		PDread2[4] = (pddata&0xF0)/16+0x30;
		PDread2[5] = (pddata&0x0F)+0x30;			
		return 1;    
	case DISPLAY_MASTERPASSWORD:
		return 1;

	case CREATE_POWEROFF:
		CreatePowerOff();
		BoolBatteryOFF = TRUE;
		screenState = DISPLAY_POWEROFF;
		return 1;
	case DISPLAY_POWEROFF:
		
		return 1;

	case CREATE_REFUSE:
		CreateRefuse();
		screenState = DISPLAY_REFUSE;
		return 1;
	case DISPLAY_REFUSE:
		
		return 1;
		
	case CREATE_BLOW_PRESS:
		CreateBlowPress();
		screenState = DISPLAY_BLOW_PRESS;
		return 1;
	case DISPLAY_BLOW_PRESS:
		return 1;
   case CREATE_Debug:

        Adj_tem = Decade[0];
		Adj_adc = Decade[1];
        Hexshow(DemarcateAD, Adj_tem);
		Hexshow(DemarcateAD1, Adj_adc);

		Adj_tem = Decade[2];
		Adj_adc = Decade[3];
        Hexshow(DemarcateAD2, Adj_tem);
		Hexshow(DemarcateAD3, Adj_adc);

        Adj_tem = Decade[4];
	    Adj_adc = Decade[5];
        Hexshow(DemarcateAD4, Adj_tem);
		Hexshow(DemarcateAD5, Adj_adc);
	

        Adj_tem = Decade[6];
	    Adj_adc = Decade[7];
        Hexshow(DemarcateAD6, Adj_tem);
		Hexshow(DemarcateAD7, Adj_adc);

		Adj_tem = Decade[8];
	    Adj_adc = Decade[9];
        Hexshow(DemarcateAD8, Adj_tem);
		Hexshow(DemarcateAD9, Adj_adc);

        Adj_tem = Decade[10];
	    Adj_adc = Decade[11];
        Hexshow(DemarcateAD10, Adj_tem);
		Hexshow(DemarcateAD11, Adj_adc);

        Adj_tem = Decade[12];
	    Adj_adc = Decade[13];
        Hexshow(DemarcateAD12, Adj_tem);
		Hexshow(DemarcateAD13, Adj_adc);

		Adj_tem = Decade[14];
	    Adj_adc = Decade[15];
        Hexshow(DemarcateAD14, Adj_tem);
		Hexshow(DemarcateAD15, Adj_adc);

		Adj_tem = Decade[16];
	    Adj_adc = Decade[17];
        Hexshow(DemarcateAD16, Adj_tem);
		Hexshow(DemarcateAD17, Adj_adc);

		Adj_tem = Decade[18];
	    Adj_adc = Decade[19];
        Hexshow(DemarcateAD18, Adj_tem);
		Hexshow(DemarcateAD19, Adj_adc);


		Adj_tem = EEPROMReadWord(ID_JIQIADDRESS+11);
	    
        Hexshow(DemarcateAD20, Adj_tem);
		
	    while(!BTN_S3); 
   	     CreateDebug();
		 screenState = DISPLAY_Debug;
		 return 1;
   		 
   case DISPLAY_Debug:

		while(BTN_S3);         
         screenState = CREATE_Debug2;
         

		 return 1;
   case CREATE_Debug2:

        Adj_tem = EEPROMReadWord(ID_JIQIADDRESS+3);
		Adj_adc = EEPROMReadWord(ID_JIQIADDRESS+7);
        Hexshow(DemarcateAD, Adj_tem);
		Hexshow(DemarcateAD1, Adj_adc);

		Adj_tem = EEPROMReadWord(ID_JIQIADDRESS+9);
		Adj_adc = EEPROMReadWord(ID_JIQIADDRESS+5);
        Hexshow(DemarcateAD2, Adj_tem);
		Hexshow(DemarcateAD3, Adj_adc);

        Adj_tem = EEPROMReadWord(ID_JIQIADDRESS+11);
	    Adj_adc = EEPROMReadWord(ID_JIQIADDRESS+13);
        Hexshow(DemarcateAD4, Adj_tem);
		Hexshow(DemarcateAD5, Adj_adc);
	

        Adj_tem = EEPROMReadWord(0x6020);
	    Adj_adc = EEPROMReadWord(0x6022);
        Hexshow(DemarcateAD6, Adj_tem);
		Hexshow(DemarcateAD7, Adj_adc);

        Adj_tem = EEPROMReadWord(ID_JIQIADDRESS+15);
	  
        Hexshow(DemarcateAD8, Adj_tem);
	
		
	    while(!BTN_S3); 
   	     CreateDebug2();
		 screenState = DISPLAY_Debug2;
		 return 1;
   		 
   case DISPLAY_Debug2:

		while(BTN_S3);         
         screenState =CREATE_DEMARCATEshow;
         

		 return 1;

   case CREATE_DEMARCATEshow:	

        Adj_tem = EEPROMReadWord(DemarcateAddress);
		Adj_adc = EEPROMReadWord(DemarcateAddress+14);
        Hexshow(DemarcateAD, Adj_tem);
		Hexshow(DemarcateAD1, Adj_adc);

		Adj_tem = EEPROMReadWord(DemarcateAddress+2);
		Adj_adc = EEPROMReadWord(DemarcateAddress+16);
        Hexshow(DemarcateAD2, Adj_tem);
		Hexshow(DemarcateAD3, Adj_adc);

        Adj_tem = EEPROMReadWord(DemarcateAddress+4);
	    Adj_adc = EEPROMReadWord(DemarcateAddress+18);
        Hexshow(DemarcateAD4, Adj_tem);
		Hexshow(DemarcateAD5, Adj_adc);

        Adj_tem = EEPROMReadWord(DemarcateAddress+6);
	    Adj_adc = EEPROMReadWord(DemarcateAddress+20);
        Hexshow(DemarcateAD6, Adj_tem);
		Hexshow(DemarcateAD7, Adj_adc);

        Adj_tem = EEPROMReadWord(DemarcateAddress+8);
		Adj_adc = EEPROMReadWord(DemarcateAddress+22);
        Hexshow(DemarcateAD8, Adj_tem);
		Hexshow(DemarcateAD9, Adj_adc);


        Adj_tem = EEPROMReadWord(DemarcateAddress+10);
	    Adj_adc = EEPROMReadWord(DemarcateAddress+24);
        Hexshow(DemarcateAD10, Adj_tem);
		Hexshow(DemarcateAD11, Adj_adc);

	    Adj_tem = EEPROMReadWord(DemarcateAddress+12);
		Adj_adc = EEPROMReadWord(DemarcateAddress+26);
        Hexshow(DemarcateAD12, Adj_tem);
		Hexshow(DemarcateAD13, Adj_adc);

		CreateDemarcateshow();
		screenState = DISPLAY_DEMARCATEshow;
		return 1;
	case DISPLAY_DEMARCATEshow:
        while(BTN_S3);
		screenState =CREATE_MAIN;
		return 1;

	case CREATE_ITEM:
#ifdef USE_ITEM
		CreateItem();
		screenState = DISPLAY_ITEM;
#endif
		return 1;
	case DISPLAY_ITEM:
		return 1;

	case CREATE_ITEM0:
#ifdef USE_ITEM
//		CreateItem();
		CreateEditABC(Item0Str,ItemMax);
		screenState = DISPLAY_ITEM0;
#endif
		return 1;
	case DISPLAY_ITEM0:
		return 1;

	case CREATE_ITEM1:
#ifdef USE_ITEM
//		CreateItem();
		CreateEditABC(Item1Str,ItemMax);
		screenState = DISPLAY_ITEM1;
#endif
		return 1;
	case DISPLAY_ITEM1:
		return 1;

	case CREATE_ITEM2:
#ifdef USE_ITEM
//		CreateItem();
		CreateEditABC(Item2Str,ItemMax);
		screenState = DISPLAY_ITEM2;
#endif
		return 1;
	case DISPLAY_ITEM2:
		return 1;

	case CREATE_ITEM3:
#ifdef USE_ITEM
//		CreateItem();
		CreateEditABC(Item3Str,ItemMax);
		screenState = DISPLAY_ITEM3;
#endif
		return 1;
	case DISPLAY_ITEM3:
		return 1;
		
	case CREATE_ITEM4:
#ifdef USE_ITEM
//		CreateItem();
		CreateEditABC(Item4Str,ItemMax);
		screenState = DISPLAY_ITEM4;
#endif
		return 1;
	case DISPLAY_ITEM4:
		return 1;

	case CREATE_ITEM5:
#ifdef USE_ITEM
//		CreateItem();
		CreateEditABC(Item5Str,ItemMax);
		screenState = DISPLAY_ITEM5;
#endif
		return 1;
	case DISPLAY_ITEM5:
		return 1;

	case CREATE_ITEM6:
#ifdef USE_ITEM
//		CreateItem();
		CreateEditABC(Item6Str,ItemMax);
		screenState = DISPLAY_ITEM6;
#endif
		return 1;
	case DISPLAY_ITEM6:
		return 1;

	case CREATE_ITEM7:
#ifdef USE_ITEM
//		CreateItem();
		CreateEditABC(Item7Str,ItemMax);
		screenState = DISPLAY_ITEM7;
#endif
		return 1;
	case DISPLAY_ITEM7:
		return 1;

	case CREATE_ITEM8:
#ifdef USE_ITEM
//		CreateItem();
		CreateEditABC(Item8Str,ItemMax);
		screenState = DISPLAY_ITEM8;
#endif
		return 1;
	case DISPLAY_ITEM8:
		return 1;

	case CREATE_ITEM9:
#ifdef USE_ITEM
//		CreateItem();
		CreateEditABC(Item9Str,ItemMax);
		screenState = DISPLAY_ITEM9;
#endif
		return 1;
	case DISPLAY_ITEM9:
		return 1;

	case CREATE_AGENT:
		CreateAgent();
		screenState = DISPLAY_AGENT;
		return 1;
	case DISPLAY_AGENT:
		return 1;
   	case CREATE_Master:
		CreateMaster();
		screenState = DISPLAY_Master;
		return 1;
	case DISPLAY_Master:
		return 1;
		
   	case CREATE_Setpassword:
		CreateSetpassword();
		screenState = DISPLAY_Setpassword;
		return 1;
	case DISPLAY_Setpassword:
		return 1;	


   	case CREATE_InputSetpassword1:
		CreateEditTest(EnterPasswordStr, 6);
		screenState = DISPLAY_InputSetpassword1;
		return 1;
	case DISPLAY_InputSetpassword1:
		return 1;		

   	case CREATE_InputSetpassword2:
		CreateEditTest(Confirmpasswordstr, 6);
		screenState = DISPLAY_InputSetpassword2;
		return 1;
	case DISPLAY_InputSetpassword2:
		return 1;	

   	case CREATE_OutcomeSetpassword:
		CreateOutcomeSetpassword();
		screenState = DISPLAY_OutcomeSetpassword;
		return 1;
	case DISPLAY_OutcomeSetpassword:
		return 1;	


		
	case CREATE_RESET:
		CreateYesNo(ResetStr);
		screenState = DISPLAY_RESET;
		return 1;
	case DISPLAY_RESET:
		return 1;

	case CREATE_DELETE:
		CreateYesNo(DeleteStr);
		screenState = DISPLAY_DELETE;
		return 1;
	case DISPLAY_DELETE:
		return 1;
		
	case CREATE_ChooseCal:

		CreateChooseCal();
		screenState = DISPLAY_ChooseCal;
		return 1;
	case DISPLAY_ChooseCal:
		return 1;
		
	case CREATE_CYC:
		//CreateCalibrate();
		CreateEditTest(inspection, 6);		
		screenState = DISPLAY_CYC;
		return 1;
	case DISPLAY_CYC:
		return 1;

		
	case CREATE_CALLOCK:
		CreateCalLock();
		screenState = DISPLAY_CALLOCK;
		return 1;
	case DISPLAY_CALLOCK:
		return 1;
	case CREATE_YESNO:
		CreateYesNo(Yes_NoStr);
		screenState = DISPLAY_YESNO;
		return 1;
	case DISPLAY_YESNO:
		return 1;

	case CREATE_AUTOOFF:
		Createautuooff();
		screenState = DISPLAY_AUTOOFF;
		alarmON = TRUE;
		//pObj = GOLFindObject(ID_PICTURE1);

		return 1;
	case DISPLAY_AUTOOFF:
		if(offcount>=1000)
			{
			AutooffSwitch = FALSE;
			offnum++;
			if(offnum == 10)
				{
                   //Off_Printer();
              	   POWER_CONTROL = 0;//开关机
		           ErrorTrap(PowerOffStr);
				   AutooffSwitch = FALSE;
			}
	            SetColor(BLACK);
	            Bar(20, 190, 200, 300);	
				
			/*
			pObj = GOLFindObject(ID_PICTURE1);
			StSetText((STATICTEXT *)pObj, Countdown);//StSetText(pSt, Countdown[1]);
			SetState(pSt, ST_DRAW);
			*/
			SetFont((void*)&BigArial);
            SetColor(RED);
			OutTextXY(100, 200, Countdown[offnum]);
			offcount = 0;
			AutooffSwitch = TRUE;
			}
		if(ADS7843_INT!=1)
			{
			screenState = CREATE_MAIN;//PrescreenState;// = screenState;
			alarmON = FALSE;
			while(ADS7843_INT!=1);
			adcX = 0;
			adcY = 0;
            AutooffSwitch = FALSE;
            offcount = 0;
			offnum = 0;
			g_standby_time = EEPROMReadWord(BackLightTimeAddress);
			g_rtc_counter = 0;
		//	ClearDevice();

		}
		return 1;

		case CREATE_BlueTooth:

			CreateBlueTooth();
			screenState = DISPLAY_BlueTooth;
			return 1;

		case DISPLAY_BlueTooth:
			if(GetState(pbutton, BTN_DISABLED))
				{
					BlueToothcount++;
     
                   if(BlueToothcount>=400)
                   	{
                        ClrState(pbutton, BTN_DISABLED);
						SetState(pbutton, BTN_DRAW);
						BlueToothcount = 0;
						PORTGbits.RG0=0;
				   }
	        	}
			if(StGetText(PST)==Nonestr)
				if(PORTEbits.RE8)
					{
						StSetText(PST, Workstr);
						SetState(PST, ST_DRAW);
				    }
				
			if(StGetText(PST)==Workstr)
				if(!PORTEbits.RE8)
					{
					  StSetText(PST, Nonestr);
					  SetState(PST, ST_DRAW);	
				}
			return 1;

    }

    return 1;    										// release drawing control to GOL

}

#ifdef TradeMark_Version
XCHAR SoftwareID[] = {'V',':',' ','1','0','1','1','2','0','5','1','0',0};
#else
XCHAR SoftwareID[] = {'V',':',' ','1','2','7','1','2','0','5','1','0',0};
#endif
// Shows intro screen and waits for touch
void StartScreen(void){
	
SHORT counter;

//XCHAR *text = TouchScreenStr;
XCHAR CoName[] = {0x002E, 0x002E, 0x002E, 0x0033, 0x00A0, 0x0020, 0x00B2, 0x00B0, 0x00A5, 0x00A7, 0x00A2, 0x00AE, 0x00B1, 0x00B2, 0x00BC, 0x0020, 0x00AD, 0x00A0, 0x0020, 0x00A4, 0x00AE, 0x00B0, 0x00AE, 0x00A3, 0x00A5, 0x0000};
XCHAR Alcovisor[] ={'A','L','C','O','V','I','S','O','R',0x0000};
#ifdef  TradeMark_Version
XCHAR Model[] = {' ',' ',' ',' ',0x009E, 0x008F, 0x0088, 0x0092, 0x0085, 0x0090, 0x0000};    //
//Model = TradeMark_Title;
//XCHAR TradeMark_Title[] = {0x009E, 0x008F, 0x0088, 0x0092, 0x0085, 0x0090, 0x0000};    //
XCHAR edition[] = {' ',' ',' ','V','1','.','0','1',0}; //edition
#else
XCHAR Model[] = {'P','R','O','-','1','0','0',' ','t','o','u','c','h',0};//"ALCOVISOR@";
XCHAR edition[] = {'R','U',0x0020,'V','1','.','2','7',0}; //edition
#endif
XCHAR PeriodsText[] = {'P','l','e','a','s','e',' ','D','e','m','a','r','c','a','t','e',0};
BOOL Demarcate=FALSE;
unsigned char EETemp0,EETemp1;

SHORT width, height;
WORD i, j, k;
WORD m;
WORD ytemp, xtemp, srRes = 0x0001; 

    SetColor(BLACK); //WHITE
    ClearDevice();      



	for(counter=0;counter<GetMaxY()-180;counter++)
	{  // move Microchip icon
 //       	PutImage(5,counter,(void*)&WellLogo,IMAGE_NORMAL);
    	}


// 如果超出标定期限，则先显示请标定界面。
// 当前时间日期 - 标定时时间日期为设定月份
// 或当前记录条数减去标定时条数大于设定条数则提示标定
/*	if(EEPROMReadByte(DemarcateYrDataAddress) -_time.yr)
	{
		EETemp0 = EEPROMReadByte(AdjustMthDataAddress);
		EETemp1 = EEPROMReadByte(DemarcateMthDataAddress)+12 -_time.mth;
		if(EETemp1>EETemp0)
			Demarcate = TRUE;
	}
	else if((EEPROMReadByte(DemarcateMthDataAddress) -_time.mth)>EEPROMReadByte(AdjustMthDataAddress))
		Demarcate = TRUE;
	else if((EEPROMReadByte(DemarcateDayDataAddress) -_time.day)==(EEPROMReadByte(AdjustDayDataAddress)))
		if(_time.day>=(EEPROMReadWord(DemarcateDayDataAddress)))
		Demarcate = TRUE;
	// 如果超出标定期限，则先显示请标定界面。
	if(EEPROMReadWord(AdjustTimesDataAddress)>=(serialnumber - EEPROMReadWord(DemarcateTimesDataAddress)))
		Demarcate = TRUE;*/
/*	
	if(Demarcate)
	{
		SaveAdjust();
		MoveTo(40,200);
		SetColor(BRIGHTRED);
		SetFont((void*)&FONTDEFAULT);// FONTMEDIUM
	    while(!OutText((XCHAR*)PeriodsText));
	    while(TouchGetX() == -1)Beep();
	    
	    delay(500);
	}
*/
    SetColor(BRIGHTBLUE);
    SetFont((void*)&FONTMEDIUM);// FONTMEDIUM

	MoveTo(88,250);
    while(!OutText((XCHAR*)edition));

   // MoveTo(70,140);
   // while(!OutText((XCHAR*)Alcovisor));

    SetColor(WHITE);
   SetFont((void*)&FONTDEFAULT);
    MoveTo(60,100);
    while(!OutText((XCHAR*)Model));
	
    MoveTo(60,20);
    while(!OutText((XCHAR*)touch100));
    MoveTo(160,145);
/*
	SetColor(BRIGHTGREEN);
	SetFont((void*)&FONTDEFAULT);// FONTMEDIUM
    MoveTo(20,250);
    while(!OutText((XCHAR*)WellStr));*/
//    while(!OutText((XCHAR*)ElecStr));
//    while(!OutText((XCHAR*)YxgsStr));
    
/*
SetFont((void*)&FONTMEDIUM);//FONTDEFAULT
    MoveTo((GetMaxX()-GetTextWidth((XCHAR*)text,(void*)&FONTMEDIUM))>>1,(GetMaxY() - GetTextHeight((void*)&FONTMEDIUM)));
    while(!OutText((XCHAR*)text));
MoveTo(0,30);
while(!OutText((XCHAR*)Newstr));//ReadTable));
*/

//#ifdef	USE_MULTIBYTECHAR
    SetColor(BRIGHTRED);
    SetFont((void*)ptrLargeAsianFont);    

/*    MoveTo((GetMaxX()- GetTextWidth(text,(void*)ptrLargeAsianFont))>>1,120);
    while(!OutText(text));*/

  //  SetColor(BRIGHTBLUE);
   // MoveTo((GetMaxX()- GetTextWidth(Name,(void*)ptrLargeAsianFont))>>1,80);
  //  while(!OutText(Name));
//#endif


   //while(TouchGetX() == -1);
    DelayMs(1000);

    Beep();

    DelayMs(1000);
#ifndef	USE_SSD1289
	// random fade effect using a Linear Feedback Shift Register (LFSR)
    SetColor(WHITE);
    for (i = 1800; i > 0 ; i--) {
		// for a 16 bit LFSR variable the taps are at bits: 1, 2, 4, and 15
   		srRes = (srRes >> 1) ^ (-(int)(srRes & 1) & 0x8006);  
    	xtemp = (srRes & 0x00FF)%40;
    	ytemp = (srRes >> 8)%30;
    
    	// by replicating the white (1x1) bars into 8 areas fading is faster
		for (j = 0; j < 8; j++) {
    		for (k = 0; k < 8; k++)
	    		PutPixel(xtemp+(j*40), ytemp+(k*30)); 
	    }		
	}
#endif
}

void CreatePage(XCHAR *pText) {
OBJ_HEADER* obj;
SHORT i;
	//pump_pwr=1;// 泵开始充电

   pWin= WndCreate(ID_WINDOW1,       		// ID
              0,0,GetMaxX(),GetMaxY(), 	// dimension
              WND_DRAW,                	// will be dislayed after creation
              NULL,               	// icon
              pText,	   				// set text 
              navScheme);               // default GOL scheme 
if(screenState!=CREATE_RECORD&&screenState!=CREATE_BLOW&&screenState!=CREATE_RESULT)
	{
	BtnCreate(ID_BUTTON_BACK,         	// button ID 
              0,GetMaxY()-40,					   	// left, top corner	
              40,GetMaxY(),0, 		   	// right, bottom corner (with radius = 0)
              BTN_DRAW,               	// will be dislayed after creation
              (void*)&LEFT,					   	// no bitmap	
              NULL,//, HomeStr			// LEFT arrow as text
              blackScheme);          	   	// use navigation scheme
              
    BtnCreate(ID_BUTTON_NEXT,   	    // button ID 
              GetMaxX()-40,GetMaxY()-40,
              GetMaxX(),GetMaxY(),0,   	// dimension (with radius = 0)
              BTN_DRAW,               	// will be dislayed and disabled after creation
              (void*)&RIGHT,					   	// no bitmap
              NULL,//, NextStr			// RIGHT arrow as text
              blackScheme);   
}
if(screenState==CREATE_RECORD)
		 	BtnCreate(ID_BUTTON_HOME,
		      60,GetMaxY()-41,					   	// left, top corner	
                   106,GetMaxY()-2,0, 		   	// right, bottom corner (with radius = 0)
                   BTN_DRAW,               	// will be dislayed after creation
                   (void*)&HOME,					   	// no bitmap	
                  NULL,//, HomeStr			// LEFT arrow as text
                  blackScheme);	
else if(screenState==CREATE_RESULT)
{
		BtnCreate(ID_BUTTON_HOME,         	// button ID 
              0,GetMaxY()-40,					   	// left, top corner	
              46,GetMaxY(),0, 		   	// right, bottom corner (with radius = 0)
              BTN_DRAW,               	// will be dislayed after creation
              (void*)&HOME,					   	// no bitmap	
              NULL,//, HomeStr			// LEFT arrow as text
              blackScheme);          	   	// use navigation scheme

    BtnCreate(ID_BUTTON_NEXT,   	    // button ID 
              GetMaxX()-40,GetMaxY()-40,
              GetMaxX(),GetMaxY(),0,   	// dimension (with radius = 0)
              BTN_DRAW,               	// will be dislayed and disabled after creation
              (void*)&ag3,					   	// no bitmap
              NULL,//, NextStr			// RIGHT arrow as text
              blackScheme);  

}
  else    
	BtnCreate(ID_BUTTON_HOME,
		      97,GetMaxY()-40,					   	// left, top corner	
              143,GetMaxY(),0, 		   	// right, bottom corner (with radius = 0)
              BTN_DRAW,               	// will be dislayed after creation
              (void*)&HOME,					   	// no bitmap	
              NULL,//, HomeStr			// LEFT arrow as text
              blackScheme);
  
	RTCCProcessEvents();				// update the date and time strings
	i = 0;						
	while (i < 12) {
		dateTimeStr[i] 	  = (XCHAR)_time_str[i];
		dateTimeStr[i+13] = (XCHAR)_date_str[i];
		i++;
	}
	dateTimeStr[12] = (XCHAR)'\n';
	dateTimeStr[25] = (XCHAR)'\0';

//	GOLSetFocus(obj); 					// set focus for the button 

}
void CreatePage0(XCHAR *pText) {
OBJ_HEADER* obj;
SHORT i;

    WndCreate(ID_WINDOW1,       		// ID
              0,0,GetMaxX(),GetMaxY(), 	// dimension
              WND_DRAW,                	// will be dislayed after creation
              NULL,//(void*)&WellLogo,               	// icon
              pText,	   				// set text 
              navScheme);               // default GOL scheme 
        	   	// use navigation scheme
        
	RTCCProcessEvents();				// update the date and time strings
	i = 0;						
	while (i < 12) {
		dateTimeStr[i] 	  = (XCHAR)_time_str[i];
		dateTimeStr[i+13] = (XCHAR)_date_str[i];
		i++;
	}
	dateTimeStr[12] = (XCHAR)'\n';
	dateTimeStr[25] = (XCHAR)'\0';

           
              
}




#define EB1STARTX		2	//月
#define EB2STARTX		75	//日
#define EB3STARTX		130	//年
#define EBSTARTY		55
#define EBSTARTY1		110
#define EB4STARTX		10	//时
#define EB5STARTX		85	//分
#define EB6STARTX		160	//秒
#define EBSTARTY2		170

#define SELECTBTNWIDTH	22


// For the units add by Spring.Chen
XCHAR TenUnits[] = { 'J','a','n',0x000A,
							'F','e','b',0x000A,
							'M','a','r',0x000A,
							'J','u','n',0x000A,
							'J','u','l',0x000A,
							'A','u','g',0x000A,
							'S','e','p',0x000A,
							'O','c','t',0x000A,
							'N','o','v',0x000A,
							'D','e','c',0x0000
//						'g','/','L',0x0000


						};






// Items list
// Months Items list
XCHAR MonthItems[] = {'0', '1', 0x000A,
					  '0', '2', 0x000A,
					  '0', '3', 0x000A,
					  '0', '4', 0x000A,
					  '0', '5', 0x000A,
					  '0', '6', 0x000A,
					  '0', '7', 0x000A,
					  '0', '8', 0x000A,
					  '0', '9', 0x000A,
					  '1', '0', 0x000A,
					  '1', '1', 0x000A,
					  '1', '2', 0x0000};
/*
XCHAR MonthItems[] = {0x009F, 0x008D, 0x0082,0x000A,
							0x0094, 0x0085, 0x0082,0x000A,
							0x008C, 0x0080, 0x0090, 0x000A,
							0x0080, 0x008F, 0x0090, 0x000A,
							0x008C, 0x0080, 0x0089,0x000A,
							0x0088, 0x009E, 0x008D, 0x000A,
							0x0088, 0x009E, 0x008B,0x000A,
							0x0080, 0x0082, 0x0083,0x000A,
							0x0091, 0x0085, 0x008D,0x000A,
							0x008E, 0x008A, 0x0092,0x000A,
							0x008D, 0x008E, 0x009F,0x000A,
							0x0084, 0x0085, 0x008A,0x0000}; 
							*/
                                 
// Days Items list
XCHAR DayItems[] = {'0','1',0x000A, '0','2',0x000A, '0','3',0x000A, '0','4',0x000A, '0','5',0x000A, 
						  '0','6',0x000A, '0','7',0x000A, '0','8',0x000A, '0','9',0x000A, '1','0',0x000A,
						  '1','1',0x000A, '1','2',0x000A, '1','3',0x000A, '1','4',0x000A, '1','5',0x000A,
						  '1','6',0x000A, '1','7',0x000A, '1','8',0x000A, '1','9',0x000A, '2','0',0x000A,
						  '2','1',0x000A, '2','2',0x000A, '2','3',0x000A, '2','4',0x000A, '2','5',0x000A,
						  '2','6',0x000A, '2','7',0x000A, '2','8',0x000A, '2','9',0x000A, '3','0',0x000A, '3','1',0x0000};

// Year Items list
XCHAR YearItems[] = {'0','0',0x000A, '0','1',0x000A, '0','2',0x000A, '0','3',0x000A, '0','4',0x000A, 
						   '0','5',0x000A, '0','6',0x000A, '0','7',0x000A, '0','8',0x000A, '0','9',0x000A, 
						   '1','0',0x000A, '1','1',0x000A, '1','2',0x000A, '1','3',0x000A, '1','4',0x000A, 
						   '1','5',0x000A, '1','6',0x000A, '1','7',0x000A, '1','8',0x000A, '1','9',0x000A, '2','0',0x0000};

XCHAR *DTSetText    = SetDateTimeStr; 
XCHAR DTPlusSym[] 	= {'+',0};
XCHAR DTMinusSym[] 	= {'-',0};
XCHAR *DTDoneText 	= ExitStr; 
XCHAR *DTDownArrow 	= RightArrowStr; 
XCHAR *DTUpArrow 	= LeftArrowStr; 


// Creates date and time screen
void CreateDateTime() {

	WORD textWidth, textHeight, tempStrWidth;
	XCHAR tempStr[] = {'M',0};

    // free memory for the objects in the previous linked list and start new list
	GOLFree();   
	RTCCProcessEvents();				// update the global time and date strings
	
	/* ************************************* */
	/* Create Month, Day and Year Edit Boxes */ 
	/* ************************************* */
	tempStrWidth = GetTextWidth(tempStr,altScheme->pFont);
	textHeight   = GetTextHeight(altScheme->pFont) + (GOL_EMBOSS_SIZE<<1);


	WndCreate(ID_WINDOW1,       		// ID
              0,0,GetMaxX(),GetMaxY(), 	// dimension
              WND_DRAW,                	// will be dislayed after creation
              NULL,//(void*)&WellLogo,               	// icon
              DTSetText,				// set text 
              navScheme);                   	// use default scheme 
              
	/* *************************** */
	// create day components
	/* *************************** */
	// day has two characters, thus we multiply by two
	textWidth = (tempStrWidth*2) +(GOL_EMBOSS_SIZE<<1);

	EbCreate(ID_EB_DAY,              	// ID
              EB1STARTX,
              EBSTARTY,
              EB1STARTX+textWidth,
              EBSTARTY+textHeight,
              EB_DRAW, 					// will be dislayed after creation
              NULL,
              2,						// max characters is 3
              altScheme);               // use alternate scheme

    BtnCreate(ID_BUTTON_DY,         	// button ID 
              EB1STARTX+textWidth+1,
              EBSTARTY,
              EB1STARTX+textWidth+SELECTBTNWIDTH+1,
              EBSTARTY+textHeight,  		   	
              0,					   	// draw rectangular button	
              BTN_DRAW,      			// will be dislayed after creation
              (void*)&arrowDown,					   	// no bitmap	
              NULL,              // DOWN arrow as text
              altScheme);               // use alternate scheme
              
	/* *************************** */
	// create month components
	/* *************************** */
	// months has three characters, thus we multiply by three
	textWidth = (tempStrWidth*3) +(GOL_EMBOSS_SIZE<<1);

	EbCreate(ID_EB_MONTH,              	// ID
              EB2STARTX-7,
              EBSTARTY,
              EB2STARTX+textWidth-17, 
              EBSTARTY+textHeight,
              EB_DRAW, 					// will be dislayed after creation
              NULL,
              2,						// max characters is 3
              altScheme);               // use alternate scheme

    BtnCreate(ID_BUTTON_MO,         	// button ID 
              EB2STARTX+textWidth+1-17,
              EBSTARTY,
              EB2STARTX+textWidth+SELECTBTNWIDTH+1-17,
              EBSTARTY+textHeight,  		   	
              0,					   	// draw rectangular button	
              BTN_DRAW,               	// will be dislayed after creation
              (void*)&arrowDown,					   	// no bitmap	
              NULL,             	// DOWN arrow as text
              altScheme);               // use alternate scheme

	/* *************************** */
	// create year components
	/* *************************** */
	// year has four characters, thus we multiply by four
	textWidth = (tempStrWidth*4) +(GOL_EMBOSS_SIZE<<1);

	EbCreate(ID_EB_YEAR,              	// ID
              EB3STARTX+10,
              EBSTARTY,
              EB3STARTX+textWidth,
              EBSTARTY+textHeight,
              EB_DRAW, 					// will be dislayed after creation
              NULL,
              4,						// max characters is 4
              altScheme);               // use alternate scheme

    BtnCreate(ID_BUTTON_YR,         	// button ID 
              EB3STARTX+textWidth+1,
              EBSTARTY,
              EB3STARTX+textWidth+SELECTBTNWIDTH+1,
              EBSTARTY+textHeight,  		   	
              0,					   	// draw rectangular button	
              BTN_DRAW,      			// will be dislayed after creation
              (void*)&arrowDown,					   	// no bitmap	
              NULL,//DTDownArrow,             	// DOWN arrow as text
              altScheme);               // use alternate scheme
              

	/* ****************************************** */
	/* Create Hour, Minute and Seconds Edit Boxes */ 
	/* ****************************************** */

	RTCCProcessEvents();				// update the time and date

	textHeight = (GetTextHeight(altScheme->pFont)<<1);
	tempStrWidth = GetTextWidth(tempStr,altScheme->pFont);

	/* *************************** */
	// create hour components
	/* *************************** */
	// items here have 2 characters each so we use 2 as multiplier
	textWidth  = (tempStrWidth*2) + (GOL_EMBOSS_SIZE<<1);

	EbCreate(ID_EB_HOUR,              	// ID
              EB4STARTX,
              EBSTARTY2,
              EB4STARTX+textWidth, 
              EBSTARTY2+textHeight,
              EB_DRAW, 					// will be dislayed after creation
              NULL,
              2,						// max characters is 2
              altScheme);               // use alternate scheme

    BtnCreate(ID_BUTTONHR_P,         	// button ID 
              EB4STARTX+textWidth+1,
              EBSTARTY2,
              EB4STARTX+textWidth+SELECTBTNWIDTH+1,
              EBSTARTY2+(textHeight>>1),  		   	
              0,					   	// draw rectangular button	
              BTN_DRAW,               	// will be dislayed after creation
              NULL,					   	// no bitmap	
              DTPlusSym,              	// plus symbol as text
              altScheme);               // use alternate scheme

    BtnCreate(ID_BUTTONHR_M,         	// button ID 
              EB4STARTX+textWidth+1,
              EBSTARTY2+(textHeight>>1)+1,
              EB4STARTX+textWidth+SELECTBTNWIDTH+1,
              EBSTARTY2+textHeight,  		   	
              0,					   	// draw rectangular button	
              BTN_DRAW,      			// will be dislayed after creation
              NULL,					   	// no bitmap	
              DTMinusSym,               // minus symbols as text
              altScheme);               // use alternate scheme

	/* *************************** */
	// create minute components
	/* *************************** */

	EbCreate(ID_EB_MINUTE,              // ID
              EB5STARTX,
              EBSTARTY2,
              EB5STARTX+textWidth, 
              EBSTARTY2+textHeight,
              EB_DRAW, 					// will be dislayed after creation
              NULL,
              2,						// max characters is 2
              altScheme);               // use alternate scheme

    BtnCreate(ID_BUTTONMN_P,         	// button ID 
              EB5STARTX+textWidth+1,
              EBSTARTY2,
              EB5STARTX+textWidth+SELECTBTNWIDTH+1,
              EBSTARTY2+(textHeight>>1),  		   	
              0,					   	// draw rectangular button	
              BTN_DRAW,               	// will be dislayed after creation
              NULL,					   	// no bitmap	
              DTPlusSym,              	// plus symbol as text
              altScheme);               // use alternate scheme

    BtnCreate(ID_BUTTONMN_M,         	// button ID 
              EB5STARTX+textWidth+1,
              EBSTARTY2+(textHeight>>1)+1,
              EB5STARTX+textWidth+SELECTBTNWIDTH+1,
              EBSTARTY2+textHeight,  		   	
              0,					   	// draw rectangular button	
              BTN_DRAW,               	// will be dislayed after creation
              NULL,					   	// no bitmap	
              DTMinusSym,               // minus symbols as text
              altScheme);               // use alternate scheme

	/* *************************** */
	// create seconds components
	/* *************************** */

	EbCreate(ID_EB_SECOND,              // ID
              EB6STARTX,
              EBSTARTY2,
              EB6STARTX+textWidth, 
              EBSTARTY2+textHeight,
              EB_DRAW, 					// will be dislayed after creation
              NULL,
              2,						// max characters is 2
              altScheme);               // use alternate scheme

    BtnCreate(ID_BUTTONSC_P,         	// button ID 
              EB6STARTX+textWidth+1,
              EBSTARTY2,
              EB6STARTX+textWidth+SELECTBTNWIDTH+1,
              EBSTARTY2+(textHeight>>1),  		   	
              0,					   	// draw rectangular button	
              BTN_DRAW,               	// will be dislayed after creation
              NULL,					   	// no bitmap	
              DTPlusSym,              	// plus symbol as text
              altScheme);               // use alternate scheme

    BtnCreate(ID_BUTTONSC_M,         	// button ID 
              EB6STARTX+textWidth+1,
              EBSTARTY2+(textHeight>>1)+1,
              EB6STARTX+textWidth+SELECTBTNWIDTH+1,
              EBSTARTY2+textHeight,  		   	
              0,					   	// draw rectangular button	
              BTN_DRAW,               	// will be dislayed after creation
              NULL,					   	// no bitmap	
              DTMinusSym,               // minus symbols as text
              altScheme);               // use alternate scheme

	 //-----------------------------------------------------
	 // Done Button
 	
 	textHeight = GetTextHeight(altScheme->pFont) + (GOL_EMBOSS_SIZE<<1) + 5;
 	
    BtnCreate(    ID_BUTTON4,         	// button ID 
                  EB6STARTX-45,
                  EBSTARTY2+2*textHeight,
                  EB6STARTX+textWidth+SELECTBTNWIDTH,
                  EBSTARTY2+3*textHeight,  		   	
                  8,					// draw rectangular button	
                  BTN_DRAW,             // will be dislayed after creation
                  NULL,					// no bitmap	
                  DTDoneText,           // set text
              	  altScheme);           // use alternate scheme

    BtnCreate(ID_BUTTON_HOME,   	    // button ID 
		      100,GetMaxY()-40,					   	// left, top corner	
              140,GetMaxY(),0, 		   	// right, bottom corner (with radius = 0)
              BTN_DRAW,               	// will be dislayed and disabled after creation
              (void*)&HOME,					   	// no bitmap
              NULL,//, NextStr			// RIGHT arrow as text
              blackScheme);  				  
              	  
	updateDateTimeEb();              	  
}

void updateDateTimeEb() {
	OBJ_HEADER* obj;
	SHORT 		i;

	RTCCProcessEvents();				// update the time and date
	
	i = 0;	
	while (i < 12) {
		dateTimeStr[i] = _time_str[i];
		dateTimeStr[i+13] = _date_str[i];
		i++;
	}
	dateTimeStr[12] = 0x000A; 	// (XCHAR)'\n';
	dateTimeStr[25] = 0x0000;	// (XCHAR)'\0';


	// update date edit boxes
	obj = GOLFindObject(ID_EB_MONTH);
	EbSetText((EDITBOX*)obj, &dateTimeStr[16]);
	SetState(obj, EB_DRAW);

	obj = GOLFindObject(ID_EB_DAY);
	EbSetText((EDITBOX*)obj, &dateTimeStr[13]);
	SetState(obj, EB_DRAW);

	obj = GOLFindObject(ID_EB_YEAR);
	EbSetText((EDITBOX*)obj, &dateTimeStr[20]);
	SetState(obj, EB_DRAW);

	// update time edit boxes
	obj = GOLFindObject(ID_EB_HOUR);
	EbSetText((EDITBOX*)obj, &dateTimeStr[4]);
	SetState(obj, EB_DRAW);
	
	obj = GOLFindObject(ID_EB_MINUTE);
	EbSetText((EDITBOX*)obj, &dateTimeStr[7]);
	SetState(obj, EB_DRAW);

	obj = GOLFindObject(ID_EB_SECOND);
	EbSetText((EDITBOX*)obj, &dateTimeStr[10]);
	SetState(obj, EB_DRAW);
}

/* this will show the pull down menus */
void ShowPullDownMenu() {
	
	WORD 		textWidth=0, textHeight, tempStrWidth;
	LISTBOX*    pLb;
	XCHAR*		pItems = NULL;
	XCHAR 		tempString[] = {'M',0};

	pListSaved = GOLGetList();
	GOLNewList();

	textHeight = GetTextHeight(altScheme->pFont) + (GOL_EMBOSS_SIZE << 1);
	
	// Note: pDwnBottom, pDwnTop, pDwnLeft and pDwnRight are globals that will
	// 		 be used to redraw the area where the pulldown menu covered.
	pDwnBottom = 230;
	pDwnTop    = EBSTARTY;

	tempStrWidth = GetTextWidth(tempString,altScheme->pFont);
	switch (DateItemID) {
		case ID_EB_MONTH:
			textWidth  = (tempStrWidth*3) + (GOL_EMBOSS_SIZE<<1);
			pDwnLeft   = EB2STARTX-10;
			pDwnRight  = EB2STARTX+textWidth+SELECTBTNWIDTH-10;
			pItems = (XCHAR*)MonthItems;
			break;
		case ID_EB_DAY:
			textWidth  = (tempStrWidth*2) + (GOL_EMBOSS_SIZE<<1);
			pDwnLeft   = EB1STARTX;
			pDwnRight  = EB1STARTX+textWidth+SELECTBTNWIDTH;
			pItems = (XCHAR*)DayItems;
			break;
		case ID_EB_YEAR:
			textWidth  = (tempStrWidth*4) + (GOL_EMBOSS_SIZE<<1);
			pDwnLeft   = EB3STARTX;
			pDwnRight  = EB3STARTX+textWidth+SELECTBTNWIDTH;
			pItems = (XCHAR*)YearItems;
			break;

		case	ID_EB_UNIT:
			textWidth  = (tempStrWidth*3) + (GOL_EMBOSS_SIZE<<1);
			pDwnLeft   = EB1STARTX;
			pDwnRight  = EB1STARTX+textWidth+SELECTBTNWIDTH;
			pItems = (XCHAR*)TenUnits;
	}

	pLb = (LISTBOX *)LbCreate(
			   ID_LISTBOX1,         	// List Box ID
		       pDwnLeft,				// left
		       pDwnTop+textHeight,		// top
		       pDwnRight-SELECTBTNWIDTH,// right
		       pDwnBottom, 				// bottom dimension
		       LB_DRAW|LB_SINGLE_SEL, 	// draw after creation, single selection
		       pItems,        			// items list 
		       altScheme);
		
	SldCreate(ID_SLIDER1,              	// Slider ID
		      pDwnRight-SELECTBTNWIDTH+1,
		      pDwnTop+(textHeight<<1),  		   	
		      pDwnRight,
		      pDwnBottom-textHeight,
		      SLD_DRAW|SLD_SCROLLBAR|
		      SLD_VERTICAL,   			// vertical, draw after creation
		      LbGetCount(pLb),       	// range
		      5,                       	// page 
		      LbGetCount(pLb)-1,       	// pos
		      altScheme);
		
	BtnCreate(ID_BUTTON_DATE_UP,       	// up button ID 
		      pDwnRight-SELECTBTNWIDTH+1,
		      pDwnTop+textHeight,
		      pDwnRight,
		      pDwnTop+(textHeight<<1),0, 		   	
		      BTN_DRAW,                 // draw after creation
		      (void*)&arrowUp,//NULL,                    	// no bitmap
		      NULL,//DTUpArrow,              	// text
		      altScheme);
		
	BtnCreate(ID_BUTTON_DATE_DN,        // down button ID 
		      pDwnRight-SELECTBTNWIDTH+1,
		      pDwnBottom-textHeight,
		      pDwnRight,
		      pDwnBottom,0,  		   	
		      BTN_DRAW,                 // draw after creation
		      (void*)&arrowDown,//NULL,                    	// no bitmap
		      NULL,//DTDownArrow,              // text
		      altScheme);
		      
}  

WORD RemovePullDownMenu() {

   OBJ_HEADER* pObjTemp;

	// check if objects has been hidden
	pObjTemp = GOLGetList();
	while(pObjTemp) {							// parse the active list
		if (GetState(pObjTemp, HIDE))			// an object is still not hidden 
			return 0;							// go back without changing state
		pObjTemp = pObjTemp->pNxtObj;			// go to next object
	}

	// all objects are hidden we can now remove the list	
	GOLFree();   								// remove the list of pull down menu
    GOLSetList(pListSaved);						// set active list back to saved list

    // redraw objects that were overlapped by pulldown menu
    GOLRedrawRec(pDwnLeft, pDwnTop, pDwnRight, pDwnBottom);		
    
	// must reset the pressed button, this code is more compact than searching  
	// which one of the three is pressed. 
	pObjTemp = GOLFindObject(ID_BUTTON_MO);
	ClrState(pObjTemp, BTN_PRESSED); 
	pObjTemp = GOLFindObject(ID_BUTTON_YR);
	ClrState(pObjTemp, BTN_PRESSED); 
    pObjTemp = GOLFindObject(ID_BUTTON_DY);
	ClrState(pObjTemp, BTN_PRESSED); 
	
	updateDateTimeEb();							// Update the edit boxes to reflect the new dates
	return 1;									// go back normally 	
	
}	

/* this selects the new month or day or year from the list box*/
WORD MsgSetDate(WORD objMsg, OBJ_HEADER* pObj, GOL_MSG* pMsg) {
	
LISTBOX*   	pLb;
SLIDER*  	pSld;
OBJ_HEADER* pObjHide;
LISTITEM*	pItemSel;

    pLb = (LISTBOX*)GOLFindObject(ID_LISTBOX1);		
    pSld = (SLIDER*)GOLFindObject(ID_SLIDER1);

    switch(GetObjID(pObj)){

        case ID_LISTBOX1:
        	if (pMsg->uiEvent == EVENT_MOVE) {
	        	pMsg->uiEvent = EVENT_PRESS;			// change event for listbox
	            // Process message by default
	            LbMsgDefault(objMsg, (LISTBOX*)pObj, pMsg);
	            // Set new list box position
	            SldSetPos(pSld,LbGetCount(pLb)-LbGetFocusedItem(pLb)-1);
	            SetState(pSld, SLD_DRAW_THUMB);
	        	pMsg->uiEvent = EVENT_MOVE;				// restore event for listbox
	        }
	       	else if (pMsg->uiEvent == EVENT_PRESS) {
		       	// call the message default processing of the list box to select the item
		       	LbMsgDefault(objMsg, (LISTBOX *)pObj, pMsg);
		    }
	       	else if (pMsg->uiEvent == EVENT_RELEASE) {
		        
		        pObjHide = GOLGetList();  				// hide all the objects in the current list
		        while (pObjHide) {						// set all objects with HIDE state
	            	SetState(pObjHide, HIDE);
	            	pObjHide = pObjHide->pNxtObj;
	            }
				pItemSel = LbGetSel(pLb,NULL);			// get the selected item
				if (pItemSel != NULL) 					// process only when an item was selected
					UpdateRTCCDates(pLb);				// update the RTCC values 
    
			    screenState = HIDE_DATE_PDMENU;			// go to hide state
	        }

	        // The message was processed
	        return 0;

        case ID_SLIDER1:
            // Process message by default
            SldMsgDefault(objMsg, (SLIDER*)pObj, pMsg);
            // Set new list box position
            if(LbGetFocusedItem(pLb) != LbGetCount(pLb)-SldGetPos(pSld)){
                LbSetFocusedItem(pLb,LbGetCount(pLb)-SldGetPos(pSld));
                SetState(pLb, LB_DRAW_ITEMS);
            }
            // The message was processed
            return 0;

        case ID_BUTTON_DATE_UP:					// slider button up was pressed
            if(objMsg == BTN_MSG_RELEASED){
                LbSetFocusedItem(pLb,LbGetFocusedItem(pLb)-1);
                SetState(pLb, LB_DRAW_ITEMS);
                SldSetPos(pSld,SldGetPos(pSld)+1);
                SetState(pSld, SLD_DRAW_THUMB);
            }
            return 1;

        case ID_BUTTON_DATE_DN:					// slider button down was pressed
            if(objMsg == BTN_MSG_RELEASED){
                LbSetFocusedItem(pLb,LbGetFocusedItem(pLb)+1);
                SetState(pLb, LB_DRAW_ITEMS);
                SldSetPos(pSld,SldGetPos(pSld)-1);
                SetState(pSld, SLD_DRAW_THUMB);
            }
            return 1;
    }    
    return 1;    	
}	
			

// Processes messages for date and time screen
WORD MsgDateTime(WORD objMsg, OBJ_HEADER* pObj) {
	
	EDITBOX *pEbObj;
	char   hour = 0, minute = 0, second = 0;
	WORD   btnID; 
	WORD   ebID = 0, i;
	//static XCHAR  tempString[3];

    switch(btnID = GetObjID(pObj)) {
	    
        case ID_BUTTON4:							// exit setting of date and time
            if(objMsg == BTN_MSG_RELEASED){
                screenState = CREATE_MAIN; 			// goto last state screen
                return 1;
            }
			break;

        case ID_BUTTON_HOME:							// exit setting of date and time
            if(objMsg == BTN_MSG_RELEASED){
                screenState = CREATE_MAIN; 			// goto last state screen
                return 1;
            }
			break;			

		//////////////////////////////////////////////////////////////////////////
		// 		Date update: this will create the pulldown menu
		//////////////////////////////////////////////////////////////////////////

        case ID_BUTTON_MO:							// month is selected show the pull down menu
            if(objMsg == BTN_MSG_PRESSED){
	            DateItemID = ID_EB_MONTH;
                return 1;   
            }
			break;

        case ID_BUTTON_DY:							// month is selected show the pull down menu
            if(objMsg == BTN_MSG_PRESSED){
	            DateItemID = ID_EB_DAY;
                return 1;
            }
			break;

        case ID_BUTTON_YR:							// month is selected show the pull down menu
            if(objMsg == BTN_MSG_PRESSED){
	            DateItemID = ID_EB_YEAR;
                return 1;
            }
			break;

		//////////////////////////////////////////////////////////////////////////
		// 		Time update through the '+' and '-' buttons
		//////////////////////////////////////////////////////////////////////////
			
		case ID_BUTTONHR_P:  						// increment hour value
		    if(objMsg == BTN_MSG_PRESSED){
				hour = +1;
				ebID = ID_EB_HOUR;
				i = 4;								
			}
			break;
        case ID_BUTTONHR_M:							// decrement hour value
			if(objMsg == BTN_MSG_PRESSED){
				hour = -1;
				ebID = ID_EB_HOUR;
				i = 4;
			}
			break;
        case ID_BUTTONMN_P:  						// increment minute value
        	if(objMsg == BTN_MSG_PRESSED){
				minute = +1;
				ebID = ID_EB_MINUTE;
				i = 7;
			}
			break;
        case ID_BUTTONMN_M:							// decrement minute value
        	if(objMsg == BTN_MSG_PRESSED){
				minute = -1;
				ebID = ID_EB_MINUTE;
				i = 7;
			}
			break;
        case ID_BUTTONSC_P:  						// increment seconds value
        	if(objMsg == BTN_MSG_PRESSED){
				second = +1;
				ebID = ID_EB_SECOND;
				i = 10;
			}
			break;				
        case ID_BUTTONSC_M:							// decrement seconds value
        	if(objMsg == BTN_MSG_PRESSED){
				second = -1;
				ebID = ID_EB_SECOND;
				i = 10;
			} 
            break; 
            
        default:
        	ebID = 0;
            break; 
    }

	if (ebID) {										// check if time was modified
		mRTCCOff();
    	RTCCSetBinHour(mRTCCGetBinHour() + hour);	// update RTCC registers
    	RTCCSetBinMin(mRTCCGetBinMin() + minute);
    	RTCCSetBinSec(mRTCCGetBinSec() + second);
		mRTCCSet();									// copy the new values to the RTCC registers

		// update the edit box that was changed. This is done to immediately update
		// the values on the edit box. The normal update by updateDateTimeEb() is done
		// for every second. This may be too long to wait when the increment or decrement 
		// was successfully performed.
		RTCCProcessEvents();						// update string values
		updateDateTimeEb();
    }	
		
    return 1;
}

//void UpdateRTCCDates(XCHAR *pText, LISTBOX* pLb) {
void UpdateRTCCDates(LISTBOX* pLb) {
	static char value;
	char ch;
	XCHAR *pItemText;

	// get the selected item
	pItemText = LbGetSel(pLb,pLb->pItemList)->pText; 	
	switch (DateItemID) {

		default :
		case ID_EB_MONTH:
			value = LbGetFocusedItem(pLb)+1;	// get the selected month item +1 is needed since 
												// RTCC stores months from index 1 as january
			RTCCSetBinMonth(value);				// set the month value in RTCC module
   	        RTCCCalculateWeekDay();				// calculate the weekday from this new value
			break;

        case ID_EB_YEAR:
		case ID_EB_DAY:
			ch = *pItemText++;					// get the first character
			value = (ch-48) * 10;				// get the value of the first character
			ch = *pItemText;					// get the next character
			value = value + (ch-48);			// add the value of the second character to the first one
			if (DateItemID == ID_EB_DAY)				
				RTCCSetBinDay(value);			// set the day value in the RTCC module
			else {
				RTCCSetBinYear(value);			// set the day value in the RTCC module
			}
            RTCCCalculateWeekDay();				// calculate the weekday from this new value
            break;
	}
	mRTCCSet();									// copy the new values to the RTCC registers

}	





//create main
void CreateMain(){
OBJ_HEADER* obj;
unsigned char	i,j,datetemp;
#define MAINSTARTX 	30
#define MAINSTARTY 	50
#define MAINSIZEX  	15
#define MAINSIZEY  	32
#define MAINCHARSIZE 52
alarmON = FALSE;
XCHAR	Markdate[]={0x30,0x30,'/',0x30,0x30,'/',0x32,0x30,0x30,0x30,0x00};
#ifdef USE_GPS
	memset(&longitude[0],1,8);
	memset(&latitude[0],1,8);
	u4_hand();	// Get the GPS 
#endif
    GOLFree();   // free memory for the objects in the previous linked list and start new list
	SetColor(BLACK);
    ClearDevice();
    obj = (OBJ_HEADER*)BtnCreate(ID_BUTTON1, 				// button ID 
              MAINSTARTX+0*(MAINCHARSIZE+MAINSIZEX)-4-7,MAINSTARTY,
              MAINSTARTX+1*MAINCHARSIZE-4-7,MAINSTARTY+1*MAINCHARSIZE,           	// dimension
              0,					   	// set radius 
              BTN_DRAW,  			   	// draw a beveled button
              (void*)&Test1,                    	// no bitmap
              NULL,//ButtonStr, 				// "Button",     	// text
              mainScheme);              	// use alternate scheme

    BtnCreate(ID_BUTTON2, 				// button ID 
              MAINSTARTX+1*(MAINCHARSIZE+MAINSIZEX)-3,MAINSTARTY,
              MAINSTARTX+2*MAINCHARSIZE+1*MAINSIZEX-3,MAINSTARTY+1*MAINCHARSIZE,         	// dimension
              0,
              BTN_DRAW, 				// will be dislayed after creation 
              (void*)&Test2,//          // use bitmap
              NULL,//HomeStr, 					// "HOME", 	    // text
              mainScheme);	            // alternative GOL scheme 

   
    	BtnCreate(ID_BUTTON3,             	// button ID 
              MAINSTARTX+2*(MAINCHARSIZE+MAINSIZEX)-2+7,MAINSTARTY,
              MAINSTARTX+3*MAINCHARSIZE+2*MAINSIZEX-2+7,MAINSTARTY+1*MAINCHARSIZE,           	// dimension
              0,					   	// set radius 
              BTN_DRAW,//|BTN_TOGGLE,   	// draw a vertical capsule button 
              						   	// that has a toggle behavior
              (void*)&Test3,                    	// no bitmap
              NULL,//,ScreenStr//LowStr, 					// "LO",       	// text
              mainScheme);        // yellowScheme  	// use alternate scheme


    BtnCreate(ID_BUTTON4,             	// button ID 
              MAINSTARTX+0*(MAINCHARSIZE+MAINSIZEX)-4-7,MAINSTARTY+1*MAINCHARSIZE+MAINSIZEY-20,
              MAINSTARTX+1*MAINCHARSIZE+0*MAINSIZEX-4-7,MAINSTARTY+2*MAINCHARSIZE+MAINSIZEY-20,           	// dimension
              0,					   	// set radius 
              BTN_DRAW, 			   	// draw a vertical capsule button
              (void*)&Picrecord,//Picsetting,                    	// no bitmap
              NULL,//SettingStr,//OnStr, 					// "ON",		// text
              mainScheme);             // use alternate scheme 

    BtnCreate(ID_BUTTON5,             	// button ID 
              MAINSTARTX+1*(MAINCHARSIZE+MAINSIZEX)-3,MAINSTARTY+1*MAINCHARSIZE+MAINSIZEY-20,
              MAINSTARTX+2*MAINCHARSIZE+MAINSIZEX-3,MAINSTARTY+2*MAINCHARSIZE+MAINSIZEY-20,         	// dimension
              0,					   	// set radius 
              BTN_DRAW,//|BTN_PRESSED,  	// draw a vertical capsule button
              							// that is initially pressed
              (void*)&PicGPS,                    	// no bitmap
              NULL, //NULL,//OffStr, 					// "OFF",      	// text
              mainScheme);            	// use alternate scheme 	

    BtnCreate(ID_BUTTON6, 				// button ID 
              MAINSTARTX+2*(MAINCHARSIZE+MAINSIZEX)-2+7,MAINSTARTY+1*MAINCHARSIZE+MAINSIZEY-20,
              MAINSTARTX+3*MAINCHARSIZE+2*MAINSIZEX-2+7,MAINSTARTY+2*MAINCHARSIZE+MAINSIZEY-20,          	// dimension
              0,
              BTN_DRAW|BTN_TEXTLEFT 	// will be dislayed after creation with text
              |BTN_TEXTTOP, 			// left top position
              (void*)&PC_copy,//Pictime,//NULL,//(void*)&bulboff,          // use bitmap
              NULL,//TimeStr,//OffBulbStr, 				// text
              mainScheme);	            // alternative GOL scheme 

			  
   BtnCreate(ID_BUTTON7, 				// button ID 
              MAINSTARTX+0*(MAINCHARSIZE+MAINSIZEX)-4-7,MAINSTARTY+2*MAINCHARSIZE+MAINSIZEY-8,
              MAINSTARTX+1*MAINCHARSIZE+0*MAINSIZEX-4-7,MAINSTARTY+3*MAINCHARSIZE+MAINSIZEY-8,          	// dimension
              0,
              BTN_DRAW|BTN_TEXTLEFT 	// will be dislayed after creation with text
              |BTN_TEXTTOP, 			// left top position
              (void*)&Pictime,//NULL,//(void*)&bulboff,          // use bitmap
              NULL,//TimeStr,//OffBulbStr, 				// text
              mainScheme);	            // alternative GOL scheme 

			  
   BtnCreate(ID_BUTTON8, 				// button ID 
              MAINSTARTX+1*(MAINCHARSIZE+MAINSIZEX)-3,MAINSTARTY+2*MAINCHARSIZE+MAINSIZEY-8,
              MAINSTARTX+2*MAINCHARSIZE+MAINSIZEX-3,MAINSTARTY+3*MAINCHARSIZE+MAINSIZEY-8,          	// dimension
              0,
              BTN_DRAW|BTN_TEXTLEFT 	// will be dislayed after creation with text
              |BTN_TEXTTOP, 			// left top position
              (void*)&Picsetting,//NULL,//(void*)&bulboff,          // use bitmap
              NULL,//TimeStr,//OffBulbStr, 				// text
              mainScheme);	            // alternative GOL scheme 

		 
   BtnCreate(ID_BUTTON9, 				// button ID 
              MAINSTARTX+2*(MAINCHARSIZE+MAINSIZEX)-2+7,MAINSTARTY+2*MAINCHARSIZE+MAINSIZEY-8,
              MAINSTARTX+3*MAINCHARSIZE+2*MAINSIZEX-2+7,MAINSTARTY+3*MAINCHARSIZE+MAINSIZEY-8,          	// dimension
              0,
              BTN_DRAW|BTN_TEXTLEFT 	// will be dislayed after creation with text
              |BTN_TEXTTOP, 			// left top position
              (void*)&set2,//NULL,//(void*)&bulboff,          // use bitmap
              NULL,//TimeStr,//OffBulbStr, 				// text
              mainScheme);	            // alternative GOL scheme 
              
    StCreate(ID_STATICTEXT1,           	// ID For the Time display
             GetMaxX()-(GetTextWidth(dateTimeStr,(void*)&GOLSmallFont)+15),
             3,							// dimensions
             GetMaxX(),
             3+(GetTextHeight((void*)&GOLSmallFont)<<1),	
             ST_DRAW|ST_FRAME|
             ST_CENTER_ALIGN,     		// will be dislayed, has frame and center aligned
             dateTimeStr, 				// text is from time value
             timemainScheme);             	// alternate scheme   

for(i=64;i>0;i--)
{	SetColor(RGB565CONVERT(4*i,4*i,4*i));
	Bar(0, (GetMaxY()-i), GetMaxX(), (GetMaxY()-i+1));
}

 serialnumber = EEPROMReadWord(EESERIAL_NUMBER);
 serialnumberRecord = serialnumber - 1;// 保持记录一致
if((Record_Over_Flag == 0xaaaa)&&(serialnumber == 1))
		serialnumberRecord = 1;
 
 serialnumberStr[0] = (serialnumberRecord /10000) +'0';// 万位
serialnumberStr[1] = (serialnumberRecord /1000 %10) +'0';// 千位
serialnumberStr[2] = (serialnumberRecord /100 %10) +'0';// 百位
serialnumberStr[3] = (serialnumberRecord /10 %100%10) +'0';// 十位
serialnumberStr[4] = (serialnumberRecord %10) +'0';// 个位

datetemp=EEPROMReadByte(inspectYearAddress);
#ifdef TradeMark_Version
datetemp++;
#endif
Markdate[8]=(datetemp /10) + '0';
Markdate[9]=(datetemp%10) + '0';
datetemp=EEPROMReadByte(inspectMonthAddress);
Markdate[3]=(datetemp /10) + '0';
Markdate[4]=(datetemp%10) + '0';
datetemp=EEPROMReadByte(inspectDayAddress);
Markdate[0]=(datetemp /10) + '0';
Markdate[1]=(datetemp%10) + '0';

 SetFont((void*)&GOLMediumFont);
 SetColor(MAGENTA);
 OutTextXY(5,270,PreCalDate);

 SetFont((void*)&GOLMediumFont);
 SetColor(MAGENTA);
 OutTextXY(148,270,Markdate);
 
 SetFont((void*)&GOLMediumFont);
 SetColor(CYAN);
 OutTextXY(5,290,NumStored);

 SetFont((void*)&GOLMediumFont);
 SetColor(CYAN);
 OutTextXY(130,290,serialnumberStr);
 
GOLSetFocus(obj);

}

WORD MsgMain(WORD objMsg, OBJ_HEADER* pObj){
	OBJ_HEADER* pOtherRbtn;
unsigned int MsgAdd;
unsigned char ik;
    switch(GetObjID(pObj)){

        case ID_BUTTON_NEXT:
            if(objMsg == BTN_MSG_RELEASED){
               screenState = CREATE_MAIN;// goto check box demo screen
            }
            return 1; 							// process by default

        case ID_BUTTON_BACK:
            if(objMsg == BTN_MSG_RELEASED){
                screenState = CREATE_MAIN; 		// goto ECG demo screen
            }
            return 1; 							// process by default

        case ID_BUTTON1:
            if(objMsg == BTN_MSG_RELEASED){
				if(Demarcate&&EEPROMReadByte(ID_CHECKBOXADDRESS+ID_CHECKBOX8))
					screenState = CREATE_CALLOCK;
			else {	
				for(ik=0;ik<200;ik++) Flashbuff[49+ik] = 0;
				TestMode=0xaa;
				g_backlight_time = 3*60;// 3分钟不吹气，自动跳出来
		        SaveReadItem(FALSE);
            	for(MsgAdd =0;MsgAdd<10;MsgAdd++)//ItemMax
            	{
            		if(EEPROMReadByte(ID_CHECKBOXADDRESS +MsgAdd))
            		break;
            	}
            	if(MsgAdd<10)//ItemMax
                screenState = 2*MsgAdd+CREATE_ITEM0;
              else
                  screenState = CREATE_Alcoholdetect;
			  

				}
            }
            return 1; 							// process by default

        case ID_BUTTON2:
            if(objMsg == BTN_MSG_RELEASED){
             if(Demarcate&&EEPROMReadByte(ID_CHECKBOXADDRESS+ID_CHECKBOX8))
					screenState = CREATE_CALLOCK;
			else {
				for(ik=0;ik<200;ik++) Flashbuff[49+ik] = 0;
				TestMode=0xbb;
                screenState = CREATE_Alcoholdetect;//CREATE_Waittest;//
				g_backlight_time = 3*60;// 3分钟不吹气，自动跳出来
            }
            	}
            return 1; 							// process by default
            
        case ID_BUTTON3:
        	if(objMsg == BTN_MSG_PRESSED){ 		
 				if(Demarcate&&EEPROMReadByte(ID_CHECKBOXADDRESS+ID_CHECKBOX8))
					screenState = CREATE_CALLOCK;
			else {
				TestMode=0xcc;
			screenState = CREATE_Waittest; 		// goto ECG demo screen
			status_Press=FALSE;
			//Masklevel = 0xaa;
			
	        for(ik=0;ik<200;ik++) Flashbuff[49+ik] = 0;//clear  buffer --add by xiao 090811 
			g_backlight_time = 3*60;// 3分钟不吹气，自动跳出来
            }
        		}
            return 1;  							// process by default

        case ID_BUTTON4:

			if(objMsg == BTN_MSG_RELEASED)//BTN_MSG_PRESSED)
			{
               if(serialnumberRecord>0)				//保证不写记录时候不显示
                screenState = CREATE_RECORD; 		// goto ECG demo screen
	            
	        }
		    return 1; 							// Do not process by default

        case ID_BUTTON5:
        	if(objMsg == BTN_MSG_RELEASED)// changed by Spring.chen
        	{

	            screenState = CREATE_GPS;
	        }
		    return 1; 							// Do not process by default

        case ID_BUTTON6:

            if(objMsg == BTN_MSG_RELEASED){

	        	screenState = CREATE_TO_PC; 		// goto Download demo screen
	        	#ifdef CHANGE4800
	        	ChangeBRG();
                #endif 
            }
            return 1; 							// process by default

        case ID_BUTTON7:
            if(objMsg == BTN_MSG_RELEASED){

				screenState = CREATE_DATETIME;

            }
            return 1; 							// process by default

        case ID_BUTTON8:
            if(objMsg == BTN_MSG_RELEASED){
               screenState = CREATE_SETTING; 
            }
            return 1; 							// process by default

        case ID_BUTTON9:
            if(objMsg == BTN_MSG_RELEASED){
                screenState = CREATE_MASTERPASSWORD; //CREATE_PASSWORD;//; 		// goto SETTING demo screenf
            }
            return 1; 							// process by default

        default:
            return 1; 							// process by default
    }
}

// create main end
void CreateAlcoholdetect(){

      GOLFree();  
mainScheme->pFont = (void*)&GOLFontDefault;
	SetColor(WHITE); //WHITE
    ClearDevice();  

		StCreate(ID_STATICTEXT3,           		// ID 
              40,40,
              200,80,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
		      Attachpipe,
              mainScheme);
		
		StCreate(ID_STATICTEXT3,           		// ID 
              40,120,
              200,160,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
		      Start_test,
              mainScheme);
		
		BtnCreate(ID_BUTTON1, 				// button ID 
              40,200,
              200,270,           	// dimension
              0,					   	// set radius 
              BTN_DRAW,  			   	// draw a beveled button
              NULL,                    	// no bitmap
              Start,//ButtonStr, 				// "Button",     	// text
              mainScheme);              	// use alternate scheme
              
    BtnCreate(ID_BUTTON_HOME,   	    // button ID 
		      100,GetMaxY()-40,					   	// left, top corner	
              140,GetMaxY(),0, 		   	// right, bottom corner (with radius = 0)
              BTN_DRAW,               	// will be dislayed and disabled after creation
              (void*)&HOME,					   	// no bitmap
              NULL,//, NextStr			// RIGHT arrow as text
              blackScheme);               

}
WORD MsgAlcoholdetect(WORD objMsg, OBJ_HEADER* pObj){

	WORD objectID;

	objectID=GetObjID(pObj);

if(objectID==ID_BUTTON1)	 {
      if(objMsg==BTN_MSG_RELEASED){
	mainScheme->pFont = (void*)&GOLMediumFont;
screenState = CREATE_Aircheck;
      	}

 }
else if(objectID==ID_BUTTON_HOME)
{
   							
   if(objMsg == BTN_MSG_RELEASED){
	       mainScheme->pFont = (void*)&GOLMediumFont;
           screenState = CREATE_MAIN; 			
           return 1;
       }

}

}

void Aircheck(){

	GOLFree();  

	//SetColor(WHITE); //WHITE
    ClearDevice(); 

		StCreate(ID_STATICTEXT3,           		// ID 
              40,40,
              200,80,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
	Aircheckstr,
              mainScheme);


	  StCreate(ID_STATICTEXT3,           		// ID 
              40,120,
              200,160,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
	    WaitStr,
              mainScheme);
			



}

void ResidentAlcoholdetected(){

	GOLFree();  
    mainScheme->TextColor0= BRIGHTRED;
	mainScheme->pFont = (void*)&GOLFontDefault;
   SetColor(WHITE);
    ClearDevice(); 
	
    WndCreate(ID_WINDOW1,       		// ID
              0,0,GetMaxX(),GetMaxY(), 	// dimension
              WND_DRAW,                	// will be dislayed after creation
              NULL,               	// icon
              Aircheckwinstr,	   				// set text 
              navScheme);               // default GOL scheme 	
	  StCreate(ID_STATICTEXT3,           		// ID 
              40,120,
              200,160,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
	     Alcoholdetected,
              mainScheme);
	

	BtnCreate(ID_BUTTON1,         	// button ID 
              0,GetMaxY()-40,					   	// left, top corner	
              40,GetMaxY(),0, 		   	// right, bottom corner (with radius = 0)
              BTN_DRAW,               	// will be dislayed after creation
              (void*)&LEFT,					   	// no bitmap	
             NULL,//TestagainStr ,//, HomeStr			// LEFT arrow as text
              blackScheme);          	   	// use navigation scheme
              
    BtnCreate(ID_BUTTON2,   	    // button ID 
              GetMaxX()-40,GetMaxY()-40,
              GetMaxX(),GetMaxY(),0,   	// dimension (with radius = 0)
              BTN_DRAW,               	// will be dislayed and disabled after creation
              (void*)&HOME,					   	// no bitmap
              NULL,//, NextStr			// RIGHT arrow as text
              blackScheme);  



}

WORD MsgAlcoholdetected(WORD objMsg, OBJ_HEADER* pObj){


if(GetObjID(pObj)==ID_BUTTON1)	
  {
      if(objMsg==BTN_MSG_RELEASED){
	  	alarmON = FALSE;
	    mainScheme->TextColor0 = BLACK;
	    mainScheme->pFont = (void*)&GOLMediumFont;	
        screenState = CREATE_Alcoholdetect;
      	}


 }
else if(GetObjID(pObj)==ID_BUTTON2)
{

      if(objMsg==BTN_MSG_RELEASED){
	    mainScheme->TextColor0 = BLACK;
	    mainScheme->pFont = (void*)&GOLMediumFont;
	  	alarmON = FALSE;
        screenState = CREATE_MAIN;
      	}



}
}

void Check_Screen()//调试触摸屏
{
	TouchCalibration();
        TouchStoreCalibration();
        EEPROMWriteWord(GRAPHICS_LIBRARY_VERSION,EEPROM_VERSION);
    

    // Load touch screen calibration parameters from EEPROM
//    TouchLoadCalibration();
    screenState = CREATE_MAIN;
}

// Creates the edit box demo screen
EDITBOX* pEbzyq;

void CreateEditTest(XCHAR *EditTestStr,WORD CharMax)
{

	static XCHAR pTempXchar[][2] = 	{	{'0',0},
										{'1',0},
										{'2',0},
										{'3',0},
										{'4',0},
										{'5',0},
										{'6',0},
										{'7',0},
										{'8',0},
										{'9',0},
										{0x2E,0},
										{'#',0}
									};

    GOLFree();  // free memory for the objects in the previous linked list and start new list
//pump_pwr = 1;
	CreatePage(EditTestStr);//EditBoxStr
	
#define KEYSTARTX 	15
#define KEYSTARTY 	38
#define KEYSIZEX	70
#define KEYSIZEY  	45
#define MAXCHARSIZE CharMax
MAXCHAROld=CharMax;
//if(CharMax != 1)
	EbCreate(ID_EDITBOX1,              	// ID
              4,KEYSTARTY+1,GetMaxX()-4,KEYSTARTY+1*KEYSIZEY-GOL_EMBOSS_SIZE,  // dimension
              EB_DRAW|EB_CARET|EB_FOCUSED, // will be dislayed after creation
              NULL,
              CharMax,//MAXCHARSIZE,
              altScheme);               // default GOL scheme

    BtnCreate(ID_KEYPAD+0,             	// ID 
              KEYSTARTX+1*KEYSIZEX,		
              KEYSTARTY+4*KEYSIZEY,
              KEYSTARTX+2*KEYSIZEX,
              KEYSTARTY+5*KEYSIZEY,    	// dimension
              0,
              BTN_DRAW,                	// will be dislayed after creation
              NULL,                    	// no bitmap
              pTempXchar[0],    		// text
              altScheme);              	// alternative GOL scheme 

    BtnCreate(ID_KEYPAD+1,             	// ID 
              KEYSTARTX+0*KEYSIZEX,
              KEYSTARTY+1*KEYSIZEY,
              KEYSTARTX+1*KEYSIZEX,
              KEYSTARTY+2*KEYSIZEY,    	// dimension
              0,
              BTN_DRAW,                	// will be dislayed after creation
              NULL,                    	// no bitmap
              pTempXchar[1],    		// text
              altScheme);              	// alternative GOL scheme   

    BtnCreate(ID_KEYPAD+2,             	// ID 
              KEYSTARTX+1*KEYSIZEX,
              KEYSTARTY+1*KEYSIZEY,
              KEYSTARTX+2*KEYSIZEX,
              KEYSTARTY+2*KEYSIZEY,    	// dimension
              0,
              BTN_DRAW,                	// will be dislayed after creation
              NULL,                    	// no bitmap
              pTempXchar[2],    		// text
              altScheme);              	// alternative GOL scheme 

    BtnCreate(ID_KEYPAD+3,             	// ID 
              KEYSTARTX+2*KEYSIZEX,
              KEYSTARTY+1*KEYSIZEY,
              KEYSTARTX+3*KEYSIZEX,
              KEYSTARTY+2*KEYSIZEY,    	// dimension
              0,
              BTN_DRAW,                	// will be dislayed after creation
              NULL,                    	// no bitmap
              pTempXchar[3],    		// text
              altScheme);              	// alternative GOL scheme 

    BtnCreate(ID_KEYPAD+4,             	// ID 
              KEYSTARTX+0*KEYSIZEX,
              KEYSTARTY+2*KEYSIZEY,
              KEYSTARTX+1*KEYSIZEX,
              KEYSTARTY+3*KEYSIZEY,    	// dimension
              0,
              BTN_DRAW,                	// will be dislayed after creation
              NULL,                    	// no bitmap
              pTempXchar[4],    		// text
              altScheme);              	// alternative GOL scheme   

    BtnCreate(ID_KEYPAD+5,             	// ID 
              KEYSTARTX+1*KEYSIZEX,
              KEYSTARTY+2*KEYSIZEY,
              KEYSTARTX+2*KEYSIZEX,
              KEYSTARTY+3*KEYSIZEY,    	// dimension
              0,
              BTN_DRAW,                	// will be dislayed after creation
              NULL,                    	// no bitmap
              pTempXchar[5],    		// text
              altScheme);              	// alternative GOL scheme 

    BtnCreate(ID_KEYPAD+6,             	// ID 
              KEYSTARTX+2*KEYSIZEX,
              KEYSTARTY+2*KEYSIZEY,
              KEYSTARTX+3*KEYSIZEX,
              KEYSTARTY+3*KEYSIZEY,    	// dimension
              0,
              BTN_DRAW,                	// will be dislayed after creation
              NULL,                    	// no bitmap
              pTempXchar[6],    		// text
              altScheme);              	// alternative GOL scheme 

    BtnCreate(ID_KEYPAD+7,             	// ID 
              KEYSTARTX+0*KEYSIZEX,
              KEYSTARTY+3*KEYSIZEY,
              KEYSTARTX+1*KEYSIZEX,
              KEYSTARTY+4*KEYSIZEY,    	// dimension
              0,
              BTN_DRAW,                	// will be dislayed after creation
              NULL,                    	// no bitmap
              pTempXchar[7],    		// text
              altScheme);              	// alternative GOL scheme   

    BtnCreate(ID_KEYPAD+8,             	// ID 
              KEYSTARTX+1*KEYSIZEX,
              KEYSTARTY+3*KEYSIZEY,
              KEYSTARTX+2*KEYSIZEX,
              KEYSTARTY+4*KEYSIZEY,    	// dimension
              0,
              BTN_DRAW,                	// will be dislayed after creation
              NULL,                    	// no bitmap
              pTempXchar[8],    		// text
              altScheme);              	// alternative GOL scheme 

    BtnCreate(ID_KEYPAD+9,             	// ID 
              KEYSTARTX+2*KEYSIZEX,
              KEYSTARTY+3*KEYSIZEY,
              KEYSTARTX+3*KEYSIZEX,
              KEYSTARTY+4*KEYSIZEY,    	// dimension
              0,
              BTN_DRAW,                	// will be dislayed after creation
              NULL,                    	// no bitmap
              pTempXchar[9],    		// text
              altScheme);              	// alternative GOL scheme 
    BtnCreate(ID_KEYPADPOID,             	// ID 
              KEYSTARTX+2*KEYSIZEX,
              KEYSTARTY+4*KEYSIZEY,
              KEYSTARTX+3*KEYSIZEX,
              KEYSTARTY+5*KEYSIZEY,    	// dimension
              0,
              BTN_DRAW,                	// will be dislayed after creation
              NULL,                    	// no bitmap
              pTempXchar[10],    		// text
              altScheme); 



    BtnCreate(ID_BACKSPACE,            	// ID 
              KEYSTARTX+0*KEYSIZEX,
              KEYSTARTY+4*KEYSIZEY,
              KEYSTARTX+1*KEYSIZEX,
              KEYSTARTY+5*KEYSIZEY,    	// dimension
              0,
              BTN_DRAW,                	// will be dislayed after creation
              NULL,                    	// no bitmap
              LeftArrowStr, 			// Left Arrow 
              altScheme);              	// alternative GOL scheme 

              


}

// Processes messages for the edit box demo screen
WORD MsgEditTest(WORD objMsg, OBJ_HEADER* pObj, GOL_MSG* pMsg){

EDITBOX* pEb;
SHORT    id;
XCHAR    temp;
static char status = 0;			// status to check if calling, holding or not

    id = GetObjID(pObj);

    // If number key is pressed
    if(objMsg == BTN_MSG_PRESSED)//BTN_MSG_RELEASED
    if(id >= ID_KEYPAD)
    if(id  < ID_KEYPAD+10){
	    if (!status) {

	        pEbzyq = (EDITBOX*)GOLFindObject(ID_EDITBOX1);
	        		EbAddChar(pEbzyq,'0'+id-ID_KEYPAD);
	        		SetState(pEbzyq, EB_DRAW);
	        		TestMiMaStr[0] = '0'+id-ID_KEYPAD;
	    }
        return 1;        
    }

    switch(id){

        case ID_BACKSPACE:
            if(objMsg == BTN_MSG_PRESSED){
                if (!status) {
	                pEb = (EDITBOX*)GOLFindObject(ID_EDITBOX1);    
	                EbDeleteChar(pEb);
	                SetState(pEb, EB_DRAW);
	            } 
            }
            return 1;


        case ID_BUTTON_NEXT:
            if(objMsg == BTN_MSG_RELEASED){
                screenState = prevState ;//++;//= CREATE_MAIN; 	// goto meter screen
//                EEPROMWriteByte((XCHAR)pEb->pBuffer, 0x7F10);	//add by Spring.Chen
            }
            if(AlcTypeaddress+16>0xF300)
            	ErrorTrap("No Memory");
            else
            	AlcTypeaddress+=16;//ErrorTrap("Memory Enough");
            return 1; // process by default

        case ID_BUTTON_BACK:
            if(objMsg == BTN_MSG_RELEASED){
                screenState = CREATE_MAIN; 	// goto list box screen
            }
            return 1; // process by default

        default:
            return 1; // process by default
    }


}
WORD MsgEditPassword(WORD objMsg, OBJ_HEADER* pObj, GOL_MSG* pMsg){

EDITBOX* pEb;
SHORT    id;
XCHAR    temp;
static char status = 0;			// status to check if calling, holding or not
static XCHAR	DIYPD[7];
BYTE i=0,pdtemp;

    id = GetObjID(pObj);

    // If number key is pressed
    if(objMsg == BTN_MSG_PRESSED)//BTN_MSG_RELEASED
    if(id >= ID_KEYPAD)
    if(id  < ID_KEYPAD+10){
	    if (!status) {

		pEb = (EDITBOX*)GOLFindObject(ID_EDITBOX1);
	        		EbAddChar(pEb,'0'+id-ID_KEYPAD);
	        		SetState(pEb, EB_DRAW);
	    }
        return 1;        
    }

    switch(id){
        case ID_BACKSPACE:
            if(objMsg == BTN_MSG_PRESSED){
                if (!status) {
	                pEb = (EDITBOX*)GOLFindObject(ID_EDITBOX1);    
	                EbDeleteChar(pEb);
	                SetState(pEb, EB_DRAW);
	            } 
            }
            return 1;


        case ID_BUTTON_NEXT:
            if(objMsg == BTN_MSG_RELEASED){

		if(screenState==DISPLAY_InputSetpassword1)
					
		{
                     for(;i<=6;i++)
                     	{
                        DIYPD[i]=PasswordBuffer[i];
						PasswordBuffer[i] = 0;
			 }
					
                      screenState=CREATE_InputSetpassword2;
		}
				
             else if(screenState==DISPLAY_InputSetpassword2)
               	{
                           if(DIYPD[0]==PasswordBuffer[0]&&DIYPD[1]==PasswordBuffer[1]&&DIYPD[2]==PasswordBuffer[2]&&DIYPD[3]==PasswordBuffer[3]&&DIYPD[4]==PasswordBuffer[4]&&DIYPD[5]==PasswordBuffer[5])        
           	         {
		               PDset=TRUE;
                       PasswordBuffer[0] = 0;
					   TempbuffStr[0] = 0;
			        if(Setpasswordposition==0xaa)
			   	   {
			               pdtemp=(DIYPD[0]-0x30)&0x0F;
			               pdtemp=(pdtemp<<4)|((DIYPD[1]-0x30)&0x0F);
			               EEPROMWriteByte(pdtemp, SettingPasswordAddress);

			               pdtemp=(DIYPD[2]-0x30)&0x0F;
			               pdtemp=(pdtemp<<4)|((DIYPD[3]-0x30)&0x0F);
			               EEPROMWriteByte(pdtemp, SettingPasswordAddress+1);

			               pdtemp=(DIYPD[4]-0x30)&0x0F;
			               pdtemp=(pdtemp<<4)|((DIYPD[5]-0x30)&0x0F);
			               EEPROMWriteByte(pdtemp, SettingPasswordAddress+2);	

						   EEPROMWriteByte(0xaa, PasswordsettedAddress); 
			   	 }
			      else if(Setpasswordposition==0xbb)
			   	{

			               pdtemp=(DIYPD[0]-0x30)&0x0F;
			               pdtemp=(pdtemp<<4)|((DIYPD[1]-0x30)&0x0F);
			               EEPROMWriteByte(pdtemp, AdvanceSettingPasswordAddress);

			               pdtemp=(DIYPD[2]-0x30)&0x0F;
			               pdtemp=(pdtemp<<4)|((DIYPD[3]-0x30)&0x0F);
			               EEPROMWriteByte(pdtemp, AdvanceSettingPasswordAddress+1);

			               pdtemp=(DIYPD[4]-0x30)&0x0F;
			               pdtemp=(pdtemp<<4)|((DIYPD[5]-0x30)&0x0F);
			               EEPROMWriteByte(pdtemp, AdvanceSettingPasswordAddress+2);

						   EEPROMWriteByte(0xaa, PasswordsettedAddress);


			       }
			   
           	}
           else  PDset=FALSE;
				   screenState=CREATE_OutcomeSetpassword;
         }
  }

             return 1; // process by default

        case ID_BUTTON_BACK:
            if(objMsg == BTN_MSG_RELEASED){
			
                    if(screenState == DISPLAY_InputSetpassword1)
			     screenState = CREATE_Setpassword;
		     else if(screenState == DISPLAY_InputSetpassword2)
			     screenState = CREATE_InputSetpassword1;
            }
            return 1; // process by default
	case ID_BUTTON_HOME:
            if(objMsg == BTN_MSG_RELEASED){
                screenState = CREATE_MAIN; 	// goto list box screen
            }
            return 1;
        default:
            return 1; // process by default
    }


}
WORD MsgEditMark(WORD objMsg, OBJ_HEADER* pObj, GOL_MSG* pMsg)

{
unsigned long Valuelong;
EDITBOX* pEb;
SHORT    id;
XCHAR    temp;
unsigned int  j=1,i=0,point=0,K_Value;
static char status = 0;			// status to check if calling, holding or not
//TempbuffStr[]={0};
    id = GetObjID(pObj);

    // If number key is pressed
    if(objMsg == BTN_MSG_PRESSED)//BTN_MSG_RELEASED
    {
    if(id >= ID_KEYPAD)
    if(id  <= ID_KEYPAD+10){
	    if (!status) {
        pEb = (EDITBOX*)GOLFindObject(ID_EDITBOX1);    
/*
          if(id  == ID_KEYPAD+10)
		  	EbAddChar2(pEb,0x2e);
          else*/
			EbAddChar2(pEb,'0'+id-ID_KEYPAD);
	        SetState(pEb, EB_DRAW);
	      /*	  pEbzyq = (EDITBOX*)GOLFindObject(ID_STATICTEXT4);
	        		EbAddChar(pEbzyq,'0'+id-ID_KEYPAD);
	        		SetState(pEbzyq, EB_DRAW);*/
	        		TestMiMaStr[0] = '0'+id-ID_KEYPAD;
                  //k=pEb->charMax;

				//  consbuffStr[]=TempbuffStr[];

				  
	    }
        return 1;        
    }

    if(id  == ID_KEYPADPOID)
    {
    	if (!status) {
        pEb = (EDITBOX*)GOLFindObject(ID_EDITBOX1);
        
		  	EbAddChar2(pEb,0x2e);
		  	SetState(pEb, EB_DRAW);
    		}
    	return 1;
    }
}


	
	
    switch(id){


        case ID_BACKSPACE:
            if(objMsg == BTN_MSG_PRESSED){
                if (!status) {
	                pEb = (EDITBOX*)GOLFindObject(ID_EDITBOX1);    
	                EbDeleteChar(pEb);
	                SetState(pEb, EB_DRAW);
	            } 
            }
            return 1;
        case ID_BUTTON_HOME:
            if(objMsg == BTN_MSG_RELEASED){

                screenState = CREATE_MAIN; 	// goto list box screen
            }
            return 1;
        case ID_BUTTON_NEXT:
            if(objMsg == BTN_MSG_RELEASED){
				 MarkTemp=0;
				 TempbuffStr[Valnum]=0;
                 for(i=0;i<19;i++){
                     consbuffStr[i]=TempbuffStr[i];
					 TempbuffStr[i] = 0x0000;
   	                   }
				 /*  if(point<=2){
					 for(i=0;i<19;i++){
                     consbuffStr[i]=0x0000;					
   	                   }
                	}*/
				if((consbuffStr[0]!=0)&&(consbuffStr[0]!=0x2E)){	

                   for(i=0;i<=Valnum-1;i++){
					
					if(consbuffStr[i]==0x2E)
						point=i;
                	}
/*
                 if(point==0){

					for(i=0,j=1;i<=(Valnum-1);i++)
						{
					      j*=10;
                	     MarkTemp+=(consbuffStr[Valnum-1-i]-48)*j;

					}
                 	}
				else {
					if(point==(Valnum-1)){
			  	   consbuffStr[Valnum-1]=0x0000;
				   for(i=0,j=1;i<=(Valnum-2);i++)
                	  {
					     j*=10;
                	     MarkTemp+=(consbuffStr[Valnum-2-i]-48)*j;
                	   }
              	 }
                  else  
                
                	{
				MarkTemp+=(consbuffStr[Valnum-1]-48);              
			    for(i=0,j=1;i<=(Valnum-2);i++)
                	{
                	    if(consbuffStr[Valnum-2-i]!=0x2e)
                	    	{
					     j*=10;
                	     MarkTemp+=(consbuffStr[Valnum-2-i]-48)*j;
                	    	}
                	}
                	}
            	}
            	*/
				// 
				MarkTemp = 0;
                MarkTemp+=(consbuffStr[4]-48);
			    for(i=1,j=1;i<=4;i++)
                	{
                	    if(consbuffStr[4-i]!=0x2e)
                	    	{
					          j*=10;
                	          MarkTemp+=(consbuffStr[4-i]-48)*j;
                	    	}
                	}  
				
				K_Value=EEPROMReadWord(KSelectDataAddress);

				K_Value=K_Value/10;
				
				Valuelong = (unsigned long)MarkTemp;
				
				Valuelong = Valuelong*K_Value;
				
				Valuelong = Valuelong/100;

				MarkTemp = (unsigned int)Valuelong;
				
			EEPROMWriteWord(MarkTemp,0x6020);
		}

                
				Marktemporary=Temperature();
				EEPROMWriteWord(Marktemporary,0x6024);
			//	MarkTem= (unsigned char)Marktemporary;                   				

				Marktem[2]=(Marktemporary%10+'0');
                
				Marktem[1]=(Marktemporary/10+'0');

				if(Kt=1)
					Marktem[0]=0x0020;
				else
					Marktem[0]=0x002D;
				
				screenState = CREATE_Mark;// goto meter screen
//                EEPROMWriteByte((XCHAR)pEb->pBuffer, 0x7F10);	//add by Spring.Chen
            }
            if(AlcTypeaddress+16>0xF300)
            	ErrorTrap("No Memory");
            else
            	AlcTypeaddress+=16;//ErrorTrap("Memory Enough");
            return 1; // process by default

        case ID_BUTTON_BACK:
            if(objMsg == BTN_MSG_RELEASED){
          //      if(Settingposition==0xaa)
           //     screenState = CREATE_SETTING;				// goto slider screen
            //    else if(Settingposition==0xbb)
					screenState = CREATE_Master;
            }
            return 1; // process by default

        default:
            return 1; // process by default
    }


}

WORD MsgPassword(WORD objMsg, OBJ_HEADER* pObj, GOL_MSG* pMsg){

EDITBOX* pEb;
SHORT    id;
XCHAR    temp;
static char status = 0;			// status to check if calling, holding or not

    id = GetObjID(pObj);

    // If number key is pressed
    if(objMsg == BTN_MSG_PRESSED)//BTN_MSG_RELEASED
    if(id >= ID_KEYPAD)
    if(id  < ID_KEYPAD+10){
	    if (!status) {

		pEb = (EDITBOX*)GOLFindObject(ID_EDITBOX1);
	        		EbAddChar(pEb,'0'+id-ID_KEYPAD);
	        		SetState(pEb, EB_DRAW);
	    }
        return 1;        
    }

    switch(id){
        case ID_BACKSPACE:
            if(objMsg == BTN_MSG_PRESSED){
                if (!status) {
	                pEb = (EDITBOX*)GOLFindObject(ID_EDITBOX1);    
	                EbDeleteChar(pEb);
	                SetState(pEb, EB_DRAW);
	            } 
            }
            return 1;


        case ID_BUTTON_NEXT:
            if(objMsg == BTN_MSG_RELEASED){
		if((PasswordBuffer[0] == PDread[0])&&(PasswordBuffer[1] == PDread[1])&&(PasswordBuffer[2] == PDread[2])\
            		&&(PasswordBuffer[3] == PDread[3])&&(PasswordBuffer[4] == PDread[4])&&(PasswordBuffer[5] == PDread[5]))
            		{
            		screenState = CREATE_AGENT;//CREATE_Choosegas;
            		PasswordBuffer[0] = 0;
					TempbuffStr[0] = 0;
            		}
            }
            else
            	{
			pEb = (EDITBOX*)GOLFindObject(ID_EDITBOX1);    
                EbSetText(pEb, WrongPasswordStr);	
                SetState(pEb, EB_DRAW);
            	}
            return 1; // process by default

        case ID_BUTTON_BACK:
            if(objMsg == BTN_MSG_RELEASED){
       //         if(Settingposition==0xaa)
        //        screenState = CREATE_SETTING;				// goto slider screen
        //        else if(Settingposition==0xbb)
					screenState = CREATE_Master;
            }
            return 1; // process by default
	    case ID_BUTTON_HOME:
            if(objMsg == BTN_MSG_RELEASED){
                screenState = CREATE_MAIN; 	// goto list box screen
            }
            return 1;
        default:
            return 1; // process by default
    }


}
WORD MsgScreenPassword(WORD objMsg, OBJ_HEADER* pObj, GOL_MSG* pMsg){

EDITBOX* pEb;
SHORT    id;
XCHAR    temp;
static char status = 0;			// status to check if calling, holding or not

    id = GetObjID(pObj);

    // If number key is pressed
    if(objMsg == BTN_MSG_PRESSED)//BTN_MSG_RELEASED
    if(id >= ID_KEYPAD)
    if(id  < ID_KEYPAD+10){
	    if (!status) {

		pEb = (EDITBOX*)GOLFindObject(ID_EDITBOX1);
	        		EbAddChar(pEb,'0'+id-ID_KEYPAD);
	        		SetState(pEb, EB_DRAW);
	    }
        return 1;        
    }

    switch(id){
        case ID_BACKSPACE:
            if(objMsg == BTN_MSG_PRESSED){
                if (!status) {
	                pEb = (EDITBOX*)GOLFindObject(ID_EDITBOX1);    
	                EbDeleteChar(pEb);
	                SetState(pEb, EB_DRAW);
	            } 
            }
            return 1;


        case ID_BUTTON_NEXT:
            if(objMsg == BTN_MSG_RELEASED){
            	if((PasswordBuffer[0] == '1')&&(PasswordBuffer[1] == '1')&&(PasswordBuffer[2] == '9')
            		&&(PasswordBuffer[3] == '1')&&(PasswordBuffer[4] == '1')&&(PasswordBuffer[5] == '9'))
            		{
            		PasswordBuffer[0] = 0;
					TempbuffStr[0] = 0;
            		 	dif=1;
				screenState = CREATE_DELETE;
				YesNoNumber = 2;// 删除记录         		
            		}
            }
            else
            	{
			pEb = (EDITBOX*)GOLFindObject(ID_EDITBOX1);    
                EbSetText(pEb, WrongPasswordStr);	
                SetState(pEb, EB_DRAW);
            	}
            return 1; // process by default

        case ID_BUTTON_BACK:
            if(objMsg == BTN_MSG_RELEASED){
				if(Settingposition==0xaa)
                screenState = CREATE_SETTING;				// goto slider screen
                else if(Settingposition==0xbb)
					screenState = CREATE_Master;
            }
            return 1; // process by default
	    case ID_BUTTON_HOME:
            if(objMsg == BTN_MSG_RELEASED){
                screenState = CREATE_MAIN; 	// goto list box screen
            }
            return 1;
        default:
            return 1; // process by default
    }


}

WORD MsgMasterPassword(WORD objMsg, OBJ_HEADER* pObj, GOL_MSG* pMsg){

EDITBOX* pEb;
SHORT    id;
XCHAR    temp;
static char status = 0;			// status to check if calling, holding or not

    id = GetObjID(pObj);

    // If number key is pressed
    if(objMsg == BTN_MSG_PRESSED)//BTN_MSG_RELEASED
    if(id >= ID_KEYPAD)
    if(id  < ID_KEYPAD+10){
	    if (!status) {
/*	        pEb = (EDITBOX*)GOLFindObject(ID_EDITBOX1);    
	        EbAddChar(pEb,'0'+id-ID_KEYPAD);
	        SetState(pEb, EB_DRAW);*/
//	        pEb = (EDITBOX*)GOLFindObject(ID_STATICTEXT4);
		pEb = (EDITBOX*)GOLFindObject(ID_EDITBOX1);
//	        		EbAddChar(pEb,0x2A);
	        		EbAddChar(pEb,'0'+id-ID_KEYPAD);
	        		SetState(pEb, EB_DRAW);
//	        		TestMiMaStr[0] = '0'+id-ID_KEYPAD;
//	        		EEPROMWriteByte(TestMiMaStr[0] , PrintTimesAddress);
	    }
        return 1;        
    }

    switch(id){

        case ID_BACKSPACE:
            if(objMsg == BTN_MSG_PRESSED){
                if (!status) {
	                pEb = (EDITBOX*)GOLFindObject(ID_EDITBOX1);    
	                EbDeleteChar(pEb);
	                SetState(pEb, EB_DRAW);
	            } 
            }
            return 1;

        case ID_BUTTON_NEXT:
			/*
            if(objMsg == BTN_MSG_RELEASED){
            	if((PasswordBuffer[0] == '1')&&(PasswordBuffer[1] == '1')&&(PasswordBuffer[2] == '2')
            		&&(PasswordBuffer[3] == '1')&&(PasswordBuffer[4] == '1')&&(PasswordBuffer[5] == '2'))
            		{
            		screenState = CREATE_AGENT;
            		PasswordBuffer[0] = 0;
            		}
            }
            else
            	{
			pEb = (EDITBOX*)GOLFindObject(ID_EDITBOX1);    
                EbSetText(pEb, WrongPasswordStr);	
                SetState(pEb, EB_DRAW);
            	}
            	*/
		   if(objMsg == BTN_MSG_RELEASED){
		if((PasswordBuffer[0] == PDread[0])&&(PasswordBuffer[1] == PDread[1])&&(PasswordBuffer[2] == PDread[2])\
            		&&(PasswordBuffer[3] == PDread[3])&&(PasswordBuffer[4] == PDread[4])&&(PasswordBuffer[5] == PDread[5]))
            		{
            		while( ADS7843_INT!=1);//wait button released
            		screenState = CREATE_Master;
            		PasswordBuffer[0] = 0;
					TempbuffStr[0] = 0;
            		}
		else  if((PasswordBuffer[0] == PDread2[0])&&(PasswordBuffer[1] == PDread2[1])&&(PasswordBuffer[2] == PDread2[2])\
            		&&(PasswordBuffer[3] == PDread2[3])&&(PasswordBuffer[4] == PDread2[4])&&(PasswordBuffer[5] == PDread2[5]))

            		{
            		while( ADS7843_INT!=1);//wait button released
            		screenState = CREATE_AGENT;//CREATE_Choosegas;//CREATE_SETTING;
            		PasswordBuffer[0] = 0;
					TempbuffStr[0] = 0;
            		}
      
            else
            	{
			pEb = (EDITBOX*)GOLFindObject(ID_EDITBOX1);    
                EbSetText(pEb, WrongPasswordStr);	
                SetState(pEb, EB_DRAW);
            	}
			      }
            return 1; // process by default

        case ID_BUTTON_BACK:
            if(objMsg == BTN_MSG_RELEASED){
                screenState = CREATE_MAIN; 	// goto list box screen
            }
            return 1; // process by default

	case ID_BUTTON_HOME:
            if(objMsg == BTN_MSG_RELEASED){
//            	abcStatus = 0;
                screenState = CREATE_MAIN; 	// goto list box screen
            }
            return 1;
        default:
            return 1; // process by default
    }


}

WORD MsgEditInspectDate(WORD objMsg, OBJ_HEADER* pObj, GOL_MSG* pMsg){

EDITBOX* pEb;
SHORT    id;
XCHAR    temp;
static char status = 0;			// status to check if calling, holding or not
static XCHAR	DIYPE[7];
BYTE i=0,pdtemp;

    id = GetObjID(pObj);

    // If number key is pressed
    if(objMsg == BTN_MSG_PRESSED)//BTN_MSG_RELEASED
    if(id >= ID_KEYPAD)
    if(id  < ID_KEYPAD+10){
	    if (!status) {

		pEb = (EDITBOX*)GOLFindObject(ID_EDITBOX1);
	        		EbAddChar3(pEb,'0'+id-ID_KEYPAD);
	        		SetState(pEb, EB_DRAW);
	    }
        return 1;        
    }

    switch(id){
        case ID_BACKSPACE:
            if(objMsg == BTN_MSG_PRESSED){
                if (!status) {
	                pEb = (EDITBOX*)GOLFindObject(ID_EDITBOX1);    
	                EbDeleteChar(pEb);
	                SetState(pEb, EB_DRAW);
	            } 
            }
            return 1;


        case ID_BUTTON_NEXT:
            if(objMsg == BTN_MSG_RELEASED){
	
			for(;i<=6;i++)
			   {
			   DIYPE[i]=TempbuffStr[i];
			   TempbuffStr[i] = 0;
	}


			               pdtemp=DIYPE[0]-0x30;
			               pdtemp=pdtemp*10+DIYPE[1]-0x30;
			               EEPROMWriteByte(pdtemp, inspectDayAddress);

			               pdtemp=DIYPE[2]-0x30;
			               pdtemp=pdtemp*10+DIYPE[3]-0x30;
			               EEPROMWriteByte(pdtemp, inspectMonthAddress);

			               pdtemp=DIYPE[4]-0x30;
			               pdtemp=pdtemp*10+DIYPE[5]-0x30;
			               EEPROMWriteByte(pdtemp, inspectYearAddress);	

					 EEPROMWriteByte(0xaa, inspectdateSettedAddress); 
		   
         	screenState = CREATE_ChooseCal;	//  
         }


             return 1; // process by default

        case ID_BUTTON_BACK:
            if(objMsg == BTN_MSG_RELEASED){
			
			screenState = CREATE_ChooseCal;	//

            }
            return 1; // process by default
	case ID_BUTTON_HOME:
            if(objMsg == BTN_MSG_RELEASED){
                screenState = CREATE_MAIN; 	// goto list box screen
            }
            return 1;
        default:
            return 1; // process by default
    }


}

XCHAR pTempXchar[][2] = 	{	{'0',0},
										{'1',0},
										{'2',0},
										{'3',0},
										{'4',0},
										{'5',0},
										{'6',0},
										{'7',0},
										{'8',0},
										{'9',0},
										{'*',0},
										{'#',0}
									};
	

XCHAR pTemprussian[][2] = {											
										{0x00B4,0},//a
										{0x00A8,0},//b
										{0x00b1,0},//c
										{0x00A2,0},//d
										{0x00B3,0},//e
										{0x00a0,0},//f
										{0x00Af,0},//g
										{0x00b0,0},//h
										{0x00B8,0},//i
										{0x00Ae,0},//j
										{0x00Ab,0},//k
										{0x00A4,0},//l 
										{0x00Bc,0},//m 
										{0x00B2,0},//n 
										{0x00B9,0},//o 
										{0x00A7,0},//p 
										{0x00A9,0},//q 
										{0x00Aa,0},//r 
										{0x00Bb,0},//s 
										{0x00A5,0},//t 
										{0x00A3,0},//u 
										{0x00Ac,0},//v 
										{0x00B6,0},//w 
										{0x00B7,0},//x 
										{0x00Ad,0},//y 
										{0x00Bf,0},//z 
										{0x00B5,0},
										{0x00Ba,0},
										{0x00A6,0},
										{0x00Bd,0},
										{0x00a1,0},
										{0x00Be,0},
										//{0x00c0,0},
										{0x007D,0},
									};
XCHAR pTempRUSSIAN[][2] = {  
										{0x0094,0},//A
										{0x0088,0},//B
										{0x0091,0},//C
										{0x0082,0},//D
										{0x0093,0},//E
										{0x0080,0},//F
										{0x008f,0},//G
										{0x0090,0},//H
										{0x0098,0},//I
										{0x008e,0},//J
										{0x008b,0},//K
										{0x0084,0},//L
										{0x009c,0},//M
										{0x0092,0},//N
										{0x0099,0},//O
										{0x0087,0},//P
										{0x0089,0},//Q
										{0x008a,0},//R
										{0x009b,0},//S
										{0x0085,0},//T
										{0x0083,0},//U
										{0x008c,0},//V
										{0x0096,0},//W
										{0x0097,0},//X
										{0x008d,0},//Y
										{0x009f,0},//Z
										{0x0095,0},
										{0x009a,0},
										{0x0086,0},
										{0x009d,0},
										{0x0081,0},
										{0x009e,0},
										//{0x007f,0},
										{0x007C,0},
										{0x002d,0},
										{0x002e,0},
									};
XCHAR KEYPADRUSSIAN[][2] = {
	                         {0x0095,0},
							 {0x009A,0},
							 {0x0086,0},
							 {0x009D,0},
							 {0x0081,0},
							 {0x009E,0},
                           };
XCHAR pTempABC[][2] = 	{  {'A',0},
	                               {'B',0},
                                   {'C',0},
								   {'D',0},
								   {'E',0},
                                   {'F',0},
								   {'G',0},
								   {'H',0},
								   {'I',0},
								   {'J',0},
								   {'K',0},
								   {'L',0},
								   {'M',0},
								   {'N',0},
								   {'O',0},
							       {'P',0},
								   {'Q',0},
								   {'R',0},
								   {'S',0},
								   {'T',0},
								   {'U',0},
								   {'V',0},
								   {'W',0},
								   {'X',0},
								   {'Y',0},
								   {'Z',0},
                                  };
XCHAR pTempabc[][2] = 	{  {'a',0},
	                               {'b',0},
                                   {'c',0},
								   {'d',0},
								   {'e',0},
                                   {'f',0},
								   {'g',0},
								   {'h',0},
								   {'i',0},
								   {'j',0},
								   {'k',0},
								   {'l',0},
								   {'m',0},
								   {'n',0},
								   {'o',0},
							       {'p',0},
								   {'q',0},
								   {'r',0},
								   {'s',0},
								   {'t',0},
								   {'u',0},
								   {'v',0},
								   {'w',0},
								   {'x',0},
								   {'y',0},
								   {'z',0},
                                  };

static unsigned char languagereturn;
static char languagechange=0xee;
void CreateEditABC(XCHAR *EditTestStr,WORD CharMax)
{
	unsigned char temp;
	OBJ_HEADER* obj;

	for(temp = 0;temp<19;temp++)
              TempbuffStr[0] = 0;

    GOLFree();  // free memory for the objects in the previous linked list and start new list

	CreatePage(EditTestStr);//EditBoxStr
	
#ifndef USE_PORTRAIT
#define KEYSTARTX 	50
#define KEYABCSTARTX 	4
#else
#define KEYABCSTARTX 	4
#endif

#define KEYSTARTY 	43
#define KEYABCSIZEX 23
#define KEYSIZEY  	39
#define MAXCHARSIZE CharMax
MAXCHAROld=CharMax;
	EDITBOX* pEb;
	unsigned char Operatorcs=0;
	unsigned int Operatorpage,Operatoraddr,i;
	XCHAR OperatorNameTemp[RecordSizeMax];
	unsigned char lockTemp[RecordSizeMax];
	unsigned int LockAdd,ItemStarAdd;

/*
if(CharMax == 32)
	EbCreate(ID_EDITBOX1,              	// ID
              KEYABCSTARTX,KEYSTARTY+1,GetMaxX()-KEYABCSTARTX,KEYSTARTY+1*KEYSIZEY-GOL_EMBOSS_SIZE,  // dimension
              EB_DRAW|EB_CARET|EB_FOCUSED, // will be dislayed after creation
              NULL,
              CharMax,//MAXCHARSIZE,
              alt3Scheme);               // default GOL scheme
else
*/	
EbCreate(ID_EDITBOX1,              	// ID
              KEYABCSTARTX,KEYSTARTY+1,GetMaxX()-KEYABCSTARTX,KEYSTARTY+1*KEYSIZEY-GOL_EMBOSS_SIZE,  // dimension
              EB_DRAW|EB_CARET|EB_FOCUSED, // will be dislayed after creation
              NULL,
              CharMax,//MAXCHARSIZE,
              alt3Scheme);               // default GOL scheme

       if(screenState>=CREATE_ITEM0)
       	if(screenState<DISPLAY_ITEM9)
       {
       	Operatorpage = (serialnumberRecord-1)/2;
		Operatoraddr = ((serialnumberRecord-1)%2)*256;
		if(serialnumberRecord>0)
		{
			switch(screenState)// ItemState
	            	{
	            	case CREATE_ITEM0:
	            		LockAdd =ID_CHECKBOX20;
	            		ItemStarAdd = Item0DataAddress;
	            		break;
	            	case CREATE_ITEM1:
	            		LockAdd =ID_CHECKBOX21;
	            		ItemStarAdd = Item1DataAddress;
	            		break;
	            	case CREATE_ITEM2:
	            		LockAdd =ID_CHECKBOX22;
	            		ItemStarAdd = Item2DataAddress;
	            		break;
	            	case CREATE_ITEM3:
	            		LockAdd =ID_CHECKBOX23;
	            		ItemStarAdd = Item3DataAddress;
	            		break;
	            	case CREATE_ITEM4:
	            		LockAdd =ID_CHECKBOX24;
	            		ItemStarAdd = Item4DataAddress;
	            		break;
	            	case CREATE_ITEM5:
	            		LockAdd =ID_CHECKBOX25;
	            		ItemStarAdd = Item5DataAddress;
	            		break;
	            	case CREATE_ITEM6:
	            		LockAdd =ID_CHECKBOX26;
	            		ItemStarAdd = Item6DataAddress;
	            		break;
	            	case CREATE_ITEM7:
	            		LockAdd =ID_CHECKBOX27;
	            		ItemStarAdd = Item7DataAddress;
	            		break;
	            	case CREATE_ITEM8:
	            		LockAdd =ID_CHECKBOX28;
	            		ItemStarAdd = Item8DataAddress;
	            		break;
	            	case CREATE_ITEM9:
	            		LockAdd =ID_CHECKBOX29;
	            		ItemStarAdd = Item9DataAddress;
	            		break;
	            	default:
	            		break;
	            	}
	            	if(EEPROMReadByte(ID_CHECKBOXADDRESS +LockAdd))
	            	{
				FLASH_RD(Operatorpage,Operatoraddr,RecordSizeMax,&lockTemp[0],Operatorcs);
					 for(i=0;i<=255;i++)
	                  OperatorNameTemp[i]=(XCHAR)lockTemp[i];
				CopyChar(&OperatorNameTempStr[0],&OperatorNameTemp[ItemStarAdd],19);
				CopyChar(TempbuffStr,OperatorNameTempStr,ItemMax);//拷贝当天信息到TempbuffStr
		       	pEb = (EDITBOX*)GOLFindObject(ID_EDITBOX1);
		             EbSetText(pEb, OperatorNameTempStr);
		             SetState(pEb, EB_DRAW);
	            	}
		}
	}
       
    obj = (OBJ_HEADER*)BtnCreate(ID_ABCPAD+16,             	// q 
              KEYABCSTARTX+0*KEYABCSIZEX,		
              KEYSTARTY+1*KEYSIZEY,
              KEYABCSTARTX+1*KEYABCSIZEX,
              KEYSTARTY+2*KEYSIZEY,    	// dimension
              0,
              BTN_DRAW,                	// will be dislayed after creation
              NULL,                    	// no bitmap
              pTempRUSSIAN[16],    		// text
              altScheme);              	// alternative GOL scheme 

    BtnCreate(ID_ABCPAD+22,             	// w 
              KEYABCSTARTX+1*KEYABCSIZEX,
              KEYSTARTY+1*KEYSIZEY,
              KEYABCSTARTX+2*KEYABCSIZEX,
              KEYSTARTY+2*KEYSIZEY,    	// dimension
              0,
              BTN_DRAW,                	// will be dislayed after creation
              NULL,                    	// no bitmap
              pTempRUSSIAN[22],    		// text
              altScheme);              	// alternative GOL scheme   

    BtnCreate(ID_ABCPAD+4,             	// e
              KEYABCSTARTX+2*KEYABCSIZEX,
              KEYSTARTY+1*KEYSIZEY,
              KEYABCSTARTX+3*KEYABCSIZEX,
              KEYSTARTY+2*KEYSIZEY,    	// dimension
              0,
              BTN_DRAW,                	// will be dislayed after creation
              NULL,                    	// no bitmap
              pTempRUSSIAN[4],    		// text
              altScheme);              	// alternative GOL scheme 

	BtnCreate(ID_ABCPAD+17,             	// r 
              KEYABCSTARTX+3*KEYABCSIZEX,		
              KEYSTARTY+1*KEYSIZEY,
              KEYABCSTARTX+4*KEYABCSIZEX,
              KEYSTARTY+2*KEYSIZEY,    	// dimension
              0,
              BTN_DRAW,                	// will be dislayed after creation
              NULL,                    	// no bitmap
              pTempRUSSIAN[17],    		// text
              altScheme);              	// alternative GOL scheme 

    BtnCreate(ID_ABCPAD+19,             	// t
              KEYABCSTARTX+4*KEYABCSIZEX,
              KEYSTARTY+1*KEYSIZEY,
              KEYABCSTARTX+5*KEYABCSIZEX,
              KEYSTARTY+2*KEYSIZEY,    	// dimension
              0,
              BTN_DRAW,                	// will be dislayed after creation
              NULL,                    	// no bitmap
              pTempRUSSIAN[19],    		// text
              altScheme);              	// alternative GOL scheme   

    BtnCreate(ID_ABCPAD+24,             	// y 
              KEYABCSTARTX+5*KEYABCSIZEX,
              KEYSTARTY+1*KEYSIZEY,
              KEYABCSTARTX+6*KEYABCSIZEX,
              KEYSTARTY+2*KEYSIZEY,    	// dimension
              0,
              BTN_DRAW,                	// will be dislayed after creation
              NULL,                    	// no bitmap
              pTempRUSSIAN[24],    		// text
              altScheme);              	// alternative GOL scheme 


    BtnCreate(ID_ABCPAD+20,             	// u
              KEYABCSTARTX+6*KEYABCSIZEX,
              KEYSTARTY+1*KEYSIZEY,
              KEYABCSTARTX+7*KEYABCSIZEX,
              KEYSTARTY+2*KEYSIZEY,    	// dimension
              0,
              BTN_DRAW,                	// will be dislayed after creation
              NULL,                    	// no bitmap
              pTempRUSSIAN[20],    		// text
              altScheme);              	// alternative GOL scheme 

    BtnCreate(ID_ABCPAD+8,             	// i 
              KEYABCSTARTX+7*KEYABCSIZEX,
              KEYSTARTY+1*KEYSIZEY,
              KEYABCSTARTX+8*KEYABCSIZEX,
              KEYSTARTY+2*KEYSIZEY,    	// dimension
              0,
              BTN_DRAW,                	// will be dislayed after creation
              NULL,                    	// no bitmap
              pTempRUSSIAN[8],    		// text
              altScheme);              	// alternative GOL scheme   

    BtnCreate(ID_ABCPAD+14,             	// o 
              KEYABCSTARTX+8*KEYABCSIZEX,
              KEYSTARTY+1*KEYSIZEY,
              KEYABCSTARTX+9*KEYABCSIZEX,
              KEYSTARTY+2*KEYSIZEY,    	// dimension
              0,
              BTN_DRAW,                	// will be dislayed after creation
              NULL,                    	// no bitmap
              pTempRUSSIAN[14],    		// text
              altScheme);              	// alternative GOL scheme 

    BtnCreate(ID_ABCPAD+15,             	// p 
              KEYABCSTARTX+9*KEYABCSIZEX,
              KEYSTARTY+1*KEYSIZEY,
              KEYABCSTARTX+10*KEYABCSIZEX,
              KEYSTARTY+2*KEYSIZEY,    	// dimension
              0,
              BTN_DRAW,                	// will be dislayed after creation
              NULL,                    	// no bitmap
              pTempRUSSIAN[15],    		// text
              altScheme);              	// alternative GOL scheme 

    BtnCreate(ID_ABCPAD+0,             	// a 
              16,
              KEYSTARTY+2*KEYSIZEY,
              38,
              KEYSTARTY+3*KEYSIZEY,    	// dimension
              0,
              BTN_DRAW,                	// will be dislayed after creation
              NULL,                    	// no bitmap
              pTempRUSSIAN[0],    		// text
              altScheme);              	// alternative GOL scheme   

    BtnCreate(ID_ABCPAD+18,             	//s
              38,
              KEYSTARTY+2*KEYSIZEY,
              61,
              KEYSTARTY+3*KEYSIZEY,    	// dimension
              0,
              BTN_DRAW,                	// will be dislayed after creation
              NULL,                    	// no bitmap
              pTempRUSSIAN[18],    		// text
              altScheme);              	// alternative GOL scheme 
              
    BtnCreate(ID_ABCPAD+3,             	// d
              61,
              KEYSTARTY+2*KEYSIZEY,
              84,
              KEYSTARTY+3*KEYSIZEY,    	// dimension
              0,
              BTN_DRAW,                	// will be dislayed after creation
              NULL,                    	// no bitmap
              pTempRUSSIAN[3],    		// text
              altScheme);              	// alternative GOL scheme 

    BtnCreate(ID_ABCPAD+5,             	// f 
              84,
              KEYSTARTY+2*KEYSIZEY,
              107,
              KEYSTARTY+3*KEYSIZEY,    	// dimension
              0,
              BTN_DRAW,                	// will be dislayed after creation
              NULL,                    	// no bitmap
              pTempRUSSIAN[5],    		// text
              altScheme);              	// alternative GOL scheme   

    BtnCreate(ID_ABCPAD+6,             	// g 
              107,
              KEYSTARTY+2*KEYSIZEY,
              130,
              KEYSTARTY+3*KEYSIZEY,    	// dimension
              0,
              BTN_DRAW,                	// will be dislayed after creation
              NULL,                    	// no bitmap
              pTempRUSSIAN[6],    		// text
              altScheme);              	// alternative GOL scheme 

    BtnCreate(ID_ABCPAD+7,             	// h 
              130,
              KEYSTARTY+2*KEYSIZEY,
              153,
              KEYSTARTY+3*KEYSIZEY,    	// dimension
              0,
              BTN_DRAW,                	// will be dislayed after creation
              NULL,                    	// no bitmap
              pTempRUSSIAN[7],    		// text
              altScheme);              	// alternative GOL scheme 

    BtnCreate(ID_ABCPAD+9,             	// j 
              153,
              KEYSTARTY+2*KEYSIZEY,
              176,
              KEYSTARTY+3*KEYSIZEY,    	// dimension
              0,
              BTN_DRAW,                	// will be dislayed after creation
              NULL,                    	// no bitmap
              pTempRUSSIAN[9],    		// text
              altScheme);              	// alternative GOL scheme   

	BtnCreate(ID_ABCPAD+10,             	// k 
              176,
              KEYSTARTY+2*KEYSIZEY,
              199,
              KEYSTARTY+3*KEYSIZEY,    	// dimension
              0,
              BTN_DRAW,                	// will be dislayed after creation
              NULL,                    	// no bitmap
              pTempRUSSIAN[10],    		// text
              altScheme);              	// alternative GOL scheme 
    BtnCreate(ID_ABCPAD+11,             //L
		      199,
              KEYSTARTY+2*KEYSIZEY,
              222,
              KEYSTARTY+3*KEYSIZEY,
              0,
              BTN_DRAW,                	// will be dislayed after creation
              NULL,                    	// no bitmap
              pTempRUSSIAN[11],    		// text
              altScheme);

/*
    BtnCreate(ID_ABC,             	// ID 
              KEYABCSTARTX+0*KEYABCSIZEX,
              KEYSTARTY+3*KEYSIZEY,
              38,
              KEYSTARTY+4*KEYSIZEY,    	// dimension
              0,
              BTN_DRAW,                	// will be dislayed after creation
              NULL,                    	// no bitmap
              abcStr,    		// text
              altScheme);              	// alternative GOL scheme  
              */
    BtnCreate(ID_ABCPAD+25,             	// z 
              38,
              KEYSTARTY+3*KEYSIZEY,
              61,
              KEYSTARTY+4*KEYSIZEY,    	// dimension
              0,
              BTN_DRAW,                	// will be dislayed after creation
              NULL,                    	// no bitmap
              pTempRUSSIAN[25],    		// text
              altScheme);              	// alternative GOL scheme 

    BtnCreate(ID_ABCPAD+23,             	// x 
              61,
              KEYSTARTY+3*KEYSIZEY,
              84,
              KEYSTARTY+4*KEYSIZEY,    	// dimension
              0,
              BTN_DRAW,                	// will be dislayed after creation
              NULL,                    	// no bitmap
              pTempRUSSIAN[23],    		// text
              altScheme);              	// alternative GOL scheme 

    BtnCreate(ID_ABCPAD+2,             	// c 
              84,
              KEYSTARTY+3*KEYSIZEY,
              107,
              KEYSTARTY+4*KEYSIZEY,    	// dimension
              0,
              BTN_DRAW,                	// will be dislayed after creation
              NULL,                    	// no bitmap
              pTempRUSSIAN[2],    		// text
              altScheme);              	// alternative GOL scheme 

    BtnCreate(ID_ABCPAD+21,             	//v 
              107,
              KEYSTARTY+3*KEYSIZEY,
              130,
              KEYSTARTY+4*KEYSIZEY,    	// dimension
              0,
              BTN_DRAW,                	// will be dislayed after creation
              NULL,                    	// no bitmap
              pTempRUSSIAN[21],    		// text
              altScheme);              	// alternative GOL scheme 


    BtnCreate(ID_ABCPAD+1,             	// b 
              130,
              KEYSTARTY+3*KEYSIZEY,
              153,
              KEYSTARTY+4*KEYSIZEY,    	// dimension
              0,
              BTN_DRAW,                	// will be dislayed after creation
              NULL,                    	// no bitmap
              pTempRUSSIAN[1],    		// text
              altScheme);              	// alternative GOL scheme 

	BtnCreate(ID_ABCPAD+13,             	// n 
              153,
              KEYSTARTY+3*KEYSIZEY,
              176,
              KEYSTARTY+4*KEYSIZEY,    	// dimension
              0,
              BTN_DRAW,                	// will be dislayed after creation
              NULL,                    	// no bitmap
              pTempRUSSIAN[13],    		// text
              altScheme);              	// alternative GOL scheme 


    BtnCreate(ID_ABCPAD+12,             	// m 
              176,
              KEYSTARTY+3*KEYSIZEY,
              199,
              KEYSTARTY+4*KEYSIZEY,    	// dimension
              0,
              BTN_DRAW,                	// will be dislayed after creation
              NULL,                    	// no bitmap
              pTempRUSSIAN[12],    		// text
              altScheme);              	// alternative GOL scheme 

			  

    BtnCreate(ID_KEYPAD+1,             	// ID 
              KEYABCSTARTX+0*KEYABCSIZEX,		
              KEYSTARTY+4*KEYSIZEY,
              KEYABCSTARTX+1*KEYABCSIZEX,
              KEYSTARTY+5*KEYSIZEY,    	// dimension
              0,
              BTN_DRAW,                	// will be dislayed after creation
              NULL,                    	// no bitmap
              pTempRUSSIAN[33],    		// text
              altScheme);              	// alternative GOL scheme 

    BtnCreate(ID_KEYPAD+2,             	// ID 
              KEYABCSTARTX+1*KEYABCSIZEX,
              KEYSTARTY+4*KEYSIZEY,
              KEYABCSTARTX+2*KEYABCSIZEX,
              KEYSTARTY+5*KEYSIZEY,    	// dimension
              0,
              BTN_DRAW,                	// will be dislayed after creation
              NULL,                    	// no bitmap
              pTempRUSSIAN[34],    		// text
              altScheme);              	// alternative GOL scheme   

    BtnCreate(ID_KEYPAD+3,             	// ID 
              KEYABCSTARTX+2*KEYABCSIZEX,
              KEYSTARTY+4*KEYSIZEY,
              KEYABCSTARTX+3*KEYABCSIZEX,
              KEYSTARTY+5*KEYSIZEY,    	// dimension
              0,
              BTN_DRAW,                	// will be dislayed after creation
              NULL,                    	// no bitmap
              pTempRUSSIAN[26],    		// text
              altScheme);              	// alternative GOL scheme 

	BtnCreate(ID_KEYPAD+4,             	// ID 
              KEYABCSTARTX+3*KEYABCSIZEX,		
              KEYSTARTY+4*KEYSIZEY,
              KEYABCSTARTX+4*KEYABCSIZEX,
              KEYSTARTY+5*KEYSIZEY,    	// dimension
              0,
              BTN_DRAW,                	// will be dislayed after creation
              NULL,                    	// no bitmap
              pTempRUSSIAN[27],    		// text
              altScheme);              	// alternative GOL scheme 

    BtnCreate(ID_KEYPAD+5,             	// ID 
              KEYABCSTARTX+4*KEYABCSIZEX,
              KEYSTARTY+4*KEYSIZEY,
              KEYABCSTARTX+5*KEYABCSIZEX,
              KEYSTARTY+5*KEYSIZEY,    	// dimension
              0,
              BTN_DRAW,                	// will be dislayed after creation
              NULL,                    	// no bitmap
              pTempRUSSIAN[28],    		// text
              altScheme);              	// alternative GOL scheme   

    BtnCreate(ID_KEYPAD+6,             	// ID 
              KEYABCSTARTX+5*KEYABCSIZEX,
              KEYSTARTY+4*KEYSIZEY,
              KEYABCSTARTX+6*KEYABCSIZEX,
              KEYSTARTY+5*KEYSIZEY,    	// dimension
              0,
              BTN_DRAW,                	// will be dislayed after creation
              NULL,                    	// no bitmap
              pTempRUSSIAN[29],    		// text
              altScheme);              	// alternative GOL scheme 


    BtnCreate(ID_KEYPAD+7,             	// ID 
              KEYABCSTARTX+6*KEYABCSIZEX,
              KEYSTARTY+4*KEYSIZEY,
              KEYABCSTARTX+7*KEYABCSIZEX,
              KEYSTARTY+5*KEYSIZEY,    	// dimension
              0,
              BTN_DRAW,                	// will be dislayed after creation
              NULL,                    	// no bitmap
              pTempRUSSIAN[30],    		// text
              altScheme);              	// alternative GOL scheme 

    BtnCreate(ID_KEYPAD+8,             	// ID 
              KEYABCSTARTX+7*KEYABCSIZEX,
              KEYSTARTY+4*KEYSIZEY,
              KEYABCSTARTX+8*KEYABCSIZEX,
              KEYSTARTY+5*KEYSIZEY,    	// dimension
              0,
              BTN_DRAW,                	// will be dislayed after creation
              NULL,                    	// no bitmap
              pTempRUSSIAN[31],    		// text
              altScheme);              	// alternative GOL scheme   

    BtnCreate(ID_KEYPAD+9,             	// ID 
              KEYABCSTARTX+8*KEYABCSIZEX,
              KEYSTARTY+4*KEYSIZEY,
              KEYABCSTARTX+9*KEYABCSIZEX,
              KEYSTARTY+5*KEYSIZEY,    	// dimension
              0,
              BTN_DRAW,                	// will be dislayed after creation
              NULL,                    	// no bitmap
              pTempRUSSIAN[32],    		// text
              altScheme);              	// alternative GOL scheme 

    BtnCreate(ID_KEYPAD+0,             	// ID 
              KEYABCSTARTX+9*KEYABCSIZEX,
              KEYSTARTY+4*KEYSIZEY,
              KEYABCSTARTX+10*KEYABCSIZEX,
              KEYSTARTY+5*KEYSIZEY,    	// dimension
              0,
              BTN_DRAW,                	// will be dislayed after creation
              NULL,                    	// no bitmap
              NULL,    		// text
              altScheme);              	// alternative GOL scheme  
    BtnCreate(ID_CHANGE,                	// ID 
              KEYABCSTARTX+0*KEYABCSIZEX,
              KEYSTARTY+5*KEYSIZEY,
              56,
              KEYSTARTY+6*KEYSIZEY,    	// dimension
              0,
              BTN_DRAW,                	// will be dislayed after creation
              NULL,                     // (void*)&greenphone,              // use green phone bitmap
              ChangeRUStr,                     // text
              altScheme);              	// alternative GOL scheme 
    BtnCreate(ID_CALL,                	// ID 
              57,
              KEYSTARTY+5*KEYSIZEY,
              KEYABCSTARTX+8*KEYABCSIZEX-5,
              KEYSTARTY+6*KEYSIZEY,    	// dimension
              0,
              BTN_DRAW,                	// will be dislayed after creation
              NULL,                     // (void*)&greenphone,              // use green phone bitmap
              SPACE_RU,                     // text
              altScheme);              	// alternative GOL scheme 

     BtnCreate(ID_BACKSPACE,            	// <- 
              KEYABCSTARTX+8*KEYABCSIZEX-4,
              KEYSTARTY+5*KEYSIZEY,
              KEYABCSTARTX+10*KEYABCSIZEX,
              KEYSTARTY+6*KEYSIZEY,    	// dimension
              0,
              BTN_DRAW,                	// will be dislayed after creation
              NULL,                    	// no bitmap
              DEL_RU, 			// Left Arrow 
              altScheme);              	// alternative GOL scheme 
              
//GOLSetFocus(obj);
languagereturn=0xee;
languagechange=0xee;
}

// Processes messages for the edit box demo screen
WORD MsgEditABC(WORD objMsg, OBJ_HEADER* pObj, GOL_MSG* pMsg){


}

/*
void CopyMonth(XCHAR FlashMth)
{

switch (FlashMth) {

			default:
				case 0x0001: DisplaydateTimeStr[18] = 0x009F; DisplaydateTimeStr[19] = 0x008D; DisplaydateTimeStr[20] = 0x0082; break; 
				case 0x0002: DisplaydateTimeStr[18] = 0x0094; DisplaydateTimeStr[19] = 0x0085; DisplaydateTimeStr[20] = 0x0082; break; 
				case 0x0003: DisplaydateTimeStr[18] = 0x008C; DisplaydateTimeStr[19] = 0x0080; DisplaydateTimeStr[20] = 0x0090; break; 
				case 0x0004: DisplaydateTimeStr[18] = 0x0080; DisplaydateTimeStr[19] = 0x008F; DisplaydateTimeStr[20] = 0x0090; break; 
				case 0x0005: DisplaydateTimeStr[18] = 0x008C; DisplaydateTimeStr[19] = 0x0080; DisplaydateTimeStr[20] = 0x0089; break; 
				case 0x0006: DisplaydateTimeStr[18] = 0x0088; DisplaydateTimeStr[19] = 0x009E; DisplaydateTimeStr[20] = 0x008D; break; 
				case 0x0007: DisplaydateTimeStr[18] = 0x0088; DisplaydateTimeStr[19] = 0x009E; DisplaydateTimeStr[20] = 0x008B; break; 
				case 0x0008: DisplaydateTimeStr[18] = 0x0080; DisplaydateTimeStr[19] = 0x0082; DisplaydateTimeStr[20] = 0x0083; break; 
				case 0x0009: DisplaydateTimeStr[18] = 0x0091; DisplaydateTimeStr[19] = 0x0085; DisplaydateTimeStr[20] = 0x008D; break; 
				case 0x0010: DisplaydateTimeStr[18] = 0x008E; DisplaydateTimeStr[19] = 0x008A; DisplaydateTimeStr[20] = 0x0092; break; 
				case 0x0011: DisplaydateTimeStr[18] = 0x008D; DisplaydateTimeStr[19] = 0x008E; DisplaydateTimeStr[20] = 0x009F; break; 
				case 0x0012: DisplaydateTimeStr[18] = 0x0082; DisplaydateTimeStr[19] = 0x0085; DisplaydateTimeStr[20] = 0x008A; break; 

			}
		

}
	*/
void CopyTime(void)
{
	DisplaydateTimeStr[0] = 0x0020;//Flashbuff[];
	DisplaydateTimeStr[1] = 0x0020;//Flashbuff[];
	DisplaydateTimeStr[2] = 0x0020;//Flashbuff[];// 星期
	DisplaydateTimeStr[4] = ((Flashbuff[6]>>4)&0x000F)|0x0030;
	DisplaydateTimeStr[5] = (Flashbuff[6]&0x000F)|0x0030;// 时
	DisplaydateTimeStr[7] = (Flashbuff[7]>>4&0x000F)|0x0030;
	DisplaydateTimeStr[8] = (Flashbuff[7]&0x000F)|0x0030;// 分
	DisplaydateTimeStr[10] = ((Flashbuff[8]>>4)&0x000F)|0x0030;
	DisplaydateTimeStr[11] = (Flashbuff[8]&0x000F)|0x0030;// 秒
	//CopyMonth(Flashbuff[4]);
	DisplaydateTimeStr[13] = (Flashbuff[5]>>4&0x000F)|0x0030;
	DisplaydateTimeStr[14] = (Flashbuff[5]&0x000F)|0x0030;// 日
	DisplaydateTimeStr[18] =  (Flashbuff[4]>>4&0X000F)|0x0030;
	DisplaydateTimeStr[19] =  (Flashbuff[4]&0x000F)|0x0030;	
	DisplaydateTimeStr[25] = (Flashbuff[3]>>4&0x000F)|0x0030;
	DisplaydateTimeStr[26] = (Flashbuff[3]&0x000F)|0x0030;// 年

}
void DisplayTestResult(void)
{
	XCHAR temp,temp1;
	temp = Flashbuff[10];
	temp1 = Flashbuff[11];
	switch(Flashbuff[9])
	{
		case 0:
			Test_Result[0]= temp>>4&0x000F|0X0030;
			Test_Result[1]= temp&0x000F|0X0030;
			Test_Result[2]= temp1>>4&0x000F|0X0030;
			Test_Result[3]= 0x002E;
			Test_Result[4]= temp1&0x000F|0X0030;
		break;
		case 1:
			Test_Result[0]= temp>>4&0x000F|0X0030;
			Test_Result[1]= 0x002E;
			Test_Result[2]= temp&0x000F|0X0030;
			Test_Result[3]= temp1>>4&0x000F|0X0030;
			Test_Result[4]= temp1&0x000F|0X0030;
		break;
		case 2:
			Test_Result[0]= temp>>4&0x000F|0X0030;
			Test_Result[1]= 0x002E;
			Test_Result[2]= temp&0x000F|0X0030;
			Test_Result[3]= temp1>>4&0x000F|0X0030;
			Test_Result[4]= temp1&0x000F|0X0030;
			//Flashbuff[15]= 0;
		break;
		case 3:
			Test_Result[0]= temp>>4&0x000F|0X0030;
			Test_Result[1]= 0x002E;
			Test_Result[2]= temp&0x000F|0X0030;
			Test_Result[3]= temp1>>4&0x000F|0X0030;
			Test_Result[4]= temp1&0x000F|0X0030;
			//Flashbuff[15]= 0;
		break;
		case 4:
			Test_Result[0]= temp>>4&0x000F|0X0030;
			Test_Result[1]= 0x002E;
			Test_Result[2]= temp&0x000F|0X0030;
			Test_Result[3]= temp1>>4&0x000F|0X0030;
			Test_Result[4]= temp1&0x000F|0X0030;
			//Flashbuff[15]= 0;
		break;
		case 5:
			Test_Result[0]= temp>>4&0x000F|0X0030;
			Test_Result[1]= temp&0x000F|0X0030;
			Test_Result[2]= temp1>>4&0x000F|0X0030;
			Test_Result[3]= 0x002E;
			Test_Result[4]= temp1&0x000F|0X0030;
			//Flashbuff[15]= 0;
		break;
		case 6:
			Test_Result[0]= temp>>4&0x000F|0X0030;
			Test_Result[1]= 0x002E;
			Test_Result[2]= temp&0x000F|0X0030;
			Test_Result[3]= temp1>>4&0x000F|0X0030;
			Test_Result[4]= temp1&0x000F|0X0030;
			//Flashbuff[15]= 0;
		break;
		case 7:
			Test_Result[0]= temp>>4&0x000F|0X0030;
			Test_Result[1]= 0x002E;
			Test_Result[2]= temp&0x000F|0X0030;
			Test_Result[3]= temp1>>4&0x000F|0X0030;
			Test_Result[4]= temp1&0x000F|0X0030;
			//Flashbuff[15]= 0;
		break;
		case 8:
			Test_Result[0]= temp>>4&0x000F|0X0030;
			Test_Result[1]= 0x002E;
			Test_Result[2]= temp&0x000F|0X0030;
			Test_Result[3]= temp1>>4&0x000F|0X0030;
			Test_Result[4]= temp1&0x000F|0X0030;
			//Flashbuff[15]= 0;
		break;
		case 9:
			Test_Result[0]= temp>>4&0x000F|0X0030;
			Test_Result[1]= 0x002E;
			Test_Result[2]= temp&0x000F|0X0030;
			Test_Result[3]= temp1>>4&0x000F|0X0030;
		 	Test_Result[4]= temp1&0x000F|0X0030;
			//Flashbuff[15]= 0;
		break;
		default:
			break;

	}

}

void CreateRecord()
{
	char temp,temp1;
	unsigned int i;
	for(i=0;i<256;i++)
	Flashbuff[i]=0;
	readrecord();
	SaveReadItem(FALSE);
	//initPrint();
		serialnumberStr[0] = (serialnumberRecord /10000) +'0';// 万位
		serialnumberStr[1] = (serialnumberRecord /1000 %10) +'0';// 千位
		serialnumberStr[2] = (serialnumberRecord /100 %10) +'0';// 百位
		serialnumberStr[3] = (serialnumberRecord /10 %100%10) +'0';// 十位
		serialnumberStr[4] = (serialnumberRecord %10) +'0';// 个位
		ST_IRstatus = Flashbuff[16];
		temp = Flashbuff[10];
		temp1 = Flashbuff[11];
		DisplayTestResult();//显示测试结果
		CopyTime();
	GOLFree();   // free memory for the objects in the previous linked list and start new list
#ifdef NO_BLUETOOTH_BUTTON
    CreatePage(no_bluetooth_button_RecordStr);
#else
	CreatePage(RecordStr);	// CreatePage("Buttons");
#endif
    mainScheme->pFont = (void*)&GOLMediumFont;	

    StCreate(ID_STATICTEXT3,           		// ID 
               positionax,positionay+0*positionmaxy,
              positionax+3*positionmaxx,positionay+1*positionmaxy,      		// dimension
              ST_DRAW,//|ST_RIGHT_ALIGN,//ST_CENTER_ALIGN,		// display text
              NumberStr, 				// "TEST", 	// text
              mainScheme);                   // use alternate scheme
    StCreate(ID_STATICTEXT3,           		// ID 
              positionax+3*positionmaxx,positionay+0*positionmaxy,
              7*(positionax+positionmaxx),positionay+1*positionmaxy,            		// dimension
              ST_DRAW,//|ST_RIGHT_ALIGN,//ST_CENTER_ALIGN,		// display text
              serialnumberStr,//NULL,//GetdataStr, 				// "TEST", 	// text
              mainScheme);        //   alt2Scheme        // use alternate scheme
              
  	StCreate(ID_STATICTEXT3,           		// ID 
              positionax,positionay+1*positionmaxy,
              positionax+3*positionmaxx,positionay+2*positionmaxy,         		// dimension
              ST_DRAW,//|ST_RIGHT_ALIGN,//ST_CENTER_ALIGN,		// display text
              DateStr, 				// "TEST", 	// text
              mainScheme);                   // use alternate scheme	
    StCreate(ID_STATICTEXT3,           		// ID 
              positionax+3*positionmaxx,positionay+1*positionmaxy,
              7*(positionax+positionmaxx),positionay+2*positionmaxy,         		// dimension
              ST_DRAW,//|ST_RIGHT_ALIGN,//ST_CENTER_ALIGN,		// display text
              &DisplaydateTimeStr[13],//&Flashbuff[86],//GetdataStr, 				// "TEST", 	// text
              mainScheme);    //    alt2Scheme           // use alternate scheme          
              
	StCreate(ID_STATICTEXT3,           		// ID 
              positionax,positionay+2*positionmaxy,
              positionax+3*positionmaxx,positionay+3*positionmaxy,         		// dimension
              ST_DRAW,//|ST_RIGHT_ALIGN,//ST_CENTER_ALIGN,		// display text
              TimeStr, 				// "TEST", 	// text
              mainScheme);                   // use alternate scheme	
    StCreate(ID_STATICTEXT3,           		// ID 
              positionax+3*positionmaxx,positionay+2*positionmaxy,
              7*(positionax+positionmaxx)-10,positionay+3*positionmaxy,         		// dimension
              ST_DRAW,//|ST_RIGHT_ALIGN,//ST_CENTER_ALIGN,		// display text
              DisplaydateTimeStr,//&Flashbuff[86],//GetdataStr, 				// "TEST", 	// text
              mainScheme);     // alt2Scheme             // use alternate scheme
                            
	StCreate(ID_STATICTEXT3,           		// ID 
              positionax,positionay+3*positionmaxy,
              positionax+4*positionmaxx,positionay+4*positionmaxy,         		// dimension
              ST_DRAW,//|ST_RIGHT_ALIGN,//ST_CENTER_ALIGN,		// display text
              TestModestr, 				// "TEST", 	// text
              mainScheme);                   // use alternate scheme	
if(ST_IRstatus&(ST_REFUSE))
	
StCreate(ID_STATICTEXT3,           		// ID 
              positionax+3*positionmaxx,positionay+3*positionmaxy,
              7*(positionax+positionmaxx),positionay+4*positionmaxy,         		// dimension
              ST_DRAW,//|ST_RIGHT_ALIGN,//ST_CENTER_ALIGN,		// display text
              record_refuse,//&GetdataStr[96],//GetdataStr, 				// "TEST", 	// text
              mainScheme);    //   alt2Scheme            // use alternate scheme
  else if(ST_IRstatus&ST_Passive)
  	StCreate(ID_STATICTEXT3,           		// ID 
              positionax+3*positionmaxx,positionay+3*positionmaxy,
              7*(positionax+positionmaxx),positionay+4*positionmaxy,         		// dimension
              ST_DRAW,//|ST_RIGHT_ALIGN,//ST_CENTER_ALIGN,		// display text
              PassivityStr,//&GetdataStr[96],//GetdataStr, 				// "TEST", 	// text
              mainScheme);

  else if(ST_IRstatus&ST_Discontinued)
  	StCreate(ID_STATICTEXT3,           		// ID 
              positionax+3*positionmaxx,positionay+3*positionmaxy,
              7*(positionax+positionmaxx),positionay+4*positionmaxy,         		// dimension
              ST_DRAW,//|ST_RIGHT_ALIGN,//ST_CENTER_ALIGN,		// display text
              record_refuse,//&GetdataStr[96],//GetdataStr, 				// "TEST", 	// text
              mainScheme);

  else //if(ST_IRstatus&0x0002)
  	StCreate(ID_STATICTEXT3,           		// ID 
              positionax+3*positionmaxx,positionay+3*positionmaxy,
              7*(positionax+positionmaxx),positionay+4*positionmaxy,         		// dimension
              ST_DRAW,//|ST_RIGHT_ALIGN,//ST_CENTER_ALIGN,		// display text
              ManualStr,//&GetdataStr[96],//GetdataStr, 				// "TEST", 	// text
              mainScheme);
              

	StCreate(ID_STATICTEXT3,           		// ID 酒精含量
              positionax,positionay+4*positionmaxy,
              positionax+3*positionmaxx+20,positionay+5*positionmaxy,         		// dimension
              ST_DRAW,//|ST_RIGHT_ALIGN,//ST_CENTER_ALIGN,		// display text
              TestResultStr, 				// "TEST", 	// text
              mainScheme);                   // use alternate scheme

			  
	if(ST_IRstatus&(ST_REFUSE))
	{
		StCreate(ID_STATICTEXT3,           		// ID 
              positionax+3*positionmaxx,positionay+4*positionmaxy,
              7*(positionax+positionmaxx),positionay+5*positionmaxy,         		// dimension
              ST_DRAW,//|ST_RIGHT_ALIGN,//ST_CENTER_ALIGN,		// display text
		      RefuseStr,//GetdataStr,// SubjectNameStr, 				// "TEST", 	// text
              mainScheme);           
	}

    else if(ST_IRstatus&ST_Discontinued)	  
	{
		StCreate(ID_STATICTEXT3,           		// ID 
              positionax+3*positionmaxx,positionay+4*positionmaxy,
              7*(positionax+positionmaxx),positionay+5*positionmaxy,         		// dimension
              ST_DRAW,//|ST_RIGHT_ALIGN,//ST_CENTER_ALIGN,		// display text
		      Discontinuedstr,//GetdataStr,// SubjectNameStr, 				// "TEST", 	// text
              mainScheme);          
	}		
	else
	{
if(Flashbuff[15] == 0xaa)
	
		StCreate(ID_STATICTEXT3,           		// ID 
              positionax+3*positionmaxx+20,positionay+4*positionmaxy,
              positionax+5*positionmaxx,positionay+5*positionmaxy,         		// dimension
              ST_DRAW|ST_RIGHT_ALIGN,		// display text
		      Beyond25mgl,//GetdataStr,// SubjectNameStr, 				// "TEST", 	// text
              mainScheme);

else
	
	StCreate(ID_STATICTEXT3,           		// ID 
              positionax+3*positionmaxx+20,positionay+4*positionmaxy,
              positionax+5*positionmaxx,positionay+5*positionmaxy,         		// dimension
              ST_DRAW|ST_RIGHT_ALIGN,		// display text
		      Test_Result,//GetdataStr,// SubjectNameStr, 				// "TEST", 	// text
              mainScheme);
//if(Flashbuff[9])
	DisplayThickness(DisplayUnitStr);
	StCreate(ID_STATICTEXT3,           		// ID 
              positionax+5*positionmaxx+10,positionay+4*positionmaxy,
              7*(positionax+positionmaxx),positionay+5*positionmaxy,         		// dimension
              ST_DRAW,//|ST_RIGHT_ALIGN,//ST_CENTER_ALIGN,		// display text
		 DisplayUnitStr,//GetdataStr,// SubjectNameStr, 				// "TEST", 	// text
              mainScheme);
	}

              

	if(EEPROMReadByte(ID_CHECKBOXADDRESS)==1)
	{
		StCreate(ID_STATICTEXT3,           		// ID 
              positionax,positionay+5*positionmaxy,
              positionax+3*positionmaxx+35,positionay+6*positionmaxy,         		// dimension
              ST_DRAW,//|ST_RIGHT_ALIGN,//ST_CENTER_ALIGN,		// display text
              Item0Str, 				// "TEST", 	// text
              mainScheme);                   // use alternate scheme
    StCreate(ID_STATICTEXT3,           		// ID 
              positionax+3*positionmaxx+35,positionay+5*positionmaxy,
              239,positionay+6*positionmaxy,         		// dimension
              ST_DRAW,//|ST_RIGHT_ALIGN,//ST_CENTER_ALIGN,		// display text
		 &Flashbuff[49],//NULL,//GetdataStr,// SubjectNameStr, 				// "TEST", 	// text
              mainScheme);  //  alt2Scheme               // use alternate scheme
		}

if(EEPROMReadByte(ID_CHECKBOXADDRESS+1)==1)
	{
	StCreate(ID_STATICTEXT3,           		// ID 
              positionax,positionay+6*positionmaxy,
              positionax+3*positionmaxx+40,positionay+7*positionmaxy,         		// dimension
              ST_DRAW,//|ST_RIGHT_ALIGN,//ST_CENTER_ALIGN,		// display text
              Item1Str, 				// "TEST", 	// text
              mainScheme);                   // use alternate scheme
StCreate(ID_STATICTEXT3,           		// ID 
              positionax+3*positionmaxx+40,positionay+6*positionmaxy,
              239,positionay+7*positionmaxy,         		// dimension
              ST_DRAW,//|ST_RIGHT_ALIGN,//ST_CENTER_ALIGN,		// display text
              &Flashbuff[68],//NULL,//&GetdataStr[16],//WriteRecordStr, 				// "TEST", 	// text
              mainScheme);    //  alt2Scheme             // use alternate scheme
		}

	if(EEPROMReadByte(ID_CHECKBOXADDRESS+2)==1)
	{
	StCreate(ID_STATICTEXT3,           		// ID 
              positionax,positionay+7*positionmaxy,
              positionax+3*positionmaxx+40,positionay+8*positionmaxy,         		// dimension
              ST_DRAW,//|ST_RIGHT_ALIGN,//ST_CENTER_ALIGN,		// display text
              Item2Str, 				// "TEST", 	// text
              mainScheme);                   // use alternate scheme	
StCreate(ID_STATICTEXT3,           		// ID 
              positionax+3*positionmaxx+40,positionay+7*positionmaxy,
              239,positionay+8*positionmaxy,         		// dimension
              ST_DRAW,//|ST_RIGHT_ALIGN,//ST_CENTER_ALIGN,		// display text
              &Flashbuff[87],//NULL,//&GetdataStr[32],//&WriteRecordStr[13],//GetdataStr, 				// "TEST", 	// text
              mainScheme);  //  alt2Scheme               // use alternate scheme
		}
	
	if(EEPROMReadByte(ID_CHECKBOXADDRESS+3)==1)
	{
	StCreate(ID_STATICTEXT3,           		// ID 
              positionax,positionay+8*positionmaxy,
              positionax+3*positionmaxx+40,positionay+9*positionmaxy,         		// dimension
              ST_DRAW,//|ST_RIGHT_ALIGN,//ST_CENTER_ALIGN,		// display text
              Item3Str, 				// "TEST", 	// text
              mainScheme);                   // use alternate scheme	
StCreate(ID_STATICTEXT3,           		// ID 
              positionax+3*positionmaxx+40,positionay+8*positionmaxy,
              239,positionay+9*positionmaxy,         		// dimension
              ST_DRAW,//|ST_RIGHT_ALIGN,//ST_CENTER_ALIGN,		// display text
              &Flashbuff[106],//NULL,//&GetdataStr[48],//GetdataStr, 				// "TEST", 	// text
              mainScheme);   //  alt2Scheme              // use alternate scheme
		}
	if(EEPROMReadByte(ID_CHECKBOXADDRESS+4)==1)
		{
	StCreate(ID_STATICTEXT3,           		// ID 
              positionax,positionay+9*positionmaxy,
              positionax+3*positionmaxx+40,positionay+10*positionmaxy,         		// dimension
              ST_DRAW,//|ST_RIGHT_ALIGN,//ST_CENTER_ALIGN,		// display text
              Item4Str, 				// "TEST", 	// text
              mainScheme);                   // use alternate scheme	
StCreate(ID_STATICTEXT3,           		// ID 
              positionax+3*positionmaxx+40,positionay+9*positionmaxy,
              239,positionay+10*positionmaxy,         		// dimension
              ST_DRAW,//|ST_RIGHT_ALIGN,//ST_CENTER_ALIGN,		// display text
              &Flashbuff[125],//,//NULL,//&GetdataStr[64],//GetdataStr, 				// "TEST", 	// text
              mainScheme);  //   alt2Scheme              // use alternate scheme
		}
	if(EEPROMReadByte(ID_CHECKBOXADDRESS+5)==1)
	{
	StCreate(ID_STATICTEXT3,           		// ID 
              positionax,positionay+10*positionmaxy,
              positionax+3*positionmaxx+40,positionay+11*positionmaxy,         		// dimension
              ST_DRAW,//|ST_RIGHT_ALIGN,//ST_CENTER_ALIGN,		// display text
              Item5Str, 				// "TEST", 	// text
              mainScheme);                   // use alternate scheme	
StCreate(ID_STATICTEXT3,           		// ID 
              positionax+3*positionmaxx+40,positionay+10*positionmaxy,
              239,positionay+11*positionmaxy,         		// dimension
              ST_DRAW,//|ST_RIGHT_ALIGN,//ST_CENTER_ALIGN,		// display text
              &Flashbuff[144],//NULL,//&GetdataStr[80],//GetdataStr, 				// "TEST", 	// text
              mainScheme);
		}
   if(EEPROMReadByte(ID_CHECKBOXADDRESS+6)==1)
	{
	StCreate(ID_STATICTEXT3,           		// ID 
              positionax,positionay+11*positionmaxy,
              positionax+3*positionmaxx+40,positionay+12*positionmaxy,         		// dimension
              ST_DRAW,//|ST_RIGHT_ALIGN,//ST_CENTER_ALIGN,		// display text
              Item6Str, 				// "TEST", 	// text
              mainScheme);                   // use alternate scheme	
StCreate(ID_STATICTEXT3,           		// ID 
              positionax+3*positionmaxx+40,positionay+11*positionmaxy,
              239,positionay+12*positionmaxy,         		// dimension
              ST_DRAW,//|ST_RIGHT_ALIGN,//ST_CENTER_ALIGN,		// display text
              &Flashbuff[163],//NULL,//&GetdataStr[80],//GetdataStr, 				// "TEST", 	// text
              mainScheme);
		}
     if(EEPROMReadByte(ID_CHECKBOXADDRESS+7)==1)
	{
	StCreate(ID_STATICTEXT3,           		// ID 
              positionax,positionay+12*positionmaxy,
              positionax+3*positionmaxx+40,positionay+13*positionmaxy,         		// dimension
              ST_DRAW,//|ST_RIGHT_ALIGN,//ST_CENTER_ALIGN,		// display text
              Item7Str, 				// "TEST", 	// text
              mainScheme);                   // use alternate scheme	
StCreate(ID_STATICTEXT3,           		// ID 
              positionax+3*positionmaxx+40,positionay+12*positionmaxy,
              239,positionay+13*positionmaxy,         		// dimension
              ST_DRAW,//|ST_RIGHT_ALIGN,//ST_CENTER_ALIGN,		// display text
              &Flashbuff[182],//NULL,//&GetdataStr[80],//GetdataStr, 				// "TEST", 	// text
              mainScheme);
		}
     if(EEPROMReadByte(ID_CHECKBOXADDRESS+8)==1)
	{
	StCreate(ID_STATICTEXT3,           		// ID 
              positionax,positionay+13*positionmaxy,
              positionax+3*positionmaxx+40,positionay+14*positionmaxy,         		// dimension
              ST_DRAW,//|ST_RIGHT_ALIGN,//ST_CENTER_ALIGN,		// display text
              Item8Str, 				// "TEST", 	// text
              mainScheme);                   // use alternate scheme	
StCreate(ID_STATICTEXT3,           		// ID 
              positionax+3*positionmaxx+40,positionay+13*positionmaxy,
              239,positionay+14*positionmaxy,         		// dimension
              ST_DRAW,//|ST_RIGHT_ALIGN,//ST_CENTER_ALIGN,		// display text
              &Flashbuff[201],//NULL,//&GetdataStr[80],//GetdataStr, 				// "TEST", 	// text
              mainScheme);
		}	


	BtnCreate(ID_BUTTONChina_M,//ID_KEYCHINAPAD+1,             	// ID 
              KEYABCSTARTX+0*KEYABCSIZEX,		
              KEYSTARTY+5*KEYSIZEY+40,
              KEYABCSTARTX+2*KEYABCSIZEX,
              KEYSTARTY+6*KEYSIZEY+40,    	// dimension
              0,
              BTN_DRAW,                	// will be dislayed after creation
              (void*)&LEFT,                    	// no bitmap
              NULL,//pTempXchina[1],    		// text
              blackScheme);
	BtnCreate(ID_BUTTONChina_P,//ID_KEYCHINAPAD+0,             	// ID 
              KEYABCSTARTX+8*KEYABCSIZEX,
              KEYSTARTY+5*KEYSIZEY+40,
              KEYABCSTARTX+10*KEYABCSIZEX,
              KEYSTARTY+6*KEYSIZEY+40,    	// dimension
              0,
              BTN_DRAW,                	// will be dislayed after creation
              (void*)&RIGHT,                    	// no bitmap
              NULL,//pTempXchina[0],    		// text
              blackScheme);              	// alternative GOL scheme 
              
	pbutton=BtnCreate(ID_BUTTON8,             	// button ID 
              123, KEYSTARTY+5*KEYSIZEY+40,
              169,KEYSTARTY+6*KEYSIZEY+40,        	// dimension
              0,					   	// set radius 
              BTN_DRAW,//|BTN_PRESSED,  	// draw a vertical capsule button
              							// that is initially pressed
              (void*)&printerbutton,//(void*)&PicGPS,                    	// no bitmap
              NULL, //NULL,//OffStr, 					// "OFF",      	// text
              blackScheme);   //redScheme         	// use alternate scheme 	

	ST_IRstatus = 0;// 恢复正常测试模式

}

WORD MsgRecord(WORD objMsg, OBJ_HEADER* pObj, GOL_MSG* pMsg){

EDITBOX* pEb;
SHORT    id,i;
XCHAR    temp;
static char status = 0;			// status to check if calling, holding or not

    id = GetObjID(pObj);


    switch(id){
        case ID_BUTTON_NEXT:
            if(objMsg == BTN_MSG_RELEASED){
				
				ChoicePrint = FALSE;

				SPrint=FALSE;

				Sprinnum = 0;
				
			    //PrintSec=0;
				
			   // PrintCount = 0;
				
               	screenState = CREATE_MAIN ;//++;prevState//= ; 	// goto meter screen
            }
            return 1; // process by default

        case ID_BUTTON_BACK:
            if(objMsg == BTN_MSG_RELEASED){

				ChoicePrint = FALSE;

				SPrint=FALSE;

				Sprinnum = 0;
				
			   // PrintSec=0;
				
			   // PrintCount = 0;
				
                screenState = CREATE_MAIN; 	// goto list box screen
            }
            return 1; // process by default
       case ID_BUTTON_HOME:
            if(objMsg == BTN_MSG_RELEASED){

				ChoicePrint = FALSE;

				SPrint=FALSE;

				Sprinnum = 0;
				
			   // PrintSec=0;
				
			   // PrintCount = 0;
				
                screenState = CREATE_MAIN; 	// goto list box screen
            }
            return 1;
        case ID_BUTTON8:
            if(objMsg == BTN_MSG_RELEASED){


          if(PORTEbits.RE8)
           	{
				SPrint=TRUE;
            	ChoicePrint = TRUE;
				Sprinnum = 0;
				PrintSec=0;
				PrintCount = 0;
	       		memset(&Flashbuff[0],0,256);
				//memset(&tx1_buff[0],0,250);
				//memset(&rx1_buff[0],0,250);
				readrecord();
	 			for(i=0;i<=255;i++)
					Printbuff[i]=(unsigned char)Flashbuff[i];
    			Inputdata();
				p_n = 0;
           	}
		  /*
		  else 
		  	{

              //Beep();
			  SetColor(YELLOW);
   			  SetFont((void*)&BigArial);
			  BtnSetText(pbutton, Sighstr);
			  SetState(pbutton, BTN_DISABLED);



		  }
		  */
			// _INT2IE = 0;
	/*
	           readrecord();
	           ChoicePrint = TRUE;
	           initPrint();
               Print_TestRecord();
               IPC6bits.T4IP = 4;	
               TickInit();
		       initI2();
               ChoicePrint = FALSE;
               IEC1bits.T4IE = 1; 
               */

            }
            return 1; // process by default
        case ID_BUTTONChina_P:
            if(objMsg == BTN_MSG_RELEASED){
			if(Record_Over_Flag != Code_OVER)
				{
                if(serialnumberRecord<serialnumber-1)
            	{
            		serialnumberRecord ++;//;= serialnumber
            		screenState = CREATE_RECORD;//
            	}
            	else
            	{
            		serialnumberRecord = serialnumber-1;
            		screenState = CREATE_RECORD; 	// goto list box screen
            	}
				}
			else{
				if(serialnumberRecord<30000)
            	{
            		serialnumberRecord ++;//;= serialnumber
            		screenState = CREATE_RECORD;//
            	}
            	else if(serialnumberRecord == 30000)
            	{
            		serialnumberRecord = 1;
            		screenState = CREATE_RECORD; 	// goto list box screen
            	}	
			}
            }
            return 1; // process by default
            case ID_BUTTONChina_M:
            if(objMsg == BTN_MSG_RELEASED){
                if(serialnumberRecord>1)
            	{
            		serialnumberRecord --;//;= serialnumber
            		screenState = CREATE_RECORD;//
            	}
            	else
            	{
            	if(Record_Over_Flag != Code_OVER)
            		{
            		serialnumberRecord = 1;
            		screenState = CREATE_RECORD; 	//
            		}
				else
					{
            		serialnumberRecord = 30000;
            		screenState = CREATE_RECORD; 	//				}
            	    }
            	}
            }
            return 1; // process by default
            
        default:
            return 1; // process by default
    }


}


void CreateJumptox(void)
{


  SHORT i;

  OBJ_HEADER* obj;
  
  WORD textWidth, textHeight, tempStrWidth;
                                               
  XCHAR tempStr[] = {'M',0};
  
  tempStrWidth = GetTextWidth(tempStr,altScheme->pFont);
  textHeight   = GetTextHeight(altScheme->pFont) + (GOL_EMBOSS_SIZE<<1);
  
	GOLFree();   // free memory for the objects in the previous linked list and start new list

   WndCreate(ID_WINDOW1,       		// ID
              0,0,GetMaxX(),GetMaxY(), 	// dimension
              WND_DRAW,                	// will be dislayed after creation
              NULL,               	// icon
              Jumpstr,	   				// set text 
              navScheme);               // default GOL scheme 
   BtnCreate(ID_BUTTON_HOME,
              2,GetMaxY()-40,					   	// left, top corner	
              42,GetMaxY(),0, 		   	// right, bottom corner (with radius = 0)
              BTN_DRAW,               	// will be dislayed after creation
              (void*)&HOME,					   	// no bitmap	
              NULL,//, HomeStr			// LEFT arrow as text
              blackScheme);
  
	RTCCProcessEvents();				// update the date and time strings
	i = 0;						
	while (i < 12) {
		dateTimeStr[i] 	  = (XCHAR)_time_str[i];
		dateTimeStr[i+13] = (XCHAR)_date_str[i];
		i++;
	}
	dateTimeStr[12] = (XCHAR)'\n';
	dateTimeStr[25] = (XCHAR)'\0';
    serialnumber = EEPROMReadWord(EESERIAL_NUMBER);
    serialnumberRecord = serialnumber - 1;// 保持记录一致
if((Record_Over_Flag == 0xaaaa)&&(serialnumber == 1))
		serialnumberRecord = 1;    
 
    serialnumberStr[0] = (serialnumberRecord /10000) +'0';// 万位
    serialnumberStr[1] = (serialnumberRecord /1000 %10) +'0';// 千位
    serialnumberStr[2] = (serialnumberRecord /100 %10) +'0';// 百位
    serialnumberStr[3] = (serialnumberRecord /10 %100%10) +'0';// 十位
    serialnumberStr[4] = (serialnumberRecord %10) +'0';// 个位
    
    StCreate(ID_STATICTEXT3,           		// ID 
              2,60,
              60,95,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,//|ST_RIGHT_ALIGN,//ST_CENTER_ALIGN,		// display text
              Totalstr, 				// "TEST", 	// text
              alt5Scheme);                   // use alternate scheme

	StCreate(ID_STATICTEXT3,           		// ID 
              78,60,
              140,95,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,//|ST_RIGHT_ALIGN,//ST_CENTER_ALIGN,		// display text
              serialnumberStr, 				// "TEST", 	// text
              alt5Scheme);                   // use alternate scheme	

    BtnCreate(ID_BUTTON1,         	// button ID 
              10,
              105,
              90,
              140,  		   	
              0,					   	// draw rectangular button	
              BTN_DRAW,               	// will be dislayed after creation
              NULL,					   	// no bitmap	
              Beginningstr,              	// plus symbol as text
              altScheme);               // use alternate scheme			

	BtnCreate(ID_BUTTON2,         	// button ID 
              130,
              105,
              200,
              140,  		   	
              0,					   	// draw rectangular button	
              BTN_DRAW,               	// will be dislayed after creation
              NULL,					   	// no bitmap	
              Endstr,              	// plus symbol as text
              altScheme);               // use alternate scheme			  

	textWidth  = (tempStrWidth*2) + (GOL_EMBOSS_SIZE<<1);

	EbCreate(ID_EB_RECORD1,              	// ID
              EB4STARTX,
              EBSTARTY2+20,
              EB4STARTX+textWidth, 
              EBSTARTY2+textHeight+20,
              EB_DRAW, 					// will be dislayed after creation
              NULL,
              1,						// max characters is 2
              altScheme);               // use alternate scheme

    BtnCreate(ID_BUTTON1_P,         	// button ID 
              EB4STARTX,
              EBSTARTY2-textHeight-1+20,
              EB4STARTX+textWidth,
              EBSTARTY2-1+20,  		   	
              0,					   	// draw rectangular button	
              BTN_DRAW,               	// will be dislayed after creation
              NULL,					   	// no bitmap	
              DTPlusSym,              	// plus symbol as text
              altScheme);               // use alternate scheme

    BtnCreate(ID_BUTTON1_M,         	// button ID 
              EB4STARTX,
              EBSTARTY2+textHeight+1+20,
              EB4STARTX+textWidth,
              EBSTARTY2+2*textHeight+1+20,  		   	
              0,					   	// draw rectangular button	
              BTN_DRAW,      			// will be dislayed after creation
              NULL,					   	// no bitmap	
              DTMinusSym,               // minus symbols as text
              altScheme);               // use alternate scheme



	EbCreate(ID_EB_RECORD2,              // ID
              EB5STARTX,
              EBSTARTY2+20,
              EB5STARTX+textWidth, 
              EBSTARTY2+textHeight+20,
              EB_DRAW, 					// will be dislayed after creation
              NULL,
              2,						// max characters is 2
              altScheme);               // use alternate scheme

    BtnCreate(ID_BUTTON2_P,         	// button ID 
              EB5STARTX,
              EBSTARTY2-textHeight-1+20,
              EB5STARTX+textWidth,
              EBSTARTY2-1+20,  		   	
              0,					   	// draw rectangular button	
              BTN_DRAW,               	// will be dislayed after creation
              NULL,					   	// no bitmap	
              DTPlusSym,              	// plus symbol as text
              altScheme);               // use alternate scheme

    BtnCreate(ID_BUTTON2_M,         	// button ID 
              EB5STARTX,
              EBSTARTY2+textHeight+1+20,
              EB5STARTX+textWidth,
              EBSTARTY2+2*textHeight+1+20,  		   	
              0,					   	// draw rectangular button	
              BTN_DRAW,               	// will be dislayed after creation
              NULL,					   	// no bitmap	
              DTMinusSym,               // minus symbols as text
              altScheme);               // use alternate scheme



	EbCreate(ID_EB_RECORD3,              // ID
              EB6STARTX,
              EBSTARTY2+20,
              EB6STARTX+textWidth, 
              EBSTARTY2+textHeight+20,
              EB_DRAW, 					// will be dislayed after creation
              NULL,
              2,						// max characters is 2
              altScheme);               // use alternate scheme

    BtnCreate(ID_BUTTON3_P,         	// button ID 
              EB6STARTX,
              EBSTARTY2-textHeight-1+20,
              EB6STARTX+textWidth,
              EBSTARTY2-1+20,  		   	
              0,					   	// draw rectangular button	
              BTN_DRAW,               	// will be dislayed after creation
              NULL,					   	// no bitmap	
              DTPlusSym,              	// plus symbol as text
              altScheme);               // use alternate scheme

    BtnCreate(ID_BUTTON3_M,         	// button ID 
              EB6STARTX,
              EBSTARTY2+textHeight+1+20,
              EB6STARTX+textWidth,
              EBSTARTY2+2*textHeight+1+20,  		   	
              0,					   	// draw rectangular button	
              BTN_DRAW,               	// will be dislayed after creation
              NULL,					   	// no bitmap	
              DTMinusSym,               // minus symbols as text
              altScheme);               // use alternate scheme

	BtnCreate(ID_BUTTON3,
              GetMaxX()-80,GetMaxY()-40,					   	// left, top corner	
              GetMaxX(),GetMaxY(),0, 		   	// right, bottom corner (with radius = 0)
              BTN_DRAW,               	// will be dislayed after creation
              NULL,					   	// no bitmap	
              Jumpstr,//, HomeStr			// LEFT arrow as text
              altScheme);		

updateJumptoxEb();

}
void updateJumptoxEb() {

    OBJ_HEADER* obj;
	
  	obj = GOLFindObject(ID_EB_RECORD1);
	EbSetText((EDITBOX*)obj, &Jumptox[0]);
	SetState(obj, EB_DRAW);	

	obj = GOLFindObject(ID_EB_RECORD2);
	EbSetText((EDITBOX*)obj, &Jumptox[2]);
	SetState(obj, EB_DRAW);	

	obj = GOLFindObject(ID_EB_RECORD3);
	EbSetText((EDITBOX*)obj, &Jumptox[5]);
	SetState(obj, EB_DRAW);


}


WORD MsgJumptox(WORD objMsg, OBJ_HEADER* pObj, GOL_MSG* pMsg){

	SHORT    id;

	OBJ_HEADER* obj;

	static char   thousand = 0, tens = 0;

	WORD ebID = 0;

     id = GetObjID(pObj);


    switch(id){
        case ID_BUTTON1:
            if(objMsg == BTN_MSG_RELEASED){
                    serialnumberRecord = 1;
            		screenState = CREATE_RECORD; 	// goto list box screen
            }
            return 1; // process by default
            

        case ID_BUTTON2:
            if(objMsg == BTN_MSG_RELEASED){
                    serialnumberRecord = serialnumber-1;
            		screenState = CREATE_RECORD; 	// goto list box screen
            }
            return 1; // process by default
            
       case ID_BUTTON_HOME:
            if(objMsg == BTN_MSG_RELEASED){

                screenState = CREATE_MAIN; 	// goto list box screen
            }
            return 1;

       case ID_BUTTON1_P:  						// increment hour value
		    if(objMsg == BTN_MSG_PRESSED){
				  Jumptox[0]++;
				if(Jumptox[0]>0x32)
					Jumptox[0]=0x30;
				ebID = ID_EB_RECORD1;
		
											
			}
			break;
        case ID_BUTTON1_M:							// decrement hour value
			if(objMsg == BTN_MSG_PRESSED){
				Jumptox[0]--;
				if(Jumptox[0]<=0x2F)
					Jumptox[0]=0x32;
				ebID = ID_EB_RECORD1;
	
			}
			break;

			
        case ID_BUTTON2_P:  						// increment minute value
        	if(objMsg == BTN_MSG_PRESSED){
				thousand++;
                 if(thousand > 99) thousand = 0;
				 ebID = ID_EB_RECORD2;
	
			}
			break;
        case ID_BUTTON2_M:							// decrement minute value
        	if(objMsg == BTN_MSG_PRESSED){
				thousand--;
                if(thousand < 0) thousand = 99;
			    if(thousand==0xff) thousand = 99;

				ebID = ID_EB_RECORD2;
	
			}
			break;



	    case ID_BUTTON3_P:  						// increment minute value
        	if(objMsg == BTN_MSG_PRESSED){
				tens++;
                 if(tens > 99) tens = 0;
				 ebID = ID_EB_RECORD3;
	
			}
			break;
        case ID_BUTTON3_M:							// decrement minute value
        	if(objMsg == BTN_MSG_PRESSED){
				tens--;
                if(tens < 0) tens = 99;
			    if(tens==0xff) tens = 99;
				 ebID = ID_EB_RECORD3;

			}
			break;
        case ID_BUTTON3:
            if(objMsg == BTN_MSG_RELEASED){
				
                    serialnumberRecord = 10000*(Jumptox[0]-'0')+1000*(Jumptox[2]-'0')+100*(Jumptox[3]-'0')+10*(Jumptox[5]-'0')+Jumptox[6]-'0';
                    if(serialnumberRecord>=serialnumber-1)
						  serialnumberRecord=serialnumber-1;
					if(serialnumberRecord==0)
						 serialnumberRecord=1;						
					screenState = CREATE_RECORD; 	// goto list box screen
            }
            return 1; // process by default           
        default:
			ebID = 0;
            return 1; // process by default
    }
if(ebID)
{
    Jumptox[2] = thousand/10+'0';
	Jumptox[3] = thousand%10+'0';


	Jumptox[5] = tens/10+'0';
	Jumptox[6] = tens%10+'0';
    updateJumptoxEb();
	
}
 return 1;
}


void	CreateSetting()
{
	GOLFree();   // free memory for the objects in the previous linked list and start new list

	CreatePage(SettingStr);	// CreatePage("Setting");

	Settingposition=0xaa;

    BtnCreate(ID_BUTTON1, 				// button ID 
              25,60,
              95,130,         	// dimension
              0,
              BTN_DRAW,//|BTN_DISABLED,//BTN_HIDE, 				// will be dislayed after creation 
              NULL,//(void*)&Picrecord,          // use bitmap
              button_AutooffStr,//HomeStr, 					// "HOME", 	    // text
              alt4Scheme);	//alt4Scheme            // alternative GOL scheme 



    BtnCreate(ID_BUTTON2,             	// button ID 
              145,60,
              215,130,         	// dimension
              0,					   	// set radius 
              BTN_DRAW,//|BTN_DISABLED,//|BTN_PRESSED,  	// draw a vertical capsule button
              							// that is initially pressed
              NULL,//(void*)&PicGPS,                    	// no bitmap
              PrintSettingStr,//DemarWendStr,//ThicknessStr, //NULL,//OffStr, 					// "OFF",      	// text
              alt4Scheme);            	// use alternate scheme 	
#ifndef NO_BLUETOOTH_BUTTON

    BtnCreate(ID_BUTTON3,             	// button ID 
              25,190,
              95,260,         	// dimension
              0,					   	// set radius 
              BTN_DRAW,//|BTN_DISABLED,//|BTN_PRESSED,  	// draw a vertical capsule button
              							// that is initially pressed
              NULL,//(void*)&PicGPS,                    	// no bitmap
              Bluetoothstr,//SoundStr,//DemarWendStr,//ThicknessStr, //NULL,//OffStr, 					// "OFF",      	// text
              alt4Scheme);            	// use alternate scheme 	
#endif         
    pbutton =  (BUTTON*)BtnCreate(ID_BUTTON4,             	// button ID 
              145,190,
              215,260,         	// dimension
              0,					   	// set radius 
              BTN_DRAW,//|BTN_TEXTTOP,//|BTN_PRESSED,  	// draw a vertical capsule button
              							// that is initially pressed
              NULL,//(void*)&PicGPS,                    	// no bitmap
              Drycheck,//DemarWendStr,//ThicknessStr, //NULL,//OffStr, 					     	
              alt4Scheme); 

/*
    BtnCreate(ID_BUTTON5,             	// button ID 
              78,145,
              162,175,         	// dimension
              0,					   	// set radius 
              BTN_DRAW,//|BTN_DISABLED,//|BTN_PRESSED,  	// draw a vertical capsule button
              							// that is initially pressed
              NULL,//(void*)&PicGPS,                    	// no bitmap
              Bluetoothstr,//DemarWendStr,//ThicknessStr, //NULL,//OffStr, 					// "OFF",      	// text
              alt4Scheme);            	// use alternate scheme 	
*/

}
WORD MsgSetting(WORD objMsg, OBJ_HEADER* pObj){
	OBJ_HEADER* pOtherRbtn;

    switch(GetObjID(pObj)){

        case ID_BUTTON_NEXT:
            if(objMsg == BTN_MSG_RELEASED){
               screenState = CREATE_MAIN;// goto check box demo screen
            }
            return 1; 							// process by default

        case ID_BUTTON_BACK:
            if(objMsg == BTN_MSG_RELEASED){
                screenState = CREATE_MAIN; 		// goto ECG demo screen
            }
            return 1; 							// process by default

	case ID_BUTTON_HOME:
            if(objMsg == BTN_MSG_RELEASED){
//            	abcStatus = 0;
                screenState = CREATE_MAIN; 	// goto list box screen
            }
            return 1;



        case ID_BUTTON1:
            if(objMsg == BTN_MSG_RELEASED){
                screenState = CREATE_SETBACKLIGHT; 		// goto ECG demo screen
            }
            return 1; 							// process by default



        case ID_BUTTON2:
        
			if(objMsg == BTN_MSG_PRESSED) {

				Settingposition=0xaa;

				screenState = CREATE_PRINTSETTING;
			}
		    return 0; 							// Do not process by default

        case ID_BUTTON3:
        
			if(objMsg == BTN_MSG_RELEASED) {
				

				screenState = CREATE_BlueTooth;//CREATE_SoundControl;2012.05.08
			
			}
		    return 1; 							// Do not process by default





     	 case ID_BUTTON4:
            if(objMsg == BTN_MSG_RELEASED){
				
			   TestMode = 0xdd;
			   Settingposition=0xaa;
               screenState = CREATE_Datadeal;
            }
            return 1; 							// process by default

        case ID_BUTTON5:

			if(objMsg == BTN_MSG_RELEASED)//BTN_MSG_PRESSED)
			screenState = CREATE_BlueTooth;

		    return 1;
/*
	case ID_BUTTON5:
            if(objMsg == BTN_MSG_RELEASED){
				
				
				
		       screenState = CREATE_EDITEEPROM;			
            }
            return 1; 							
    */ 
        default:
            return 1; 							// process by default
    }
}
void CreateSoundControl()
{
WORD pos ;
GOLFree();
CreatePage(SoundStr);	// CreatePage("Buttons");
pos=EEPROMReadWord(Beepsld1Address);
pSld1=SldCreate
        (
            ID_SLIDER1,                // object抯 ID
            20,
            100,
            200,
            130,// object抯 dimension
            SLD_DRAW,               // draw the object after creation
            1000,                    // range
            100,                      // page
            pos,                     // initial position
            NULL
        );

pos=EEPROMReadWord(Beepsld2Address);
pSld2=SldCreate
        (
            ID_SLIDER2,                // object抯 ID
            20,
            200,
            200,
            230,// object抯 dimension
            SLD_DRAW,               // draw the object after creation
            1000,                    // range
            100,                      // page
            pos,                     // initial position
            NULL
        );	
}
VOID SoundControlupdate(void)
{
   WORD        value, y, x;    // variables for the slider position
   static WORD prevValue = 0;
if(update1)
{

value = SldGetPos(pSld1);
EEPROMWriteWord(value, Beepsld1Address);
BEEP_TIME=10+value;
  update1 = 0;
}

if(update2)
{

value = SldGetPos(pSld2);
EEPROMWriteWord(value, Beepsld2Address);
TIMER_BASE=10+value;

  update2 = 0;
}



}
WORD MsgSoundControl(WORD objMsg, OBJ_HEADER * pObj, GOL_MSG * pMsg){

   switch(GetObjID(pObj)){

        case ID_BUTTON_NEXT:
            if(objMsg == BTN_MSG_RELEASED){
               screenState = CREATE_MAIN;// goto check box demo screen
            }
            return 1; 							// process by default

        case ID_BUTTON_BACK:
            if(objMsg == BTN_MSG_RELEASED){
                screenState = CREATE_MAIN; 		// goto ECG demo screen
            }
            return 1; 							// process by default

	case ID_BUTTON_HOME:
            if(objMsg == BTN_MSG_RELEASED){
                screenState = CREATE_MAIN; 	// goto list box screen
            }
            return 1;


	case ID_SLIDER1:
		    if((objMsg == SLD_MSG_INC) || (objMsg == SLD_MSG_DEC))      
            update1 = 1;

			 return 1;
			 
     case ID_SLIDER2:
		    if((objMsg == SLD_MSG_INC) || (objMsg == SLD_MSG_DEC))      
            update2 = 1;

			 return 1;
  

        default:
            return 1; 
 	}
}
void CreateEditeeprom(XCHAR *EditTestStr,WORD CharMax)
{

	static XCHAR pTempXchar[][2] = 	{	{'0',0},
										{'1',0},
										{'2',0},
										{'3',0},
										{'4',0},
										{'5',0},
										{'6',0},
										{'7',0},
										{'8',0},
										{'9',0},
										{'A',0},
										{'B',0},
										{'C',0},
										{'D',0},
										{'E',0},
										{'F',0}
									};
#define KEYSTARTX 	10
#define KEYSTARTY 	38
#define KEYSIZEX	55
#define KEYSIZEY  	45
#define MAXCHARSIZE CharMax
MAXCHAROld=CharMax;	
OBJ_HEADER* obj;
SHORT i;
//char *pText="EEPROM EDIT";
    GOLFree();  // free memory for the objects in the previous linked list and start new list

 WndCreate(ID_WINDOW1,       		// ID
              0,0,GetMaxX(),GetMaxY(), 	// dimension
              WND_DRAW,                	// will be dislayed after creation
              NULL,               	// icon
              EditTestStr,	   				// set text 
              navScheme);               // default GOL scheme 
BtnCreate(ID_BUTTON_BACK,         	// button ID 
              0,GetMaxY()-40,					   	// left, top corner	
              40,GetMaxY(),0, 		   	// right, bottom corner (with radius = 0)
              BTN_DRAW,               	// will be dislayed after creation
              (void*)&LEFT,					   	// no bitmap	
              NULL,//, HomeStr			// LEFT arrow as text
              blackScheme);          	   	// use navigation scheme
              
BtnCreate(ID_BUTTON_NEXT,   	    // button ID 
              GetMaxX()-40,GetMaxY()-40,
              GetMaxX(),GetMaxY(),0,   	// dimension (with radius = 0)
              BTN_DRAW,               	// will be dislayed and disabled after creation
              (void*)&RIGHT,					   	// no bitmap
              NULL,//, NextStr			// RIGHT arrow as text
              blackScheme);   


BtnCreate(ID_BACKSPACE,
		100,GetMaxY()-40,					   	// left, top corner	
              140,GetMaxY(),0, 		   	// right, bottom corner (with radius = 0)
              BTN_DRAW,               	// will be dislayed after creation
              NULL,					   	// no bitmap	
              UpArrowStr,			// LEFT arrow as text
              altScheme);

  
	RTCCProcessEvents();				// update the date and time strings
	i = 0;						
	while (i < 12) {
		dateTimeStr[i] 	  = (XCHAR)_time_str[i];
		dateTimeStr[i+13] = (XCHAR)_date_str[i];
		i++;
	}
	dateTimeStr[12] = (XCHAR)'\n';
	dateTimeStr[25] = (XCHAR)'\0';
	

 EbCreate(ID_EDITBOX1,              	// ID
              4,KEYSTARTY+1,GetMaxX()-4,KEYSTARTY+1*KEYSIZEY-GOL_EMBOSS_SIZE,  // dimension
              EB_DRAW|EB_CARET|EB_FOCUSED, // will be dislayed after creation
              NULL,
              MAXCHARSIZE,
              altScheme);               // default GOL scheme

    BtnCreate(ID_KEYPAD+7,             	// ID 
              KEYSTARTX+0*KEYSIZEX,
              KEYSTARTY+1*KEYSIZEY,
              KEYSTARTX+1*KEYSIZEX,
              KEYSTARTY+2*KEYSIZEY,    	// dimension
              0,
              BTN_DRAW,                	// will be dislayed after creation
              NULL,                    	// no bitmap
              pTempXchar[7],    		// text
              altScheme);              	// alternative GOL scheme   

    BtnCreate(ID_KEYPAD+8,             	// ID 
              KEYSTARTX+1*KEYSIZEX,
              KEYSTARTY+1*KEYSIZEY,
              KEYSTARTX+2*KEYSIZEX,
              KEYSTARTY+2*KEYSIZEY,    	// dimension
              0,
              BTN_DRAW,                	// will be dislayed after creation
              NULL,                    	// no bitmap
              pTempXchar[8],    		// text
              altScheme);              	// alternative GOL scheme 

    BtnCreate(ID_KEYPAD+9,             	// ID 
              KEYSTARTX+2*KEYSIZEX,
              KEYSTARTY+1*KEYSIZEY,
              KEYSTARTX+3*KEYSIZEX,
              KEYSTARTY+2*KEYSIZEY,    	// dimension
              0,
              BTN_DRAW,                	// will be dislayed after creation
              NULL,                    	// no bitmap
              pTempXchar[9],    		// text
              altScheme);              	// alternative GOL scheme 
              
     BtnCreate(ID_KEYPAD+15,             	// F
              KEYSTARTX+3*KEYSIZEX,
              KEYSTARTY+1*KEYSIZEY,
              KEYSTARTX+4*KEYSIZEX,
              KEYSTARTY+2*KEYSIZEY,    	// dimension
              0,
              BTN_DRAW,                	// will be dislayed after creation
              NULL,                    	// no bitmap
              pTempXchar[15],    		// text
              altScheme);              	// alternative GOL scheme            

    BtnCreate(ID_KEYPAD+4,             	// ID 
              KEYSTARTX+0*KEYSIZEX,
              KEYSTARTY+2*KEYSIZEY,
              KEYSTARTX+1*KEYSIZEX,
              KEYSTARTY+3*KEYSIZEY,    	// dimension
              0,
              BTN_DRAW,                	// will be dislayed after creation
              NULL,                    	// no bitmap
              pTempXchar[4],    		// text
              altScheme);              	// alternative GOL scheme   

    BtnCreate(ID_KEYPAD+5,             	// ID 
              KEYSTARTX+1*KEYSIZEX,
              KEYSTARTY+2*KEYSIZEY,
              KEYSTARTX+2*KEYSIZEX,
              KEYSTARTY+3*KEYSIZEY,    	// dimension
              0,
              BTN_DRAW,                	// will be dislayed after creation
              NULL,                    	// no bitmap
              pTempXchar[5],    		// text
              altScheme);              	// alternative GOL scheme 

    BtnCreate(ID_KEYPAD+6,             	// ID 
              KEYSTARTX+2*KEYSIZEX,
              KEYSTARTY+2*KEYSIZEY,
              KEYSTARTX+3*KEYSIZEX,
              KEYSTARTY+3*KEYSIZEY,    	// dimension
              0,
              BTN_DRAW,                	// will be dislayed after creation
              NULL,                    	// no bitmap
              pTempXchar[6],    		// text
              altScheme);              	// alternative GOL scheme 

    BtnCreate(ID_KEYPAD+14,             	// ID 
              KEYSTARTX+3*KEYSIZEX,
              KEYSTARTY+2*KEYSIZEY,
              KEYSTARTX+4*KEYSIZEX,
              KEYSTARTY+3*KEYSIZEY,    	// dimension
              0,
              BTN_DRAW,                	// will be dislayed after creation
              NULL,                    	// no bitmap
              pTempXchar[14],    		// text
              altScheme);              	// alternative GOL scheme 		

    BtnCreate(ID_KEYPAD+1,             	// ID 
              KEYSTARTX+0*KEYSIZEX,
              KEYSTARTY+3*KEYSIZEY,
              KEYSTARTX+1*KEYSIZEX,
              KEYSTARTY+4*KEYSIZEY,    	// dimension
              0,
              BTN_DRAW,                	// will be dislayed after creation
              NULL,                    	// no bitmap
              pTempXchar[1],    		// text
              altScheme);              	// alternative GOL scheme   

    BtnCreate(ID_KEYPAD+2,             	// ID 
              KEYSTARTX+1*KEYSIZEX,
              KEYSTARTY+3*KEYSIZEY,
              KEYSTARTX+2*KEYSIZEX,
              KEYSTARTY+4*KEYSIZEY,    	// dimension
              0,
              BTN_DRAW,                	// will be dislayed after creation
              NULL,                    	// no bitmap
              pTempXchar[2],    		// text
              altScheme);              	// alternative GOL scheme 

    BtnCreate(ID_KEYPAD+3,             	// ID 
              KEYSTARTX+2*KEYSIZEX,
              KEYSTARTY+3*KEYSIZEY,
              KEYSTARTX+3*KEYSIZEX,
              KEYSTARTY+4*KEYSIZEY,    	// dimension
              0,
              BTN_DRAW,                	// will be dislayed after creation
              NULL,                    	// no bitmap
              pTempXchar[3],    		// text
              altScheme);              	// alternative GOL scheme 

    BtnCreate(ID_KEYPAD+13,             	// ID 
              KEYSTARTX+3*KEYSIZEX,
              KEYSTARTY+3*KEYSIZEY,
              KEYSTARTX+4*KEYSIZEX,
              KEYSTARTY+4*KEYSIZEY,    	// dimension
              0,
              BTN_DRAW,                	// will be dislayed after creation
              NULL,                    	// no bitmap
              pTempXchar[13],    		// text
              altScheme);              	// alternative GOL scheme 			  


    BtnCreate(ID_KEYPAD+0,            	// ID 
              KEYSTARTX+0*KEYSIZEX,
              KEYSTARTY+4*KEYSIZEY,
              KEYSTARTX+1*KEYSIZEX,
              KEYSTARTY+5*KEYSIZEY,    	// dimension
              0,
              BTN_DRAW,                	// will be dislayed after creation
              NULL,                    	// no bitmap
              pTempXchar[0],			// Left Arrow 
              altScheme);              	// alternative GOL scheme 

BtnCreate(ID_KEYPAD+10,             	// ID 
              KEYSTARTX+1*KEYSIZEX,		
              KEYSTARTY+4*KEYSIZEY,
              KEYSTARTX+2*KEYSIZEX,
              KEYSTARTY+5*KEYSIZEY,    	// dimension
              0,
              BTN_DRAW,                	// will be dislayed after creation
              NULL,                    	// no bitmap
              pTempXchar[10],    		// text
              altScheme);              	// alternative GOL scheme 
              
BtnCreate(ID_KEYPAD+11,             	// ID 
              KEYSTARTX+2*KEYSIZEX,
              KEYSTARTY+4*KEYSIZEY,
              KEYSTARTX+3*KEYSIZEX,
              KEYSTARTY+5*KEYSIZEY,    	// dimension
              0,
              BTN_DRAW,                	// will be dislayed after creation
              NULL,                    	// no bitmap
              pTempXchar[11],    		// text
              altScheme); 
			  
BtnCreate(ID_KEYPAD+12,             	// ID 
              KEYSTARTX+3*KEYSIZEX,
              KEYSTARTY+4*KEYSIZEY,
              KEYSTARTX+4*KEYSIZEX,
              KEYSTARTY+5*KEYSIZEY,    	// dimension
              0,
              BTN_DRAW,                	// will be dislayed after creation
              NULL,                    	// no bitmap
              pTempXchar[12],    		// text
              altScheme); 
              

}
WORD MsgEditeeprom(WORD objMsg, OBJ_HEADER* pObj, GOL_MSG* pMsg)

{

EDITBOX* pEb;
SHORT    id;
XCHAR    temp;
unsigned int  j=1,i=0,point=0;
static char status = 0;			// status to check if calling, holding or not
//XCHAR  EEPROMbuffStr[6]={0};
unsigned int  eepromaddress=0;
BYTE  eepromvalue=0;
    id = GetObjID(pObj);

    // If number key is pressed
    if(objMsg == BTN_MSG_PRESSED)//BTN_MSG_RELEASED
    {
    if(id >= ID_KEYPAD)
    if(id  <= ID_KEYPAD+9){
	    if (!status) {
        pEb = (EDITBOX*)GOLFindObject(ID_EDITBOX1);    

			EbAddChar3(pEb,'0'+id-ID_KEYPAD);
	               SetState(pEb, EB_DRAW);
	        	TestMiMaStr[0] = '0'+id-ID_KEYPAD;

				  

        return 1;        
    }
}	
    if(id >= ID_KEYPAD+10)
    if(id  <= ID_KEYPAD+15){
	    if (!status) {
        pEb = (EDITBOX*)GOLFindObject(ID_EDITBOX1);    

			EbAddChar3(pEb,'A'+id-ID_KEYPAD-10);
	               SetState(pEb, EB_DRAW);
	        	TestMiMaStr[0] = 'A'+id-ID_KEYPAD-10;

				  

        return 1;        
    }
}	
    	}	
    switch(id){


        case ID_BACKSPACE:
            if(objMsg == BTN_MSG_PRESSED){
                if (!status) {
	                pEb = (EDITBOX*)GOLFindObject(ID_EDITBOX1);    
	                EbDeleteChar(pEb);
	                SetState(pEb, EB_DRAW);
	            } 
            }
            return 1;
        case ID_BUTTON_NEXT:
            if(objMsg == BTN_MSG_RELEASED){
                 
                       eepromaddress=(TempbuffStr[0]-'0')&0x000F;
			eepromaddress=eepromaddress<<4;
			if(TempbuffStr[1]<='9')
			 eepromaddress=eepromaddress|((TempbuffStr[1]-'0')&0x000F);
			else 
			eepromaddress=eepromaddress|((TempbuffStr[1]-'A'+10)&0x000F);
			eepromaddress=eepromaddress<<4;
			
			if(TempbuffStr[2]<='9')
			 eepromaddress=eepromaddress|((TempbuffStr[2]-'0')&0x000F);
			else 
			eepromaddress=eepromaddress|((TempbuffStr[2]-'A'+10)&0x000F);
			eepromaddress=eepromaddress<<4;
			if(TempbuffStr[3]<='9')
			 eepromaddress=eepromaddress|((TempbuffStr[3]-'0')&0x000F);
			else 
			eepromaddress=eepromaddress|((TempbuffStr[3]-'A'+10)&0x000F);
		      	if(TempbuffStr[4]<='9')
			 eepromvalue=eepromvalue|((TempbuffStr[4]-'0')&0x000F);
			else 
			eepromvalue=eepromvalue|((TempbuffStr[4]-'A'+10)&0x000F);
			eepromvalue=eepromvalue<<4;
			if(TempbuffStr[5]<='9')
			 eepromvalue=eepromvalue|((TempbuffStr[5]-'0')&0x0F);
			else 
			eepromvalue=eepromvalue|((TempbuffStr[5]-'A'+10)&0x0F);
		
                        for(i=0;i<6;i++){
			TempbuffStr[i] = 0;
   	                   }	
				 
			EEPROMWriteByte(eepromvalue, eepromaddress);
			 screenState = CREATE_SETTING; 	// goto list box screen            
            }
        case ID_BUTTON_BACK:
            if(objMsg == BTN_MSG_RELEASED){
                screenState = CREATE_SETTING; 	// goto list box screen
            }
            return 1; // process by default

        default:
            return 1; // process by default
    }


}

WORD MsgEditDemarcate(WORD objMsg, OBJ_HEADER* pObj, GOL_MSG* pMsg)

{

EDITBOX* pEb;
SHORT    id;
XCHAR    temp;
unsigned int  j=1,i=0,point=0;
static char status = 0;			// status to check if calling, holding or not
unsigned int  INPUTtem=0,INPUTAD=0;
    id = GetObjID(pObj);

    // If number key is pressed
    if(objMsg == BTN_MSG_PRESSED)//BTN_MSG_RELEASED
    {
    if(id >= ID_KEYPAD)
    if(id  <= ID_KEYPAD+9){
	    if (!status) {
        pEb = (EDITBOX*)GOLFindObject(ID_EDITBOX1);    

			EbAddChar3(pEb,'0'+id-ID_KEYPAD);
	               SetState(pEb, EB_DRAW);
	        	TestMiMaStr[0] = '0'+id-ID_KEYPAD;

				  

        return 1;        
    }
}	
    if(id >= ID_KEYPAD+10)
    if(id  <= ID_KEYPAD+15){
	    if (!status) {
        pEb = (EDITBOX*)GOLFindObject(ID_EDITBOX1);    

			EbAddChar3(pEb,'A'+id-ID_KEYPAD-10);
	               SetState(pEb, EB_DRAW);
	        	TestMiMaStr[0] = 'A'+id-ID_KEYPAD-10;

				  

        return 1;        
    }
}	
    	}	
    switch(id){


        case ID_BACKSPACE:
            if(objMsg == BTN_MSG_PRESSED){
                if (!status) {
	                pEb = (EDITBOX*)GOLFindObject(ID_EDITBOX1);    
	                EbDeleteChar(pEb);
	                SetState(pEb, EB_DRAW);
	            } 
            }
            return 1;
        case ID_BUTTON_NEXT:
            if(objMsg == BTN_MSG_RELEASED){
         if((TempbuffStr[0]<='0')&&(TempbuffStr[1]<='0')&&(TempbuffStr[2]<='0')&&(TempbuffStr[3]<='0'))
         	{
			if(ADLIST>=1&&ADLIST<=3)
                screenState = CREATE_DEMARCATE; 	// goto list box screen
                else  screenState = CREATE_DEMARCATE2;
         	}
		 else {
          if(TempbuffStr[0]<='9')
			INPUTtem=(TempbuffStr[0]-'0')&0x000F;
	      else 
	  	    INPUTtem=(TempbuffStr[0]-'A'+10)&0x000F;
	        INPUTtem=INPUTtem<<4;
	  
			if(TempbuffStr[1]<='9')
			 INPUTtem=INPUTtem|((TempbuffStr[1]-'0')&0x000F);
			else 
			INPUTtem=INPUTtem|((TempbuffStr[1]-'A'+10)&0x000F);
			INPUTtem=INPUTtem<<4;
			
			if(TempbuffStr[2]<='9')
			 INPUTtem=INPUTtem|((TempbuffStr[2]-'0')&0x000F);
			else 
			INPUTtem=INPUTtem|((TempbuffStr[2]-'A'+10)&0x000F);
			INPUTtem=INPUTtem<<4;
			if(TempbuffStr[3]<='9')
			 INPUTtem=INPUTtem|((TempbuffStr[3]-'0')&0x000F);
			else 
			INPUTtem=INPUTtem|((TempbuffStr[3]-'A'+10)&0x000F);
			
      if(TempbuffStr[4]<='9')
			INPUTAD=(TempbuffStr[4]-'0')&0x000F;
	  else 
	  	    INPUTAD=(TempbuffStr[4]-'A'+10)&0x000F;
			INPUTAD=INPUTAD<<4;
			if(TempbuffStr[5]<='9')
			 INPUTAD=INPUTAD|((TempbuffStr[5]-'0')&0x000F);
			else 
			INPUTAD=INPUTAD|((TempbuffStr[5]-'A'+10)&0x000F);
			INPUTAD=INPUTAD<<4;
			
			if(TempbuffStr[6]<='9')
			 INPUTAD=INPUTAD|((TempbuffStr[6]-'0')&0x000F);
			else 
			INPUTAD=INPUTAD|((TempbuffStr[6]-'A'+10)&0x000F);
			INPUTAD=INPUTAD<<4;
			if(TempbuffStr[7]<='9')
			 INPUTAD=INPUTAD|((TempbuffStr[7]-'0')&0x000F);
			else 
			INPUTAD=INPUTAD|((TempbuffStr[7]-'A'+10)&0x000F);
		
           for(i=0;i<6;i++){
			TempbuffStr[i] = 0;
   	                   }	
		if(ADLIST==1){
		
			EEPROMWriteWord(INPUTtem, 0x6000);
			EEPROMWriteWord(INPUTAD, 0x600e);
		screenState = CREATE_DEMARCATE; 	// goto list box screen  
			}
		else if(ADLIST==2)
			{
			EEPROMWriteWord(INPUTtem, 0x6002);
			EEPROMWriteWord(INPUTAD, 0x6010);
			screenState = CREATE_DEMARCATE; 	// goto list box screen  
			}
		else if(ADLIST==3)
			{
			EEPROMWriteWord(INPUTtem, 0x6004);
			EEPROMWriteWord(INPUTAD, 0x6012);
			screenState = CREATE_DEMARCATE; 	// goto list box screen  
			}
			else if(ADLIST==4)
				{
			EEPROMWriteWord(INPUTtem, 0x6014);
			EEPROMWriteWord(INPUTAD, 0x6016);
			screenState = CREATE_DEMARCATE2; 	// goto list box screen  
				}
			else if(ADLIST==5)
				{
			EEPROMWriteWord(INPUTtem, 0x600a);
			EEPROMWriteWord(INPUTAD, 0x6018);
			screenState = CREATE_DEMARCATE2; 
				}
			
			else if(ADLIST==6)
				{
			EEPROMWriteWord(INPUTtem, 0x600c);
			EEPROMWriteWord(INPUTAD, 0x601a);
			screenState = CREATE_DEMARCATE2; 
			} 
			return 1;
            }
            	}
        case ID_BUTTON_BACK:
            if(objMsg == BTN_MSG_RELEASED){
				 if(ADLIST>=1&&ADLIST<=3)
                screenState = CREATE_DEMARCATE; 	// goto list box screen
                else  screenState = CREATE_DEMARCATE2;
            }
            return 1; // process by default

        default:
            return 1; // process by default
    }


}
WORD MsgEditMarkValue(WORD objMsg, OBJ_HEADER* pObj, GOL_MSG* pMsg)

{

EDITBOX* pEb;
SHORT    id;
XCHAR    temp;
unsigned int  j=1,i=0,point=0;
static char status = 0;			// status to check if calling, holding or not
unsigned int  INPUTtem=0,INPUTAD=0;
    id = GetObjID(pObj);

    // If number key is pressed
    if(objMsg == BTN_MSG_PRESSED)//BTN_MSG_RELEASED
    {
    if(id >= ID_KEYPAD)
    if(id  <= ID_KEYPAD+9){
	    if (!status) {
        pEb = (EDITBOX*)GOLFindObject(ID_EDITBOX1);    

			EbAddChar3(pEb,'0'+id-ID_KEYPAD);
	               SetState(pEb, EB_DRAW);
	        	TestMiMaStr[0] = '0'+id-ID_KEYPAD;

				  

        return 1;        
    }
}	
    if(id >= ID_KEYPAD+10)
    if(id  <= ID_KEYPAD+15){
	    if (!status) {
        pEb = (EDITBOX*)GOLFindObject(ID_EDITBOX1);    

			EbAddChar3(pEb,'A'+id-ID_KEYPAD-10);
	               SetState(pEb, EB_DRAW);
	        	TestMiMaStr[0] = 'A'+id-ID_KEYPAD-10;

				  

        return 1;        
    }
}	
    	}	
    switch(id){


        case ID_BACKSPACE:
            if(objMsg == BTN_MSG_PRESSED){
             
	                pEb = (EDITBOX*)GOLFindObject(ID_EDITBOX1);    
	                EbDeleteChar(pEb);
	                SetState(pEb, EB_DRAW);
	        
            }
            return 1;
        case ID_BUTTON_NEXT:
            if(objMsg == BTN_MSG_RELEASED){
         if((TempbuffStr[0]<='0')&&(TempbuffStr[1]<='0')&&(TempbuffStr[2]<='0')&&(TempbuffStr[3]<='0'))

              screenState = CREATE_Mark;

		 else {
          if(TempbuffStr[0]<='9')
			INPUTtem=(TempbuffStr[0]-'0')&0x000F;
	      else 
	  	    INPUTtem=(TempbuffStr[0]-'A'+10)&0x000F;
	        INPUTtem=INPUTtem<<4;
	  
			if(TempbuffStr[1]<='9')
			 INPUTtem=INPUTtem|((TempbuffStr[1]-'0')&0x000F);
			else 
			INPUTtem=INPUTtem|((TempbuffStr[1]-'A'+10)&0x000F);
			INPUTtem=INPUTtem<<4;
			
			if(TempbuffStr[2]<='9')
			 INPUTtem=INPUTtem|((TempbuffStr[2]-'0')&0x000F);
			else 
			INPUTtem=INPUTtem|((TempbuffStr[2]-'A'+10)&0x000F);
			INPUTtem=INPUTtem<<4;
			if(TempbuffStr[3]<='9')
			 INPUTtem=INPUTtem|((TempbuffStr[3]-'0')&0x000F);
			else 
			INPUTtem=INPUTtem|((TempbuffStr[3]-'A'+10)&0x000F);
					
           for(i=0;i<6;i++){
			TempbuffStr[i] = 0;
   	                   }			
			EEPROMWriteWord(INPUTtem, 0x6022);
		    screenState = CREATE_Mark; 	//  
	
			return 1;
            }
            	}
        case ID_BUTTON_BACK:
            if(objMsg == BTN_MSG_RELEASED){

                  screenState = CREATE_Mark;
            }
            return 1; // process by default

        default:
            return 1; // process by default
    }


}


WORD MsgEditID(WORD objMsg, OBJ_HEADER* pObj, GOL_MSG* pMsg)

{

EDITBOX* pEb;
SHORT    id;
XCHAR    temp;
unsigned char  j=0,i=0;//,Tempbuffchar[9]={0,0,0,0,0,0,0,0,0};

BYTE  IDvalue[3]={0};
    id = GetObjID(pObj);

    // If number key is pressed
    if(objMsg == BTN_MSG_PRESSED)//BTN_MSG_RELEASED
    {
    if(id >= ID_KEYPAD)
    if(id  <= ID_KEYPAD+9){

        pEb = (EDITBOX*)GOLFindObject(ID_EDITBOX1);    

			EbAddChar3(pEb,'0'+id-ID_KEYPAD);
	               SetState(pEb, EB_DRAW);
	        	TestMiMaStr[0] = '0'+id-ID_KEYPAD;

				  

        return 1;        
}	
    if(id >= ID_KEYPAD+10)
    if(id  <= ID_KEYPAD+15){

        pEb = (EDITBOX*)GOLFindObject(ID_EDITBOX1);    

			EbAddChar3(pEb,'A'+id-ID_KEYPAD-10);
	               SetState(pEb, EB_DRAW);
	        	TestMiMaStr[0] = 'A'+id-ID_KEYPAD-10;

				  

        return 1;        

}	
    	}	
    switch(id){


        case ID_BACKSPACE:
            if(objMsg == BTN_MSG_PRESSED){
            
	                pEb = (EDITBOX*)GOLFindObject(ID_EDITBOX1);    
	                EbDeleteChar(pEb);
	                SetState(pEb, EB_DRAW);
	        
            }
            return 1;
        case ID_BUTTON_NEXT:
            if(objMsg == BTN_MSG_RELEASED){
				
    // for(i=0;i<8;i++)
		//Tempbuffchar[i] = (unsigned char)TempbuffStr[i];
	 
	if((TempbuffStr[0]!=0)&&(TempbuffStr[5]!=0))
		{
           j = 0;
		for(i=0;i<3;i++)
			{  
                IDvalue[i]=(TempbuffStr[j]-'0')&0x0F;
			  
			    IDvalue[i]=IDvalue[i]<<4;
			

			    IDvalue[i]=IDvalue[i]|((TempbuffStr[1+j]-'0')&0x0F);


			  j=j+2;

			}

		
            for(i=0;i<9;i++)
			    TempbuffStr[i] = 0;
	
		if(screenState==DISPLAY_EDITID)
			{
			EEPROMWriteByte(IDvalue[0], ID_JIQIADDRESS);
			EEPROMWriteByte(IDvalue[1], ID_JIQIADDRESS+1);
			EEPROMWriteByte(IDvalue[2], ID_JIQIADDRESS+2);
			//EEPROMWriteByte(IDvalue[3], ID_JIQIADDRESS+3);

		}

			 screenState = CREATE_DISPLAYID; 	// goto list box screen            
            }
        case ID_BUTTON_BACK:
            if(objMsg == BTN_MSG_RELEASED){
                screenState = CREATE_DISPLAYID; 	// goto list box screen
            }
            return 1; // process by default

        default:
            return 1; // process by default
    }

}
}

void CreateDisplayMachineID(void)
{
unsigned char j=0,i=0;

for(i=0;i<3;i++)
{

IDumberStr[j] = EEPROMReadByte(ID_JIQIADDRESS+i)/16+'0';
Nop();
IDumberStr[j+1] = EEPROMReadByte(ID_JIQIADDRESS+i)%16+'0';
Nop();

j=j+2;

}

GOLFree();   // free memory for the objects in the previous linked list and start new list

	CreatePage(EditID_Title);	// CreatePage("Setting");
	
    BtnCreate(ID_BUTTON1, 				// button ID 
              MAINSTARTX+0*(MAINCHARSIZE+MAINSIZEX),MAINSTARTY+10,
              MAINSTARTX+1*MAINCHARSIZE,MAINSTARTY+1*MAINCHARSIZE+10,           	// dimension
              0,					   	// set radius 
              BTN_DRAW|IDSTATUS,  			   	// draw a beveled button
              NULL,//(void*)&Pictest,                    	// no bitmap
              EditIDstr,//ButtonStr, 				// "Button",     	// text
              alt4Scheme);              	
	
	StCreate(ID_STATICTEXT3,				// button ID 
              MAINSTARTX+1*(MAINCHARSIZE+MAINSIZEX),MAINSTARTY+15,
              220,MAINSTARTY+1*MAINCHARSIZE+15,         	// dimension
              ST_DRAW|ST_CENTER_ALIGN,//|BTN_DISABLED,//BTN_HIDE, 				// will be dislayed after creation 
              Serialnumber,//HomeStr, 					// "HOME", 	    // text
              mainScheme);	//alt4Scheme           
              

	StCreate(ID_STATICTEXT3,				// button ID 
              MAINSTARTX+1*(MAINCHARSIZE+MAINSIZEX),MAINSTARTY+40,
              220,MAINSTARTY+1*MAINCHARSIZE+40,         	// dimension
              ST_DRAW|ST_CENTER_ALIGN,//|BTN_DISABLED,//BTN_HIDE, 				// will be dislayed after creation 
              IDumberStr,//HomeStr, 					// "HOME", 	    // text
              mainScheme);	//alt4Scheme            
              
	StCreate(ID_STATICTEXT3,				// button ID 
              7,170,
              220,200,         	// dimension
              ST_DRAW|ST_CENTER_ALIGN,//|BTN_DISABLED,//BTN_HIDE, 				// will be dislayed after creation 
              Version,//HomeStr, 					// "HOME", 	    // text
              mainScheme);	//alt4Scheme            // alternative GOL scheme 

	StCreate(ID_STATICTEXTID,				// button ID 
              7,200,
              220,230,         	// dimension
              ST_DRAW|ST_CENTER_ALIGN,//|BTN_DISABLED,//BTN_HIDE, 				// will be dislayed after creation 
              SoftwareID,//HomeStr, 					// "HOME", 	    // text
              mainScheme);	//alt4Scheme            // alternative GOL scheme 

}

WORD MsgDisplayMachineID(WORD objMsg, OBJ_HEADER* pObj)
{
    switch(GetObjID(pObj)){

        case ID_BUTTON_NEXT:
            if(objMsg == BTN_MSG_RELEASED){
               screenState = CREATE_AGENT;// goto check box demo screen
            }
            return 1; 							// process by default

        case ID_BUTTON_BACK:
            if(objMsg == BTN_MSG_RELEASED){
                screenState = CREATE_AGENT; 		// goto ECG demo screen
            }
            return 1; 							// process by default
        case ID_BUTTON_HOME:
            if(objMsg == BTN_MSG_RELEASED){

                screenState = CREATE_MAIN; 	// goto list box screen
            }
            return 1;

        case ID_BUTTON1:
            if(objMsg == BTN_MSG_RELEASED){

                screenState = CREATE_EDITID; 		// goto ECG demo screen
            }
            return 1; 							// process by default


	    default:
            return 1;	
    	}
}

void  CreateBlowPlease()
{
    alarmON = FALSE;
	GOLFree();   // free memory for the objects in the previous linked list and start new list
	CreatePage(BlowStr);	// CreatePage("Blow Please");
//	pump_pwr=1;// 泵充电
	pProgressBar = PbCreate(ID_PROGRESSBAR1,// ID
              40,85,(GetMaxX()-40),135,          		// dimension
              PB_DRAW,                 		// will be dislayed after creation
              25,                      		// position
              50,                      		// range
              NULL);         				// use default scheme
	
	StCreate(ID_STATICTEXT3,           		// ID 
              MAINSTARTX+0*(MAINCHARSIZE+MAINSIZEX),MAINSTARTY+2*MAINCHARSIZE+MAINSIZEY,
              MAINSTARTX+3*(MAINCHARSIZE+MAINSIZEX),MAINSTARTY+2*(MAINCHARSIZE+MAINSIZEY),          		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              BlowStr, 				// "GPS", 	// text
              altScheme);    // alt2Scheme              // use alternate scheme
     BtnCreate(ID_BUTTON1,             	// button ID 
               40,185,(GetMaxX()-40),235,           	// dimension
              0,					   	// set radius 
              BTN_DRAW,//|BTN_TOGGLE,   	// draw a vertical capsule button 
              						   	// that has a toggle behavior
              NULL,                    	// no bitmap
              PassivityStr,//LowStr, 					// "LO",       	// text
              altScheme);           	// use alternate scheme
}

WORD MsgBlowPlease(WORD objMsg, OBJ_HEADER* pObj){


        switch(GetObjID(pObj)){

        case ID_BUTTON_NEXT:
            if(objMsg == BTN_MSG_RELEASED){
            	g_blow_time = 0;		// 结果出来恢复正常//不吹气跳转
                screenState = CREATE_MAIN; 	// goto radio buttons screen
            }
            return 1; 								// process by default

        case ID_BUTTON_BACK:
            if(objMsg == BTN_MSG_RELEASED){
            	g_blow_time = 0;		// 结果出来恢复正常//不吹气跳转
                screenState = CREATE_MAIN;  		// goto round buttons screen
            }
            return 1; 
       case ID_BUTTON_HOME:
            if(objMsg == BTN_MSG_RELEASED){
		        g_blow_time = 0;		// 结果出来恢复正常//不吹气跳转
		        alarmON = FALSE;
                screenState = CREATE_MAIN; 	// goto list box screen
            }
            return 1;

        case  ID_BUTTON1:

		 if(objMsg == BTN_MSG_RELEASED){

              screenState = CREATE_Datadeal;
			  ST_IRstatus = ST_Passive;

            }
            return 1; 

         default:
   //      	pump_pwr=1;// 泵充电
                 return 1; 
        }
}
void  CreateDatadeal()
{

    GOLFree();   // free memory for the objects in the previous linked list and start new list

	CreatePage0(PROCCESING);	//DeleteStr CreatePage("Blow Please");
    StCreate(ID_STATICTEXT3,           		// ID 
              40,140,
              200,180,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              PROCCESINGwait, 				// "TEST", 	// text
              alt5Scheme);   //mainScheme   alt2Scheme
/*
    StCreate(ID_STATICTEXT5,           		// ID 
              40,180,
              200,220,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              arrow1, 				// "TEST", 	// text
              alt5Scheme);   //mainScheme   alt2Scheme
*/
}
WORD  MsgDatadeal(WORD objMsg, OBJ_HEADER* pObj)
{

  
}
void CreateDISCONTINUED()
{

	GOLFree();   // free memory for the objects in the previous linked list and start new list
	CreatePage(BlowDiscontinuedstr);	// CreatePage("Blow Please");
	BtnCreate(ID_BUTTON1,             	// button ID 
               40,185,(GetMaxX()-40),235,           	// dimension
              0,					   	// set radius 
              BTN_DRAW,//|BTN_TOGGLE,   	// draw a vertical capsule button 
              						   	// that has a toggle behavior
              NULL,                    	// no bitmap
              RepeatStr,//LowStr, 					// "LO",       	// text
              altScheme);           	// use alternate scheme
        BtnCreate(ID_BUTTON2,             	// button ID 
               40,105,(GetMaxX()-40),155,           	// dimension
              0,					   	// set radius 
              BTN_DRAW,//|BTN_TOGGLE,   	// draw a vertical capsule button 
              						   	// that has a toggle behavior
              NULL,                    	// no bitmap
              Discontinuedstr,//InterruptedStr,//RefuseStr,//LowStr, 					// "LO",       	// text
              altScheme);           	// use alternate scheme


}

WORD MsgDISCONTINUED(WORD objMsg, OBJ_HEADER* pObj, GOL_MSG* pMsg){

EDITBOX* pEb;
SHORT    id;
XCHAR    temp;
static char status = 0;			// status to check if calling, holding or not

    id = GetObjID(pObj);


    switch(id){
        case ID_BUTTON_NEXT:
            if(objMsg == BTN_MSG_RELEASED){
               	screenState = CREATE_MAIN ;//++;prevState//= ; 	// goto meter screen
            }
            return 1; // process by default

        case ID_BUTTON_BACK:
            if(objMsg == BTN_MSG_RELEASED){
                screenState = CREATE_BLOW; 	// goto list box screen
            }
            return 1; // process by default
            
	case ID_BUTTON_HOME:
            if(objMsg == BTN_MSG_RELEASED){
                screenState = CREATE_MAIN; 	// goto list box screen
            }
            return 1;
        case ID_BUTTON1:
            if(objMsg == BTN_MSG_RELEASED){
			ST_IRstatus = 0;
			screenState = CREATE_BLOW;     	// goto radio buttons screen
			g_backlight_time = 3*60;// 3分钟不吹气，自动跳出来
            }
            return 1; // process by default
        case ID_BUTTON2:
            if(objMsg == BTN_MSG_RELEASED){
		ST_IRstatus=ST_Discontinued;
                screenState = CREATE_RESULT; 
            }
            return 1; // process by default

        default:
            return 1; // process by default
    }


}

void  CreateGPS()
{
   #ifdef  USE_GPS
	memset(&longitude[0],1,8);
	memset(&latitude[0],1,8);
	u4_hand();	// Get the GPS 
   #endif
	GOLFree();   // free memory for the objects in the previous linked list and start new list

    mainScheme->pFont = (void*)&GOLMediumFont;
	CreatePage(GPSStr);	// CreatePage("Blow Please");
	
	StCreate(ID_STATICTEXT3,           		// ID 
              MAINSTARTX-10,2*MAINSTARTY,
              MAINSTARTX+1*MAINCHARSIZE+10,MAINSTARTY+1*MAINCHARSIZE+MAINSIZEY,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              LongitudeStr, 	// 经度			// "TEST", 	// text
              mainScheme);   //mainScheme   alt2Scheme             // use alternate scheme
        StCreate(ID_STATICTEXTLON,           		// ID 
              MAINSTARTX+1*(MAINCHARSIZE+MAINSIZEX),2*MAINSTARTY,
              MAINSTARTX+3*(MAINCHARSIZE+MAINSIZEX),MAINSTARTY+1*MAINCHARSIZE+MAINSIZEY,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              sLongitude, 				// "TEST", 	// text
              mainScheme);   //mainScheme   alt2Scheme             // use alternate scheme
              
	StCreate(ID_STATICTEXT3,           		// ID 
              MAINSTARTX-10,MAINSTARTY+2*MAINCHARSIZE+MAINSIZEY,
              MAINSTARTX+1*MAINCHARSIZE+10,MAINSTARTY+2*(MAINCHARSIZE+MAINSIZEY),         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              LatitudeStr, 		// 纬度		// "TEST", 	// text
              mainScheme); 

	StCreate(ID_STATICTEXTLAT,           		// ID 
              MAINSTARTX+1*(MAINCHARSIZE+MAINSIZEX),MAINSTARTY+2*MAINCHARSIZE+MAINSIZEY,
              MAINSTARTX+3*(MAINCHARSIZE+MAINSIZEX),MAINSTARTY+2*(MAINCHARSIZE+MAINSIZEY),          		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              sLatitude, 				// "GPS", 	// text
              mainScheme);    // alt2Scheme              // use alternate scheme


	CbCreate(ID_CHECKBOX9,             	// ID 
              positionax+3,235,positionmaxx-2,260,         	// dimension
              CB_DRAW,      			// will be dislayed and checked after creation
              G_Print, 			// "Text Bottom"
              alt5Scheme);              	// alternative GOL scheme 			  


}

void Battery()
{
	SetColor(WHITE);
				Line(130,13,130,17);	//|
				Line(130,13,132,13);		//--
				Line(130,17,132,17);		//_
//				Line(125,5,125,8);

				Line(132,10,132,20);		//|
				Line(132,10,156,10);		//_
				Line(132,20,156,20);		//_
				Line(156,10,156,20);		//|
}
void ShowBatteryEmpty()
{
	SetColor(BLACK);//WHITE
	Bar(125,9,157,21);
				
}
void	ShowBattery(int value, char batt_type)
{
	static int cur_value=0;    //当前电量值
	static int BatteryOFF = 0,cur_value_prev=0;// 当前电池警告次数
	BYTE i=0;

    //ASSERT(value == -1 || (value >= 0 && value <= 15));
    if(value == -1)
    {
        cur_value++;
        if(cur_value > 7) cur_value = 0;
    }
    else
    {
        if(batt_type == BATT_TYPE_ALKALINE)    //碱性电池
        {
            if(value < 62) cur_value = 0;    //batt empty
            else if(value > 116 ) cur_value = 8;    //batt full
            else cur_value = (value - 62) / 7;      // 116 - 62
        }
        else if(batt_type == BATT_TYPE_NIH)    //镍氢电池
        {
            if(value < 69) cur_value = 0;    //batt empty
            else if(value > 116 ) cur_value = 8;    //batt full
            else cur_value = (value - 69) / 6;
        }
        else if(batt_type == BATT_TYPE_LITHIUM)    //锂电池
        {
            if(value < BatteryMin) cur_value = 0;    //batt empty
            else if(value > BatteryMax)cur_value = 8;
/*            else if(0x0B09<value < 0x0B20)cur_value = 7;
            else if(0x0AF2<value < 0x0B09)cur_value = 6;
            else if(0x0ADB<value < 0x0AF2)cur_value = 5;
            else if(0x0AC4<value < 0x0ADB)cur_value = 4;
            else if(0x0AAD<value < 0x0AC4)cur_value = 3;
            else if(0x0A96<value < 0x0AAD)cur_value = 2;
            else if(0x0A7F<value < 0x0A96)cur_value = 1;*/
            else cur_value = (value - BatteryMin) / BatterySize;    //
        }
        else ErrorTrap("FALSE");
    }

//       PutImage(50,cur_value,(void*)&WellLogo,IMAGE_NORMAL);
//	Bar(100, 0, 100+cur_value, 30);
//	SetColor(WHITE);
	if(cur_value<=cur_value_prev)
	switch(cur_value)
		{
			
			case	8:
				ShowBatteryEmpty();
				
				SetColor(BRIGHTGREEN);
				Bar(130,13,132,17);
				
				for(i=0;i<10;i++)
                 {	SetColor(RGB565CONVERT(0,35*i-35*i*i/10+128,0));
	                Bar(132, 10+i, 155, 10+i+1);
                 }
				BatteryOFF =0;
				break;
			case	7:
				ShowBatteryEmpty();
				//Bar(132,10,155,20);
				
				SetColor(BRIGHTGREEN);
				Bar(130,13,132,17);
				
				for(i=0;i<10;i++)
                 {	SetColor(RGB565CONVERT(0,35*i-35*i*i/10+128,0));
	                Bar(132, 10+i, 155, 10+i+1);
                 }				
				BatteryOFF =0;
				break;
			case	6:
				ShowBatteryEmpty();
				Bar(132,10,155,20);
				/*
				SetColor(BRIGHTGREEN);
				Bar(136,10,155,20);
				*/
				for(i=0;i<10;i++)
                 {	SetColor(RGB565CONVERT(0,35*i-35*i*i/10+128,0));
	                Bar(136, 10+i, 155, 10+i+1);
                 }				
				BatteryOFF = 0;
				break;
			case	5:
				ShowBatteryEmpty();
				Bar(132,10,155,20);
				/*
				SetColor(BRIGHTGREEN);
				Bar(140,10,155,20);
				*/
				for(i=0;i<10;i++)
                 {	SetColor(RGB565CONVERT(0,35*i-35*i*i/10+128,0));
	                Bar(140, 10+i, 155, 10+i+1);
                 }				
				BatteryOFF =0;
				break;
			case	4:
				ShowBatteryEmpty();
				Bar(132,10,155,20);
				/*
				SetColor(BRIGHTYELLOW);
				Bar(144,10,155,20);
				*/
				for(i=0;i<10;i++)
                 {	SetColor(RGB565CONVERT(255,35*i-35*i*i/10+128,0));
	                Bar(144, 10+i, 155, 10+i+1);
                 }				
				break;
			case	3:
				ShowBatteryEmpty();
				Bar(132,10,155,20);
				/*
				SetColor(BRIGHTYELLOW);
				Bar(148,10,155,20);
				*/
				for(i=0;i<10;i++)
                 {	SetColor(RGB565CONVERT(255,35*i-35*i*i/10+128,0));
	                Bar(148, 10+i, 155, 10+i+1);
                 }				
				BatteryOFF = 0;
				break;
			case	2:
				ShowBatteryEmpty();
				Bar(132,10,155,20);
				/*
				SetColor(BRIGHTYELLOW);
				Bar(152,10,155,20);
				*/
				for(i=0;i<10;i++)
                 {	SetColor(RGB565CONVERT(255,35*i-35*i*i/10+128,0));
	                Bar(152, 10+i, 155, 10+i+1);
                 }				
				BatteryOFF =0;
				break;
			case	1:
				ShowBatteryEmpty();
				Bar(132,10,155,20);
				SetColor(BRIGHTRED);
				Bar(153,10,155,20);
				BatteryOFF =0;
				break;
			case	0:
				ShowBatteryEmpty();
				BatteryOFF++;
				if(BatteryOFF > 5)
					screenState = CREATE_POWEROFF;
			defalt:
//				ShowBatteryEmpty();
				break;
#if	0
			case	4:
				Bar();
				Line(120,5,145,5);
				Line(120,5,120,8);
				Line(120,8,125,8);
				Line(125,5,125,8);
//				PutImage(80,0,(void*)&Engine1,IMAGE_NORMAL);
				break;
			case	1:
			case	5:
//				PutImage(80,0,(void*)&Engine2,IMAGE_NORMAL);
				break;
			case	2:
			case	6:
//				PutImage(80,0,(void*)&Engine3,IMAGE_NORMAL);
				break;
			case	3:
			case	7:
//				PutImage(80,0,(void*)&Engine4,IMAGE_NORMAL);
				break;
#endif
		}
		cur_value_prev=cur_value;
	SetColor(BLACK);
	Battery();
}


void	CreateSetBackLight()
{
	GOLFree();   // free memory for the objects in the previous linked list and start new list
	CreatePage(AutooffStr);	// CreatePage("Blow Please");
		RbCreate(ID_RADIOBUTTONStar + 0,          	// ID 
              1,(GetMaxY() - 180),50,(GetMaxY() - 150),           // dimension
              RB_DRAW|RB_GROUP,//|	\
              //RB_CHECKED, 				// will be dislayed and checked after creation
                                       	// first button in the group
              OneMinStr, 					// "Left"
              alt4Scheme);              	// use alternate scheme

    RbCreate(ID_RADIOBUTTONStar + 1,          // ID 
              110,(GetMaxY() - 180),170,(GetMaxY() - 150),         // dimension
              RB_DRAW,                 // will be dislayed after creation
              TwoMinStr, 			   // "Center"
              alt4Scheme);              // use alternate scheme

    RbCreate(ID_RADIOBUTTONStar + 2,          // ID 
              2,(GetMaxY() - 140),80,(GetMaxY() - 110),         // dimension
              RB_DRAW,                 // will be dislayed after creation
              FineMinStr, 			   // "Right"
              alt4Scheme);              // use alternate scheme

     RbCreate(ID_RADIOBUTTONStar + 3,          // ID 
              110,(GetMaxY() - 140),170,(GetMaxY() - 110),         // dimension
              RB_DRAW,                 // will be dislayed after creation
              TenMinStr, 			   // "Center"
              alt4Scheme);              // use alternate scheme

}


WORD MsgSetBackLight(WORD objMsg, OBJ_HEADER* pObj){
	OBJ_HEADER* pOtherRbtn;

    switch(GetObjID(pObj)){

        case ID_BUTTON_NEXT:
            if(objMsg == BTN_MSG_RELEASED){
               screenState = CREATE_SETTING;// goto check box demo screen
            }
            return 1; 							// process by default

        case ID_BUTTON_BACK:
            if(objMsg == BTN_MSG_RELEASED){
                screenState = CREATE_SETTING; 		// goto ECG demo screen
            }
            return 1; 							// process by default

	case ID_BUTTON_HOME:
            if(objMsg == BTN_MSG_RELEASED){
                screenState = CREATE_MAIN; 	// goto list box screen
            }
            return 1;

         case ID_BUTTON1:
            if(objMsg == BTN_MSG_RELEASED){
//            	g_standby_time = 1*60;
               screenState = CREATE_MAIN; 		// goto ECG demo screen
            }
            return 1; 							// process by default

        case ID_BUTTON4:

			if(objMsg == BTN_MSG_RELEASED)//BTN_MSG_PRESSED)
			{
//				g_standby_time = 2*60;
				screenState = CREATE_MAIN;

	            }
		    return 1; 							// Do not process by default

        default:
            return 1; 							// process by default
    }
}

void  CreateUnitPDmenu()
{
	WORD textWidth, textHeight, tempStrWidth;
	XCHAR tempStr[] = {'M',0};

    // free memory for the objects in the previous linked list and start new list
	GOLFree();   
	RTCCProcessEvents();				// update the global time and date strings
	
	/* ************************************* */
	/* Create Month, Day and Year Edit Boxes */ 
	/* ************************************* */
	tempStrWidth = GetTextWidth(tempStr,altScheme->pFont);
	textHeight   = GetTextHeight(altScheme->pFont) + (GOL_EMBOSS_SIZE<<1);


	WndCreate(ID_WINDOW1,       		// ID
              0,0,GetMaxX(),GetMaxY(), 	// dimension
              WND_DRAW,                	// will be dislayed after creation
              NULL,//(void*)&WellLogo,               	// icon
              DTSetText,				// set text 
              navScheme);                   	// use default scheme 
              
	/* *************************** */
	// create month components
	/* *************************** */
	// months has three characters, thus we multiply by three
	textWidth = (tempStrWidth*3) +(GOL_EMBOSS_SIZE<<1);

	EbCreate(ID_EB_MONTH,              	// ID
              EB1STARTX,
              EBSTARTY,
              EB1STARTX+textWidth, 
              EBSTARTY+textHeight,
              EB_DRAW, 					// will be dislayed after creation
              NULL,
              3,						// max characters is 3
              altScheme);               // use alternate scheme

    BtnCreate(ID_BUTTON_MO,         	// button ID 
              EB1STARTX+textWidth+1,
              EBSTARTY,
              EB1STARTX+textWidth+SELECTBTNWIDTH+1,
              EBSTARTY+textHeight,  		   	
              0,					   	// draw rectangular button	
              BTN_DRAW,               	// will be dislayed after creation
              NULL,					   	// no bitmap	
              DTDownArrow,             	// DOWN arrow as text
              altScheme);               // use alternate scheme
	updateDateTimeEb();
}

WORD MsgUnitPDment(WORD objMsg, OBJ_HEADER* pObj, GOL_MSG* pMsg)
{
	
LISTBOX*   	pLb;
SLIDER*  	pSld;
OBJ_HEADER* pObjHide;
LISTITEM*	pItemSel;

    pLb = (LISTBOX*)GOLFindObject(ID_LISTBOX4);		
    pSld = (SLIDER*)GOLFindObject(ID_SLIDER4);

    switch(GetObjID(pObj)){

        case ID_LISTBOX4:
        	if (pMsg->uiEvent == EVENT_MOVE) {
	        	pMsg->uiEvent = EVENT_PRESS;			// change event for listbox
	            // Process message by default
	            LbMsgDefault(objMsg, (LISTBOX*)pObj, pMsg);
	            // Set new list box position
	            SldSetPos(pSld,LbGetCount(pLb)-LbGetFocusedItem(pLb)-1);
	            SetState(pSld, SLD_DRAW_THUMB);
	        	pMsg->uiEvent = EVENT_MOVE;				// restore event for listbox
	        }
	       	else if (pMsg->uiEvent == EVENT_PRESS) {
		       	// call the message default processing of the list box to select the item
		       	LbMsgDefault(objMsg, (LISTBOX *)pObj, pMsg);
		    }
	       	else if (pMsg->uiEvent == EVENT_RELEASE) {
		        
		        pObjHide = GOLGetList();  				// hide all the objects in the current list
		        while (pObjHide) {						// set all objects with HIDE state
	            	SetState(pObjHide, HIDE);
	            	pObjHide = pObjHide->pNxtObj;
	            }
				pItemSel = LbGetSel(pLb,NULL);			// get the selected item
				if (pItemSel != NULL) 					// process only when an item was selected
					UpdateRTCCDates(pLb);				// update the RTCC values 
    
//			    screenState = HIDE_DATE_PDMENU;			// go to hide state
	        }

	        // The message was processed
	        return 0;

        case ID_SLIDER4:
            // Process message by default
            SldMsgDefault(objMsg, (SLIDER*)pObj, pMsg);
            // Set new list box position
            if(LbGetFocusedItem(pLb) != LbGetCount(pLb)-SldGetPos(pSld)){
                LbSetFocusedItem(pLb,LbGetCount(pLb)-SldGetPos(pSld));
                SetState(pLb, LB_DRAW_ITEMS);
            }
            // The message was processed
            return 0;

        case ID_BUTTON_UNIT_UP:					// slider button up was pressed
            if(objMsg == BTN_MSG_RELEASED){
                LbSetFocusedItem(pLb,LbGetFocusedItem(pLb)-1);
                SetState(pLb, LB_DRAW_ITEMS);
                SldSetPos(pSld,SldGetPos(pSld)+1);
                SetState(pSld, SLD_DRAW_THUMB);
            }
            return 1;

        case ID_BUTTON_UNIT_DN:					// slider button down was pressed
            if(objMsg == BTN_MSG_RELEASED){
                LbSetFocusedItem(pLb,LbGetFocusedItem(pLb)+1);
                SetState(pLb, LB_DRAW_ITEMS);
                SldSetPos(pSld,SldGetPos(pSld)-1);
                SetState(pSld, SLD_DRAW_THUMB);
            }
            return 1;
    }    
    return 1;
    
}

void ShowUnitPullDownMeny()
{
	WORD 		textWidth=0, textHeight, tempStrWidth;
	LISTBOX*    pLb;
	XCHAR*		pItems = NULL;
	XCHAR 		tempString[] = {'M',0};

	pListSaved = GOLGetList();
	GOLNewList();

	textHeight = GetTextHeight(altScheme->pFont) + (GOL_EMBOSS_SIZE << 1);
	
	// Note: pDwnBottom, pDwnTop, pDwnLeft and pDwnRight are globals that will
	// 		 be used to redraw the area where the pulldown menu covered.
	pDwnBottom = 230;
	pDwnTop    = EBSTARTY;

	tempStrWidth = GetTextWidth(tempString,altScheme->pFont);
	switch (DateItemID) {
		case ID_EB_MONTH:
			textWidth  = (tempStrWidth*3) + (GOL_EMBOSS_SIZE<<1);
			pDwnLeft   = EB1STARTX;
			pDwnRight  = EB1STARTX+textWidth+SELECTBTNWIDTH;
			pItems = (XCHAR*)MonthItems;
			break;
		case ID_EB_DAY:
			textWidth  = (tempStrWidth*2) + (GOL_EMBOSS_SIZE<<1);
			pDwnLeft   = EB2STARTX;
			pDwnRight  = EB2STARTX+textWidth+SELECTBTNWIDTH;
			pItems = (XCHAR*)DayItems;
			break;
		case ID_EB_YEAR:
			textWidth  = (tempStrWidth*4) + (GOL_EMBOSS_SIZE<<1);
			pDwnLeft   = EB3STARTX;
			pDwnRight  = EB3STARTX+textWidth+SELECTBTNWIDTH;
			pItems = (XCHAR*)YearItems;
			break;

		case	ID_EB_UNIT:
			textWidth  = (tempStrWidth*3) + (GOL_EMBOSS_SIZE<<1);
			pDwnLeft   = EB1STARTX;
			pDwnRight  = EB1STARTX+textWidth+SELECTBTNWIDTH;
			pItems = (XCHAR*)TenUnits;
	}

	pLb = (LISTBOX *)LbCreate(
			   ID_LISTBOX1,         	// List Box ID
		       pDwnLeft,				// left
		       pDwnTop+textHeight,		// top
		       pDwnRight-SELECTBTNWIDTH,// right
		       pDwnBottom, 				// bottom dimension
		       LB_DRAW|LB_SINGLE_SEL, 	// draw after creation, single selection
		       pItems,        			// items list 
		       altScheme);
		
	SldCreate(ID_SLIDER1,              	// Slider ID
		      pDwnRight-SELECTBTNWIDTH+1,
		      pDwnTop+(textHeight<<1),  		   	
		      pDwnRight,
		      pDwnBottom-textHeight,
		      SLD_DRAW|SLD_SCROLLBAR|
		      SLD_VERTICAL,   			// vertical, draw after creation
		      LbGetCount(pLb),       	// range
		      5,                       	// page 
		      LbGetCount(pLb)-1,       	// pos
		      altScheme);
		
	BtnCreate(ID_BUTTON_DATE_UP,       	// up button ID 
		      pDwnRight-SELECTBTNWIDTH+1,
		      pDwnTop+textHeight,
		      pDwnRight,
		      pDwnTop+(textHeight<<1),0, 		   	
		      BTN_DRAW,                 // draw after creation
		      NULL,                    	// no bitmap
		      DTUpArrow,              	// text
		      altScheme);
		
	BtnCreate(ID_BUTTON_DATE_DN,        // down button ID 
		      pDwnRight-SELECTBTNWIDTH+1,
		      pDwnBottom-textHeight,
		      pDwnRight,
		      pDwnBottom,0,  		   	
		      BTN_DRAW,                 // draw after creation
		      NULL,                    	// no bitmap
		      DTDownArrow,              // text
		      altScheme);

}

// creates Connect to the pc demo screen
void CreateConnectToPC(){
LISTBOX*    pLb;

    GOLFree();  // free memory for the objects in the previous linked list and start new list

	CreatePage(ConnectToPCStr); 			// CreatePage("List box");

	/*
		StCreate(ID_STATICTEXT3,           		// ID 
              MAINSTARTX+0*(MAINCHARSIZE+MAINSIZEX)-8,MAINSTARTY+2*MAINCHARSIZE+MAINSIZEY-80,
              MAINSTARTX+3*(MAINCHARSIZE+MAINSIZEX)-8,MAINSTARTY+2*(MAINCHARSIZE+MAINSIZEY)-80,          		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              SoftwareID, 				// "GPS", 	// text
              altScheme);    // alt2Scheme              // use alternate scheme
     */
        StCreate(ID_STATICTEXT3,           		// ID 
              MAINSTARTX+0*(MAINCHARSIZE+MAINSIZEX)-8,MAINSTARTY+2*MAINCHARSIZE+MAINSIZEY-40,
              MAINSTARTX+3*(MAINCHARSIZE+MAINSIZEX)-8,MAINSTARTY+2*(MAINCHARSIZE+MAINSIZEY),          		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              PlseConnectToPCStr, 				// "GPS", 	// text
              altScheme);    // alt2Scheme              // use alternate scheme

}

WORD MsgToPC(WORD objMsg, OBJ_HEADER* pObj){
	OBJ_HEADER* pOtherRbtn;

    switch(GetObjID(pObj)){

        case ID_BUTTON_NEXT:
            if(objMsg == BTN_MSG_RELEASED){
               screenState = CREATE_MAIN;// goto check box demo screen
            }
            return 1; 							// process by default

        case ID_BUTTON_BACK:
            if(objMsg == BTN_MSG_RELEASED){
                screenState = CREATE_MAIN; 		// goto ECG demo screen
            }
            return 1; 							// process by default
        case ID_BUTTON_HOME:
            if(objMsg == BTN_MSG_RELEASED){
 //           	abcStatus = 0;
                screenState = CREATE_MAIN; 	// goto list box screen
            }
            return 1;
	default:
            return 1; 							// process by default
    	}
}

// Creates progress bar screen

void CreateResult(void)
{
unsigned int i;
    CopyChar(&Flashbuff[17],sLongitude,15);
    CopyChar(&Flashbuff[32],sLatitude,14);
	XCHAR* UnitPoit;
	GetThickness();
	if(TestMode!=0xcc)
	writerecord();
	GOLFree();   // free memory for the objects in the previous linked list and start new list

	CreatePage(TestResultStr);	// CreatePage("Buttons");

	if(TestMode!=0xcc)
		{
	StCreate(ID_STATICTEXT3,           		// ID 
              positionax*10,positionay+2*positionmaxy,
              positionax*10+4*positionmaxx+20,positionay+6*positionmaxy,         		// dimension
              ST_DRAW,//|ST_RIGHT_ALIGN,//ST_CENTER_ALIGN,		// display text
              TestResultStr, 				// "TEST", 	// text
              alt5Scheme);                   // use alternate scheme	
	if((ST_IRstatus == ST_REFUSE))
	{
		StCreate(ID_STATICTEXT3,           		// ID 
		              positionax+0*positionmaxx,positionay+5*positionmaxy,
		              5*(positionax+positionmaxx),positionay+9*positionmaxy,         		// dimension
		              ST_DRAW|ST_CENTER_ALIGN,		// display text
		              RefuseStr,//GetdataStr, 				// "TEST", 	// text
		              failScheme);
	}
	else if(ST_IRstatus == ST_Discontinued)
		{
		StCreate(ID_STATICTEXT3,           		// ID 
		              positionax+0*positionmaxx,positionay+5*positionmaxy,
		              5*(positionax+positionmaxx),positionay+9*positionmaxy,         		// dimension
		              ST_DRAW|ST_CENTER_ALIGN,		// display text
		              Discontinuedstr,//GetdataStr, 				// "TEST", 	// text
		              failScheme);
	        }
	else
	{

		redScheme->pFont=(void*)&BigArial;
				
		greenScheme->pFont=(void*)&BigArial;
		
	if((Test_Result[0]>'0') |(Test_Result[2]>'1'))//|(Test_Result[2]>='2'))// 超标
		{
	if(PeakScreen ==0xaa)
			StCreate(ID_STATICTEXT3,           		// ID 
		              positionax+0*positionmaxx,positionay+5*positionmaxy,
		              5*(positionax+positionmaxx),positionay+9*positionmaxy,         		// dimension
		              ST_DRAW,//|ST_RIGHT_ALIGN,//ST_CENTER_ALIGN,		// display text
		              Beyond25mgl,//GetdataStr, 				// "TEST", 	// text
		              redScheme);   //   alt2Scheme             // use alternate scheme

     else 
	 	StCreate(ID_STATICTEXT3,           		// ID 
		              positionax+0*positionmaxx,positionay+5*positionmaxy,
		              5*(positionax+positionmaxx),positionay+9*positionmaxy,         		// dimension
		              ST_DRAW,//|ST_RIGHT_ALIGN,//ST_CENTER_ALIGN,		// display text
		              Test_Result,//GetdataStr, 				// "TEST", 	// text
		              redScheme);   //   alt2Scheme             // use alternate scheme
	}
	else
		{

           StCreate(ID_STATICTEXT3,           		// ID 
                     positionax+0*positionmaxx,positionay+5*positionmaxy,
                     5*(positionax+positionmaxx),positionay+9*positionmaxy,         		// dimension
              		 ST_DRAW,//|ST_RIGHT_ALIGN,//ST_CENTER_ALIGN,		// display text
              		 Test_Result,//GetdataStr, 				// "TEST", 	// text
              		 greenScheme);   //   alt2Scheme             // use alternate scheme
	   }


	       StCreate(ID_STATICTEXT3,           		// ID 
                     positionax+4*positionmaxx+10,positionay+5*positionmaxy+10+5,
                     7*(positionax+positionmaxx),positionay+7*positionmaxy+10+5,         		// dimension
                     ST_DRAW,//|ST_RIGHT_ALIGN,//ST_CENTER_ALIGN,		// display text
                     DisplayUnitStr,//GetdataStr, 				// "TEST", 	// text
                     alt5Scheme);   //   alt2Scheme             // use alternate scheme

	}

	pbutton=BtnCreate(ID_BUTTON8,             	// button ID 
              MAINSTARTX+1*(MAINCHARSIZE+MAINSIZEX)+3,GetMaxY()-40,
              MAINSTARTX+2*MAINCHARSIZE+MAINSIZEX-6,GetMaxY(),         	// dimension
              0,					   	// set radius 
              BTN_DRAW,//|BTN_PRESSED,  	// draw a vertical capsule button
              							// that is initially pressed
              (void*)&printerbutton,//(void*)&PicGPS,                    	// no bitmap
              NULL,
              blackScheme);
		}
	else {

	if((ST_IRstatus == ST_REFUSE))
	{
		StCreate(ID_STATICTEXT3,           		// ID 
		              positionax+0*positionmaxx,positionay+5*positionmaxy,
		              5*(positionax+positionmaxx),positionay+9*positionmaxy,         		// dimension
		              ST_DRAW|ST_CENTER_ALIGN,		// display text
		              RefuseStr,//GetdataStr, 				// "TEST", 	// text
		              failScheme);
	}
	else{
		redScheme->pFont=(void*)&FONTDEFAULT;
		greenScheme->pFont=(void*)&FONTDEFAULT;

		 if(alocholdetecet)

           			StCreate(ID_STATICTEXT3,           		// ID 
		              positionax+0*positionmaxx+20,positionay+5*positionmaxy,
		              5*(positionax+positionmaxx)+40,positionay+9*positionmaxy,         		// dimension
		              ST_DRAW|ST_CENTER_ALIGN,		// display text
		              alcoholdetect,//GetdataStr, 				// "TEST", 	// text
		              redScheme);   //   alt2Scheme             // use alternate scheme

		else  			  

                   StCreate(ID_STATICTEXT3,           		// ID 
                     positionax+0*positionmaxx+5,positionay+5*positionmaxy,
                     5*(positionax+positionmaxx)+50,positionay+9*positionmaxy,         		// dimension
              		 ST_DRAW|ST_CENTER_ALIGN,		// display text
              		 NoAlocholstr,//GetdataStr, 				// "TEST", 	// text
              		 greenScheme);   //   alt2Scheme             // use alternate scheme
		}

	}
		
		
	ST_IRstatus = 0;		// 测试模式恢复正常
	PeakScreen =0;
}

WORD MsgResult(WORD objMsg, OBJ_HEADER* pObj){
	OBJ_HEADER* pOtherRbtn;
unsigned char ik;
unsigned int MsgAdd,i;
    switch(GetObjID(pObj)){

        case ID_BUTTON_HOME:
            if(objMsg == BTN_MSG_RELEASED){
				redScheme->pFont=(void*)&BigArial;
		        greenScheme->pFont=(void*)&BigArial;
            	alarmON = FALSE;
				
				ChoicePrint = FALSE;

				SPrint=FALSE;

				Sprinnum = 0;
				Pressflag = 0;
               screenState = CREATE_MAIN;// goto check box demo screen
            }
            return 1; 							// process by default

        case ID_BUTTON_NEXT:
            if(objMsg == BTN_MSG_RELEASED){

				ChoicePrint = FALSE;

				SPrint=FALSE;

				Sprinnum = 0;
				if(TestMode==0xaa)
					{
				      if(Demarcate&&EEPROMReadByte(ID_CHECKBOXADDRESS+ID_CHECKBOX8))
					      screenState = CREATE_CALLOCK;
			          else {	
				          for(ik=0;ik<200;ik++) Flashbuff[49+ik] = 0;
				          TestMode=0xaa;
				          g_backlight_time = 3*60;// 3分钟不吹气，自动跳出来
		                  SaveReadItem(FALSE);
            	          for(MsgAdd =0;MsgAdd<10;MsgAdd++)//ItemMax
            	             {
            		            if(EEPROMReadByte(ID_CHECKBOXADDRESS +MsgAdd))
            		            break;
            	              }
            	           if(MsgAdd<10)//ItemMax
                              screenState = 2*MsgAdd+CREATE_ITEM0;
                           else
                              screenState = CREATE_Alcoholdetect;
				             }
					}
				if(TestMode==0xbb)
					{
                       if(Demarcate&&EEPROMReadByte(ID_CHECKBOXADDRESS+ID_CHECKBOX8))
					        screenState = CREATE_CALLOCK;
			           else {
				              for(ik=0;ik<200;ik++) Flashbuff[49+ik] = 0;
				              TestMode=0xbb;
                              screenState = CREATE_Alcoholdetect;
				              g_backlight_time = 3*60;// 3分钟不吹气，自动跳出来
                             }
            	    }
				if(TestMode==0xcc)
                    { 		
 				       if(Demarcate&&EEPROMReadByte(ID_CHECKBOXADDRESS+ID_CHECKBOX8))
					         screenState = CREATE_CALLOCK;
			           else {
				               TestMode=0xcc;
			                    screenState = CREATE_Waittest; 		// goto ECG demo screen
			                   status_Press=FALSE;		
	                           for(ik=0;ik<200;ik++) Flashbuff[49+ik] = 0;//clear  buffer --add by xiao 090811 
			                   g_backlight_time = 3*60;// 3分钟不吹气，自动跳出来
                            }
        		    }
				redScheme->pFont=(void*)&BigArial;
		        greenScheme->pFont=(void*)&BigArial;
            	alarmON = FALSE;
				Pressflag = 0;

            	}
                  return 1; 							// process by default

       case ID_BUTTON8:
            if(objMsg == BTN_MSG_RELEASED)
            {

		if(PORTEbits.RE8)
           	{
				SPrint=TRUE;
            	ChoicePrint = TRUE;
				Sprinnum = 0;
				PrintSec=0;
				PrintCount = 0;
				//Pressflag = 0xaa;
	       		memset(&Flashbuff[0],0,256);
				//memset(&tx1_buff[0],0,250);
				//memset(&rx1_buff[0],0,250);
				readrecord();
	 			for(i=0;i<=255;i++)
					Printbuff[i]=(unsigned char)Flashbuff[i];
    			Inputdata();
				p_n = 0;
           	}

            	}
            return 1; // process by default
            
	default:
            return 1; 							// process by default
    	}
}

void CreateUnits(){

    GOLFree();  // free memory for the objects in the previous linked list and start new list

	CreatePage(UnitsStr); 	// CreatePage("Static text");

	RbCreate(ID_RADIOBUTTONStar + 7,          	// ID 
              positionax,(GetMaxY() - 270),positionmaxx,(GetMaxY() - 240),            // dimension
              RB_DRAW|RB_GROUP,//|	\
             	//RB_CHECKED, 				// will be dislayed and checked after creation
                                       	// first button in the group
              mg_100mlStr, 					// "Left"
              alt5Scheme);              	// use alternate scheme

    RbCreate(ID_RADIOBUTTONStar + 8,          // ID 
              positionax+4*positionmaxx,(GetMaxY() - 270),positionax+6*positionmaxx,(GetMaxY() - 240),         // dimension
              RB_DRAW,                 // will be dislayed after creation
              mg_lStr, 			   // "Center"
              alt5Scheme);              // use alternate scheme
              
}

void CreateThickness(){

     GOLFree();  // free memory for the objects in the previous linked list and start new list

	CreatePage(ThicknessStr); 	// CreatePage("Static text");
	StCreate(ID_STATICTEXT3,          	// ID 
              positionax,(GetMaxY() - 270),3*positionmaxx,(GetMaxY() - 240),            // dimension
              ST_DRAW|ST_CENTER_ALIGN, 				// will be dislayed and checked after creation
                                       	// first button in the group
              Breath, 					// "Left"
              alt4Scheme);              	// use alternate scheme
   StCreate(ID_STATICTEXT3,          // ID 
              positionax+4*positionmaxx-20,(GetMaxY() - 270),positionax+7*positionmaxx,(GetMaxY() - 240),         // dimension
              ST_DRAW|ST_CENTER_ALIGN,           // will be dislayed after creation
              Blood, 			   // "Center"
              alt4Scheme);               
	RbCreate(ID_RADIOBUTTONStar + 7,          	// ID 
              positionax+4*positionmaxx,(GetMaxY() - 180),positionax+6*positionmaxx,(GetMaxY() - 150),        //positionax,(GetMaxY() - 270),positionmaxx,(GetMaxY() - 240),            // dimension
              RB_DRAW|RB_GROUP,//|	\
             	//RB_CHECKED, 				// will be dislayed and checked after creation
                                       	// first button in the group
              mg_100mlStr, 					// "Left"
              alt4Scheme);              	// use alternate scheme

    RbCreate(ID_RADIOBUTTONStar + 8,          // ID 
              positionax+0*positionmaxx,(GetMaxY() - 240),positionax+2*positionmaxx,(GetMaxY() - 210),        // positionax+4*positionmaxx,(GetMaxY() - 270),positionax+6*positionmaxx,(GetMaxY() - 240),         // dimension
              RB_DRAW,                 // will be dislayed after creation
              mg_lStr, 			   // "Center"
              alt4Scheme);              // use alternate scheme

    RbCreate(ID_RADIOBUTTONStar + 9,          // ID 
              positionax+4*positionmaxx,(GetMaxY() - 210),positionax+6*positionmaxx,(GetMaxY() - 180),       //positionax+0*positionmaxx,(GetMaxY() - 240),positionax+2*positionmaxx,(GetMaxY() - 210),        // dimension
              RB_DRAW,                 // will be dislayed after creation
              mg_100Str, 			   // "Right"
              alt4Scheme);              // use alternate scheme
/*
     RbCreate(ID_RADIOBUTTONStar + 10,          	// ID 
              positionax+4*positionmaxx,(GetMaxY() - 240),positionax+6*positionmaxx,(GetMaxY() - 210),             // dimension
              RB_DRAW,//|RB_GROUP,//|	\
              //RB_CHECKED, 				// will be dislayed and checked after creation
                                       	// first button in the group
              g_100mlStr, 					// "Left"
              alt4Scheme);              	// use alternate scheme
*/
    RbCreate(ID_RADIOBUTTONStar + 11,          // ID 
              positionax+4*positionmaxx,(GetMaxY() - 240),positionax+6*positionmaxx,(GetMaxY() - 210),             //positionax+0*positionmaxx,(GetMaxY() - 210),positionax+2*positionmaxx,(GetMaxY() - 180),        // dimension
              RB_DRAW,                 // will be dislayed after creation
              mg_1000mlStr, 			   // "Center"
              alt4Scheme);              // use alternate scheme

    RbCreate(ID_RADIOBUTTONStar + 12,          // ID 
              positionax+0*positionmaxx,(GetMaxY() - 180),positionax+2*positionmaxx,(GetMaxY() - 150),             //positionax+4*positionmaxx,(GetMaxY() - 210),positionax+6*positionmaxx,(GetMaxY() - 180),       // dimension
              RB_DRAW,                 // will be dislayed after creation
              ug_100mlStr, 			   // "Right"
              alt4Scheme);              // use alternate scheme
/*
	RbCreate(ID_RADIOBUTTONStar + 13,          	// ID 
              positionax+0*positionmaxx,(GetMaxY() - 180),positionax+2*positionmaxx,(GetMaxY() - 150),             // dimension
              RB_DRAW,//|RB_GROUP,//|	\
              //RB_CHECKED, 				// will be dislayed and checked after creation
                                       	// first button in the group
              mg_mlStr, 					// "Left"
              alt4Scheme);              	// use alternate scheme

    RbCreate(ID_RADIOBUTTONStar + 14,          // ID 
              positionax+4*positionmaxx,(GetMaxY() - 180),positionax+6*positionmaxx,(GetMaxY() - 150),        // dimension
              RB_DRAW,                 // will be dislayed after creation
              mg_mg100Str, 			   // "Center"
              alt4Scheme);              // use alternate scheme
*/
	RbCreate(ID_RADIOBUTTONStar + 15,          	// ID 
              positionax+0*positionmaxx,(GetMaxY() - 210),positionax+2*positionmaxx,(GetMaxY() - 180),        // dimension
              RB_DRAW,//|RB_GROUP,//|	\
              //RB_CHECKED, 				// will be dislayed and checked after creation
                                       	// first button in the group
              ug_lStr,//mg_mlStr, 					// "Left"
              alt4Scheme);   


	RbCreate(ID_RADIOBUTTONStar + 26,          	// ID 
              1,(GetMaxY() - 140),50,(GetMaxY() - 110),           // dimension
              RB_DRAW|RB_GROUP,//|	\
              //RB_CHECKED, 				// will be dislayed and checked after creation
                                       	// first button in the group
              K2000StrA,//LeftStr, 					// "Left"
              alt5Scheme);              	// use alternate scheme

    RbCreate(ID_RADIOBUTTONStar + 28,          // ID 
              positionax+3*positionmaxx,(GetMaxY() - 140),
              positionax+5*positionmaxx,(GetMaxY() - 110),           // dimension
              RB_DRAW,                 // will be dislayed after creation
              K2100StrA, 			   // "Center"
              alt5Scheme);              // use alternate scheme

    RbCreate(ID_RADIOBUTTONStar + 29,          // ID 
              positionax+3*positionmaxx,(GetMaxY() - 110),
              positionax+5*positionmaxx,(GetMaxY() - 80),           // dimension
              RB_DRAW,                 // will be dislayed after creation
              K2150StrA, 			   // "Center"
              alt5Scheme);              // use alternate scheme

	RbCreate(ID_RADIOBUTTONStar + 30,          	// ID 
              positionax+0*positionmaxx,(GetMaxY() - 110),
              positionax+3*positionmaxx,(GetMaxY() - 80),           // dimension
              RB_DRAW,//|RB_GROUP,//|	\
              //RB_CHECKED, 				// will be dislayed and checked after creation
                                       	// first button in the group
              K2200StrA, 					// "Left"
              alt5Scheme);              	// use alternate scheme

    RbCreate(ID_RADIOBUTTONStar + 32,          // ID 
             1,(GetMaxY() - 80),50,(GetMaxY() - 50),           // dimension
              RB_DRAW,                 // will be dislayed after creation
              K2300StrA, 			   // "Right"
              alt5Scheme);
              
}

WORD MsgThickness(WORD objMsg, OBJ_HEADER* pObj){
GROUPBOX   *pGb;
STATICTEXT *pSt;
/*
	uch i;
	i = g_standby_time/60;
	switch(i)
		{
			case 0:
				default:
				GetObjID(pObj) = ID_RADIOBUTTON2;
				break;
			case 1:
				GetObjID(pObj) = ID_RADIOBUTTON1;
				break;
			case 2:
				GetObjID(pObj) = ID_RADIOBUTTON3;
				break;
		}
		*/	

    switch(GetObjID(pObj)){
        case ID_RADIOBUTTON1: // change aligment to left
            pGb = (GROUPBOX*) GOLFindObject(ID_GROUPBOX1); 	// get pointer to group box
            ClrState(pGb,GB_RIGHT_ALIGN|GB_CENTER_ALIGN);  	// clear right and center alignment states
            SetState(pGb, GB_DRAW);                        	// set redraw state
            pSt = (STATICTEXT*) GOLFindObject(ID_STATICTEXT3); // get pointer to static text
            ClrState(pSt,ST_RIGHT_ALIGN|ST_CENTER_ALIGN);  	// clear right and center alignment states    
            SetState(pSt, ST_DRAW);                        	// set redraw state
            return 1; 										// process by default

        case ID_RADIOBUTTON2:
            pGb = (GROUPBOX*) GOLFindObject(ID_GROUPBOX1); 	// get pointer to group box
            ClrState(pGb, GB_RIGHT_ALIGN);  				// clear right alignment state
            SetState(pGb, GB_CENTER_ALIGN|GB_DRAW);  		// set center alignment and redraw states
            pSt = (STATICTEXT*) GOLFindObject(ID_STATICTEXT3); // get pointer to static text
            ClrState(pSt,ST_RIGHT_ALIGN);  					// clear right aligment state
            SetState(pSt, ST_DRAW|ST_CENTER_ALIGN);  		// set center alignment and redraw states
            return 1; 										// process by default

        case ID_RADIOBUTTON3:
            pGb = (GROUPBOX*) GOLFindObject(ID_GROUPBOX1); 	// get pointer to group box
            ClrState(pGb, GB_CENTER_ALIGN);  				// clear center alignment state
            SetState(pGb, GB_RIGHT_ALIGN|GB_DRAW);  		// set right alignment and redraw state
            pSt = (STATICTEXT*) GOLFindObject(ID_STATICTEXT3); // get pointer to static text
            ClrState(pSt,ST_CENTER_ALIGN);  				// clear center aligment state
            SetState(pSt,ST_RIGHT_ALIGN|ST_DRAW); 			// set right alignment and redraw states
            return 1; // process by default

        case ID_BUTTON_NEXT:
            if(objMsg == BTN_MSG_RELEASED){
               	if(dif)
					screenState = CREATE_Master;
				else
                    screenState = CREATE_AGENT;				// goto slider screen
            }
            return 1; 										// process by default

        case ID_BUTTON_BACK:
            if(objMsg == BTN_MSG_RELEASED){
                if(dif)
					screenState = CREATE_Master;
				else
                    screenState = CREATE_AGENT; 			// goto radio buttons screen
            }
            return 1; 										// process by default

	case ID_BUTTON_HOME:
            if(objMsg == BTN_MSG_RELEASED){
//            	abcStatus = 0;
                screenState = CREATE_MAIN; 	// goto list box screen
            }
            return 1;

        default:
            return 1;
    }
}


void GetStatus(CHECKBOX* pCb)
{
	BOOL pCbstate = FALSE;
	WORD Address;
	Address = (0x7F00 + pCb->ID - ID_CHECKBOX1 + 1);//得到不同复选筐的地址
	if(pCb->ID == ID_CHECKBOX1)
		Address--;
	if(EEPROMReadByte(Address))//如果返回值非零，则表示选中
		{
			SetState(pCb, CB_CHECKED|CB_DRAW_CHECK); // Set checked and redraw
			pCbstate = TRUE;
		}
	else
		{
			ClrState(pCb, CB_CHECKED);        // Reset check   
            		SetState(pCb, CB_DRAW_CHECK);     // Redraw
            		pCbstate = FALSE;
		}
	EEPROMWriteByte(pCbstate, Address);// 保存状态
		
}

void CreatePrintSetting(){

    OBJ_HEADER* obj;
	
    GOLFree();   // free memory for the objects in the previous linked list and start new list

	CreatePage(PrintSettingStr); 	// CreatePage("Checkboxes");



  StCreate(ID_STATICTEXT3,           		// ID 
              10,90,
              100,125,         		// dimension
              ST_DRAW,//|ST_RIGHT_ALIGN,//ST_CENTER_ALIGN,		// display text
              ZeroStr, 				// "TEST", 	// text
              alt5Scheme);                   // use alternate scheme	
          
    EbCreate(ID_EB_HOUR,              	// ID
              130,
              85,
              160, 
              115,
              EB_DRAW, 					// will be dislayed after creation
              NULL,
              1,						// max characters is 2
              altScheme);               // use alternate scheme

    BtnCreate(ID_BUTTONHR_P,         	// button ID 
              165,
              77,
              185,
              97 ,		   	
              0,					   	// draw rectangular button	
              BTN_DRAW,               	// will be dislayed after creation
              NULL,					   	// no bitmap	
              DTPlusSym,              	// plus symbol as text
              altScheme);               // use alternate scheme

    BtnCreate(ID_BUTTONHR_M,         	// button ID 
              165,
              104,
              185,
              124,  		   	
              0,					   	// draw rectangular button	
              BTN_DRAW,      			// will be dislayed after creation
              NULL,					   	// no bitmap	
              DTMinusSym,               // minus symbols as text
              altScheme);               // use alternate scheme
  StCreate(ID_STATICTEXT3,           		// ID 
              10,160,
              100,195,         		// dimension
              ST_DRAW,//|ST_RIGHT_ALIGN,//ST_CENTER_ALIGN,		// display text
              BeyondZeroStr, 				// "TEST", 	// text
              alt5Scheme);                   // use alternate scheme	

	EbCreate(ID_EB_MINUTE,              // ID
              130,
              155,
              160, 
              185,
              EB_DRAW, 					// will be dislayed after creation
              NULL,
              1,						// max characters is 2
              altScheme);               // use alternate scheme

    BtnCreate(ID_BUTTONMN_P,         	// button ID 
              165,
              147,
              185,
              167,  		   	
              0,					   	// draw rectangular button	
              BTN_DRAW,               	// will be dislayed after creation
              NULL,					   	// no bitmap	
              DTPlusSym,              	// plus symbol as text
              altScheme);               // use alternate scheme

    BtnCreate(ID_BUTTONMN_M,         	// button ID 
              165,
              174,
              185,
              194,  		   	
              0,					   	// draw rectangular button	
              BTN_DRAW,               	// will be dislayed after creation
              NULL,					   	// no bitmap	
              DTMinusSym,               // minus symbols as text
              altScheme);               // use alternate scheme          



	CbCreate(ID_CHECKBOX7,             	// ID 
              positionax,230,positionmaxx,265,         	// dimension
              CB_DRAW,      			// will be dislayed and checked after creation
              AutoPrintStr, 			// "Text Bottom"
              alt5Scheme);              	// alternative GOL scheme 

BnumberStr[0]= EEPROMReadByte(PrintSelectAddress);
UnumberStr[0]= EEPROMReadByte(PrintSelectAddress+1);
if(BnumberStr[0]==0xff||BnumberStr[0]==0)
	BnumberStr[0]=0x30;
EEPROMWriteByte(BnumberStr[0], PrintSelectAddress);
if(UnumberStr[0]==0xff||UnumberStr[0]==0)
	UnumberStr[0]=0x30;
EEPROMWriteByte(UnumberStr[0], PrintSelectAddress+1);

	obj = GOLFindObject(ID_EB_HOUR);
	EbSetText((EDITBOX*)obj, BnumberStr);
	SetState(obj, EB_DRAW);
	
	obj = GOLFindObject(ID_EB_MINUTE);
	EbSetText((EDITBOX*)obj, UnumberStr);
	SetState(obj, EB_DRAW);

}

WORD MsgPrintSetting(WORD objMsg, OBJ_HEADER* pObj){

OBJ_HEADER* obj;
switch(GetObjID(pObj)){


       case ID_BUTTONHR_P:  						// increment hour value
		    if(objMsg == BTN_MSG_PRESSED){
				BnumberStr[0]++;
				if(BnumberStr[0]>=0x35)
					BnumberStr[0]=0x35;
			//	EEPROMWriteByte(BnumberStr[0], PrintSelectAddress);
											
			}
			break;
        case ID_BUTTONHR_M:							// decrement hour value
			if(objMsg == BTN_MSG_PRESSED){
				BnumberStr[0]--;
				if(BnumberStr[0]<=0x2F)
					BnumberStr[0]=0x30;
			//	EEPROMWriteByte(BnumberStr[0], PrintSelectAddress);
			}
			break;
        case ID_BUTTONMN_P:  						// increment minute value
        	if(objMsg == BTN_MSG_PRESSED){
				UnumberStr[0] ++;
				if(UnumberStr[0]>=0x35)
					UnumberStr[0]=0x35;
			//	EEPROMWriteByte(UnumberStr[0], PrintSelectAddress+1);
			}
			break;
        case ID_BUTTONMN_M:							// decrement minute value
        	if(objMsg == BTN_MSG_PRESSED){
				UnumberStr[0] --;
				if(UnumberStr[0]<=0x2F)
					UnumberStr[0]=0x30;
			//	EEPROMWriteByte(UnumberStr[0], PrintSelectAddress+1);
			}
			break;

        case ID_BUTTON_NEXT:
            if(objMsg == BTN_MSG_RELEASED){

				if(Settingposition==0xaa)
                screenState = CREATE_SETTING;				// goto slider screen
                else if(Settingposition==0xbb)
					screenState = CREATE_Master;
            }
            return 1; 										// process by default

        case ID_BUTTON_BACK:
            if(objMsg == BTN_MSG_RELEASED){
				
                if(Settingposition==0xaa)
                screenState = CREATE_SETTING;				// goto slider screen
                else if(Settingposition==0xbb)
					screenState = CREATE_Master;
            }
            return 1; 										// process by default

		case ID_BUTTON_HOME:
            if(objMsg == BTN_MSG_RELEASED){
                screenState = CREATE_MAIN; 	// 
            }
            return 1;

        default:
            return 1;
 

}
/*
BnumberStr[0]= EEPROMReadByte(PrintSelectAddress);
UnumberStr[0]= EEPROMReadByte(PrintSelectAddress+1);
if(BnumberStr[0]==0xff||BnumberStr[0]==0)
	BnumberStr[0]=0x30;
EEPROMWriteByte(BnumberStr[0], PrintSelectAddress);
if(UnumberStr[0]==0xff||UnumberStr[0]==0)
	UnumberStr[0]=0x30;
EEPROMWriteByte(UnumberStr[0], PrintSelectAddress+1);
*/

	obj = GOLFindObject(ID_EB_HOUR);
	EbSetText((EDITBOX*)obj, BnumberStr);
	SetState(obj, EB_DRAW);
	
	obj = GOLFindObject(ID_EB_MINUTE);
	EbSetText((EDITBOX*)obj, UnumberStr);
	SetState(obj, EB_DRAW);
	
	EEPROMWriteByte(BnumberStr[0], PrintSelectAddress);
	EEPROMWriteByte(UnumberStr[0], PrintSelectAddress+1);
}


void CreateCalibrate()
{	// 校准

 GOLFree();   // free memory for the objects in the previous linked list and start new list

	CreatePage(CalibrationPeriodStr); 	// CreatePage("Checkboxes");


	StCreate(ID_STATICTEXT3,           		// ID 
              positionax,50,
              3*positionmaxx,80,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              TestMthStr, 				// "TEST", 	// text
              altScheme);        //  mainScheme         // use alternate scheme

       StCreate(ID_STATICTEXT3,           		// ID 
              positionax+5*positionmaxx,70,
              positionax+7*positionmaxx,100,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              TestNumberStr, 				// "TEST", 	// text
              altScheme);         //  mainScheme        // use alternate scheme

 	RbCreate(ID_RADIOBUTTONStar + 18,          	// ID 
              positionax,(GetMaxY() - 240),positionmaxx,(GetMaxY() - 210),           // dimension
              RB_DRAW|RB_GROUP,//|	\
              //RB_CHECKED, 				// will be dislayed and checked after creation
                                       	// first button in the group
              ThreeMthStr, 					// "Left"
              altScheme);              	// use alternate scheme

    RbCreate(ID_RADIOBUTTONStar + 19,          // ID 
              positionax,(GetMaxY() - 200),positionmaxx,(GetMaxY() - 170),         // dimension
              RB_DRAW,                 // will be dislayed after creation
              SixMthStr, 			   // "Center"
              altScheme);              // use alternate scheme

    RbCreate(ID_RADIOBUTTONStar + 20,          // ID 
              2,(GetMaxY() - 160),80,(GetMaxY() - 130),         // dimension
              RB_DRAW,                 // will be dislayed after creation
              NineMthStr, 			   // "Right"
              altScheme);              // use alternate scheme

 	RbCreate(ID_RADIOBUTTONStar + 21,          	// ID 
              positionax,(GetMaxY() - 120),positionmaxx,(GetMaxY() - 90),           // dimension
              RB_DRAW,//|RB_GROUP,//|	\
              //RB_CHECKED, 				// will be dislayed and checked after creation
                                       	// first button in the group
              ElevenMthStr, 					// "Left"
              altScheme);              	// use alternate scheme

	RbCreate(ID_RADIOBUTTONStar + 22,          // ID 
              positionax+4*positionmaxx,(GetMaxY() - 220),positionax+6*positionmaxx,(GetMaxY() - 190),         // dimension
              RB_DRAW,                 // will be dislayed after creation
              OneNumStr, 			   // "Center"
              altScheme);              // use alternate scheme

    RbCreate(ID_RADIOBUTTONStar + 23,          // ID 
              positionax+4*positionmaxx,(GetMaxY() - 180),positionax+6*positionmaxx,(GetMaxY() - 150),         // dimension
              RB_DRAW,                 // will be dislayed after creation
              ThreeNumStr, 			   // "Center"
              altScheme);              // use alternate scheme

    RbCreate(ID_RADIOBUTTONStar + 24,          // ID 
              positionax+4*positionmaxx,(GetMaxY() - 140),positionax+6*positionmaxx,(GetMaxY() - 110),         // dimension
              RB_DRAW,                 // will be dislayed after creation
              FineNumStr, 			   // "Right"
              altScheme);              // use alternate scheme

 	RbCreate(ID_RADIOBUTTONStar + 25,          	// ID 
              positionax+4*positionmaxx,(GetMaxY() - 100),positionax+6*positionmaxx,(GetMaxY() - 70),           // dimension
              RB_DRAW,//|RB_GROUP,//|	\
              //RB_CHECKED, 				// will be dislayed and checked after creation
                                       	// first button in the group
              SevenNumStr, 					// "Left"
              altScheme);              	// use alternate scheme
	CbCreate(ID_CHECKBOX8,             	// ID 
              positionax+3,235,positionmaxx-2,260,         	// dimension
              CB_DRAW,      			// will be dislayed and checked after creation
              LockStr, 			// "Text Bottom"
              alt5Scheme);              	// alternative GOL scheme              

}

WORD MsgCalibrate(WORD objMsg, OBJ_HEADER* pObj){

switch(GetObjID(pObj)){

        case ID_CHECKBOX1:
        	if (objMsg == CB_MSG_CHECKED) {
				SetState(pGenObj, 
						 BTN_TEXTLEFT| 
						 BTN_DRAW);        			// set align left and redraw button	
			} else {
				ClrState(pGenObj, BTN_TEXTLEFT);	// clear all text alignment	
				SetState(pGenObj, BTN_DRAW);       	// set align left and redraw button	
			}
            return 1; 								// process by default

        case ID_CHECKBOX2:
        	if (objMsg == CB_MSG_CHECKED) {
				SetState(pGenObj, 
						 BTN_TEXTBOTTOM| 
						 BTN_DRAW);        			// set align bottom and redraw button	
			} else {
				ClrState(pGenObj, BTN_TEXTBOTTOM);	// clear all text alignment	
				SetState(pGenObj, BTN_DRAW);       	// set align left and redraw button	
			}
            return 1; 								// process by default

        case ID_BUTTON_NEXT:
            if(objMsg == BTN_MSG_RELEASED){
                screenState = CREATE_Master; 	// goto radio buttons screen
            }
            return 1; 								// process by default

        case ID_BUTTON_BACK:
            if(objMsg == BTN_MSG_RELEASED){
                screenState = CREATE_Master;  		// goto round buttons screen
            }
            return 1; 								// process by default

	case ID_BUTTON_HOME:
            if(objMsg == BTN_MSG_RELEASED){
//            	abcStatus = 0;
                screenState = CREATE_MAIN; 	// goto list box screen
            }
            return 1;
            
        default:
            return 1; 								// process by default
    }
}

void CreateChooseCal()
{	// 校准

 GOLFree();   // free memory for the objects in the previous linked list and start new list

 CreatePage(inspection); 	// CreatePage("Checkboxes");

 BtnCreate(ID_BUTTON1,             	// button ID 
           40,105,(GetMaxX()-40),155,           	// dimension
              0,					   	// set radius 
              BTN_DRAW,//|BTN_TOGGLE,   	// draw a vertical capsule button 
              						   	// that has a toggle behavior
              NULL,                    	// no bitmap
              inspection,//LowStr, 					// "LO",       	// text
              altScheme);           	// use alternate scheme

 CbCreate(ID_CHECKBOX8,             	// ID 
              positionax+3,235,positionmaxx-2,260,         	// dimension
              CB_DRAW,      			// will be dislayed and checked after creation
              LockStr, 			// "Text Bottom"
              alt5Scheme);              	// alternative GOL scheme  		  

}

WORD MsgChooseCal(WORD objMsg, OBJ_HEADER* pObj){


    switch(GetObjID(pObj)){

        case ID_BUTTON_NEXT:
            if(objMsg == BTN_MSG_RELEASED){
              screenState = CREATE_Master;// goto check box demo screen
            }
            return 1; 							// process by default

        case ID_BUTTON_BACK:
            if(objMsg == BTN_MSG_RELEASED){
              screenState = CREATE_Master; 		// goto ECG demo screen
            }
            return 1; 							// process by default

	case ID_BUTTON_HOME:
            if(objMsg == BTN_MSG_RELEASED){

                screenState = CREATE_MAIN; 	// goto list box screen
            }
            return 1;
            
	case  ID_BUTTON1:
		 if(objMsg == BTN_MSG_RELEASED){

			screenState = CREATE_CYC;     	// goto radio buttons screen

            }
            return 1;
    	}
}
CreateCalLock(){

GOLFree();

CreatePage(NULL);//(AdjustStr); 	// CreatePage("Checkboxes");

redScheme->pFont=(void*)&GOLFontDefault;

StCreate(ID_STATICTEXT3,           		// ID 
         35,140,//positionax=2
         200,180,  // positionmaxx=32      		// dimension
         ST_DRAW|ST_CENTER_ALIGN,		// display text
         Calibrationneed, 				// "TEST", 	// text
         redScheme);        //  mainScheme         // use alternate scheme

}
WORD MsgCalLock(WORD objMsg, OBJ_HEADER* pObj){

 
switch(GetObjID(pObj)){

        case ID_BUTTON_NEXT:
            if(objMsg == BTN_MSG_RELEASED){
				redScheme->pFont = (void*)&BigArial;
                screenState = CREATE_MAIN; 	// goto radio buttons screen
            }
            return 1; 								// process by default

        case ID_BUTTON_BACK:
            if(objMsg == BTN_MSG_RELEASED){
				redScheme->pFont = (void*)&BigArial;
                screenState = CREATE_MAIN;  		// goto round buttons screen
            }
            return 1; 								// process by default

        case ID_BUTTON_HOME:
            if(objMsg == BTN_MSG_RELEASED){
				redScheme->pFont = (void*)&BigArial;
                screenState = CREATE_MAIN; 	// goto list box screen
            }
            return 1;


}

}
void CreateDemarcate(){	// 标定

 GOLFree();   // free memory for the objects in the previous linked list and start new list

 mainScheme->pFont = (void*)&GOLMediumFont;

//9
/*       EEPROMWriteWord(0x008f,0x6000);//-2温度
       EEPROMWriteWord(0x0cdd,0x600e);//-2 AD

       EEPROMWriteWord(0x00ad,0x6002);//5 tem
       EEPROMWriteWord(0x0d77,0x6010);//5 AD

       EEPROMWriteWord(0x00cb,0x6004);//15 tem
       EEPROMWriteWord(0x0fb9,0x6012);//15 AD

       EEPROMWriteWord(0x10d9,0x6014);//22 AD
       EEPROMWriteWord(0x10d9,0x6016);//28 AD

       EEPROMWriteWord(0x0105,0x600a);//35 tem
       EEPROMWriteWord(0x11ba,0x6018);//35 AD

       EEPROMWriteWord(0x0124,0x600c);//45 tem
       EEPROMWriteWord(0x10a3,0x601a);//45 AD

       EEPROMWriteWord(0x0320,0x6020);//Mark  cons
       EEPROMWriteWord(0x1104,0x6022);//mark AD 
         */
	CreatePage(AdjustStr); 	// CreatePage("Checkboxes");
//	pump_pwr=1;// 泵开始充电

	StCreate(ID_STATICTEXT3,           		// ID 
              positionax,70,//positionax=2
              2*positionmaxx,100,  // positionmaxx=32      		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              DemarcateAD, 				// "TEST", 	// text
              mainScheme);        //  mainScheme         // use alternate scheme
    StCreate(ID_STATICTEXT3,           		// ID 
              2*(positionax+positionmaxx),70,
              4*positionmaxx,100,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              DemarcateAD1, 				// "TEST", 	// text
              mainScheme);        //  mainScheme         // use alternate scheme


	StCreate(ID_STATICTEXT3,           		// ID 
              positionax,140,
              2*positionmaxx,170,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              DemarcateAD2, 				// "TEST", 	// text
              mainScheme);        //  mainScheme         // use alternate scheme
    StCreate(ID_STATICTEXT3,           		// ID 
              2*(positionax+positionmaxx),140,
              4*positionmaxx,170,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              DemarcateAD3, 				// "TEST", 	// text
              mainScheme);        //  mainScheme         // use alternate scheme

	StCreate(ID_STATICTEXT3,           		// ID 
              positionax,215,
              2*positionmaxx,245,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              DemarcateAD4, 				// "TEST", 	// text
              mainScheme);        //  mainScheme         // use alternate scheme
    StCreate(ID_STATICTEXT3,           		// ID 
              2*(positionax+positionmaxx),215,
              4*positionmaxx,245,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              DemarcateAD5, 				// "TEST", 	// text
              mainScheme);        //  mainScheme         // use alternate scheme

	BtnCreate(ID_BUTTON1,             	// button ID 
              140,60,
              180,100,           	// dimension
              0,					   	// set radius 
              BTN_DRAW,//|BTN_TOGGLE,   	// draw a vertical capsule button 
              						   	// that has a toggle behavior
              NULL,//,(void*)&Picscreen,                    	// no bitmap
              T1Str,//,ScreenStr//LowStr, 					// "LO",       	// text
              alt5Scheme);        // yellowScheme  	// use alternate scheme

    BtnCreate(ID_BUTTON2,             	// button ID 
              140,130,
              180,170,           	// dimension
              0,					   	// set radius 
              BTN_DRAW,//|BTN_TOGGLE,   	// draw a vertical capsule button 
              						   	// that has a toggle behavior
              NULL,//(void*)&Picscreen,                    	// no bitmap
              T2Str,//,ScreenStr//LowStr, 					// "LO",       	// text
              alt5Scheme);        // yellowScheme  	// use alternate scheme

     BtnCreate(ID_BUTTON3,             	// button ID 
              140,205,
              180,245,           	// dimension
              0,					   	// set radius 
              BTN_DRAW,//|BTN_TOGGLE,   	// draw a vertical capsule button 
              						   	// that has a toggle behavior
              NULL,//(void*)&Picscreen,                    	// no bitmap
              T3Str,//,ScreenStr//LowStr, 					// "LO",       	// text
              alt5Scheme);        // yellowScheme  	// use alternate scheme
              
BtnCreate(ID_BUTTON4,             	// button ID 
              200,60,
              235,100,           	// dimension
              0,					   	// set radius 
              BTN_DRAW,//|BTN_TOGGLE,   	// draw a vertical capsule button 
              						   	// that has a toggle behavior
              NULL,//,(void*)&Picscreen,                    	// no bitmap
              Pointr,//,ScreenStr//LowStr, 					// "LO",       	// text
              alt5Scheme);        // yellowScheme  	// use alternate scheme
              
    BtnCreate(ID_BUTTON5,             	// button ID 
              200,130,
              235,170,           	// dimension
              0,					   	// set radius 
              BTN_DRAW,//|BTN_TOGGLE,   	// draw a vertical capsule button 
              						   	// that has a toggle behavior
              NULL,//(void*)&Picscreen,                    	// no bitmap
              Pointr,//,ScreenStr//LowStr, 					// "LO",       	// text
              alt5Scheme);        // yellowScheme  	// use alternate scheme    

     BtnCreate(ID_BUTTON6,             	// button ID 
              200,205,
              235,245,           	// dimension
              0,					   	// set radius 
              BTN_DRAW,//|BTN_TOGGLE,   	// draw a vertical capsule button 
              						   	// that has a toggle behavior
              NULL,//(void*)&Picscreen,                    	// no bitmap
              Pointr,//,ScreenStr//LowStr, 					// "LO",       	// text
              alt5Scheme);        // yellowScheme  	// use alternate scheme			  

}
void CreateDemarcate2(){	// 标定

 GOLFree();   // free memory for the objects in the previous linked list and start new list
 mainScheme->pFont = (void*)&GOLMediumFont;

	CreatePage(AdjustStr); 	// CreatePage("Checkboxes");
//	pump_pwr=1;// 泵开始充电

	StCreate(ID_STATICTEXT3,           		// ID 
              positionax,60,
              2*positionmaxx,90,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              DemarcateAD6, 				// "TEST", 	// text
              mainScheme);        //  mainScheme         // use alternate scheme
    StCreate(ID_STATICTEXT3,           		// ID 
              2*(positionax+positionmaxx),60,
              4*positionmaxx,90,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              DemarcateAD7, 				// "TEST", 	// text
              mainScheme);        //  mainScheme         // use alternate scheme

	StCreate(ID_STATICTEXT3,           		// ID 
              positionax,90,
              2*positionmaxx,120,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              DemarcateAD8, 				// "TEST", 	// text
              mainScheme);        //  mainScheme         // use alternate scheme
    StCreate(ID_STATICTEXT3,           		// ID 
              2*(positionax+positionmaxx),90,
              4*positionmaxx,120,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              DemarcateAD9, 				// "TEST", 	// text
              mainScheme);        //  mainScheme         // use alternate scheme
	StCreate(ID_STATICTEXT3,           		// ID 
              positionax,145,
              2*positionmaxx,175,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              DemarcateAD10, 				// "TEST", 	// text
              mainScheme);        //  mainScheme         // use alternate scheme
    StCreate(ID_STATICTEXT3,           		// ID 
              2*(positionax+positionmaxx),145,
              4*positionmaxx,175,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              DemarcateAD11, 				// "TEST", 	// text
              mainScheme);        //  mainScheme         // use alternate scheme

	StCreate(ID_STATICTEXT3,           		// ID 
              positionax,215,
              2*positionmaxx,245,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              DemarcateAD12, 				// "TEST", 	// text
              mainScheme);        //  mainScheme         // use alternate scheme
    StCreate(ID_STATICTEXT3,           		// ID 
              2*(positionax+positionmaxx),215,
              4*positionmaxx,245,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              DemarcateAD13, 				// "TEST", 	// text
              mainScheme);        //  mainScheme         // use alternate scheme

	
	BtnCreate(ID_BUTTON7,             	// button ID 
              140,135,
              180,175,          	// dimension
              0,					   	// set radius 
              BTN_DRAW,//|BTN_TOGGLE,   	// draw a vertical capsule button 
              						   	// that has a toggle behavior
              NULL,//(void*)&Picscreen,                    	// no bitmap
              T6Str,//,ScreenStr//LowStr, 					// "LO",       	// text
              alt5Scheme);        // yellowScheme  	// use alternate scheme

    BtnCreate(ID_BUTTON8,             	// button ID 
              140,205,
              180,245,          	// dimension
              0,					   	// set radius 
              BTN_DRAW,//|BTN_TOGGLE,   	// draw a vertical capsule button 
              						   	// that has a toggle behavior
              NULL,//(void*)&Picscreen,                    	// no bitmap
              T7Str,//,ScreenStr//LowStr, 					// "LO",       	// text
              alt5Scheme);        // yellowScheme  	// use alternate scheme
    BtnCreate(ID_BUTTON9,             	// button ID 
              140,60,
              180,100,          	// dimension
              0,					   	// set radius 
              BTN_DRAW,//|BTN_TOGGLE,   	// draw a vertical capsule button 
              						   	// that has a toggle behavior
              NULL,//(void*)&Picscreen,                    	// no bitmap
              T8Str,//,ScreenStr//LowStr, 					// "LO",       	// text
              alt5Scheme);        // yellowScheme  	// use alternate scheme          

BtnCreate(ID_BUTTON10,             	// button ID 
              200,135,
              235,175,          	// dimension
              0,					   	// set radius 
              BTN_DRAW,//|BTN_TOGGLE,   	// draw a vertical capsule button 
              						   	// that has a toggle behavior
              NULL,//(void*)&Picscreen,                    	// no bitmap
              Pointr,//,ScreenStr//LowStr, 					// "LO",       	// text
              alt5Scheme);        // yellowScheme  	// use alternate scheme

   BtnCreate(ID_BUTTON11,             	// button ID 
              200,205,
              235,245,          	// dimension
              0,					   	// set radius 
              BTN_DRAW,//|BTN_TOGGLE,   	// draw a vertical capsule button 
              						   	// that has a toggle behavior
              NULL,//(void*)&Picscreen,                    	// no bitmap
              Pointr,//,ScreenStr//LowStr, 					// "LO",       	// text
              alt5Scheme);        // yellowScheme  	// use alternate scheme	

    BtnCreate(ID_BUTTON12,             	// button ID 
              200,60,
              235,100,          	// dimension
              0,					   	// set radius 
              BTN_DRAW,//|BTN_TOGGLE,   	// draw a vertical capsule button 
              						   	// that has a toggle behavior
              NULL,//(void*)&Picscreen,                    	// no bitmap
              Pointr,//,ScreenStr//LowStr, 					// "LO",       	// text
              alt5Scheme);        // yellowScheme  	// use alternate scheme    			  


}

WORD MsgDemarcate(WORD objMsg, OBJ_HEADER* pObj){


int i;

switch(GetObjID(pObj)){

        case ID_BUTTON_NEXT:
            if(objMsg == BTN_MSG_RELEASED){
                screenState = CREATE_DEMARCATE2; 	// goto radio buttons screen
            }
            return 1; 								// process by default

        case ID_BUTTON_BACK:
            if(objMsg == BTN_MSG_RELEASED){
                screenState = CREATE_AGENT;  		// goto round buttons screen
            }
            return 1; 								// process by default
        case ID_BUTTON_HOME:
            if(objMsg == BTN_MSG_RELEASED){
                screenState = CREATE_MAIN; 	// goto list box screen
            }
            return 1;
	case ID_BUTTON1:
            if(objMsg == BTN_MSG_RELEASED){


			   Adj_tem=Adc_Count(7,5);
			   Adj_adc=caiyang();
               
               //DISICNT = 0x0000; 

               // if((Adj_adc>=0x0010)&&(Adj_adc < 0x0fff)){
                delay_ms(10);
                EEPROMWriteWord(Adj_tem,0x6000);//-5 温度
                EEPROMWriteWord(Adj_adc,0x600e);//-5  AD

                Hexshow(DemarcateAD,Adj_tem);
				Hexshow(DemarcateAD1,Adj_adc);
	
                screenState = CREATE_Waitdem;
              //  prepare_and_Charge();
                //	}
            }
            return 1; 							// process by default

        case ID_BUTTON2:
            if(objMsg == BTN_MSG_RELEASED){
                prepare_and_Charge();


				Adj_tem=Adc_Count(7,5);
				Adj_adc=caiyang();

				delay_ms(10);
               // if((Adj_adc>=0x0010)&&(Adj_adc < 0x0fff)){
                EEPROMWriteWord(Adj_tem,0x6002);// 5 tem
                EEPROMWriteWord(Adj_adc,0x6010);//5  AD

				Hexshow(DemarcateAD2,Adj_tem);
				Hexshow(DemarcateAD3,Adj_adc);


                screenState = CREATE_Waitdem;

                
            	}
            return 1; 							// process by default
            
        case ID_BUTTON3:
        	if(objMsg == BTN_MSG_RELEASED){ 		// change text and scheme
                prepare_and_Charge();

				Adj_tem=Adc_Count(7,5);
				Adj_adc=caiyang();

				delay_ms(10);
                //if((Adj_adc>=0x0010)&&(Adj_adc < 0x0fff)){
                EEPROMWriteWord(Adj_tem,0x6004);//15  tem
                EEPROMWriteWord(Adj_adc,0x6012);//15  AD

				Hexshow(DemarcateAD4,Adj_tem);
				Hexshow(DemarcateAD5,Adj_adc);


                screenState = CREATE_Waitdem; 

        		}
            return 1;  							// process by default

	    case ID_BUTTON4:
            if(objMsg == BTN_MSG_RELEASED){
				ADLIST=1;
                screenState = CREATE_EDITDEMARCATE1;  		// goto round buttons screen
            }
            return 1;
			
	    case ID_BUTTON5:
            if(objMsg == BTN_MSG_RELEASED){
				ADLIST=2;
                screenState = CREATE_EDITDEMARCATE2;  		// goto round buttons screen
            }
            return 1;	

	    case ID_BUTTON6:
            if(objMsg == BTN_MSG_RELEASED){
				ADLIST=3;
                screenState = CREATE_EDITDEMARCATE3;  		// goto round buttons screen
            }
            return 1;			

        default:
            return 1; 								// process by default
    }
}
WORD MsgDemarcate2(WORD objMsg, OBJ_HEADER* pObj){


int i;

switch(GetObjID(pObj)){

        case ID_BUTTON_NEXT:
            if(objMsg == BTN_MSG_RELEASED){
                screenState = CREATE_AGENT; 	// goto radio buttons screen
            }
            return 1; 								// process by default

        case ID_BUTTON_BACK:
            if(objMsg == BTN_MSG_RELEASED){
                screenState = CREATE_DEMARCATE;  		// goto round buttons screen
            }
            return 1; 								// process by default
        case ID_BUTTON_HOME:
            if(objMsg == BTN_MSG_RELEASED){
                screenState = CREATE_MAIN; 	// goto list box screen
            }
            return 1;


        case ID_BUTTON7:
            if(objMsg == BTN_MSG_RELEASED){


                prepare_and_Charge();


                Adj_tem=Adc_Count(7,5);
				Adj_adc=caiyang();


				delay_ms(10);
                //if((Adj_adc>=0x0010)&&(Adj_adc < 0x0fff)){
                EEPROMWriteWord(Adj_tem,0x600a);//35  tem
                EEPROMWriteWord(Adj_adc,0x6018);//35   AD

                Hexshow(DemarcateAD10,Adj_tem);
				Hexshow(DemarcateAD11,Adj_adc);

				
               screenState = CREATE_DEMARCATE2;
			  // prepare_and_Charge();
           // }
            	}
            return 1; 							// process by default

	case ID_BUTTON8:
            if(objMsg == BTN_MSG_RELEASED){

                prepare_and_Charge();

                Adj_tem=Adc_Count(7,5);
				Adj_adc=caiyang();


				delay_ms(10);
               // if((Adj_adc>=0x0010)&&(Adj_adc < 0x0fff)){
                EEPROMWriteWord(Adj_tem,0x600c);//45  tem
                EEPROMWriteWord(Adj_adc,0x601a);//45   AD

				Hexshow(DemarcateAD12,Adj_tem);
				Hexshow(DemarcateAD13,Adj_adc);


               screenState = CREATE_DEMARCATE2;
			  // prepare_and_Charge();
            //}
            	}
            return 1;
   case ID_BUTTON9:
            if(objMsg == BTN_MSG_RELEASED){

                prepare_and_Charge();

                //Adj_tem=ADC(7);
				Adj_adc=caiyang();


				delay_ms(10);
               // if((Adj_adc>=0x0010)&&(Adj_adc < 0x0fff)){
                EEPROMWriteWord(Adj_adc,0x6016);
                EEPROMWriteWord(Adj_adc,0x6014);
                EEPROMWriteWord(0x00df,0x6006);
                EEPROMWriteWord(0x00f3,0x6008);


                Hexshow(DemarcateAD7,Adj_tem);
				Hexshow(DemarcateAD9,Adj_adc);

               screenState = CREATE_DEMARCATE2;
			  // prepare_and_Charge();
            //}
            	}
            return 1;			
	    case ID_BUTTON10:
            if(objMsg == BTN_MSG_RELEASED){
				ADLIST=5;
                screenState = CREATE_EDITDEMARCATE4;  		// goto round buttons screen
            }
            return 1;
			
	    case ID_BUTTON11:
            if(objMsg == BTN_MSG_RELEASED){
				ADLIST=6;
                screenState = CREATE_EDITDEMARCATE5;  		// goto round buttons screen
            }
            return 1;	

	    case ID_BUTTON12:
            if(objMsg == BTN_MSG_RELEASED){
				ADLIST=4;
                screenState = CREATE_EDITDEMARCATE6;  		// goto round buttons screen
            }
            return 1;
        default:
            return 1; 								// process by default
    }
}

void CreateChoosegas(){	// 标定

 GOLFree();   // free memory for the objects in the previous linked list and start new list

 CreatePage(W_Gaschoosestr); 	// CreatePage("Checkboxes");

  BtnCreate(ID_BUTTON1, 				// button ID 
              60,60,
              180,130,         	// dimension
              0,
              BTN_DRAW,//|BTN_DISABLED,//BTN_HIDE, 				// will be dislayed after creation 
              NULL,//(void*)&Picrecord,          // use bitmap
              DryGasstr,//HomeStr, 					// "HOME", 	    // text
              alt4Scheme);	//alt4Scheme            // alternative GOL scheme 



    BtnCreate(ID_BUTTON2,             	// button ID 
              60,190,
              180,260,         	// dimension
              0,					   	// set radius 
              BTN_DRAW,//|BTN_DISABLED,//|BTN_PRESSED,  	// draw a vertical capsule button
              							// that is initially pressed
              NULL,//(void*)&PicGPS,                    	// no bitmap
              WetGasstr,//DemarWendStr,//ThicknessStr, //NULL,//OffStr, 					// "OFF",      	// text
              alt4Scheme);            	// use alternate scheme
}
WORD MsgChoosegas(WORD objMsg, OBJ_HEADER* pObj){

	    switch(GetObjID(pObj)){

        case ID_BUTTON_NEXT:
            if(objMsg == BTN_MSG_RELEASED){
                if(Settingposition==0xaa)
                screenState = CREATE_SETTING;				// 
                else if(Settingposition==0xbb)
					screenState = CREATE_Master;
            }
            return 1; 							// process by default

        case ID_BUTTON_BACK:
            if(objMsg == BTN_MSG_RELEASED){
                if(Settingposition==0xaa)
                screenState = CREATE_SETTING;				// 
                else if(Settingposition==0xbb)
					screenState = CREATE_Master;
            }
            return 1; 							// process by default

	case ID_BUTTON_HOME:
            if(objMsg == BTN_MSG_RELEASED){
                screenState = CREATE_MAIN; 	// goto list box screen
            }
            return 1;



        case ID_BUTTON1:
            if(objMsg == BTN_MSG_RELEASED){
				
				//EEPROMWriteByte(Drygas,DryorWetAddress);
				DryorWet = Drygas;
				
                screenState = CREATE_Gascal; 		// goto ECG demo screen
            }
            return 1; 							// process by default



        case ID_BUTTON2:
        
			if(objMsg == BTN_MSG_PRESSED) {

				//EEPROMWriteByte(Wetgas,DryorWetAddress);

                DryorWet = Wetgas;
				
				screenState = CREATE_Gascal;
			}
		    return 0; 							// Do not process by default
	    	}
}

void CreategasCal(){	// 标定

 GOLFree();   // free memory for the objects in the previous linked list and start new list

if(DryorWet == Drygas)
{
 CreatePage(DryGasstr); 	// CreatePage("Checkboxes");
 

  BtnCreate(ID_BUTTON1, 				// button ID 
              60,60,
              180,130,         	// dimension
              0,
              BTN_DRAW,//|BTN_DISABLED,//BTN_HIDE, 				// will be dislayed after creation 
              NULL,//(void*)&Picrecord,          // use bitmap
              Dryprodure,//HomeStr, 					// "HOME", 	    // text
              alt4Scheme);	//alt4Scheme            // alternative GOL scheme 
		  
}
else if(DryorWet == Wetgas)

{

 CreatePage(WetGasstr); 	// CreatePage("Checkboxes");
 

  BtnCreate(ID_BUTTON1, 				// button ID 
              60,60,
              180,130,         	// dimension
              0,
              BTN_DRAW,//|BTN_DISABLED,//BTN_HIDE, 				// will be dislayed after creation 
              NULL,//(void*)&Picrecord,          // use bitmap
              Dryprodure,//HomeStr, 					// "HOME", 	    // text
              alt4Scheme);	//alt4Scheme            // alternative GOL scheme 


}

BtnCreate(ID_BUTTON2,				  // button ID 
			60,190,
			180,260,		  // dimension
			0,						  // set radius 
			BTN_DRAW,//|BTN_DISABLED,//|BTN_PRESSED,	  // draw a vertical capsule button
									  // that is initially pressed
			NULL,//(void*)&PicGPS,						  // no bitmap
			Drycheck,//DemarWendStr,//ThicknessStr, //NULL,//OffStr,				  // "OFF", 	  // text
			alt4Scheme);			  // use alternate scheme 


}

WORD MsggasCal(WORD objMsg, OBJ_HEADER* pObj){


	    switch(GetObjID(pObj)){

        case ID_BUTTON_NEXT:
            if(objMsg == BTN_MSG_RELEASED){
               screenState = CREATE_Choosegas;// goto check box demo screen
            }
            return 1; 							// process by default

        case ID_BUTTON_BACK:
            if(objMsg == BTN_MSG_RELEASED){
                screenState = CREATE_Choosegas; 		// goto ECG demo screen
            }
            return 1; 							// process by default

	case ID_BUTTON_HOME:
            if(objMsg == BTN_MSG_RELEASED){
                screenState = CREATE_MAIN; 	// goto list box screen
            }
            return 1;



        case ID_BUTTON1:
            if(objMsg == BTN_MSG_RELEASED){
				
				screenState = CREATE_INPUT;
            }
            return 1; 							// process by default



        case ID_BUTTON2:
        
			if(objMsg == BTN_MSG_PRESSED) {

			   TestMode = 0xdd;
			   Settingposition=0xbb;
               screenState = CREATE_Datadeal;
				

			}
		    return 1; 							// Do not process by default
	    	}
	
}
	
void CreateCalTestresult(void)
{
	GOLFree();   // free memory for the objects in the previous linked list and start new list
	GetThickness();
	//CreatePage(DryCalibrationTest);
	
	if(Settingposition==0xaa)
	CreatePage(DryCalibrationTest);
	else if(Settingposition==0xbb)
		{
         if( Drygas == DryorWet)
            CreatePage(DryCalibrationTest);
		 else CreatePage(WetCalibrationTest);
		 

	}
		
    alarmON = FALSE;
	StCreate(ID_STATICTEXT3,           		// ID 
              positionax*10,positionay+2*positionmaxy,
              positionax*10+4*positionmaxx+20,positionay+6*positionmaxy,         		// dimension
              ST_DRAW,//|ST_RIGHT_ALIGN,//ST_CENTER_ALIGN,		// display text
              TestResultStr, 				// "TEST", 	// text
              alt5Scheme);                   // use alternate scheme

    StCreate(ID_STATICTEXT3,           		// ID 
                     60,positionay+5*positionmaxy+10+5,
                     5*(positionax+positionmaxx),positionay+7*positionmaxy+10+5,         		// dimension
              		 ST_DRAW,//|ST_RIGHT_ALIGN,//ST_CENTER_ALIGN,		// display text
              		 Test_Result,//GetdataStr, 				// "TEST", 	// text
              		 alt5Scheme);   //   alt2Scheme             // use alternate scheme
			  
	StCreate(ID_STATICTEXT3,           		// ID 
                     positionax+4*positionmaxx+10,positionay+5*positionmaxy+10+5,
                     7*(positionax+positionmaxx),positionay+7*positionmaxy+10+5,         		// dimension
                     ST_DRAW,//|ST_RIGHT_ALIGN,//ST_CENTER_ALIGN,		// display text
                     DisplayUnitStr,//GetdataStr, 				// "TEST", 	// text
                     alt5Scheme);   //   alt2Scheme             // use alternate scheme

}

WORD MsgCalTestresult(WORD objMsg, OBJ_HEADER* pObj){

	    switch(GetObjID(pObj)){

        case ID_BUTTON_NEXT:
            if(objMsg == BTN_MSG_RELEASED){
				alarmON = FALSE;
		      if(Settingposition==0xaa)
                screenState = CREATE_SETTING;				
                else if(Settingposition==0xbb)		
               screenState = CREATE_Gascal;// 
            }
            return 1; 							// process by default

        case ID_BUTTON_BACK:
            if(objMsg == BTN_MSG_RELEASED){
				alarmON = FALSE;
		      if(Settingposition==0xaa)
                screenState = CREATE_SETTING;				
                else if(Settingposition==0xbb)		
               screenState = CREATE_Gascal;// 
            }
            return 1; 							// process by default

	case ID_BUTTON_HOME:
            if(objMsg == BTN_MSG_RELEASED){
				alarmON = FALSE;
                screenState = CREATE_MAIN; 	// goto list box screen
            }
            return 1;
	    	}
	
}
void CreateMark(){	// 标定

 GOLFree();   // free memory for the objects in the previous linked list and start new list
	mainScheme->pFont = (void*)&GOLMediumFont;
if(DryorWet == Drygas)
	CreatePage(Calibrate_Dry); 	// CreatePage("Checkboxes");
else if(DryorWet == Wetgas)
	CreatePage(Calibrate_Wet); 	
//	pump_pwr=1;// 泵充电

	StCreate(ID_STATICTEXT3,            
              positionax-2,60,
              3*positionmaxx,80,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              MarkconsStr, 				// "TEST", 	// text
              mainScheme);        //alt5Scheme  mainScheme         // use alternate scheme
	StCreate(ID_STATICTEXT3,           		// ID 
              3*positionmaxx,60,
              5*positionmaxx-10,80,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              consbuffStr, 				// "TEST", 	// text
              mainScheme);
	StCreate(ID_STATICTEXT3,           		// ID 
              5*positionmaxx,60,
              8*positionmaxx-3,80,         		// dimension
              ST_DRAW,		// display text
              mg_lStr, 				// "TEST", 	// text
              mainScheme);	//alt5Scheme

	StCreate(ID_STATICTEXT3,           		// ID 
              positionax,100,
              3*positionmaxx+10,120,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              MarktemStr, 				// "TEST", 	// text
              mainScheme); // alt5Scheme          // use alternate scheme
    StCreate(ID_STATICTEXT3,           		// ID 
              3*positionmaxx+15,100,
              5*positionmaxx+5,120,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              Marktem,
              mainScheme);    // "TEST", 	// text
 
	StCreate(ID_STATICTEXT3,           		// ID 
              positionax-2,140,
              3*positionmaxx-17,160,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              MarkvaluStr, 				// "TEST", 	// text
              mainScheme);        // alt5Scheme mainScheme         // use alternate scheme
              
             
	StCreate(ID_STATICTEXT3,           		// ID 
              3*positionmaxx,140,
              6*positionmaxx-10,160,         		// dimension
              ST_DRAW,		// display text
              MarkAD2, 				// "TEST", 	// text
              mainScheme);


	BtnCreate(ID_BUTTON1,             	// button ID 
              2*(positionax+positionmaxx),200,
              5*(positionax+positionmaxx),240,           	// dimension
              0,					   	// set radius 
              BTN_DRAW,//|BTN_TOGGLE,   	// draw a vertical capsule button 
              NULL,//,(void*)&Picscreen,                    	// no bitmap
              DemarcateStr,//,ScreenStr//LowStr, 					// "LO",       	// text
              mainScheme);        // yellowScheme  	// use alternate scheme
    BtnCreate(ID_BUTTON2,             	// button ID 
              185,140,
              230,160,           	// dimension
              0,					   	// set radius 
              BTN_DRAW,//|BTN_TOGGLE,   	// draw a vertical capsule button 
              NULL,//,(void*)&Picscreen,                    	// no bitmap
              EDITMarkstr,//,ScreenStr//LowStr, 					// "LO",       	// text
              mainScheme);        // yellowScheme  	// use alternate scheme
 /*             
    BtnCreate(ID_BUTTON2,             	// button ID 
              5*(positionax+positionmaxx),positionay+2*positionsizey+positionax,
              6*(positionax+positionmaxx),positionay+3*positionsizey,           	// dimension
              0,					   	// set radius 
              BTN_DRAW|BTN_DISABLED,//|BTN_TOGGLE,   	// draw a vertical capsule button 
              						   	// that has a toggle behavior
              NULL,//(void*)&Picscreen,                    	// no bitmap
              Mark2Str,//,ScreenStr//LowStr, 					// "LO",       	// text
              mainScheme);        // yellowScheme  	// use alternate scheme
    BtnCreate(ID_BUTTON3,             	// button ID 
              5*(positionax+positionmaxx),positionay+4*positionsizey+positionax,
              6*(positionax+positionmaxx),positionay+5*positionsizey,          	// dimension
              0,					   	// set radius 
              BTN_DRAW|BTN_DISABLED,//|BTN_TOGGLE,   	// draw a vertical capsule button 
              						   	// that has a toggle behavior
              NULL,//(void*)&Picscreen,                    	// no bitmap
              Mark3Str,//,ScreenStr//LowStr, 					// "LO",       	// text
              mainScheme);        // yellowScheme  	// use alternate scheme  
    BtnCreate(ID_BUTTON4,             	// button ID 
              5*(positionax+positionmaxx),positionay+6*positionsizey+positionax,
              6*(positionax+positionmaxx),positionay+7*positionsizey+positionax,          	// dimension
              0,					   	// set radius 
              BTN_DRAW,//|BTN_TOGGLE,   	// draw a vertical capsule button 
              						   	// that has a toggle behavior
              NULL,//(void*)&Picscreen,                    	// no bitmap
              Mark4Str,//,ScreenStr//LowStr, 					// "LO",       	// text
              mainScheme);        // yellowScheme  	// use alternate scheme           

*/
}



	
WORD MsgMark(WORD objMsg, OBJ_HEADER* pObj){


unsigned char Data_Adjust;
unsigned long Data_Multi;
	
switch(GetObjID(pObj)){

        case ID_BUTTON_NEXT:
            if(objMsg == BTN_MSG_RELEASED){
            //   if(Settingposition==0xaa)
          //      screenState = CREATE_SETTING;				// goto slider screen
            //    else if(Settingposition==0xbb)
					screenState = CREATE_Gascal;
            }
            return 1; 								// process by default

        case ID_BUTTON_BACK:
            if(objMsg == BTN_MSG_RELEASED){
                screenState = CREATE_INPUT;  		// goto round buttons screen
            }
            return 1; 								// process by default

        case ID_BUTTON_HOME:
            if(objMsg == BTN_MSG_RELEASED){
//            	abcStatus = 0;
                screenState = CREATE_MAIN; 	// goto list box screen
            }
            return 1;

	case ID_BUTTON1:
            if(objMsg == BTN_MSG_RELEASED){
        
            MarkAdc=caiyang();//80001000;//

            //MarkTem=Adc_Count(7,5);

			if( Drygas == DryorWet)
				if(0 != EEPROMReadByte(DrydataAddress))
				{
				
					Data_Adjust= EEPROMReadByte(DrydataAddress);

					Data_Multi = (unsigned long)MarkAdc;

					   
				       Data_Multi = Data_Multi+Data_Multi*Data_Adjust/100;
					   
				     MarkAdc = (unsigned int)Data_Multi;
			}
            Marktemporary=MarkAdc;
            EEPROMWriteWord(MarkAdc,0x6022);
            EEPROMWriteWord(0, TestTimesDataAddress);
            Hexshow(MarkAD2,MarkAdc);
			EEPROMWriteByte(DryorWet,DryorWetAddress);
			SaveAdjust();
            screenState = CREATE_Mark;
              
            }

           return 1; 
		   
        case ID_BUTTON2:
            if(objMsg == BTN_MSG_RELEASED){

                screenState = CREATE_EDITMARK; 	// goto list box screen
            }
            return 1;		   
	}

}			

void Createcaladjust(void)
{

    OBJ_HEADER* obj;
	
	
	GOLFree();   

	CreatePage(caladjustWINstr); 	// CreatePage("Checkboxes");



  StCreate(ID_STATICTEXT3,           		// ID 
              10,90,
              100,125,         		// dimension
              ST_DRAW,//|ST_RIGHT_ALIGN,//ST_CENTER_ALIGN,		// display text
              DryGasstr, 				// "TEST", 	// text
              alt5Scheme);                   // use alternate scheme	
          
    EbCreate(ID_EB_DryGas,              	// ID
              100,
              85,
              160, 
              115,
              EB_DRAW, 					// will be dislayed after creation
              NULL,
              3,						// max characters is 2
              altScheme);               // use alternate scheme

    BtnCreate(ID_BUTTONHR_P,         	// button ID 
              165,
              77,
              185,
              97 ,		   	
              0,					   	// draw rectangular button	
              BTN_DRAW,               	// will be dislayed after creation
              NULL,					   	// no bitmap	
              DTPlusSym,              	// plus symbol as text
              altScheme);               // use alternate scheme

    BtnCreate(ID_BUTTONHR_M,         	// button ID 
              165,
              104,
              185,
              124,  		   	
              0,					   	// draw rectangular button	
              BTN_DRAW,      			// will be dislayed after creation
              NULL,					   	// no bitmap	
              DTMinusSym,               // minus symbols as text
              altScheme);               // use alternate scheme
              /*
  StCreate(ID_STATICTEXT3,           		// ID 
              10,160,
              100,195,         		// dimension
              ST_DRAW,//|ST_RIGHT_ALIGN,//ST_CENTER_ALIGN,		// display text
              WetGasstr, 				// "TEST", 	// text
              alt5Scheme);                   // use alternate scheme	

	EbCreate(ID_EB_WetGas,              // ID
              100,
              155,
              160, 
              185,
              EB_DRAW, 					// will be dislayed after creation
              NULL,
              4,						// max characters is 2
              altScheme);               // use alternate scheme

    BtnCreate(ID_BUTTONMN_P,         	// button ID 
              165,
              147,
              185,
              167,  		   	
              0,					   	// draw rectangular button	
              BTN_DRAW,               	// will be dislayed after creation
              NULL,					   	// no bitmap	
              DTPlusSym,              	// plus symbol as text
              altScheme);               // use alternate scheme

    BtnCreate(ID_BUTTONMN_M,         	// button ID 
              165,
              174,
              185,
              194,  		   	
              0,					   	// draw rectangular button	
              BTN_DRAW,               	// will be dislayed after creation
              NULL,					   	// no bitmap	
              DTMinusSym,               // minus symbols as text
              altScheme);               // use alternate scheme          

*/

/***********Dry***********************/
//savetemp = EEPROMReadByte(DrysignAddress);
//DrygasadustStr[0] = (XCHAR)savetemp;
Drygas_D = EEPROMReadByte(DrydataAddress);

//if((DrygasadustStr[0]!='+')&&(DrygasadustStr[0]!='-'))
	//DrygasadustStr[0]='+';
//savetemp = (unsigned char)DrygasadustStr[0];
//EEPROMWriteByte(savetemp, DrysignAddress);

if(Drygas_D>20)	
{
	Drygas_D=0;
EEPROMWriteByte(Drygas_D, DrydataAddress);
}
DrygasadustStr[0] = 0x0030+Drygas_D/10;
DrygasadustStr[1] = 0x0030+Drygas_D%10;

/***********Dry***********************/

/************Wet**********************/
/*
savetemp = EEPROMReadByte(WetsignAddress);
WetgasadustStr[0] = (XCHAR)savetemp;
Wetgas_d = EEPROMReadByte(WetdataAddress);

if((WetgasadustStr[0]!='+')&&(WetgasadustStr[0]!='-'))
	WetgasadustStr[0]='+';
savetemp = (unsigned char)WetgasadustStr[0];
EEPROMWriteByte(savetemp, WetsignAddress);

if(Wetgas_d>20)
	Wetgas_d=0;
EEPROMWriteByte(Wetgas_d, WetdataAddress);

WetgasadustStr[1] = 0x0030+Wetgas_d/10;
WetgasadustStr[2] = 0x0030+Wetgas_d%10;
*/
/************Wet**********************/


	obj = GOLFindObject(ID_EB_DryGas);
	EbSetText((EDITBOX*)obj, DrygasadustStr);
	SetState(obj, EB_DRAW);
/*	
	obj = GOLFindObject(ID_EB_WetGas);
	EbSetText((EDITBOX*)obj, WetgasadustStr);
	SetState(obj, EB_DRAW);
*/
}	

WORD Msgcaladjust(WORD objMsg, OBJ_HEADER* pObj){

OBJ_HEADER* obj;


switch(GetObjID(pObj)){


       case ID_BUTTONHR_P:  						// increment hour value
		    if(objMsg == BTN_MSG_PRESSED){

                	 if((Drygas_D<20)&&(Drygas_D>=0))
					    Drygas_D++;
			else 	Drygas_D = 20;	 
					
			}
			break;
        case ID_BUTTONHR_M:							// decrement hour value
			if(objMsg == BTN_MSG_PRESSED){
				
                	 if((Drygas_D<=20)&&(Drygas_D>0))
					    Drygas_D--;

				else   Drygas_D=0;	 
			}
			break;
			/*
        case ID_BUTTONMN_P:  						// increment minute value
        	if(objMsg == BTN_MSG_PRESSED){
                if(WetgasadustStr[0] == '+')
                	{
                	 if((Wetgas_d<20)&&(Wetgas_d>=0))
					    Wetgas_d++;
                	}
				else if(WetgasadustStr[0] == '-')
					{

                       if((Wetgas_d<=20)&&(Wetgas_d>0))
					       Wetgas_d--;
					   if(Wetgas_d==0)
					   	WetgasadustStr[0] = '+';
				}
			}
			break;
        case ID_BUTTONMN_M:							// decrement minute value
        	if(objMsg == BTN_MSG_PRESSED){
                if(WetgasadustStr[0] == '+')
                	{
                	 if((Wetgas_d<=20)&&(Wetgas_d>0))
					    Wetgas_d--;
					  if(Wetgas_d==0)
					   	WetgasadustStr[0] = '-'; 
                	}
				else if(WetgasadustStr[0] == '-')
					{

                       if((Wetgas_d<20)&&(Wetgas_d>=0))
					       Wetgas_d++;
				   }
			}
			break;
*/
        case ID_BUTTON_NEXT:
            if(objMsg == BTN_MSG_RELEASED){

				//    savetemp = (unsigned char)DrygasadustStr[0];
                  //  EEPROMWriteByte(savetemp, DrysignAddress);
	                EEPROMWriteByte(Drygas_D, DrydataAddress);
	/*
                    savetemp = (unsigned char)WetgasadustStr[0];
                    EEPROMWriteByte(savetemp, WetsignAddress);
	                EEPROMWriteByte(Wetgas_d, WetdataAddress);
*/
					screenState = CREATE_AGENT;
            }
            return 1; 										// process by default

        case ID_BUTTON_BACK:
            if(objMsg == BTN_MSG_RELEASED){

				  //  savetemp = (unsigned char)DrygasadustStr[0];
                  //  EEPROMWriteByte(savetemp, DrysignAddress);
	                EEPROMWriteByte(Drygas_D, DrydataAddress);
	/*
                    savetemp = (unsigned char)WetgasadustStr[0];
                    EEPROMWriteByte(savetemp, WetsignAddress);
	                EEPROMWriteByte(Wetgas_d, WetdataAddress);				
	*/			
					screenState = CREATE_AGENT;
            }
            return 1; 										// process by default

		case ID_BUTTON_HOME:
            if(objMsg == BTN_MSG_RELEASED){

				   // savetemp = (unsigned char)DrygasadustStr[0];
                  //  EEPROMWriteByte(savetemp, DrysignAddress);
	                EEPROMWriteByte(Drygas_D, DrydataAddress);
/*	
                    savetemp = (unsigned char)WetgasadustStr[0];
                    EEPROMWriteByte(savetemp, WetsignAddress);
	                EEPROMWriteByte(Wetgas_d, WetdataAddress);
	*/				
                screenState = CREATE_MAIN; 	// 
            }
            return 1;

        default:
            return 1;
 

}
DrygasadustStr[0] = 0x0030+Drygas_D/10;
DrygasadustStr[1] = 0x0030+Drygas_D%10;

/*
WetgasadustStr[1] = 0x30+Wetgas_d/10;
WetgasadustStr[2] = 0x30+Wetgas_d%10;
*/
	obj = GOLFindObject(ID_EB_DryGas);
	EbSetText((EDITBOX*)obj, DrygasadustStr);
	SetState(obj, EB_DRAW);

/*	
	obj = GOLFindObject(ID_EB_WetGas);
	EbSetText((EDITBOX*)obj, WetgasadustStr);
	SetState(obj, EB_DRAW);
	*/
}
void  CreateWait()
{
	GOLFree();   // free memory for the objects in the previous linked list and start new list
	CreatePage0(WaitStr);	//DeleteStr CreatePage("Blow Please");
if(TestMode != 0xcc)	
{
    StCreate(ID_STATICTEXT3,           		// ID 
              40,80,
              200,100,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              Airblank1, 				// "TEST", 	// text
              alt5Scheme);   //mainScheme   alt2Scheme	
    StCreate(ID_STATICTEXT3,           		// ID 
              40,100,
              200,120,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              Airblank2, 				// "TEST", 	// text
              alt5Scheme);   //mainScheme   alt2Scheme	
}
    StCreate(ID_STATICTEXT3,           		// ID 
              40,180,
              200,200,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              WaitStr, 				// "TEST", 	// text
              alt5Scheme);   //mainScheme   alt2Scheme
          	// use alternate scheme

}
WORD  MsgWait(WORD objMsg, OBJ_HEADER* pObj)
{

  
 
prepare_and_Charge();


 //screenState =CREATE_Mark;
  
}
void  CreatePowerOff()
{
	GOLFree();   // free memory for the objects in the previous linked list and start new list
	CreatePage(PowerOffStr);	// CreatePage("Blow Please");
	StCreate(ID_STATICTEXT3,           		// ID 
              MAINSTARTX-10,2*MAINSTARTY,
              MAINSTARTX+4*MAINCHARSIZE,MAINSTARTY+1*MAINCHARSIZE+MAINSIZEY,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              PowerOffStr, 				// "TEST", 	// text
              alt5Scheme);   //mainScheme   alt2Scheme             // use alternate scheme

}

WORD MsgPowerOFF(WORD objMsg, OBJ_HEADER* pObj){

}

void  CreateRefuse()
{
	GOLFree();   // free memory for the objects in the previous linked list and start new list
	CreatePage(Noblowstr);	// CreatePage("Blow Please");
	BtnCreate(ID_BUTTON1,             	// button ID 
               40,185,(GetMaxX()-40),235,           	// dimension
              0,					   	// set radius 
              BTN_DRAW,//|BTN_TOGGLE,   	// draw a vertical capsule button 
              						   	// that has a toggle behavior
              NULL,                    	// no bitmap
              RepeatStr,//LowStr, 					// "LO",       	// text
              altScheme);           	// use alternate scheme
        BtnCreate(ID_BUTTON2,             	// button ID 
               40,105,(GetMaxX()-40),155,           	// dimension
              0,					   	// set radius 
              BTN_DRAW,//|BTN_TOGGLE,   	// draw a vertical capsule button 
              						   	// that has a toggle behavior
              NULL,                    	// no bitmap
              RefuseStr,//LowStr, 					// "LO",       	// text
              altScheme);           	// use alternate scheme
}

WORD MsgRefuse(WORD objMsg, OBJ_HEADER* pObj){
	OBJ_HEADER* pOtherRbtn;

    switch(GetObjID(pObj)){

        case ID_BUTTON_NEXT:
            if(objMsg == BTN_MSG_RELEASED){
              screenState = CREATE_MAIN;// goto check box demo screen
            }
            return 1; 							// process by default

        case ID_BUTTON_BACK:
            if(objMsg == BTN_MSG_RELEASED){
              screenState = CREATE_MAIN; 		// goto ECG demo screen
            }
            return 1; 							// process by default

	case ID_BUTTON_HOME:
            if(objMsg == BTN_MSG_RELEASED){
 //           	abcStatus = 0;
                screenState = CREATE_MAIN; 	// goto list box screen
            }
            return 1;
            
	case  ID_BUTTON1:
		 if(objMsg == BTN_MSG_RELEASED){
		 	ST_IRstatus = 0;
			screenState = CREATE_BLOW;     	// goto radio buttons screen
			g_backlight_time = 3*60;// 3分钟不吹气，自动跳出来
            }
            return 1;
        case  ID_BUTTON2:
		 if(objMsg == BTN_MSG_RELEASED){
//                Manual_Analyze();
                ST_IRstatus = ST_REFUSE;
                screenState = CREATE_RESULT; 	// goto radio buttons screen
            }
            return 1;
        default:
            return 1; 							// process by default
    }
}

void  CreateBlowPress()
{
	GOLFree();  // free memory for the objects in the previous linked list and start new list

	CreatePage(BlowTimePressStr); 	// CreatePage("Static text");

	StCreate(ID_STATICTEXT3,          	// ID 
              positionax,(GetMaxY() - 270),3*positionmaxx,(GetMaxY() - 240),            // dimension
              ST_DRAW|ST_CENTER_ALIGN, 				// will be dislayed and checked after creation
                                       	// first button in the group
              BlowTimeStr, 					// "Left"
              alt5Scheme);              	// use alternate scheme

 	RbCreate(ID_RADIOBUTTONStar + 35,          	// ID 
              positionax,(GetMaxY() - 240),2*positionmaxx,(GetMaxY() - 210),            // dimension
              RB_DRAW|RB_GROUP,//|	\
             	//RB_CHECKED, 				// will be dislayed and checked after creation
                                       	// first button in the group
              TwoFineSecStr, 					// "Left"
              alt5Scheme);              	// use alternate scheme


    RbCreate(ID_RADIOBUTTONStar + 36,          // ID 
              positionax+0*positionmaxx,(GetMaxY() - 210),positionax+2*positionmaxx,(GetMaxY() - 180),        // dimension
              RB_DRAW,                 // will be dislayed after creation
              ThreeSecStr, 			   // "Right"
              alt5Scheme);              // use alternate scheme

    RbCreate(ID_RADIOBUTTONStar + 37,          // ID 
              positionax+0*positionmaxx,(GetMaxY() - 180),positionax+2*positionmaxx,(GetMaxY() - 150),        // dimension
              RB_DRAW,                 // will be dislayed after creation
              ThreeFineSecStr, 			   // "Center"
              alt5Scheme);              // use alternate scheme

	RbCreate(ID_RADIOBUTTONStar + 38,          	// ID 
              positionax+0*positionmaxx,(GetMaxY() - 150),positionax+2*positionmaxx,(GetMaxY() - 120),             // dimension
              RB_DRAW,//|RB_GROUP,//|	\
              //RB_CHECKED, 				// will be dislayed and checked after creation
                                       	// first button in the group
              FourSecStr, 					// "Left"
              alt5Scheme);              	// use alternate scheme

    RbCreate(ID_RADIOBUTTONStar + 39,          // ID 
              positionax+0*positionmaxx,(GetMaxY() - 120),positionax+2*positionmaxx,(GetMaxY() - 90),       // dimension
              RB_DRAW,                 // will be dislayed after creation
              FourFineSecStr, 			   // "Right"
              alt5Scheme);              // use alternate scheme
 
    RbCreate(ID_RADIOBUTTONStar + 40,          // ID 
              positionax+0*positionmaxx,(GetMaxY() - 90),positionax+2*positionmaxx,(GetMaxY() - 60),       // dimension
              RB_DRAW,                 // will be dislayed after creation
              FineSecStr, 			   // "Right"
              alt5Scheme);              // use alternate scheme


	// for the Press setting
   StCreate(ID_STATICTEXT3,          // ID 
              positionax+4*positionmaxx,(GetMaxY() - 270),positionax+7*positionmaxx,(GetMaxY() - 240),         // dimension
              ST_DRAW|ST_CENTER_ALIGN,           // will be dislayed after creation
              BlowPressStr, 			   // "Center"
              alt5Scheme); 

    RbCreate(ID_RADIOBUTTONStar + 41,          // ID 
              positionax+4*positionmaxx,(GetMaxY() - 240),positionax+6*positionmaxx,(GetMaxY() - 210),         // dimension
              RB_DRAW,                 // will be dislayed after creation
              OneStr, 			   // "Center"
              alt5Scheme);              // use alternate scheme

     RbCreate(ID_RADIOBUTTONStar + 42,          	// ID 
              positionax+4*positionmaxx,(GetMaxY() - 210),positionax+6*positionmaxx,(GetMaxY() - 180),             // dimension
              RB_DRAW,//|RB_GROUP,//|	\
              //RB_CHECKED, 				// will be dislayed and checked after creation
                                       	// first button in the group
              TwoStr, 					// "Left"
              alt5Scheme);              	// use alternate scheme

    RbCreate(ID_RADIOBUTTONStar + 43,          // ID 
              positionax+4*positionmaxx,(GetMaxY() - 180),positionax+6*positionmaxx,(GetMaxY() - 150),       // dimension
              RB_DRAW,                 // will be dislayed after creation
              ThreeStr, 			   // "Right"
              alt5Scheme);              // use alternate scheme

    RbCreate(ID_RADIOBUTTONStar + 44,          // ID 
              positionax+4*positionmaxx,(GetMaxY() - 150),positionax+6*positionmaxx,(GetMaxY() - 120),        // dimension
              RB_DRAW,                 // will be dislayed after creation
              FourStr, 			   // "Center"
              alt5Scheme);              // use alternate scheme
   RbCreate(ID_RADIOBUTTONStar + 45,          // ID 
              positionax+4*positionmaxx,(GetMaxY() - 120),positionax+6*positionmaxx,(GetMaxY() - 90),       // dimension
              RB_DRAW,                 // will be dislayed after creation
              FineStr, 			   // "Right"
              alt5Scheme);

    RbCreate(ID_RADIOBUTTONStar + 46,          // ID 
              positionax+4*positionmaxx,(GetMaxY() - 90),positionax+6*positionmaxx,(GetMaxY() - 60),       // dimension
              RB_DRAW,                 // will be dislayed after creation
              SixStr, 			   // "Right"
              alt5Scheme);

}
WORD MsgBlowPress(WORD objMsg, OBJ_HEADER* pObj)
{
	GROUPBOX   *pGb;
	STATICTEXT *pSt;

    switch(GetObjID(pObj)){
        case ID_RADIOBUTTON1: // change aligment to left
            pGb = (GROUPBOX*) GOLFindObject(ID_GROUPBOX1); 	// get pointer to group box
            ClrState(pGb,GB_RIGHT_ALIGN|GB_CENTER_ALIGN);  	// clear right and center alignment states
            SetState(pGb, GB_DRAW);                        	// set redraw state
            pSt = (STATICTEXT*) GOLFindObject(ID_STATICTEXT3); // get pointer to static text
            ClrState(pSt,ST_RIGHT_ALIGN|ST_CENTER_ALIGN);  	// clear right and center alignment states    
            SetState(pSt, ST_DRAW);                        	// set redraw state
            return 1; 										// process by default

        case ID_RADIOBUTTON2:
            pGb = (GROUPBOX*) GOLFindObject(ID_GROUPBOX1); 	// get pointer to group box
            ClrState(pGb, GB_RIGHT_ALIGN);  				// clear right alignment state
            SetState(pGb, GB_CENTER_ALIGN|GB_DRAW);  		// set center alignment and redraw states
            pSt = (STATICTEXT*) GOLFindObject(ID_STATICTEXT3); // get pointer to static text
            ClrState(pSt,ST_RIGHT_ALIGN);  					// clear right aligment state
            SetState(pSt, ST_DRAW|ST_CENTER_ALIGN);  		// set center alignment and redraw states
            return 1; 										// process by default

        case ID_RADIOBUTTON3:
            pGb = (GROUPBOX*) GOLFindObject(ID_GROUPBOX1); 	// get pointer to group box
            ClrState(pGb, GB_CENTER_ALIGN);  				// clear center alignment state
            SetState(pGb, GB_RIGHT_ALIGN|GB_DRAW);  		// set right alignment and redraw state
            pSt = (STATICTEXT*) GOLFindObject(ID_STATICTEXT3); // get pointer to static text
            ClrState(pSt,ST_CENTER_ALIGN);  				// clear center aligment state
            SetState(pSt,ST_RIGHT_ALIGN|ST_DRAW); 			// set right alignment and redraw states
            return 1; // process by default

        case ID_BUTTON_NEXT:
            if(objMsg == BTN_MSG_RELEASED){
				if(dif)
					screenState = CREATE_Master;
				else
                    screenState = CREATE_AGENT;				// goto slider screen
            }
            return 1; 										// process by default

        case ID_BUTTON_BACK:
            if(objMsg == BTN_MSG_RELEASED){
                if(dif)
					screenState = CREATE_Master;
				else
                    screenState = CREATE_AGENT; 			// goto radio buttons screen
            }
            return 1; 										// process by default
	case ID_BUTTON_HOME:
            if(objMsg == BTN_MSG_RELEASED){
 //           	abcStatus = 0;
                screenState = CREATE_MAIN; 	// goto list box screen
            }
            return 1;
        default:
            return 1;
    }

}
void  CreateDebug()
{
    alarmON = FALSE;
	GOLFree();   // free memory for the objects in the previous linked list and start new list
	CreatePage0(DebugStr);	//DeleteStr CreatePage("Blow Please");
    StCreate(ID_STATICTEXT3,           		// ID 
              positionax,60,
              2*positionmaxx,100,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              DemarcateAD, 				// "TEST", 	// text
              mainScheme);        //  mainScheme         // use alternate scheme
        StCreate(ID_STATICTEXT3,           		// ID 
              2*(positionax+positionmaxx),60,
              4*positionmaxx,100,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              DemarcateAD1, 				// "TEST", 	// text
              mainScheme);        //  mainScheme         // use alternate scheme
    StCreate(ID_STATICTEXT3,           		// ID 
              132,60,
              192,100,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              DemarcateAD2, 				// "TEST", 	// text
              mainScheme);        //  mainScheme         // use alternate scheme


	StCreate(ID_STATICTEXT3,           		// ID 
              positionax,90,
              2*positionmaxx,130,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              DemarcateAD3, 				// "TEST", 	// text
              mainScheme);        //  mainScheme         // use alternate scheme
        StCreate(ID_STATICTEXT3,           		// ID 
              2*(positionax+positionmaxx),90,
              4*positionmaxx,130,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              DemarcateAD4, 				// "TEST", 	// text
              mainScheme);        //  mainScheme         // use alternate scheme
    StCreate(ID_STATICTEXT3,           		// ID 
              132,90,
              192,130,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              DemarcateAD5, 				// "TEST", 	// text
              mainScheme);        //  mainScheme         // use alternate scheme


	StCreate(ID_STATICTEXT3,           		// ID 
              positionax,125,
              2*positionmaxx,160,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              DemarcateAD6, 				// "TEST", 	// text
              mainScheme);        //  mainScheme         // use alternate scheme

    StCreate(ID_STATICTEXT3,           		// ID 
              2*(positionax+positionmaxx),125,
              4*positionmaxx,160,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              DemarcateAD7, 				// "TEST", 	// text
              mainScheme);        //  mainScheme         // use alternate scheme
    StCreate(ID_STATICTEXT3,           		// ID 
              132,125,
              192,160,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              DemarcateAD8, 				// "TEST", 	// text
              mainScheme);        //  mainScheme         // use alternate scheme

	StCreate(ID_STATICTEXT3,           		// ID 
              positionax,158,
              2*positionmaxx,190,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              DemarcateAD9, 				// "TEST", 	// text
              mainScheme);        //  mainScheme         // use alternate scheme
        StCreate(ID_STATICTEXT3,           		// ID 
              2*(positionax+positionmaxx),158,
              4*positionmaxx,190,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              DemarcateAD10, 				// "TEST", 	// text
              mainScheme);        //  mainScheme         // use alternate scheme
    StCreate(ID_STATICTEXT3,           		// ID 
              132,158,
              192,190,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              DemarcateAD11, 				// "TEST", 	// text
              mainScheme);        //  mainScheme         // use alternate scheme

    StCreate(ID_STATICTEXT3,           		// ID 
              positionax,189,
              2*positionmaxx,220,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              DemarcateAD12, 				// "TEST", 	// text
              mainScheme);        //  mainScheme         // use alternate scheme
        StCreate(ID_STATICTEXT3,           		// ID 
              2*(positionax+positionmaxx),189,
              4*positionmaxx,220,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              DemarcateAD13, 				// "TEST", 	// text
              mainScheme);        //  mainScheme         // use alternate scheme
    StCreate(ID_STATICTEXT3,           		// ID 
              132,189,
              192,220,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              DemarcateAD14, 				// "TEST", 	// text
              mainScheme);        //  mainScheme         // use alternate scheme


	StCreate(ID_STATICTEXT3,           		// ID 
              positionax,220,
              2*positionmaxx,250,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              DemarcateAD15, 				// "TEST", 	// text
              mainScheme);        //  mainScheme         // use alternate scheme
        StCreate(ID_STATICTEXT3,           		// ID 
              2*(positionax+positionmaxx),220,
              4*positionmaxx,250,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              DemarcateAD16, 				// "TEST", 	// text
              mainScheme);        //  mainScheme         // use alternate scheme
    StCreate(ID_STATICTEXT3,           		// ID 
              132,220,
              192,250,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              DemarcateAD17, 				// "TEST", 	// text
              mainScheme);        //  mainScheme         // use alternate scheme

StCreate(ID_STATICTEXT3,           		// ID 
              positionax,250,
              2*positionmaxx,280,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              DemarcateAD18, 				// "TEST", 	// text
              mainScheme);        //  mainScheme         // use alternate scheme
   StCreate(ID_STATICTEXT3,           		// ID 
              2*(positionax+positionmaxx),250,
              4*positionmaxx,280,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              DemarcateAD19, 				// "TEST", 	// text
              mainScheme);        //  mainScheme         // use alternate schemee
   StCreate(ID_STATICTEXT3,           		// ID 
              132,250,
              192,280,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              DemarcateAD20, 				// "TEST", 	// text
              mainScheme);  

}
void  CreateDebug2()
{
	GOLFree();   // free memory for the objects in the previous linked list and start new list
	CreatePage0(DebugStr);	//DeleteStr CreatePage("Blow Please");
    StCreate(ID_STATICTEXT3,           		// ID 
              positionax,60,
              2*positionmaxx,100,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              DemarcateAD, 				// "TEST", 	// text
              mainScheme);        //  mainScheme         // use alternate scheme
        StCreate(ID_STATICTEXT3,           		// ID 
              2*(positionax+positionmaxx),60,
              4*positionmaxx,100,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              DemarcateAD1, 				// "TEST", 	// text
              mainScheme);        //  mainScheme         // use alternate scheme
    StCreate(ID_STATICTEXT3,           		// ID 
              132,60,
              192,100,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              DemarcateAD2, 				// "TEST", 	// text
              mainScheme);        //  mainScheme         // use alternate scheme


	StCreate(ID_STATICTEXT3,           		// ID 
              positionax,90,
              2*positionmaxx,130,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              DemarcateAD3, 				// "TEST", 	// text
              mainScheme);        //  mainScheme         // use alternate scheme
        StCreate(ID_STATICTEXT3,           		// ID 
              2*(positionax+positionmaxx),90,
              4*positionmaxx,130,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              DemarcateAD4, 				// "TEST", 	// text
              mainScheme);        //  mainScheme         // use alternate scheme
    StCreate(ID_STATICTEXT3,           		// ID 
              132,90,
              192,130,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              DemarcateAD5, 				// "TEST", 	// text
              mainScheme);        //  mainScheme         // use alternate scheme


	StCreate(ID_STATICTEXT3,           		// ID 
              positionax,125,
              2*positionmaxx,160,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              DemarcateAD6, 				// "TEST", 	// text
              mainScheme);        //  mainScheme         // use alternate scheme

    StCreate(ID_STATICTEXT3,           		// ID 
              2*(positionax+positionmaxx),125,
              4*positionmaxx,160,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              DemarcateAD7, 				// "TEST", 	// text
              mainScheme);        //  mainScheme         // use alternate scheme
    StCreate(ID_STATICTEXT3,           		// ID 
              132,125,
              192,160,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              DemarcateAD8, 				// "TEST", 	// text
              mainScheme);        //  mainScheme         // use alternate scheme


}
void  CreateDemarcateshow()
{
	GOLFree();   // free memory for the objects in the previous linked list and start new list
	CreatePage0(DemarcateshowStr);	//DeleteStr CreatePage("Blow Please");
	StCreate(ID_STATICTEXT3,           		// ID 
              positionax,60,
              2*positionmaxx,100,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              DemarcateAD, 				// "TEST", 	// text
              mainScheme);        //  mainScheme         // use alternate scheme
        StCreate(ID_STATICTEXT3,           		// ID 
              2*(positionax+positionmaxx),60,
              4*positionmaxx,100,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              DemarcateAD1, 				// "TEST", 	// text
              mainScheme);        //  mainScheme         // use alternate scheme


	StCreate(ID_STATICTEXT3,           		// ID 
              positionax,90,
              2*positionmaxx,130,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              DemarcateAD2, 				// "TEST", 	// text
              mainScheme);        //  mainScheme         // use alternate scheme
        StCreate(ID_STATICTEXT3,           		// ID 
              2*(positionax+positionmaxx),90,
              4*positionmaxx,130,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              DemarcateAD3, 				// "TEST", 	// text
              mainScheme);        //  mainScheme         // use alternate scheme

	StCreate(ID_STATICTEXT3,           		// ID 
              positionax,125,
              2*positionmaxx,160,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              DemarcateAD4, 				// "TEST", 	// text
              mainScheme);        //  mainScheme         // use alternate scheme
        StCreate(ID_STATICTEXT3,           		// ID 
              2*(positionax+positionmaxx),125,
              4*positionmaxx,160,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              DemarcateAD5, 				// "TEST", 	// text
              mainScheme);        //  mainScheme         // use alternate scheme

	StCreate(ID_STATICTEXT3,           		// ID 
              positionax,158,
              2*positionmaxx,190,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              DemarcateAD6, 				// "TEST", 	// text
              mainScheme);        //  mainScheme         // use alternate scheme
        StCreate(ID_STATICTEXT3,           		// ID 
              2*(positionax+positionmaxx),158,
              4*positionmaxx,190,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              DemarcateAD7, 				// "TEST", 	// text
              mainScheme);        //  mainScheme         // use alternate scheme

	StCreate(ID_STATICTEXT3,           		// ID 
              positionax,189,
              2*positionmaxx,220,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              DemarcateAD8, 				// "TEST", 	// text
              mainScheme);        //  mainScheme         // use alternate scheme
        StCreate(ID_STATICTEXT3,           		// ID 
              2*(positionax+positionmaxx),189,
              4*positionmaxx,220,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              DemarcateAD9, 				// "TEST", 	// text
              mainScheme);        //  mainScheme         // use alternate scheme
	StCreate(ID_STATICTEXT3,           		// ID 
              positionax,220,
              2*positionmaxx,250,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              DemarcateAD10, 				// "TEST", 	// text
              mainScheme);        //  mainScheme         // use alternate scheme
        StCreate(ID_STATICTEXT3,           		// ID 
              2*(positionax+positionmaxx),220,
              4*positionmaxx,250,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              DemarcateAD11, 				// "TEST", 	// text
              mainScheme);        //  mainScheme         // use alternate scheme

	StCreate(ID_STATICTEXT3,           		// ID 
              positionax,250,
              2*positionmaxx,280,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              DemarcateAD12, 				// "TEST", 	// text
              mainScheme);        //  mainScheme         // use alternate scheme
        StCreate(ID_STATICTEXT3,           		// ID 
              2*(positionax+positionmaxx),250,
              4*positionmaxx,280,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              DemarcateAD13, 				// "TEST", 	// text
              mainScheme);        //  mainScheme         // use alternate schemee

}
#ifdef USE_ITEM
	static char Itemchar = 0;

void SaveReadItem(BOOL SaveRead)
{
	DWORD ItemAddress;
	BYTE* Itempoint;
	unsigned char i;
	for(i= 0;i<20;i++)// 最大只给16个字符ItemMax
	{
		if(SaveRead)
	       {
//	            	EEPROMWriteByte(Itempoint, ItemAddress);
//	            	Itempoint++;
		}
		else
		{
//			Itempoint=EEPROMReadByte(ItemAddress);
//			Itempoint++;
			Item0Str[i]=EEPROMReadByte(Item0Address+i);
			Item1Str[i]=EEPROMReadByte(Item1Address+i);
			Item2Str[i]=EEPROMReadByte(Item2Address+i);
			Item3Str[i]=EEPROMReadByte(Item3Address+i);
			Item4Str[i]=EEPROMReadByte(Item4Address+i);
			Item5Str[i]=EEPROMReadByte(Item5Address+i);
			Item6Str[i]=EEPROMReadByte(Item6Address+i);
			Item7Str[i]=EEPROMReadByte(Item7Address+i);
			Item8Str[i]=EEPROMReadByte(Item8Address+i);
			Item9Str[i]=EEPROMReadByte(Item9Address+i);
		}
	}

}

void SaveItemByte(unsigned char ItemcharTemp,unsigned int ID_PAD_TEMP)
{
	WORD ItemAddress,Itemcounter;
	if(ItemcharTemp>(ItemMax-1)) return;
	switch(ItemID)
	 {
	        	case ID_STATICTEXT10:
	        		Item0Str[ItemcharTemp] = ID_PAD_TEMP;
	        		Item0Str[ItemcharTemp+1] = 0;
	        		ItemAddress = Item0Address;
	        		break;
	        	case ID_STATICTEXT11:
	        		Item1Str[ItemcharTemp] = ID_PAD_TEMP;
	        		Item1Str[ItemcharTemp+1] = 0;
	        		ItemAddress = Item1Address;
	        		break;
	        	case ID_STATICTEXT12:
	        		Item2Str[ItemcharTemp] = ID_PAD_TEMP;
	        		Item2Str[ItemcharTemp+1] = 0;
				ItemAddress = Item2Address;
	        		break;
	        	case ID_STATICTEXT13:
	        		Item3Str[ItemcharTemp] = ID_PAD_TEMP;
	        		Item3Str[ItemcharTemp+1] = 0;
	        		ItemAddress = Item3Address;
	        		break;
	        	case ID_STATICTEXT14:
	        		Item4Str[ItemcharTemp] = ID_PAD_TEMP;
	        		Item4Str[ItemcharTemp+1] = 0;
	        		ItemAddress = Item4Address;
	        		break;
	        	case ID_STATICTEXT15:
	        		Item5Str[ItemcharTemp] = ID_PAD_TEMP;
	        		Item5Str[ItemcharTemp+1] = 0;
	        		ItemAddress = Item5Address;
	        		break;
	        	case ID_STATICTEXT16:
	        		Item6Str[ItemcharTemp] = ID_PAD_TEMP;
	        		Item6Str[ItemcharTemp+1] = 0;
	        		ItemAddress = Item6Address;
	        		break;
	        	case ID_STATICTEXT17:
	        		Item7Str[ItemcharTemp] = ID_PAD_TEMP;
	        		Item7Str[ItemcharTemp+1] = 0;
	        		ItemAddress = Item7Address;
	        		break;
	        	case ID_STATICTEXT18:
	        		Item8Str[ItemcharTemp] = ID_PAD_TEMP;
	        		Item8Str[ItemcharTemp+1] = 0;
	        		ItemAddress = Item8Address;
	        		break;
	        	case ID_STATICTEXT19:
	        		Item9Str[ItemcharTemp] = ID_PAD_TEMP;
	        		Item9Str[ItemcharTemp+1] = 0;
	        		ItemAddress = Item9Address;
	        		break;

	        	default:
	        		break;

	        }

	        
		        EEPROMWriteByte((BYTE)(ID_PAD_TEMP), ItemAddress+ItemcharTemp);

			for(Itemcounter=ItemcharTemp+1;Itemcounter<ItemMax;Itemcounter++)
		        EEPROMWriteByte((BYTE)(0), ItemAddress+Itemcounter);//结束符号后面的全部清零
	        
	    

}

void  CreateItem()// 创建项目add by Spring.Chen
{

	unsigned char i;
	GOLFree();  // free memory for the objects in the previous linked list and start new list

	CreatePage(ItemStr);
	SaveReadItem(FALSE);
    	CbCreate(ID_CHECKBOX10,             	// ID 
              positionax,(positionay+0*positionItem),
              positionItem,(positionay+1*positionItem),          	// dimension
              CB_DRAW,                 	// will be dislayed after creation
              NULL,//Item0Str, 				// "Text Left"
              altScheme);              	// alternative GOL scheme 
       CbCreate(ID_CHECKBOX20,             	// ID 
              100*positionax,(positionay-2+0*positionItem),
              11*positionItem-8,(positionay+1*positionItem),          	// dimension
              CB_DRAW,                 	// will be dislayed after creation
              NULL,//Item0Str, 				// "Text Left"
              altScheme);
	StCreate(ID_STATICTEXT10,           		// ID 
              positionax+positionItem,(positionay+0*positionItem),
              9*positionItem,(positionay+1*positionItem),         		// dimension
              ST_DRAW,		// display text
              Item0Str, 				// "TEST", 	// text
              alt5Scheme);

	CbCreate(ID_CHECKBOX11,             	// ID 
              positionax,(positionay+1*positionItem),
              positionItem,(positionay+2*positionItem),         	// dimension
              CB_DRAW,                 	// will be dislayed after creation
              NULL,//TextLeftStr, 				// "Text Left"
              altScheme);              	// alternative GOL scheme 
       CbCreate(ID_CHECKBOX21,             	// ID 
              100*positionax,(positionay+1*positionItem-2),
              11*positionItem-8,(positionay+2*positionItem),         	// dimension
              CB_DRAW,                 	// will be dislayed after creation
              NULL,//TextLeftStr, 				// "Text Left"
              altScheme);
	StCreate(ID_STATICTEXT11,           		// ID 
              positionax+positionItem,(positionay+1*positionItem),
              9*positionItem,(positionay+2*positionItem),          		// dimension
              ST_DRAW,		// display text
              Item1Str, 				// "TEST", 	// text
              alt5Scheme);

	CbCreate(ID_CHECKBOX12,             	// ID 
              positionax,(positionay+2*positionItem),
              positionItem,(positionay+3*positionItem),         	// dimension
              CB_DRAW,                 	// will be dislayed after creation
              NULL,//TextLeftStr, 				// "Text Left"
              altScheme);              	// alternative GOL scheme 
       CbCreate(ID_CHECKBOX22,             	// ID 
              100*positionax,(positionay+2*positionItem-2),
              11*positionItem-8,(positionay+3*positionItem),         	// dimension
              CB_DRAW,                 	// will be dislayed after creation
              NULL,//TextLeftStr, 				// "Text Left"
              altScheme);
	StCreate(ID_STATICTEXT12,           		// ID 
              positionax+positionItem,(positionay+2*positionItem),
              9*positionItem,(positionay+3*positionItem),         		// dimension
              ST_DRAW,		// display text
              Item2Str, 				// "TEST", 	// text
              alt5Scheme);

	CbCreate(ID_CHECKBOX13,             	// ID 
              positionax,(positionay+3*positionItem),
              positionItem,(positionay+4*positionItem),         	// dimension
              CB_DRAW,                 	// will be dislayed after creation
              NULL,//TextLeftStr, 				// "Text Left"
              altScheme);              	// alternative GOL scheme 
       CbCreate(ID_CHECKBOX23,             	// ID 
              100*positionax,(positionay+3*positionItem-2),
              11*positionItem-8,(positionay+4*positionItem),         	// dimension
              CB_DRAW,                 	// will be dislayed after creation
              NULL,//TextLeftStr, 				// "Text Left"
              altScheme);
	StCreate(ID_STATICTEXT13,           		// ID 
              positionax+positionItem,(positionay+3*positionItem),
              9*positionItem,(positionay+4*positionItem),         		// dimension
              ST_DRAW,		// display text
              Item3Str, 				// "TEST", 	// text
              alt5Scheme);

	CbCreate(ID_CHECKBOX14,             	// ID 
              positionax,(positionay+4*positionItem),
              positionItem,(positionay+5*positionItem),         	// dimension
              CB_DRAW,                 	// will be dislayed after creation
              NULL,//TextLeftStr, 				// "Text Left"
              altScheme);              	// alternative GOL scheme 
       CbCreate(ID_CHECKBOX24,             	// ID 
              100*positionax,(positionay+4*positionItem-2),
              11*positionItem-8,(positionay+5*positionItem),         	// dimension
              CB_DRAW,                 	// will be dislayed after creation
              NULL,//TextLeftStr, 				// "Text Left"
              altScheme);
	StCreate(ID_STATICTEXT14,           		// ID 
              positionax+positionItem,(positionay+4*positionItem),
              9*positionItem,(positionay+5*positionItem),         		// dimension
              ST_DRAW,		// display text
              Item4Str, 				// "TEST", 	// text
              alt5Scheme);

	CbCreate(ID_CHECKBOX15,             	// ID 
              positionax,(positionay+5*positionItem),
              positionItem,(positionay+6*positionItem),         	// dimension
              CB_DRAW,                 	// will be dislayed after creation
              NULL,//TextLeftStr, 				// "Text Left"
              altScheme);              	// alternative GOL scheme 
       CbCreate(ID_CHECKBOX25,             	// ID 
              100*positionax,(positionay+5*positionItem-2),
              11*positionItem-8,(positionay+6*positionItem),         	// dimension
              CB_DRAW,                 	// will be dislayed after creation
              NULL,//TextLeftStr, 				// "Text Left"
              altScheme);
	StCreate(ID_STATICTEXT15,           		// ID 
              positionax+positionItem,(positionay+5*positionItem),
              9*positionItem,(positionay+6*positionItem),         		// dimension
              ST_DRAW,		// display text
              Item5Str, 				// "TEST", 	// text
              alt5Scheme);

	CbCreate(ID_CHECKBOX16,             	// ID 
              positionax,(positionay+6*positionItem),
              positionItem,(positionay+7*positionItem),         	// dimension
              CB_DRAW,                 	// will be dislayed after creation
              NULL,//TextLeftStr, 				// "Text Left"
              altScheme);              	// alternative GOL scheme 
       CbCreate(ID_CHECKBOX26,             	// ID 
              100*positionax,(positionay+6*positionItem-2),
              11*positionItem-8,(positionay+7*positionItem),         	// dimension
              CB_DRAW,                 	// will be dislayed after creation
              NULL,//TextLeftStr, 				// "Text Left"
              altScheme);
	StCreate(ID_STATICTEXT16,           		// ID 
              positionax+positionItem,(positionay+6*positionItem),
              9*positionItem,(positionay+7*positionItem),         		// dimension
              ST_DRAW,		// display text
              Item6Str, 				// "TEST", 	// text
              alt5Scheme);

	CbCreate(ID_CHECKBOX17,             	// ID 
              positionax,(positionay+7*positionItem),
              positionItem,(positionay+8*positionItem),         	// dimension
              CB_DRAW,                 	// will be dislayed after creation
              NULL,//TextLeftStr, 				// "Text Left"
              altScheme);              	// alternative GOL scheme 
       CbCreate(ID_CHECKBOX27,             	// ID 
              100*positionax,(positionay+7*positionItem-2),
              11*positionItem-8,(positionay+8*positionItem),         	// dimension
              CB_DRAW,                 	// will be dislayed after creation
              NULL,//TextLeftStr, 				// "Text Left"
              altScheme);
	StCreate(ID_STATICTEXT17,           		// ID 
              positionax+positionItem,(positionay+7*positionItem),
              9*positionItem,(positionay+8*positionItem),         		// dimension
              ST_DRAW,		// display text
              Item7Str, 				// "TEST", 	// text
              alt5Scheme);

	CbCreate(ID_CHECKBOX18,             	// ID 
              positionax,(positionay+8*positionItem),
              positionItem,(positionay+9*positionItem),         	// dimension
              CB_DRAW,                 	// will be dislayed after creation
              NULL,//TextLeftStr, 				// "Text Left"
              altScheme);              	// alternative GOL scheme 
       CbCreate(ID_CHECKBOX28,             	// ID 
              100*positionax,(positionay+8*positionItem-2),
              11*positionItem-8,(positionay+9*positionItem),         	// dimension
              CB_DRAW,                 	// will be dislayed after creation
              NULL,//TextLeftStr, 				// "Text Left"
              altScheme);
	StCreate(ID_STATICTEXT18,           		// ID 
              positionax+positionItem,(positionay+8*positionItem),
              9*positionItem,(positionay+9*positionItem),        		// dimension
              ST_DRAW,		// display text
              Item8Str, 				// "TEST", 	// text
              alt5Scheme);

	CbCreate(ID_CHECKBOX19,             	// ID 
              positionax,(positionay+9*positionItem),
              positionItem,(positionay+10*positionItem),         	// dimension
              CB_DRAW,                 	// will be dislayed after creation
              NULL,//TextLeftStr, 				// "Text Left"
              altScheme);              	// alternative GOL scheme 
       CbCreate(ID_CHECKBOX29,             	// ID 
              100*positionax,(positionay+9*positionItem-2),
              11*positionItem-8,(positionay+10*positionItem),         	// dimension
              CB_DRAW,                 	// will be dislayed after creation
              NULL,//TextLeftStr, 				// "Text Left"
              altScheme);
	StCreate(ID_STATICTEXT19,           		// ID 
              positionax+positionItem,(positionay+9*positionItem),
              9*positionItem,(positionay+10*positionItem),       		// dimension
              ST_DRAW,		// display text
              Item9Str, 				// "TEST", 	// text
              alt5Scheme);

}
WORD MsgItem(WORD objMsg, OBJ_HEADER* pObj, GOL_MSG* pMsg)
{

	EDITBOX* pEb;
	SHORT    id;
	XCHAR    temp;
	static char status = 0;			// status to check if calling, holding or not
	static BOOL abcStatus = 0;		// status to check if Large ABC or not
	BYTE* Itempoit;
	WORD ItemAddress;
	unsigned int ID_PAD_TEMP;
       static char languagechange=0xee;
       unsigned int ItemAdd;
      unsigned int KeyNumber=0;
    id = GetObjID(pObj);
    // If number key is pressed
#ifdef	USE_BTN_MSG_RELEASED
    if(objMsg == BTN_MSG_RELEASED)// BTN_MSG_PRESSED 
#ifdef	USE_adcX_adcy
	if((adcX == -1)||(adcY == -1))
#endif
#else
	if(objMsg == BTN_MSG_PRESSED)//BTN_MSG_RELEASED 
#endif
    {
    	if(id >= ID_KEYPAD)
    		if(id  < ID_KEYPAD+10)
    		{
	    		if (!status) 
	    		{
	    		if(languagechange||languagereturn)
	    			{
	    			if((id  < ID_KEYPAD+9)&&(id > ID_KEYPAD+2))
	    				{
	        		pEb = (EDITBOX*)GOLFindObject(ID_EDITBOX1);
					if(abcStatus)
						{
	        		EbAddChar(pEb,0x00B3+id-ID_KEYPAD);
					ID_PAD_TEMP=0x00B3+id-ID_KEYPAD;
						}
					else
						{
	                EbAddChar(pEb,KEYPADRUSSIAN[id-ID_KEYPAD-4][2]);
					ID_PAD_TEMP=KEYPADRUSSIAN[id-ID_KEYPAD-4][2];
					}
	        		SetState(pEb, EB_DRAW);
	    				}
					if(id == ID_KEYPAD+9)
						{
                 	pEb = (EDITBOX*)GOLFindObject(ID_EDITBOX1);
	        		EbAddChar(pEb,0x007C);
				ID_PAD_TEMP=0x007C;
	        		SetState(pEb, EB_DRAW);
					}
				
				}
				else{
	        		pEb = (EDITBOX*)GOLFindObject(ID_EDITBOX1);
	        		EbAddChar(pEb,'0'+id-ID_KEYPAD);
				ID_PAD_TEMP='0'+id-ID_KEYPAD;
	        		SetState(pEb, EB_DRAW);
					}
	    		}
                       		   if(pEb->length - 1 < pEb->charMax)
		        		SaveItemByte(Itemchar,ID_PAD_TEMP);
		        		Itemchar++;
        		return 1;        
    		}

	    if(id >= ID_ABCPAD)
	    if(id  < ID_ABCPAD+26)
	    	{
	    		if (!status) 
	    		{
	        		pEb = (EDITBOX*)GOLFindObject(ID_EDITBOX1);
					if(languagechange||languagereturn)
						{
	        		if(abcStatus)
	        		{
	        			EbAddChar(pEb,pTemprussian[id-ID_ABCPAD-1][2]);
					ID_PAD_TEMP=pTemprussian[id-ID_ABCPAD-1][2];
	        		}
	        		else
	        		{
	        			EbAddChar(pEb,pTempRUSSIAN[id-ID_ABCPAD-1][2]);
					ID_PAD_TEMP=pTempRUSSIAN[id-ID_ABCPAD-1][2];
	        		}
					
						}	
					else
						{
	        		if(abcStatus)
	        		{
	        			EbAddChar(pEb,'a'+id-ID_ABCPAD);
				        ID_PAD_TEMP='a'+id-ID_ABCPAD;
	        		}
	        		else
	        		{
	        			EbAddChar(pEb,'A'+id-ID_ABCPAD);
					 ID_PAD_TEMP='A'+id-ID_ABCPAD;
	        		}


					}
	                        if(pEb->length - 1 < pEb->charMax)
		        	SaveItemByte(Itemchar,ID_PAD_TEMP);
		        	Itemchar++;
	        		SetState(pEb, EB_DRAW);

	    		} 
        		return 1;        
    		}
	}

    switch(id){

        case ID_CALL:
            if(objMsg == BTN_MSG_RELEASED)
				if((adcX == -1)||(adcY == -1)){
                pEb = (EDITBOX*)GOLFindObject(ID_EDITBOX1);    
//                EbSetText(pEb, CallingStr);
                EbAddChar(pEb,(XCHAR)' ');	// changed by Spring.Chen
                 ID_PAD_TEMP=' ';
                SaveItemByte(Itemchar,ID_PAD_TEMP);
		   Itemchar++;
                SetState(pEb, EB_DRAW);
                status = 0;// 1; Changed by Spring.chen
            }
            return 1;

        case ID_BACKSPACE:
            if(objMsg == BTN_MSG_RELEASED)
				if((adcX == -1)||(adcY == -1)){
                if (!status) {
	                pEb = (EDITBOX*)GOLFindObject(ID_EDITBOX1);    
	                EbDeleteChar(pEb);
	                SetState(pEb, EB_DRAW);
		        Itemchar--;
	                if(Itemchar<=0)
	                	Itemchar = 0;
	                SaveItemByte(Itemchar,(0));// 去掉一个字符
	            } 
            }
            return 1;


        case ID_BUTTON_NEXT:
            if(objMsg == BTN_MSG_RELEASED)
				if((adcX == -1)||(adcY == -1)){
            	Itemchar = 0;
            	abcStatus = 0;
                screenState = CREATE_ITEM;//CREATE_SETTING;				// goto slider screen
            }
            return 1; 

         case ID_BUTTON_BACK:
                       if(objMsg == BTN_MSG_RELEASED)
				if((adcX == -1)||(adcY == -1)){
            	Itemchar = 0;
            	abcStatus = 0;
                if(dif)
					screenState = CREATE_Master;
				else
                    screenState = CREATE_AGENT; 			// goto radio buttons screen
            }
            return 1;

        case ID_BUTTON_HOME:
            if(objMsg == BTN_MSG_RELEASED)
				if((adcX == -1)||(adcY == -1)){
            	abcStatus = 0;
                         if(dif)
					screenState = CREATE_Master;
				else
                    screenState = CREATE_AGENT; 			// goto radio buttons screen
            }
            return 1; // process by default

        case ID_ABC:
        	if(objMsg == BTN_MSG_RELEASED)
				if((adcX == -1)||(adcY == -1)){
                 if(languagechange)
                 	{
				if(abcStatus)
        		{

                 for(KeyNumber=0;KeyNumber<26;KeyNumber++)
				{
					pEb = (EDITBOX*)GOLFindObject(ID_ABCPAD+KeyNumber);    
				         BtnSetText((BUTTON*)pEb, pTempRUSSIAN[KeyNumber]);
				         SetState(pEb, EB_DRAW);
			        }
				for(KeyNumber=3;KeyNumber<9;KeyNumber++)
               {
				pEb = (EDITBOX*)GOLFindObject(ID_KEYPAD+KeyNumber);    
				         BtnSetText((BUTTON*)pEb, pTempRUSSIAN[KeyNumber+23]);
				         SetState(pEb, EB_DRAW);				
					}
				BtnSetText((BUTTON*)pObj, ABCStr);
        		}
        		else
        		{
        		for(KeyNumber=0;KeyNumber<26;KeyNumber++)
               {
				pEb = (EDITBOX*)GOLFindObject(ID_ABCPAD+KeyNumber);    
				         BtnSetText((BUTTON*)pEb, pTemprussian[KeyNumber]);
				         SetState(pEb, EB_DRAW);				
					}
			  for(KeyNumber=3;KeyNumber<9;KeyNumber++)
               {
				pEb = (EDITBOX*)GOLFindObject(ID_KEYPAD+KeyNumber);    
				         BtnSetText((BUTTON*)pEb, pTemprussian[KeyNumber+23]);
				         SetState(pEb, EB_DRAW);				
					}
				BtnSetText((BUTTON*)pObj, abcStr);
        		}
        			}
        			
				else
					{
			if(abcStatus)
        		{

				for(KeyNumber=0;KeyNumber<26;KeyNumber++)
				{
					pEb = GOLFindObject(ID_ABCPAD+KeyNumber);    
				         BtnSetText(pEb, pTempABC[KeyNumber]);
				         SetState(pEb, EB_DRAW);
					}

				}
			else 
				{

				for(KeyNumber=0;KeyNumber<26;KeyNumber++)
				{
					pEb = GOLFindObject(ID_ABCPAD+KeyNumber);    
				         BtnSetText(pEb, pTempabc[KeyNumber]);
				         SetState(pEb, EB_DRAW);
					}


			}
					        		
            }
		abcStatus = !abcStatus;
					}
            return 1; // process by default
      case ID_CHANGE:
	  	   if(objMsg == BTN_MSG_RELEASED)
				if((adcX == -1)||(adcY == -1)){
					abcStatus=0;
              if(languagechange)
              	{             languagereturn=0;
				for(KeyNumber=0;KeyNumber<26;KeyNumber++)
				{
					pEb = GOLFindObject(ID_ABCPAD+KeyNumber);    
				         BtnSetText(pEb, pTempABC[KeyNumber]);
				         SetState(pEb, EB_DRAW);
					}
			   for(KeyNumber=0;KeyNumber<10;KeyNumber++)
               {
				pEb = GOLFindObject(ID_KEYPAD+KeyNumber);    
				         BtnSetText(pEb, pTempXchar[KeyNumber]);
				         SetState(pEb, EB_DRAW);				
					}
			     pEb = GOLFindObject(ID_CALL);
				     BtnSetText((BUTTON*)pEb, SPACE_EN);
					  SetState(pEb, EB_DRAW);
			    pEb = GOLFindObject(ID_CHANGE);    
				BtnSetText(pEb, ChangeENStr);
				SetState(pEb, EB_DRAW);
				pEb = GOLFindObject(ID_BACKSPACE);    
				BtnSetText(pEb, DEL_EN);
				SetState(pEb, EB_DRAW);
              	}
			  else
			  	{

        		for(KeyNumber=0;KeyNumber<26;KeyNumber++)
               {
				pEb = GOLFindObject(ID_ABCPAD+KeyNumber);    
				         BtnSetText(pEb, pTempRUSSIAN[KeyNumber]);
				         SetState(pEb, EB_DRAW);				
					}
			  for(KeyNumber=3;KeyNumber<10;KeyNumber++)
               {
				pEb = GOLFindObject(ID_KEYPAD+KeyNumber);    
				         BtnSetText(pEb, pTempRUSSIAN[KeyNumber+23]);
				         SetState(pEb, EB_DRAW);				
					}
                 pEb = GOLFindObject(ID_KEYPAD);
				     BtnSetText((BUTTON*)pEb, NULL);
					  SetState(pEb, EB_DRAW);
			     pEb = GOLFindObject(ID_KEYPAD+1);
				     BtnSetText((BUTTON*)pEb, NULL);
					  SetState(pEb, EB_DRAW);		  
		 	     pEb = GOLFindObject(ID_KEYPAD+2);
				     BtnSetText((BUTTON*)pEb, NULL);
					  SetState(pEb, EB_DRAW);
			     pEb = GOLFindObject(ID_CALL);
				     BtnSetText((BUTTON*)pEb, SPACE_RU);
					  SetState(pEb, EB_DRAW);
			    pEb = GOLFindObject(ID_CHANGE);    
				BtnSetText(pEb, ChangeRUStr);
				SetState(pEb, EB_DRAW);
				pEb = GOLFindObject(ID_BACKSPACE);    
				BtnSetText(pEb, DEL_RU);
				SetState(pEb, EB_DRAW);
        		}
			    languagechange = !languagechange;
			  }
			
				
			  return 1; // process by default	
        default:
            return 1; // process by default
    }
	}
#endif

// Processes messages for the edit box demo screen
WORD MsgItemEditABC(WORD objMsg, OBJ_HEADER* pObj, GOL_MSG* pMsg)
{
EDITBOX* pEb;
SHORT    id;
XCHAR    temp;
static char status = 0;			// status to check if calling, holding or not
static BOOL abcStatus = 0;		// status to check if Large ABC or not

unsigned int ItemAdd;
unsigned int KeyNumber=0;
    id = GetObjID(pObj);
    // If number key is pressed
#ifdef	USE_BTN_MSG_RELEASED
    if(objMsg == BTN_MSG_RELEASED)// BTN_MSG_PRESSED 
#ifdef	USE_adcX_adcy
	if((adcX == -1)||(adcY == -1))
#endif
#else
	if(objMsg == BTN_MSG_PRESSED)//BTN_MSG_RELEASED 
#endif
    {
    	if(id >= ID_KEYPAD)
    		if(id  < ID_KEYPAD+10)
    		{
	    		if (!status) 
	    		{
	    		if(languagechange||languagereturn)
	    			{
	    			if((id  < ID_KEYPAD+9)&&(id > ID_KEYPAD+2))
	    				{
	        		                   pEb = (EDITBOX*)GOLFindObject(ID_EDITBOX1);
					    if(abcStatus)
	        		                   EbAddChar(pEb,0x00B3+id-ID_KEYPAD);

					    else
	                                        EbAddChar(pEb,KEYPADRUSSIAN[id-ID_KEYPAD-4][2]);

	        		           SetState(pEb, EB_DRAW);
	    				}
				if(id == ID_KEYPAD+1)
				{
                 	            pEb = (EDITBOX*)GOLFindObject(ID_EDITBOX1);
	        		     EbAddChar(pEb,0x002d);
	        		     SetState(pEb, EB_DRAW);
				}
				if(id == ID_KEYPAD+2)
				{
                 	            pEb = (EDITBOX*)GOLFindObject(ID_EDITBOX1);
	        		     EbAddChar(pEb,0x002e);
	        		     SetState(pEb, EB_DRAW);
				}				
				if(id == ID_KEYPAD+9)
				{
                 	            pEb = (EDITBOX*)GOLFindObject(ID_EDITBOX1);
	        		     EbAddChar(pEb,0x007C);
	        		     SetState(pEb, EB_DRAW);
				}
					
				}
				else{
	        		pEb = (EDITBOX*)GOLFindObject(ID_EDITBOX1);
	        		EbAddChar(pEb,'0'+id-ID_KEYPAD);
	        		SetState(pEb, EB_DRAW);
					}
	    		}

        		return 1;        
    		}

	    if(id >= ID_ABCPAD)
	    if(id  < ID_ABCPAD+26)
	    	{
	    		if (!status) 
	    		{
	        		pEb = (EDITBOX*)GOLFindObject(ID_EDITBOX1);
					if(languagechange||languagereturn)
						{
	        		if(abcStatus)
	        		{
	        			EbAddChar(pEb,pTemprussian[id-ID_ABCPAD-1][2]);
	        		}
	        		else
	        		{
	        			EbAddChar(pEb,pTempRUSSIAN[id-ID_ABCPAD-1][2]);
	        		}
				
						}	
					else
						{
	        		if(abcStatus)
	        		{
	        			EbAddChar(pEb,'a'+id-ID_ABCPAD);
	        		}
	        		else
	        		{
	        			EbAddChar(pEb,'A'+id-ID_ABCPAD);
	        		}


					}
	        		SetState(pEb, EB_DRAW);
	    		} 
        		return 1;        
    		}
	}

    switch(id){

        case ID_CALL:
            if(objMsg == BTN_MSG_RELEASED)
				if((adcX == -1)||(adcY == -1)){
                pEb = (EDITBOX*)GOLFindObject(ID_EDITBOX1);    
//                EbSetText(pEb, CallingStr);
                EbAddChar(pEb,(XCHAR)' ');	// changed by Spring.Chen
                SetState(pEb, EB_DRAW);
                status = 0;// 1; Changed by Spring.chen
            }
            return 1;

        case ID_BACKSPACE:
            if(objMsg == BTN_MSG_RELEASED)
				if((adcX == -1)||(adcY == -1)){
                if (!status) {
	                pEb = (EDITBOX*)GOLFindObject(ID_EDITBOX1);    
	                EbDeleteChar(pEb);
	                SetState(pEb, EB_DRAW);
	            } 
            }
            return 1;


        case ID_BUTTON_NEXT:
            if(objMsg == BTN_MSG_RELEASED)
				if((adcX == -1)||(adcY == -1)){
            	abcStatus = 0;
           		unsigned char ik;
            	switch(screenState)// ItemState
            	{
            	case DISPLAY_ITEM0:
					for(ik=0;ik<19;ik++)Flashbuff[49+ik] = 0; 
            		CopyChar(&Flashbuff[49],TempbuffStr,19);
            		ItemAdd =0+1;
            		break;
            	case DISPLAY_ITEM1:
					for(ik=0;ik<19;ik++)Flashbuff[68+ik] = 0;  
            		CopyChar(&Flashbuff[68],TempbuffStr,19);
            		ItemAdd =1+1;
            		break;
            	case DISPLAY_ITEM2:
					for(ik=0;ik<19;ik++)Flashbuff[87+ik] = 0; 
            		CopyChar(&Flashbuff[87],TempbuffStr,19);
            		ItemAdd =2+1;
            		break;
            	case DISPLAY_ITEM3:
					for(ik=0;ik<19;ik++)Flashbuff[106+ik] = 0; 
            		CopyChar(&Flashbuff[106],TempbuffStr,19);
            		ItemAdd =3+1;
            		break;
            	case DISPLAY_ITEM4:
					for(ik=0;ik<19;ik++)Flashbuff[125+ik] = 0; 
            		CopyChar(&Flashbuff[125],TempbuffStr,19);
            		ItemAdd =4+1;
            		break;
            	case DISPLAY_ITEM5:
					for(ik=0;ik<19;ik++)Flashbuff[144+ik] = 0; 
            		CopyChar(&Flashbuff[144],TempbuffStr,19);
            		ItemAdd =5+1;
            		break;
            	case DISPLAY_ITEM6:
					for(ik=0;ik<19;ik++)Flashbuff[163+ik] = 0; 
            		CopyChar(&Flashbuff[163],TempbuffStr,19);
            		ItemAdd =6+1;
            		break;
            	case DISPLAY_ITEM7:
					for(ik=0;ik<19;ik++)Flashbuff[182+ik] = 0; 
            		CopyChar(&Flashbuff[182],TempbuffStr,19);
            		ItemAdd =7+1;
            		break;
            	case DISPLAY_ITEM8:
					for(ik=0;ik<19;ik++)Flashbuff[201+ik] = 0; 
            		CopyChar(&Flashbuff[201],TempbuffStr,19);
            		ItemAdd =8+1;
            		break;
            	case DISPLAY_ITEM9:
					for(ik=0;ik<19;ik++)Flashbuff[220+ik] = 0; 
            		CopyChar(&Flashbuff[220],TempbuffStr,19);
            		ItemAdd =9+1;
            		break;
            	default:
            		break;
            	}
            	SaveReadItem(FLASH);
            	for(temp = 0;temp<19;temp++)
              TempbuffStr[temp] = 0;
              for(;ItemAdd<10;ItemAdd++)//ItemMax ItemAdd =0
            	{// 读写下一个地址
            		if(EEPROMReadByte(ID_CHECKBOXADDRESS +ItemAdd))
            		break;
            	}
            	if(ItemAdd<10)//ItemMax
                screenState = 2*ItemAdd+CREATE_ITEM0;// 		// goto ECG demo screen
		else
              { 

			    screenState = CREATE_Alcoholdetect;
          	//	screenState = CREATE_Waittest;
          		ItemAdd =0;
              }
		//for(temp = 0;temp<19;temp++)
            //  TempbuffStr[0] = 0;
            }
            return 1; // process by default

         case ID_BUTTON_BACK:
            if(objMsg == BTN_MSG_RELEASED)
				if((adcX == -1)||(adcY == -1)){
            	abcStatus = 0;
            	switch(screenState)// ItemState
            	{
            	case DISPLAY_ITEM0:
 //           		CopyChar(&Flashbuff[49],TempbuffStr,ItemMax);
            		ItemAdd =0;
            		break;
            	case DISPLAY_ITEM1:
//            		CopyChar(&Flashbuff[68],TempbuffStr,ItemMax);
            		ItemAdd =1;
            		break;
            	case DISPLAY_ITEM2:
//            		CopyChar(&Flashbuff[87],TempbuffStr,ItemMax);
            		ItemAdd =2;
            		break;
            	case DISPLAY_ITEM3:
//            		CopyChar(&Flashbuff[106],TempbuffStr,ItemMax);
            		ItemAdd =3;
            		break;
            	case DISPLAY_ITEM4:
//            		CopyChar(&Flashbuff[125],TempbuffStr,ItemMax);
            		ItemAdd =4;
            		break;
            	case DISPLAY_ITEM5:
//            		CopyChar(&Flashbuff[144],TempbuffStr,ItemMax);
            		ItemAdd =5;
            		break;
            	case DISPLAY_ITEM6:
 //           		CopyChar(&Flashbuff[163],TempbuffStr,ItemMax);
            		ItemAdd =6;
            		break;
            	case DISPLAY_ITEM7:
//            		CopyChar(&Flashbuff[182],TempbuffStr,ItemMax);
            		ItemAdd =7;
            		break;
            	case DISPLAY_ITEM8:
//            		CopyChar(&Flashbuff[201],TempbuffStr,ItemMax);
            		ItemAdd =8;
            		break;
            	case DISPLAY_ITEM9:
//            		CopyChar(&Flashbuff[220],TempbuffStr,ItemMax);
            		ItemAdd =9;
            		break;
            	default:
            		break;
            	}
             for(;ItemAdd>0;ItemAdd--)//ItemMax ItemAdd =0
            	{// 读写上一个地址
            		if(EEPROMReadByte(ID_CHECKBOXADDRESS +ItemAdd-1))
            		{
            			screenState = screenState - 3;
            			break;
            		}
            		else screenState = screenState - 2;
            	}
            	if(ItemAdd==0)//ItemMax
              {
          		screenState = CREATE_MAIN;
          		ItemAdd =0;
              }
            }
            return 1; 

        case ID_BUTTON_HOME:
            if(objMsg == BTN_MSG_RELEASED)
				if((adcX == -1)||(adcY == -1)){
            	abcStatus = 0;
                screenState = CREATE_MAIN; 	// goto list box screen
            }
            return 1; // process by default

        case ID_ABC:
        	if(objMsg == BTN_MSG_RELEASED)
				if((adcX == -1)||(adcY == -1)){
                 if(languagechange)
                 	{
				if(abcStatus)
        		{

                 for(KeyNumber=0;KeyNumber<26;KeyNumber++)
				{
					pEb = (EDITBOX*)GOLFindObject(ID_ABCPAD+KeyNumber);    
				         BtnSetText((BUTTON*)pEb, pTempRUSSIAN[KeyNumber]);
				         SetState(pEb, EB_DRAW);
			        }
				for(KeyNumber=3;KeyNumber<9;KeyNumber++)
               {
				pEb = (EDITBOX*)GOLFindObject(ID_KEYPAD+KeyNumber);    
				         BtnSetText((BUTTON*)pEb, pTempRUSSIAN[KeyNumber+23]);
				         SetState(pEb, EB_DRAW);				
					}
				BtnSetText((BUTTON*)pObj, ABCStr);
        		}
        		else
        		{
        		for(KeyNumber=0;KeyNumber<26;KeyNumber++)
               {
				pEb = (EDITBOX*)GOLFindObject(ID_ABCPAD+KeyNumber);    
				         BtnSetText((BUTTON*)pEb, pTemprussian[KeyNumber]);
				         SetState(pEb, EB_DRAW);				
					}
			  for(KeyNumber=3;KeyNumber<9;KeyNumber++)
               {
				pEb = (EDITBOX*)GOLFindObject(ID_KEYPAD+KeyNumber);    
				         BtnSetText((BUTTON*)pEb, pTemprussian[KeyNumber+23]);
				         SetState(pEb, EB_DRAW);				
					}
				BtnSetText((BUTTON*)pObj, abcStr);
        		}
        			}
        			
				else
					{
			if(abcStatus)
        		{

				for(KeyNumber=0;KeyNumber<26;KeyNumber++)
				{
					pEb = GOLFindObject(ID_ABCPAD+KeyNumber);    
				         BtnSetText(pEb, pTempABC[KeyNumber]);
				         SetState(pEb, EB_DRAW);
					}

				}
			else 
				{

				for(KeyNumber=0;KeyNumber<26;KeyNumber++)
				{
					pEb = GOLFindObject(ID_ABCPAD+KeyNumber);    
				         BtnSetText(pEb, pTempabc[KeyNumber]);
				         SetState(pEb, EB_DRAW);
					}


			}
					        		
            }
		abcStatus = !abcStatus;
					}
            return 1; // process by default
      case ID_CHANGE:
	  	   if(objMsg == BTN_MSG_RELEASED)
				if((adcX == -1)||(adcY == -1)){
					abcStatus=0;
              if(languagechange)
              	{
              	                languagereturn=0;
				for(KeyNumber=0;KeyNumber<26;KeyNumber++)
				{
					pEb = GOLFindObject(ID_ABCPAD+KeyNumber);    
				         BtnSetText(pEb, pTempABC[KeyNumber]);
				         SetState(pEb, EB_DRAW);
					}
			   for(KeyNumber=0;KeyNumber<10;KeyNumber++)
               {
				pEb = GOLFindObject(ID_KEYPAD+KeyNumber);    
				         BtnSetText(pEb, pTempXchar[KeyNumber]);
				         SetState(pEb, EB_DRAW);				
					}
			     pEb = GOLFindObject(ID_CALL);
				     BtnSetText((BUTTON*)pEb, SPACE_EN);
					  SetState(pEb, EB_DRAW);
			    pEb = GOLFindObject(ID_CHANGE);    
				BtnSetText(pEb, ChangeENStr);
				SetState(pEb, EB_DRAW);
				pEb = GOLFindObject(ID_BACKSPACE);    
				BtnSetText(pEb, DEL_EN);
				SetState(pEb, EB_DRAW);
              	}
			  else
			  	{

        		for(KeyNumber=0;KeyNumber<26;KeyNumber++)
               {
				pEb = GOLFindObject(ID_ABCPAD+KeyNumber);    
				         BtnSetText(pEb, pTempRUSSIAN[KeyNumber]);
				         SetState(pEb, EB_DRAW);				
					}
			  for(KeyNumber=3;KeyNumber<10;KeyNumber++)
               {
				pEb = GOLFindObject(ID_KEYPAD+KeyNumber);    
				         BtnSetText(pEb, pTempRUSSIAN[KeyNumber+23]);
				         SetState(pEb, EB_DRAW);				
					}
                 pEb = GOLFindObject(ID_KEYPAD);
				     BtnSetText((BUTTON*)pEb, NULL);
					  SetState(pEb, EB_DRAW);
			     pEb = GOLFindObject(ID_KEYPAD+1);
				     BtnSetText((BUTTON*)pEb, pTempRUSSIAN[33]);
					  SetState(pEb, EB_DRAW);		  
		 	     pEb = GOLFindObject(ID_KEYPAD+2);
				     BtnSetText((BUTTON*)pEb, pTempRUSSIAN[34]);
					  SetState(pEb, EB_DRAW);
			     pEb = GOLFindObject(ID_CALL);
				     BtnSetText((BUTTON*)pEb, SPACE_RU);
					  SetState(pEb, EB_DRAW);
			    pEb = GOLFindObject(ID_CHANGE);    
				BtnSetText(pEb, ChangeRUStr);
				SetState(pEb, EB_DRAW);
				pEb = GOLFindObject(ID_BACKSPACE);    
				BtnSetText(pEb, DEL_RU);
				SetState(pEb, EB_DRAW);
        		}
			    languagechange = !languagechange;
			  }
			
				
			  return 1; // process by default	
        default:
            return 1; // process by default
    }
	}
void CreateAgent(void)
{

	GOLFree();   // free memory for the objects in the previous linked list and start new list

	CreatePage(AgentStr);	// CreatePage("Setting");
	
    BtnCreate(ID_BUTTON1, 				// button ID 
              25,60,
              95,130,         	// dimension
              0,
              BTN_DRAW,//|BTN_DISABLED,//BTN_HIDE, 				// will be dislayed after creation 
              NULL,//(void*)&Picrecord,          // use bitmap
              DeleteStr,//HomeStr, 					// "HOME", 	    // text
              alt4Scheme);	//alt4Scheme            // alternative GOL scheme 



    BtnCreate(ID_BUTTON2,             	// button ID 
              145,60,
              215,130,         	// dimension
              0,					   	// set radius 
              BTN_DRAW,//|BTN_DISABLED,//|BTN_PRESSED,  	// draw a vertical capsule button
              							// that is initially pressed
              NULL,//(void*)&PicGPS,                    	// no bitmap
              AdjustStr,//DemarWendStr,//ThicknessStr, //NULL,//OffStr, 					// "OFF",      	// text
              alt4Scheme);            	// use alternate scheme 	

    BtnCreate(ID_BUTTON3,             	// button ID 
              25,190,
              95,260,         	// dimension
              0,					   	// set radius 
              BTN_DRAW,//|BTN_DISABLED,//|BTN_PRESSED,  	// draw a vertical capsule button
              							// that is initially pressed
              NULL,//(void*)&PicGPS,                    	// no bitmap
              PasswordStr,//DemarWendStr,//ThicknessStr, //NULL,//OffStr, 					// "OFF",      	// text
              alt4Scheme);            	// use alternate scheme 

          
    BtnCreate(ID_BUTTON4,             	// button ID 
              145,190,
              215,260,         	// dimension
              0,					   	// set radius 
              BTN_DRAW,//|BTN_DISABLED,//|BTN_PRESSED,  	// draw a vertical capsule button
              							// that is initially pressed
              NULL,//(void*)&PicGPS,                    	// no bitmap
              caladjustBTNstr,//DemarWendStr,//ThicknessStr, //NULL,//OffStr, 					// "OFF",      	// text
              alt4Scheme);            	// use alternate scheme 	
			  

    BtnCreate(ID_BUTTON5,             	// button ID 
              98,135,
              142,185,         	// dimension
              10,					   	// set radius 
              BTN_DRAW,//|BTN_DISABLED,//|BTN_PRESSED,  	// draw a vertical capsule button
              							// that is initially pressed
              NULL,//(void*)&PicGPS,                    	// no bitmap
              IDStr,//DemarWendStr,//ThicknessStr, //NULL,//OffStr, 					// "OFF",      	// text
              alt4Scheme);            	// use alternate scheme 
            
}


WORD MsgAgent(WORD objMsg, OBJ_HEADER* pObj){
	OBJ_HEADER* pOtherRbtn;

    switch(GetObjID(pObj)){

        case ID_BUTTON_NEXT:
            if(objMsg == BTN_MSG_RELEASED){
               screenState = CREATE_MAIN;// goto check box demo screen
            }
            return 1; 							// process by default

        case ID_BUTTON_BACK:
            if(objMsg == BTN_MSG_RELEASED){
                screenState = CREATE_MAIN; 		// goto ECG demo screen
            }
            return 1; 							// process by default
        case ID_BUTTON_HOME:
            if(objMsg == BTN_MSG_RELEASED){
 //           	abcStatus = 0;
                screenState = CREATE_MAIN; 	// goto list box screen
            }
            return 1;

        case ID_BUTTON1:
            if(objMsg == BTN_MSG_RELEASED){
				
				Settingposition=0xcc;
				dif=1;
				screenState = CREATE_DELETE;
				YesNoNumber = 2;// 删除记?

            }
            return 1; 							// process by default

        case ID_BUTTON2:
            if(objMsg == BTN_MSG_RELEASED){

                screenState = CREATE_DEMARCATE; 		// goto ECG demo screen         
            }
            return 1; 							// process by default

	    case ID_BUTTON3:
        	if(objMsg == BTN_MSG_RELEASED){
			screenState = CREATE_Setpassword;
            }

            return 1;  							// process by default


        case ID_BUTTON4:

			if(objMsg == BTN_MSG_RELEASED)//BTN_MSG_PRESSED)
			screenState = CREATE_Caladjust;

		    return 1; 							// Do not process by default


        case ID_BUTTON5:

			if(objMsg == BTN_MSG_RELEASED)//BTN_MSG_PRESSED)
			screenState = CREATE_DISPLAYID;

		    return 1; 
/*
	    case ID_BUTTON7:
            if(objMsg == BTN_MSG_RELEASED){
		       screenState = CREATE_PASSWORD;			// goto Demarcate demo screen
            }
            return 1; 							// process by default

        case ID_BUTTON8:
            if(objMsg == BTN_MSG_RELEASED){
                screenState = CREATE_CYC;//CREATE_KSETTING;//CREATE_DEMARCATE; 		// goto Download demo screen
            }
            return 1; 							// process by default

        case ID_BUTTON9:
            if(objMsg == BTN_MSG_RELEASED){
              screenState = CREATE_DEMARCATE; 		// goto anyother model demo screen
            }
            return 1;
*/
        default:
            return 1; 							// process by default
    }
}
void CreateMaster(void)
{

	GOLFree();   // free memory for the objects in the previous linked list and start new list

    Settingposition=0xbb;
	CreatePage(MasterStr);	// CreatePage("Setting");
    BtnCreate(ID_BUTTON1, 				// button ID 
              MAINSTARTX+0*(MAINCHARSIZE+MAINSIZEX),MAINSTARTY,
              MAINSTARTX+1*MAINCHARSIZE,MAINSTARTY+1*MAINCHARSIZE,           	// dimension
              0,					   	// set radius 
              BTN_DRAW,  			   	// draw a beveled button
              NULL,//(void*)&Pictest,                    	// no bitmap
              BlowTimePressStr,//ButtonStr, 				// "Button",     	// text
              alt4Scheme);              	// use alternate scheme

    BtnCreate(ID_BUTTON2, 				// button ID 
              MAINSTARTX+1*(MAINCHARSIZE+MAINSIZEX),MAINSTARTY,
              MAINSTARTX+2*MAINCHARSIZE+1*MAINSIZEX,MAINSTARTY+1*MAINCHARSIZE,         	// dimension
              0,
              BTN_DRAW,//|BTN_DISABLED,//BTN_HIDE, 				// will be dislayed after creation 
              NULL,//(void*)&Picrecord,          // use bitmap
              ThicknessStr,//HomeStr, 					// "HOME", 	    // text
              alt4Scheme);	//alt4Scheme            // alternative GOL scheme 

    BtnCreate(ID_BUTTON3,             	// button ID 
              MAINSTARTX+2*(MAINCHARSIZE+MAINSIZEX),MAINSTARTY,
              MAINSTARTX+3*MAINCHARSIZE+2*MAINSIZEX,MAINSTARTY+1*MAINCHARSIZE,           	// dimension
              0,					   	// set radius 
              BTN_DRAW,//|BTN_DISABLED,//|BTN_TOGGLE,   	// draw a vertical capsule button 
              						   	// that has a toggle behavior
              NULL,                    	// no bitmap
              inspection, //PrintStr,//LowStr, 					// "LO",       	// text
              alt4Scheme);           	// use alternate scheme

BtnCreate(ID_BUTTON4,             	// button ID 
              MAINSTARTX+0*(MAINCHARSIZE+MAINSIZEX),MAINSTARTY+1*MAINCHARSIZE+MAINSIZEY,
              MAINSTARTX+1*MAINCHARSIZE+0*MAINSIZEX,MAINSTARTY+2*MAINCHARSIZE+MAINSIZEY,           	// dimension
              0,					   	// set radius 
              BTN_DRAW,//|BTN_DISABLED, 			   	// draw a vertical capsule button
              NULL,//(void*)&Picsetting,                    	// no bitmap
              DemarcateStr,//MemoryStr,//OnStr, 					// "ON",		// text
              alt4Scheme);             // use alternate scheme 

    BtnCreate(ID_BUTTON5,             	// button ID 
              MAINSTARTX+1*(MAINCHARSIZE+MAINSIZEX),MAINSTARTY+1*MAINCHARSIZE+MAINSIZEY,
              MAINSTARTX+2*MAINCHARSIZE+MAINSIZEX,MAINSTARTY+2*MAINCHARSIZE+MAINSIZEY,         	// dimension
              0,					   	// set radius 
              BTN_DRAW,//|BTN_DISABLED,//|BTN_PRESSED,  	// draw a vertical capsule button
              NULL,//(void*)&PicGPS,                    	// no bitmap
              ItemStr,//DemarWendStr,//ThicknessStr, //NULL,//OffStr, 					// "OFF",      	// text
              alt4Scheme);            	// use alternate scheme 	

    BtnCreate(ID_BUTTON6, 				// button ID 
              MAINSTARTX+2*(MAINCHARSIZE+MAINSIZEX),MAINSTARTY+1*MAINCHARSIZE+MAINSIZEY,
              MAINSTARTX+3*MAINCHARSIZE+2*MAINSIZEX,MAINSTARTY+2*MAINCHARSIZE+MAINSIZEY,          	// dimension
              0,
              BTN_DRAW,//BTN_DRAW|BTN_TEXTLEFT 	// will be dislayed after creation with text              
              NULL,//(void*)&bulboff,          // use bitmap
              ResetStr,//KSettingStr,//TimeStr,//NULL,//OffBulbStr, 				// text
              alt4Scheme);	            // alternative GOL scheme 
/*
	BtnCreate(ID_BUTTON7,             	// button ID 
              MAINSTARTX+0*(MAINCHARSIZE+MAINSIZEX),MAINSTARTY+2*(MAINCHARSIZE+MAINSIZEY),
              MAINSTARTX+2*MAINCHARSIZE+1*MAINSIZEX,MAINSTARTY+3*MAINCHARSIZE+2*MAINSIZEY,           	// dimension
              0,					   	// set radius 
              BTN_DRAW,//|BTN_DISABLED, 			   	// draw a vertical capsule button
              NULL,//(void*)&Picsetting,                    	// no bitmap
              Bluetoothstr,//NULL,//CheckScreenStr,//OnStr, 					// "ON",		// text
              alt4Scheme);  //  greenScheme         // use alternate scheme 
*/
/*
	BtnCreate(ID_BUTTON8,             	// button ID 
              MAINSTARTX+1*(MAINCHARSIZE+MAINSIZEX),MAINSTARTY+2*(MAINCHARSIZE+MAINSIZEY),
              MAINSTARTX+2*MAINCHARSIZE+1*MAINSIZEX,MAINSTARTY+3*MAINCHARSIZE+2*MAINSIZEY,           	// dimension
              0,					   	// set radius 
              BTN_DRAW, 			   	// draw a vertical capsule button
              NULL,//(void*)&Picsetting,                    	// no bitmap
              inspection,//CalPerStr,//KSettingStr,//DemarWendStr,//CheckScreenStr,//OnStr, 					// "ON",		// text
              alt4Scheme);  //  greenScheme         // use alternate scheme 
          
   BtnCreate(ID_BUTTON9,             	// button ID 
              MAINSTARTX+2*(MAINCHARSIZE+MAINSIZEX),MAINSTARTY+2*(MAINCHARSIZE+MAINSIZEY),
              MAINSTARTX+3*MAINCHARSIZE+2*MAINSIZEX,MAINSTARTY+3*MAINCHARSIZE+2*MAINSIZEY,           	// dimension
              0,					   	// set radius 
              BTN_DRAW, 			   	// draw a vertical capsule button
              NULL,//(void*)&Picsetting,                    	// no bitmap
              PasswordStr,//KSettingStr,//DemarWendStr,//CheckScreenStr,//OnStr, 					// "ON",		// text
              alt4Scheme);  //  greenScheme         // use alternate scheme 
              */

}
WORD MsgMaster(WORD objMsg, OBJ_HEADER* pObj){
	OBJ_HEADER* pOtherRbtn;

    switch(GetObjID(pObj)){

        case ID_BUTTON_NEXT:
            if(objMsg == BTN_MSG_RELEASED){
               screenState = CREATE_MAIN;// goto check box demo screen
            }
            return 1; 							// process by default

        case ID_BUTTON_BACK:
            if(objMsg == BTN_MSG_RELEASED){
                screenState = CREATE_MAIN; 		// goto ECG demo screen
            }
            return 1; 							// process by default
        case ID_BUTTON_HOME:
            if(objMsg == BTN_MSG_RELEASED){
 //           	abcStatus = 0;
                screenState = CREATE_MAIN; 	// goto list box screen
            }
            return 1;

        case ID_BUTTON1:
            if(objMsg == BTN_MSG_RELEASED){
				dif=1;
                screenState = CREATE_BLOW_PRESS; 		// goto ECG demo screen
            }
            return 1; 							// process by default

        case ID_BUTTON2:
            if(objMsg == BTN_MSG_RELEASED){
					dif=1;
				screenState = CREATE_THICKNESS;

            }
            return 1; 							// process by default

	case ID_BUTTON3:
        	if(objMsg == BTN_MSG_RELEASED){

				 screenState = CREATE_ChooseCal;//

            }
     
            return 1;  							// process by default

        case ID_BUTTON4:

			if(objMsg == BTN_MSG_RELEASED)//BTN_MSG_PRESSED)
               screenState =CREATE_Choosegas;
		    return 1; 							// Do not process by default

        case ID_BUTTON5:
        
			if(objMsg == BTN_MSG_PRESSED) {

				dif=1;
			    screenState = CREATE_ITEM;//				

				/*
				Settingposition=0xbb;
				dif=1;
				screenState = CREATE_DELETE;
				//screenState = CREATE_ScreenPASSWORD;
				YesNoNumber = 2;// 删除记录
				*/
			}
		    return 1; 							// Do not process by default

        case ID_BUTTON6:
            if(objMsg == BTN_MSG_RELEASED){
				
				Settingposition=0xbb;
				dif=1;
                screenState = CREATE_RESET; 		// goto ECG demo screen
                YesNoNumber = 1;//Default setting

            }
            return 1; 							// process by default

	    case ID_BUTTON7:
            if(objMsg == BTN_MSG_RELEASED){

				//Settingposition=0xbb;
				
		       screenState =CREATE_BlueTooth; //CREATE_PASSWORD;			// goto Demarcate demo screen
            }
            return 1; 							// process by default

/*
        case ID_BUTTON8:
            if(objMsg == BTN_MSG_RELEASED){
				
                screenState = CREATE_CYC;//CREATE_KSETTING;//CREATE_DEMARCATE; 		// goto Download demo screen
            }
            return 1; 							// process by default


        case ID_BUTTON9:
            if(objMsg == BTN_MSG_RELEASED){
			
              screenState = CREATE_Setpassword; 		// goto anyother model demo screen
            }
            return 1;
*/
        default:
            return 1; 							// process by default
    }
}
void CreateSetpassword(void)
{

	GOLFree();   // free memory for the objects in the previous linked list and start new list

	CreatePage(Setpasswordstr);	// CreatePage("Setting");

    BtnCreate(ID_BUTTON1, 				// button ID 
              MAINSTARTX+0*(MAINCHARSIZE+MAINSIZEX)-10,MAINSTARTY+30,
              MAINSTARTX+3*MAINCHARSIZE+3*MAINSIZEX-10,MAINSTARTY+1*MAINCHARSIZE+30,         	// dimension
              0,
              BTN_DRAW,//|BTN_DISABLED,//BTN_HIDE, 				// will be dislayed after creation 
              NULL,//(void*)&Picrecord,          // use bitmap
              Engineer2,//HomeStr, 					// "HOME", 	    // text
              alt4Scheme);	//alt4Scheme            // alternative GOL scheme 



    BtnCreate(ID_BUTTON2,             	// button ID 
              MAINSTARTX+0*(MAINCHARSIZE+MAINSIZEX)-10,MAINSTARTY+1*MAINCHARSIZE+MAINSIZEY+40,
              MAINSTARTX+3*MAINCHARSIZE+3*MAINSIZEX-10,MAINSTARTY+2*MAINCHARSIZE+MAINSIZEY+40,         	// dimension
              0,					   	// set radius 
              BTN_DRAW,//|BTN_DISABLED,//|BTN_PRESSED,  	// draw a vertical capsule button
              							// that is initially pressed
              NULL,//(void*)&PicGPS,                    	// no bitmap
              Engineer1,//DemarWendStr,//ThicknessStr, //NULL,//OffStr, 					// "OFF",      	// text
              alt4Scheme);            	// use alternate scheme


}

WORD MsgSetpassword(WORD objMsg, OBJ_HEADER* pObj){

    switch(GetObjID(pObj)){

        case ID_BUTTON_NEXT:
            if(objMsg == BTN_MSG_RELEASED){
               screenState = CREATE_AGENT;// goto check box demo screen
            }
            return 1; 							// process by default

        case ID_BUTTON_BACK:
            if(objMsg == BTN_MSG_RELEASED){
                screenState = CREATE_AGENT; 		// goto ECG demo screen
            }
            return 1; 							// process by default
        case ID_BUTTON_HOME:
            if(objMsg == BTN_MSG_RELEASED){
                screenState = CREATE_MAIN; 	// goto list box screen
            }
            return 1;

		case ID_BUTTON1:
            if(objMsg == BTN_MSG_RELEASED){
				Setpasswordposition=0xaa;
                screenState = CREATE_InputSetpassword1; 	// goto list box screen
            }
            return 1;

		case ID_BUTTON2:
            if(objMsg == BTN_MSG_RELEASED){
				Setpasswordposition=0xbb;
                screenState = CREATE_InputSetpassword1; 	// goto list box screen
            }
            return 1;

    	}

}

void CreateOutcomeSetpassword(void)
{

	GOLFree();   // 
	CreatePage(PasswordSettingStr);	// 

	
	
  if(PDset)
  	{

	// alt5Scheme->TextColor0 = BRIGHTGREEN;
	// alt5Scheme->Color0 = BRIGHTGREEN;
	// alt5Scheme->Color1 = BRIGHTGREEN;
		
	 StCreate(ID_STATICTEXT3,           		// ID 
              5,2*MAINSTARTY,
              235,MAINSTARTY+1*MAINCHARSIZE+MAINSIZEY+60,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              SettingsuccessStr, 				// "TEST", 	// text
              alt5Scheme);   //mainScheme   alt2Scheme             // use alternate scheme

  	}
   else  
   	{

	// alt5Scheme->TextColor0 = BRIGHTRED;
	 
     StCreate(ID_STATICTEXT3,           		// ID 
              5,2*MAINSTARTY,
              235,MAINSTARTY+1*MAINCHARSIZE+MAINSIZEY+60,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              NotmatchStr, 				// "TEST", 	// text
              alt5Scheme);   //mainScheme   alt2Scheme             // use alternate scheme


   }


}

WORD MsgOutcomeSetpassword(WORD objMsg, OBJ_HEADER* pObj){


    switch(GetObjID(pObj)){

        case ID_BUTTON_NEXT:
            if(objMsg == BTN_MSG_RELEASED){
				alt5Scheme->TextColor0 = BRIGHTBLUE;
               screenState = CREATE_Setpassword;// 
            }
            return 1; 							// process by default

        case ID_BUTTON_BACK:
            if(objMsg == BTN_MSG_RELEASED){
				alt5Scheme->TextColor0 = BRIGHTBLUE;
                screenState = CREATE_MAIN; 		// 
            }
            return 1; 							// process by default
        case ID_BUTTON_HOME:
            if(objMsg == BTN_MSG_RELEASED){
				alt5Scheme->TextColor0 = BRIGHTBLUE;
                screenState = CREATE_MAIN; 	// 
            }
            return 1;


    	}

}

void CreateYesNo(XCHAR* YesOrNo)
{

	GOLFree();   // free memory for the objects in the previous linked list and start new list

	CreatePage(YesOrNo);	// CreatePage("Setting");
if(YesNoNumber == 1)
{
    BtnCreate(ID_BUTTON2, 				// button ID 
              MAINSTARTX+1*(MAINCHARSIZE+MAINSIZEX),MAINSTARTY+20,
              MAINSTARTX+2*MAINCHARSIZE+1*MAINSIZEX,MAINSTARTY+1*MAINCHARSIZE+20,         	// dimension
              0,
              BTN_DRAW,//|BTN_DISABLED,//BTN_HIDE, 				// will be dislayed after creation 
              NULL,//(void*)&Picrecord,          // use bitmap
              YesStr,//HomeStr, 					// "HOME", 	    // text
              altScheme);	//alt4Scheme            // alternative GOL scheme 

	BtnCreate(ID_BUTTON5,             	// button ID 
              MAINSTARTX+1*(MAINCHARSIZE+MAINSIZEX),MAINSTARTY+1*MAINCHARSIZE+MAINSIZEY+20,
              MAINSTARTX+2*MAINCHARSIZE+MAINSIZEX,MAINSTARTY+2*MAINCHARSIZE+MAINSIZEY+20,         	// dimension
              0,					   	// set radius 
              BTN_DRAW,//|BTN_DISABLED,//|BTN_PRESSED,  	// draw a vertical capsule button
              NULL,//(void*)&PicGPS,                    	// no bitmap
              NoStr,//ThicknessStr, //NULL,//OffStr, 					// "OFF",      	// text
              altScheme);            	// use alternate scheme 	
}
else if(YesNoNumber == 2)
{
    BtnCreate(ID_BUTTON2, 				// button ID 
              MAINSTARTX+1*(MAINCHARSIZE+MAINSIZEX)-70,MAINSTARTY+20,
              MAINSTARTX+2*MAINCHARSIZE+1*MAINSIZEX+70,MAINSTARTY+1*MAINCHARSIZE+20,         	// dimension
              0,
              BTN_DRAW,//|BTN_DISABLED,//BTN_HIDE, 				// will be dislayed after creation 
              NULL,//(void*)&Picrecord,          // use bitmap
              DelectAllRecords,//HomeStr, 					// "HOME", 	    // text
              altScheme);	//alt4Scheme            // alternative GOL scheme 

	BtnCreate(ID_BUTTON5,             	// button ID 
              MAINSTARTX+1*(MAINCHARSIZE+MAINSIZEX)-70,MAINSTARTY+1*MAINCHARSIZE+MAINSIZEY+30,
              MAINSTARTX+2*MAINCHARSIZE+MAINSIZEX+70,MAINSTARTY+2*MAINCHARSIZE+MAINSIZEY+30,         	// dimension
              0,					   	// set radius 
              BTN_DRAW,//|BTN_DISABLED,//|BTN_PRESSED,  	// draw a vertical capsule button
              NULL,//(void*)&PicGPS,                    	// no bitmap
              Cancel,//ThicknessStr, //NULL,//OffStr, 					// "OFF",      	// text
              altScheme);            	// use alternate scheme 




}
}


WORD MsgYesNo(WORD objMsg, OBJ_HEADER* pObj){
	OBJ_HEADER* pOtherRbtn;

    switch(GetObjID(pObj)){

        case ID_BUTTON_NEXT:
            if(objMsg == BTN_MSG_RELEASED){
//               screenState = CREATE_MAIN;// goto check box demo screen
            }
            return 1; 							// process by default

        case ID_BUTTON_BACK:
            if(objMsg == BTN_MSG_RELEASED){
//                screenState = CREATE_MAIN; 		// goto ECG demo screen
            }
            return 1; 							// process by default
        case ID_BUTTON_HOME:
            if(objMsg == BTN_MSG_RELEASED){
//            	abcStatus = 0;
                screenState = CREATE_MAIN; 	// goto list box screen
            }
            return 1;

        case ID_BUTTON2:
            if(objMsg == BTN_MSG_RELEASED){
            	if(YesNoNumber == 1)
            		{
                    DefaultSETTING();
		 screenState = CREATE_Master;			
            		}
            	else if(YesNoNumber == 2)
            	{
            		EEPROMWriteWord(1,EESERIAL_NUMBER);// 写一作为记录的条数开始
            		EEPROMWriteWord(0,Record_over_30000);
			        serialnumber = 1;//EEPROMReadWord(EESERIAL_NUMBER);
			        serialnumberRecord = serialnumber - 1;// 保持记录一致
			     //   EEPROMWriteWord(0, TestTimesDataAddress);
				screenState = CREATE_AGENT;	
            	}
				/*
				if(Settingposition==0xaa)
                screenState = CREATE_SETTING;				// goto slider screen
                else if(Settingposition==0xbb)
					screenState = CREATE_Master;
					*/
            }
            return 1; 							// process by default

        case ID_BUTTON5:
			if(objMsg == BTN_MSG_PRESSED) {
            	if(YesNoNumber == 1)

					screenState = CREATE_Master;
		else if(YesNoNumber == 2)	
			       screenState = CREATE_AGENT;

				}			
		    return 1; 							// Do not process by default

        default:
            return 1; 							// process by default
    }
}

void Createautuooff(void){
    SetColor(BLACK);
    ClearDevice();
    SetFont((void*)&FONTDEFAULT);
    SetColor(WHITE);    
    OutTextXY((GetMaxX()-GetTextWidth((XCHAR*)PowerOffStr,(void*)&GOLFontDefault))>>1,GetMaxY()>>1-50,PowerOffStr);

    SetFont((void*)&BigArial);
    SetColor(RED);

    OutTextXY(100, 200, Countdown);
		/*
	StCreate(ID_PICTURE1,           		// ID 
              positionax,60,
              2*positionmaxx,100,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              Countdown, 				// "TEST", 	// text
              redScheme);        //  mainScheme         // use alternate scheme
              */
    DelayMs(1000);

}

void  CreateBlueTooth()
{

	GOLFree();   // 

	OutPio4;
	
	CreatePage(Bluetoothstr);	//DeleteStr CreatePage("Blow Please");
	
	if(PORTEbits.RE9)
		{
    		StCreate(ID_STATICTEXT3,           		// ID 
              		 4,75,
              		 236,113,         		// dimension
              		 ST_DRAW|ST_CENTER_ALIGN,		// display text
              		 PrintMstr, 				// "TEST", 	// text
              		 alt5Scheme);   //mainScheme   alt2Scheme
          	

    		PST=StCreate(ID_STATICTEXT0,           		// ID 
              		 4,138,
              		 236,181,         		// dimension
             		 ST_DRAW|ST_CENTER_ALIGN,		// display text
              		 Workstr, 				// "TEST", 	// text
              		 alt5Scheme);   //mainScheme   alt2Scheme	
		}

  else {

            StCreate(ID_STATICTEXT3,           		// ID 
                      4,75,
                      236,113,         		// dimension
              		  ST_DRAW|ST_CENTER_ALIGN,		// display text
                      BluetoothMstr, 				// "TEST", 	// text
                      alt5Scheme);   //mainScheme   alt2Scheme
         if(PORTEbits.RE8) 	                 

            PST=StCreate(ID_STATICTEXT0,           		// ID 
                      4,138,
                      236,181,         		// dimension
                      ST_DRAW|ST_CENTER_ALIGN,		// display text
                      Workstr, 				// "TEST", 	// text
                      alt5Scheme);   //mainScheme   alt2Scheme
          else             

            PST=StCreate(ID_STATICTEXT0,           		// ID 
                      4,138,
                      236,181,         		// dimension
                      ST_DRAW|ST_CENTER_ALIGN,		// display text
                      Nonestr, 				// "TEST", 	// text
                      alt5Scheme);   //mainScheme   alt2Scheme


        }	
              
	pbutton=BtnCreate(ID_BUTTON1,             	// button ID 
              		  33,221,206,262,           	// dimension
              		  0,					   	// set radius 
              		  BTN_DRAW,//|BTN_TOGGLE,   	// draw a vertical capsule button              						   	
              		  NULL,                    	// no bitmap
              		  Matchstr,//LowStr, 					// "LO",       	// text
              		  altScheme);           	// use alternate scheme
			  
}

WORD MsgBlueTooth(WORD objMsg, OBJ_HEADER* pObj){


    switch(GetObjID(pObj)){

        case ID_BUTTON_NEXT:
            if(objMsg == BTN_MSG_RELEASED){
				PORTGbits.RG0=0;
				BlueToothcount = 0;
				InPio4;
              screenState = CREATE_SETTING;//CREATE_Master;// 
            }
            return 1; 							// process by default

        case ID_BUTTON_BACK:
            if(objMsg == BTN_MSG_RELEASED){
				PORTGbits.RG0=0;
				BlueToothcount = 0;
				InPio4;
              	screenState = CREATE_SETTING;//CREATE_Master;//
            }
            return 1; 							// process by default

		case ID_BUTTON_HOME:
            if(objMsg == BTN_MSG_RELEASED){

				PORTGbits.RG0=0;
                BlueToothcount = 0;
				InPio4;
                screenState = CREATE_MAIN; 	// 
            }
            return 1;
            
		case  ID_BUTTON1:
		 	if(objMsg == BTN_MSG_RELEASED){
              	PORTGbits.RG0=1;
				BlueToothcount = 0;
			  	SetState(pbutton, BTN_DISABLED);
		  
            }
            return 1;

        default:
            return 1; 							// process by default
    }
}

// Output text message on screen and stop execution
void ErrorTrap(XCHAR* message){
    SetColor(BLACK);
    ClearDevice();
    SetFont((void*)&FONTDEFAULT);
    SetColor(WHITE);    
    OutTextXY((GetMaxX()-GetTextWidth((XCHAR*)message,(void*)&GOLFontDefault))>>1,GetMaxY()>>1,message);
//    while(1);
    DelayMs(1000);
while(POWER_CONTROL==0);
}



void TickInit(void){
#define TICK_PERIOD    16000
    // Initialize Timer4
    TMR4 = 0;
//    T3CONbits.TCKPS = 1;         //Set prescale to 1:8
    PR4 = TICK_PERIOD;
    IFS1bits.T4IF = 0;              //Clear flag
    IEC1bits.T4IE = 1;              //Enable interrupt
    T4CONbits.TON = 1;              //Run timer  
}

