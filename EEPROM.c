/*****************************************************************************
 *
 * Basic access to SPI EEPROM 25LC256.
 *
 *****************************************************************************
 * FileName:        25LC256.c
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
#ifndef	USE_I2C
/************************************************************************
* Function: EEPROMInit                                                  
*                                                                       
* Overview: this function setup SPI and IOs connected to EEPROM
*                                                                       
* Input: none                                                          
*                                                                       
* Output: none
*                                                                       
************************************************************************/
void EEPROMInit()
{
    // Initialize SPI
#ifdef __PIC32MX
    SPI2STAT = 0;
    SPI2CON = 0;
    SPI2BRG = 0;
    SPI2CONbits.MSTEN = 1; 
    SPI2CONbits.CKP = 1;
    SPI2CONbits.SMP = 1;
    SPI2BRG = 1;
    SPI2CONbits.ON = 1;
#else
    SPI2STAT = 0;
    SPI2CON1 = 0x001b;
    SPI2CON1bits.MSTEN = 1; 
    SPI2CON2 = 0;
    SPI2CON1bits.MODE16 = 0;
    SPI2CON1bits.CKE = 0;
    SPI2CON1bits.CKP = 1;
    SPI2CON1bits.SMP = 1;
    SPI2STATbits.SPIEN = 1;
#endif

    // Set IOs directions for EEPROM SPI
    EEPROM_SS_PORT = 1;Nop();
    EEPROM_SS_TRIS = 0;
    Nop();
	Nop();
    EEPROM_SCK_TRIS = 0;Nop();
    EEPROM_SDO_TRIS = 0;
    Nop();
	Nop();
    EEPROM_SDI_TRIS = 1;

// set FLASH CS 
    FLASH1_SS_PORT = 1;Nop();
    FLASH1_SS_TRIS = 0;
    Nop();
	Nop();
    FLASH2_SS_PORT = 1;Nop();
    FLASH2_SS_TRIS = 0;
    Nop();
	Nop();
}

/************************************************************************
* Function SPIPut(BYTE data)                                                       
*                                                                       
* Overview:  this function sends a byte                     
*                                                                       
* Input: byte to be sent
*                                                                       
* Output: none
*                                                                       
************************************************************************/           
void SPIPut(BYTE data)
{
#ifdef __PIC32MX
    // Wait for free buffer
    while(!SPI2STATbits.SPITBE);
    SPI2BUF = data;
    // Wait for a data byte reception
    while(!SPI2STATbits.SPIRBF);
#else
    // Wait for free buffer
    while(SPI2STATbits.SPITBF);
    SPI2BUF = data;
    // Wait for a data byte reception
 while(!SPI2STATbits.SPIRBF);


#endif
}

/************************************************************************
* Macros SPIGet()                                                       
*                                                                       
* Overview:  this macros gets a byte from SPI                      
*                                                                       
* Input: none
*                                                                       
* Output: none
*                                                                       
************************************************************************/           
#define SPIGet() SPI2BUF

/************************************************************************
* Function: void EEPROMWriteByte(BYTE data, WORD address)                                           
*                                                                       
* Overview: this function writes a byte to the address specified
*                                                                       
* Input: data to be written and address
*                                                                       
* Output: none                                 
*                                                                       
************************************************************************/
void EEPROMWriteByte(BYTE data, WORD address)
{
    EEPROMWriteEnable();
    EEPROMSSLow();

    SPIPut(EEPROM_CMD_WRITE);
    SPIGet();

    SPIPut(((WORD_VAL)address).v[1]);
    SPIGet();

    SPIPut(((WORD_VAL)address).v[0]);
    SPIGet();

    SPIPut(data);
    SPIGet();

    EEPROMSSHigh();

    // Wait for write end
    while(EEPROMReadStatus().Bits.WIP);
}

/************************************************************************
* Function: BYTE EEPROMReadByte(WORD address)             
*                                                                       
* Overview: this function reads a byte from the address specified         
*                                                                       
* Input: address                                                     
*                                                                       
* Output: data read
*                                                                       
************************************************************************/
BYTE EEPROMReadByte(WORD address){
BYTE temp;
    EEPROMSSLow();

    SPIPut(EEPROM_CMD_READ);
    SPIGet();

    SPIPut(((WORD_VAL)address).v[1]);
    SPIGet();

    SPIPut(((WORD_VAL)address).v[0]);
    SPIGet();

    SPIPut(0);
    temp = SPIGet();

    EEPROMSSHigh();
    return temp;
}

