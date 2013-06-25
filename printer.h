#ifndef _PRINTER_H
#define _PRINTER_H


#define ENGLISH

#define  n_Offset_JILUHAO               0x06
#define  n_Offset_RIQI                  0x09
#define  n_Offset_SHIJIAN               0x0c
#define  n_Offset_WENDU                 0x0e
#define  n_Offset_GUILING               0x10
#define  n_Offset_JIUJINGHANLIANG       0x12



#define uint   unsigned int
#define uchar  unsigned char
#define SPIGet() SPI2BUF


#define Lo(X)   (unsigned char)(X&0x00ff)
#define Hi(X)   (unsigned char)((X>>8)&0x00ff)

#define EEPROM
#define n_Addr_Print_Status  0x7f10
#define n_Addr_Print_Copies  0x7f11
#define n_Addr_DeviceID  0x7000


void initialUART1(void);
//void inti_time5(void);
//uchar Calculate_N(uchar dotline);
void BCD_to_ASCII(const uchar Table[],uchar n,uchar Offset,uchar m);
void Byte_to_PRN_TextBuffer(uchar a,int b);
//void String_to_PRN_TextBuffer(int Table_char,uint b,uint a);
void Print_Table_Text_YIQIHAO();
void Print_Table_Text_JILUHAO();
void Print_Table_Text_RIQI();
void Print_Table_Text_SHIJIAN();
void Print_Table_Text_GUILING();
void Print_Table_Text_RefuseTest();
void Print_Table_Text_TestModePassive();
void Print_Table_Text_TestModeManual();
void Print_Table_Text_JIUJINGHANLIANG();
void Print_Table_Text_NONGDU();



void Print_Table_Text_Eng(WORD ItemAddress);
void Print_TestRecord();
//void ChangeModel(unsigned char n1,unsigned char n2,unsigned char n3,unsigned char n4);
void Inputdata(void);
void initPrint(void);
extern unsigned char p_n;
void Off_Printer(void);
void ChangeBRG(void);
//void ChangeDeviceType(void);
#endif	//_PRINTER_H