/************************************************************************
* Function: void EEPROMWriteWord(WODR data, WORD address)                                           
*                                                                       
* Overview: this function writes a 16-bit word to the address specified
*                                                                       
* Input: data to be written and address
*                                                                       
* Output: none                                                         
*                                                                       
************************************************************************/
void EEPROMWriteWord(WORD data, WORD address)
{
    EEPROMWriteByte(((WORD_VAL)data).v[0],address);
    EEPROMWriteByte(((WORD_VAL)data).v[1],address+1);
}

/************************************************************************
* Function: WORD EEPROMReadWord(WORD address)             
*                                                                       
* Overview: this function reads a 16-bit word from the address specified         
*                                                                       
* Input: address                                                     
*                                                                       
* Output: data read
*                                                                       
************************************************************************/
WORD EEPROMReadWord(WORD address){
WORD_VAL temp;

    temp.v[0] = EEPROMReadByte(address);
    temp.v[1] = EEPROMReadByte(address+1);

    return temp.Val;
}

/************************************************************************
* Function: EEPROMWriteEnable()                                         
*                                                                       
* Overview: this function allows write/erase EEPROM. Must be called  
* before every write/erase command.                                         
*                                                                       
* Input: none                                                          
*                                                                       
* Output: none                                 
*                                                                       
************************************************************************/
void EEPROMWriteEnable(){
    EEPROMSSLow();
    SPIPut(EEPROM_CMD_WREN);
    SPIGet();
    EEPROMSSHigh();
}

/************************************************************************
* Function: EEPROMReadStatus()                                          
*                                                                       
* Overview: this function reads status register
*                                                                       
* Input: none                                                          
*                                                                       
* Output: status register value
*                                                                       
************************************************************************/
union _EEPROMStatus_ EEPROMReadStatus(){
BYTE temp;

    EEPROMSSLow();
    SPIPut(EEPROM_CMD_RDSR);
    SPIGet();

    SPIPut(0);
    temp = SPIGet();
    EEPROMSSHigh();

    return (union _EEPROMStatus_)temp;
}

/************************************************************************
* Function: BYTE EEPROMWriteArray(DWORD address, BYTE* pData, nCount)
*                                                                       
* Overview: this function writes a data array at the address specified
*                                                                       
* Input: flash memory address, pointer to the data array, data number
*                                                                       
* Output: return 1 if the operation was successfull
*                                                                     
************************************************************************/
BYTE EEPROMWriteArray(DWORD address, BYTE* pData, WORD nCount)
{
DWORD_VAL addr;
BYTE*     pD;
WORD      counter;


    addr.Val = address;
    pD       = pData;

    // WRITE

    EEPROMWriteEnable();
    EEPROMSSLow();

    SPIPut(EEPROM_CMD_WRITE);
    SPIGet();

    SPIPut(addr.v[1]);
    SPIGet();
    
    SPIPut(addr.v[0]);
    SPIGet();

    for(counter=0; counter<nCount; counter++)
    {
        SPIPut(*pD++);
        SPIGet();
        addr.Val++;

        // check for page rollover
        if((addr.v[0]&0x7f) == 0){

            EEPROMSSHigh();

            // Wait for completion of the write operation
            while(EEPROMReadStatus().Bits.WIP);
                        
            // Start writing of the next page
    
            EEPROMWriteEnable();
            EEPROMSSLow();

            SPIPut(EEPROM_CMD_WRITE);
            SPIGet();

            SPIPut(addr.v[1]);
            SPIGet();

            SPIPut(addr.v[0]);
            SPIGet();
        }
    }


    EEPROMSSHigh();

    // Wait for write end
    while(EEPROMReadStatus().Bits.WIP);
    
    // VERIFY

    for(counter=0; counter<nCount; counter++)
    {              
        if(*pData != EEPROMReadByte(address))
            return 0;
        pData++;
        address++;
    }

    return 1;
}

/************************************************************************
* Function: void EEPROMReadArray(WORD address, BYTE* pData, nCount)
*                                                                       
* Overview: this function reads data into buffer specified
*                                                                       
* Input: flash memory address, pointer to the data buffer, data number
*                                                                       
************************************************************************/
void EEPROMReadArray(WORD address, BYTE* pData, WORD nCount)
{

    EEPROMSSLow();

    SPIPut(EEPROM_CMD_READ);
    SPIGet();

    SPIPut(((WORD_VAL)address).v[1]);
    SPIGet();

    SPIPut(((WORD_VAL)address).v[0]);
    SPIGet();

    while(nCount--){
        SPIPut(0);
        *pData++ = SPIGet();
    }

    EEPROMSSHigh();
}
void delay(WORD I)
{
	DelayMs(I);
/*	unsigned int j;
	while(I--)
		for(j = 0; j<255; j++)
			Nop();*/
}


#else
#if	0
#define uch unsigned char
#define unint unsigned int
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

void EEPROMInit()
{
//	PORTA=0XFF;
//    TRISA=0;
	TRISAbits.TRISA2=0;         //置输出
   TRISAbits.TRISA3=0;
}


void delay(void)        //延时
{
     unint m;
     for(m=0;m<0xffff;m++)
     continue;
}   

void start (void)    //  IIC开始
{
   TRISAbits.TRISA2=0;         //置输出
   TRISAbits.TRISA3=0;
   SDA=1;        
   adc_delay(1);
   SCL=1;            //在至少4u秒期间SDA从高电平到低电平
   adc_delay(1);
   adc_delay(1);
   adc_delay(1);
   SDA=0;
   adc_delay(1);
   adc_delay(1);
   adc_delay(1);
   SCL=0;
   adc_delay(1);
   adc_delay(1);
  }

void stop(void)      // IIC结束
{  TRISAbits.TRISA2=0;
    TRISAbits.TRISA3=0;         //置输出
    SDA=0;
    adc_delay(1);
    SCL=1;           //在至少4u秒期间SDA从低电平到高电平
    adc_delay(1);
    adc_delay(1);
    adc_delay(1);
    SDA=1;
    adc_delay(1);
    adc_delay(1);
    adc_delay(1);
    SCL=0;
    adc_delay(1);
    adc_delay(1);
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
          adc_delay(1);
          adc_delay(1);
          adc_delay(1);
          adc_delay(1);
          adc_delay(1);
          SCL=0; 
          data=data<<1;
          bitcount--;
          } while(bitcount);
           TRISAbits.TRISA3=1 ;                  //释放总线等待应答
           adc_delay(1);
           adc_delay(1);
           adc_delay(1);
           adc_delay(1);
           adc_delay(1);   
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
     adc_delay(1);
     adc_delay(1);   
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
    adc_delay(1);
    adc_delay(1);
    adc_delay(1);
    adc_delay(1);
    adc_delay(1);
    SCL=0;
}
    
  void nack(void)      //停止继续读
  { 
    SDA=1;
    SCL=0;
    SCL=1;
    adc_delay(1);
    adc_delay(1);
    adc_delay(1);
    adc_delay(1);
    adc_delay(1);
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
void EEPROMWriteByte(BYTE data, WORD address)
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

BYTE EEPROMReadByte(WORD address)
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

WORD EEPROMReadWord(WORD address)
{
	WORD_VAL temp;

    temp.v[0] = EEPROMReadByte(address);
    temp.v[1] = EEPROMReadByte(address+1);

//	rdfromROM(&temp.v[0],address,1);
//address++;
//rdfromROM(&temp.v[1],address,1);
    return temp.Val;
}

void EEPROMReadArray(WORD address, BYTE* pData, WORD nCount)
{
/*
	while(nCount--)
	{
        	*pData++ = EEPROMReadWord(address);
    	}
	*/
	rdfromROM(pData,address,nCount);

}

#else
unsigned int I2C_COUNTER,jhm98,NByte;
unsigned char I2C_ERROR;

unsigned int WriteTable[16] = {0x00,0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x99,0xaa,0xbb,0xcc,0xdd,0xee,0xff};//{"well co Ltd. lcs"};
unsigned int ReadTable[16];
#if	0
unsigned int ControlByteW = 0xa0;	// 控制字:写准备(器件地址000)
unsigned int ControlByteR = 0xa1;	//控制字:读准备(器件地址000)
#else
unsigned int ControlByteW = 0xa2;	// 控制字:写准备(器件地址000)
unsigned int ControlByteR = 0xa3;	//控制字:读准备(器件地址000)
#endif
unsigned int AddressH = 0x00;		//数据地址高7位
unsigned int AddressL = 0x30;		// 数据地址低8位
unsigned long int i = 0;

void delay(WORD I)
{
	unsigned int j;
	while(I--)
		for(j = 0; j<255; j++)
			Nop();
}

void EEPROMInit()
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
     I2C2CONbits.I2CEN = 1;		// 启动I2C模块
     I2C2BRG = 0X064;			// I2C波特率
     
}
/*
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

		EEPROMInit();
	}
}*/

/* generate the start condition            *
 *  it is called by I2C_read and I2C_write */
void I2C_start(void)
{
     //clear the collision flag
     I2C2STATbits.IWCOL = 0;

     //initiate the stop condition
     I2C2CONbits.PEN = 1;
 //    jhm98 = 0;
     //ensure the stop condition is complete
     while (I2C2CONbits.PEN == 1)//=0 after STOP xulie
     {
;//	jhm98++;
//	if(jhm98 > 0x320) break;
     }
     //initiate the START condition
     I2C2CONbits.SEN = 1;
//     jhm98 = 0;
     //ensure the START condition is completion
     while (I2C2CONbits.SEN == 1) //=0 after START xulie
     {
;//     	jhm98++;
//     	if(jhm98 > 0x320) break;
     }
//     I2C_ERROR = 0xaa;         
}

void I2C_Stop(void)
{
	I2C2CONbits.PEN = 1;						//发送停止位
	while(I2C2CONbits.PEN == 1);				//等待停止操作完成
}

void SetEEPromSelect(BYTE EEPROM_Select)
{
	switch(EEPROM_Select)
		{
			case EEPROM_S2:
			ControlByteW = 0xa1;
			ControlByteR = 0xa2;
			break;

			case EEPROM_PCF8563:
			ControlByteW = 0xa2;
			ControlByteR = 0xa3;
			break;

			case EEPROM_S1:
			default:
			ControlByteW = 0xa0;
			ControlByteR = 0xa1;
			break;
		}

}

void SetEEPROMRead(void)
{
	I2C2CONbits.RSEN = 1;						//发送重起始位
	while(I2C2CONbits.RSEN == 1);				// 等待重起动操作完成
	I2C2TRN = ControlByteR;					// 发送控制字:读准备
	while(I2C2STATbits.TRSTAT == 1);			//等待发送完成
}

void SetEEPROMAddress(WORD address)
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

#if	0
void EEPROMWriteByte(BYTE Data, WORD address)
{
	//initiate the start condition
	I2C_start();
	
	//transmission the slave address
	I2C2TRN = (((WORD_VAL)address).v[1]);//Addr&0xfe;	// Hight byte
	while (I2C2STATbits.TBF == 1)
	{
;/*		I2C_COUNTER++;
		if(I2C_COUNTER > 0x0320)
		{
			I2C_ERROR = 0x55;
			break;
		}*/
	}
//	I2C_COUNTER = 0;
	
	//wait for the acknowledge
	while (I2C2STATbits.ACKSTAT == 1)
	{
;/*		I2C_COUNTER++;
		if(I2C_COUNTER > 0x320)
		{
			I2C_ERROR = 0x55;
			break;
		}*/
	}

	I2C2TRN = (((WORD_VAL)address).v[0]);//Addr&0xfe;	// Low byte
	while (I2C2STATbits.TBF == 1)
	{
;/*		I2C_COUNTER++;
		if(I2C_COUNTER > 0x0320)
		{
			I2C_ERROR = 0x55;
			break;
		}*/
	}
//	I2C_COUNTER = 0;
	
	//wait for the acknowledge
	while (I2C2STATbits.ACKSTAT == 1)
	{
;/*		I2C_COUNTER++;
		if(I2C_COUNTER > 0x320)
		{
			I2C_ERROR = 0x55;
			break;
		}*/
	}
	NByte = 1;// writeByte
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
			I2C2TRN = Data++;
			NByte--;
			//wait for the slave data's transmission was complete
			while (I2C2STATbits.TBF == 1)
			{
				I2C_COUNTER++;
				if(I2C_COUNTER > 0x0320)
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
//	return (NByte);
}

#else

void EEPROMWriteByte(BYTE Data, WORD address)
{
//	_AD1IE = 0;
	SetEEPROMAddress(address);
	I2C2TRN = Data;
	while(I2C2STATbits.TRSTAT == 1);			//等待发送完成
	I2C_Stop();
//	_AD1IE = 1;	//开中断
}

#endif

BYTE EEPROMReadByte(WORD address)
{
	BYTE temp;
//	_AD1IE = 0;
	SetEEPROMAddress(address);
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


void EEPROMWriteWord(WORD data, WORD address)
{

#if	0
//	_AD1IE = 0;
	unsigned long i;
	SetEEPROMAddress(address);
	for(i = 0; i<2; i++)
	{
		I2C2TRN = (((WORD_VAL)data).v[i]);		// DataL;
		while(I2C2STATbits.TRSTAT == 1);			//等待发送完成
	}
	I2C_Stop();
#else
#if	1
	unsigned int i;
	SetEEPROMAddress(address);
	for(i=0;i<2;i++)
	{
	
	I2C2TRN = ((WORD_VAL)data).v[i];
	while(I2C2STATbits.TRSTAT == 1);			//等待发送完成
	}
	I2C_Stop();
#else
	EEPROMWriteByte(((WORD_VAL)data).v[0],address);
	EEPROMWriteByte(((WORD_VAL)data).v[1],address +1);//这个有错误存在-颠倒了*/
#endif
#endif
//	_AD1IE = 1;	//开中断
}

WORD EEPROMReadWord(WORD address)
{

//	unsigned int i;
//	_AD1IE = 0;
#if	0
	SetEEPROMAddress(address);
	SetEEPROMRead( );
	for(i = 0; i<2; i++)
		{
			I2C2CONbits.RCEN = 1;				//接收数据使能
			while(I2C2STATbits.RBF == 0);		//
			temp.v[i] = I2C2RCV;
			I2C2CONbits.ACKDT = 0;
			if(i == 1)
				I2C2CONbits.ACKDT = 1;		// 最后一字节则不发ACK
			I2C2CONbits.ACKEN = 1;
			while(I2C2CONbits.ACKEN == 1);
		}
	I2C_Stop();
#else
    temp.v[0] = EEPROMReadByte(address);
    temp.v[1] = EEPROMReadByte(address+1);

#endif
//	_AD1IE = 1;	//开中断
    return temp.Val;
}

BYTE EEPROMWriteArray(DWORD address, BYTE* pData, WORD nCount)
{
	DWORD_VAL addr;
	BYTE*     pD;
	WORD      counter;

    addr.Val = address;
    pD       = pData;
	
	SetEEPROMAddress(addr.Val);
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
					SetEEPROMAddress(addr.Val);
		        	}
		}
	I2C_Stop();
}

void EEPROMReadArray(WORD address, BYTE* pData, WORD nCount)
{
	unsigned int i;
	SetEEPROMAddress(address);
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


// 向24LC256 中写入16字节数据
void PageWrite()
{
	I2C2CONbits.SEN = 1;						//发送起始位
	while(I2C2CONbits.SEN == 1);				//等待启动操作完成
	I2C2TRN = ControlByteW;					//发送控制字:写准备
	while(I2C2STATbits.TRSTAT == 1);			//等待发送完成
	I2C2TRN = AddressH;//(((WORD_VAL)address).v[1]);		// AddressH;
	while(I2C2STATbits.TRSTAT == 1);			//等待发送完成
	I2C2TRN = AddressL;//(((WORD_VAL)address).v[0]);		// AddressL;
	while(I2C2STATbits.TRSTAT == 1);			//等待发送完成
	for(i = 0; i<16; i++)
		{
			I2C2TRN = WriteTable[i];//(((WORD_VAL)address).v[0]);		// AddressL;
			while(I2C2STATbits.TRSTAT == 1);			//等待发送完成

		}
	I2C2CONbits.PEN = 1;						//发送停止位
	while(I2C2CONbits.PEN == 1);				//等待停止操作完成
}


//从24LC256 中读出16字节数据
void PageRead()
{
	I2C2CONbits.SEN = 1;						//发送起始位
	while(I2C2CONbits.SEN == 1);				//等待启动操作完成
	I2C2TRN = ControlByteW;					//发送控制字:写准备
	while(I2C2STATbits.TRSTAT == 1);			//等待发送完成
	I2C2TRN = AddressH;//(((WORD_VAL)address).v[1]);// AddressH;
	while(I2C2STATbits.TRSTAT == 1);			//等待发送完成
	I2C2TRN = AddressL;//(((WORD_VAL)address).v[0]);// AddressL;
	while(I2C2STATbits.TRSTAT == 1);			//等待发送完成
	I2C2CONbits.RSEN = 1;						//发送重起始位
	while(I2C2CONbits.RSEN == 1);				// 等待重起动操作完成
	I2C2TRN = ControlByteR;					// 发送控制字:读准备
	while(I2C2STATbits.TRSTAT == 1);			//等待发送完成
	for(i = 0; i<16; i++)
		{
			I2C2CONbits.RCEN = 1;				//接收数据使能
			while(I2C2STATbits.RBF == 0);		//
			ReadTable[i] = I2C2RCV;
			I2C2CONbits.ACKDT = 0;
			if(i == 15)
				I2C2CONbits.ACKDT = 1;		// 最后一字节则不发ACK
			I2C2CONbits.ACKEN = 1;
			while(I2C2CONbits.ACKEN == 1);
		}
	I2C2CONbits.PEN = 1;						// 发送停止位
	while(I2C2CONbits.PEN == 1);				// 等待停止操作完成
	
}



#endif

#endif
