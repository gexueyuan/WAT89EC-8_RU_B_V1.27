#include "MainDemo.h"
 

unsigned char textbuffer[41]={0x00};

unsigned char TestRecord[64]={0x00};


#define RX1_BUFFLEN 255
#define TX1_BUFFLEN 255


volatile unsigned char PrintDataBuff[600];

//static unsigned int Datanum;
volatile unsigned char tx1_buff[TX1_BUFFLEN],*tx1_ptr,*tx1_ptrend;

volatile unsigned int uart1_rtimer,t1len;

volatile unsigned char rx1_buff[RX1_BUFFLEN],*rx1_ptr;


volatile BOOL SPrint = FALSE,endprint = FALSE;

static uchar pn=0;

extern XCHAR 	dateTimeStr[26];

volatile unsigned char Printbuff[256];
extern volatile BOOL ChoicePrint;

 extern volatile BYTE Datetype;
volatile unsigned int Sprinnum=0,Datanum=0;
unsigned char p_n=0;

extern volatile unsigned int serialnumberRecord;


/*---------------------------------------------------------------------
  Function Name: BCD_to_ASCII
  Description:   
  Inputs:        None
  Returns:       
-----------------------------------------------------------------------*/
void BCD_to_ASCII(const uchar Table[],uchar n,uchar Offset,uchar m)
{
    uchar tem_hign,tem_low;
    uchar iii;
    uchar *p_TestRecord = TestRecord;
    uchar *p_tem=Table;

    for(iii = 0; iii < n; iii++)
    {
        tem_low = *(p_TestRecord+Offset+iii+m);

        tem_hign = *(p_TestRecord+Offset+iii+m);

        tem_low &= 0x0f;

        tem_low += 0x30;

        tem_hign &= 0xf0;

        tem_hign>>= 4;

        tem_hign += 0x30; 
    
        *(p_tem+iii*2) = tem_hign;

        *(p_tem+(iii*2+1)) = tem_low;        
    }
       
}

void ASCII_to_BCD(const uint Table,uchar n,uchar Offset,uchar m)
{
    uchar tem_hign,tem_low;
    uchar iii;
    uchar *p_TestRecord = TestRecord;
    uint *p_tem=Table;

    for(iii = 0; iii < n; iii++)
    {
        tem_low = *(p_TestRecord+Offset+iii+m);

        tem_hign = *(p_TestRecord+Offset+iii+m);

        tem_low &= 0x0f;

        //tem_low += 0x30;

        tem_hign &= 0xf0;

        tem_hign>>= 4;

        //tem_hign += 0x30; 
    
        *(p_tem+iii*2) = (uint)tem_hign;

        *(p_tem+(iii*2+1)) = (uint)tem_low;        
    }
       
}

void ASCII_to_BCD_Eng(uchar *p_tem,uchar n,uchar Offset,uchar m)
{
    //uchar tem_hign,tem_low;
    //uchar tem;
    uchar iii;
    uchar *p_TestRecord = TestRecord;

    //for(iii = 0; iii < n; iii++)
    //{
    /*
        tem_low = *(p_TestRecord+Offset+iii+m);

        tem_hign = *(p_TestRecord+Offset+iii+m);

        tem_low &= 0x0f;

        tem_low += 0x30;

        tem_hign &= 0xf0;

        tem_hign>>= 4;

        tem_hign += 0x30; 
    
        *(p_tem+iii*2) = (uint)tem_hign;

        *(p_tem+(iii*2+1)) = (uint)tem_low; 
        */
        //*(p_tem+0)=0;
         *(p_tem+0) = (serialnumberRecord /10000) +'0';// 万位
         *(p_tem+1) = (serialnumberRecord /1000 %10) +'0';// 千位
		*(p_tem+2) = (serialnumberRecord /100 %10) +'0';// 百位
		*(p_tem+3) = (serialnumberRecord /10 %100%10) +'0';// 十位
		*(p_tem+4) = (serialnumberRecord %10) +'0';// 个位
        
        //*(p_tem+iii) = *(p_TestRecord+Offset+iii+m);
        //*(p_tem+iii) = tem;
        
    //}
       
}
/*---------------------------------------------------------------------
  Function Name:   Byte_to_PRN_TextBuffer
  Description:   None
  Inputs:        None
  Returns:       None
-----------------------------------------------------------------------*/
void Byte_to_PRN_TextBuffer(uchar a,int b)
{
 PrintDataBuff[Datanum] = a;

 Datanum++;
}

void initialBlueTooth(void)
{

 _TRISE8 = 1;

 _TRISE9 = 1;

 //_TRISG0 = 0;

 _TRISC1 = 1;

 _TRISC2 = 0;

 PORTCbits.RC2=1;


}

void initialUART1(void)
{

     U1MODE = 0xa000;
	 U1STA  = 0x8510; 
	 #ifdef CHANGE4800
     U1BRG=103;
	 #else
	 U1BRG=207;
     #endif

	  _U1RXIF = 0;             //clear the receive interrupt flag
        _U1TXIF = 0;             //clear the transmit interrupt flag
         _U1RXIF = 0;             //clear the transmit interrupt flag
        _U1RXIP = 5;             //set the receive priority level 5
        _U1TXIP = 5;             //set the transmit priority level 5
 
        _U1RXIE = 1;             //enable the receive interrupt

        _U1TXIE = 0;             //disable the transmit interrupt
        
	 _TRISB6 = 1;//u1tx input
	 AD1PCFGbits.PCFG6 = 1;//
	 AD1PCFGbits.PCFG5 = 1;//
     Nop();
	 _TRISB5 = 0;//u1rx output
     Nop();
    tx1_ptr = &tx1_buff[0];
	rx1_ptr = &rx1_buff[0];

	initialBlueTooth();
     
}

//uart1 send interrupt 
void __attribute__ ((__interrupt__)) _U1TXInterrupt(void)
{
	DISICNT = 0x3FFF; /* disable interrupts */
	
	_U1TXIF = 0;
	
	if((t1len != 0) && (tx1_ptr < &tx1_buff[TX1_BUFFLEN])) 
	{
		
		U1TXREG = *tx1_ptr++;
		
		t1len--;
		
	}
	else _U1TXIE = 0;
	DISICNT = 0x0000; /* enable interrupts */
}
/**********************************************************************/
//uart1 receive interrupt 
void __attribute__ ((__interrupt__)) _U1RXInterrupt(void)
{
	DISICNT = 0x3FFF; /* disable interrupts */
	unsigned char i;
	_U1RXIF = 0;
	while(U1STAbits.URXDA == 1) 
	{
		i = U1RXREG;
		if(0x10==i)
			SPrint = TRUE;
		if(0xaa==i)
			endprint = TRUE;
		if(rx1_ptr < &rx1_buff[RX1_BUFFLEN])
		{
			*rx1_ptr++ = i;
			*rx1_ptr = 0;
		}
	}
	DISICNT = 0x0000; /* enable interrupts */
}

void UART1transtart(void)
{
	
	t1len = tx1_ptrend - &tx1_buff[0];

    tx1_ptr = &tx1_buff[0];
    
    _U1TXIE = 1;
    
    t1len--;
    
    U1TXREG = *tx1_ptr++;
    
}
void ChangeBRG(void)
{
	U1STAbits.UTXEN=1;
	
    tx1_ptr       = &tx1_buff[0];
   
    tx1_ptrend    = tx1_ptr; 

   
    tx1_buff[0]  = 0x01;
   
    tx1_buff[1]  = 0xFC;  

	tx1_buff[2]  = 0x03;
   
    tx1_buff[3]  = 0x04;

	tx1_buff[4]  = 0x30;
   
    tx1_buff[5]  = 0x00;

	tx1_buff[6]  = 0x01;
   
    tx1_buff[7]  = 0x00;	
       
    tx1_ptrend   += 8; 

	UART1transtart();	
}
/*
void ChangeDeviceType(void)
{
	U1STAbits.UTXEN=1;
	
    tx1_ptr       = &tx1_buff[0];
   
    tx1_ptrend    = tx1_ptr; 

   
    tx1_buff[0]  = 0x01;
   
    tx1_buff[1]  = 0xFC;  

	tx1_buff[2]  = 0x10;
   
    tx1_buff[3]  = 0x03;

	tx1_buff[4]  = 0x80;
   
    tx1_buff[5]  = 0x06;

	tx1_buff[6]  = 0x04;
   
   // tx1_buff[7]  = 0x00;	
       
    tx1_ptrend   += 7; 

	UART1transtart();	
}
*/
void initPrint(void)
{
	U1STAbits.UTXEN=1;
	
    tx1_ptr       = &tx1_buff[0];
   
    tx1_ptrend    = tx1_ptr; 

   
    tx1_buff[0]  = 0x1b;
   
    tx1_buff[1]  = 0x40;  
       
    tx1_ptrend   += 2; 

	UART1transtart();
}

void StartPrint(void)
{
	U1STAbits.UTXEN=1;
	
    tx1_ptr       = &tx1_buff[0];
   
    tx1_ptrend    = tx1_ptr; 

   
    tx1_buff[0]  = 0x1b;
   
    tx1_buff[1]  = 0xaa;


//if()
//  {
	if(Flashbuff[10]==0&&Flashbuff[11]==0)
          tx1_buff[2]=(EEPROMReadByte(PrintSelectAddress)-0x30);
	
		else 
			
		  tx1_buff[2]=(EEPROMReadByte(PrintSelectAddress+1)-0x30);
 //  }

//else  tx1_buff[2]  = 0x01; 
     
       
    tx1_ptrend   += 3; 

	UART1transtart();
}

/*---------------------------------------------------------------------
  Function Name: Off_Printer
  Description:   None
  Inputs:        None
  Returns:       None
-----------------------------------------------------------------------*/
void Off_Printer(void)
{
	U1STAbits.UTXEN=1;
	
    tx1_ptr       = &tx1_buff[0];
   
    tx1_ptrend    = tx1_ptr; 

   
    tx1_buff[0]  = 0x1b;
   
    tx1_buff[1]  = 0xbb;

	tx1_buff[2]  = 0xbb;

	tx1_ptrend   += 3; 

	UART1transtart();

}
/*---------------------------------------------------------------------
  Function Name: Search_Table
  Description:   None
  Inputs:        None
  Returns:       None
-----------------------------------------------------------------------*/
uchar Search_Table(uchar Table)
{
    unsigned char Table_ru_display[]=   //显示屏所用的省份简称的汉字代码
    { 
    0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 
    0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F, 0x90, 0x91, 0x92, 0x93, 
    0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D,
    0x9E, 0x9F, 0x7F, 0x7C, 0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 
    0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF,
    0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9,
    0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF, 0xC0, 0x7D
    };

    unsigned char Table_ru_printer[]= //打印机所用的省份简称的汉字代码
    {
     0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,
     0x8a,0x8b,0x8c,0x8d,0x8e,0x8f,0x90,0x91,0x92,0x93,
     0x94,0x95,0x96,0x97,0x98,0x99,0x9a,0x9b,0x9c,0x9d,
     0x9e,0x9f,0xa0,0xa1,0xa2,0xa3,0xa4,0xa5,0xa6,0xa7,
     0xa8,0xa9,0xaa,0xab,0xac,0xad,0xae,0xaf,0xb0,0xb1,
     0xb2,0xb3,0xb4,0xb5,0xb6,0xb7,0xb8,0xb9,0xba,0xbb,
     0xbc,0xbd,0xbe,0xbf,0xc0,0xc1,0xc2,0xc3
    };

    char tem_2;

    char loop_E;



    for(loop_E = 0; loop_E < 68; loop_E++)
    {
        if(Table_ru_display[loop_E] == Table)
           {
            tem_2 = Table_ru_printer[loop_E];
            break;
           }
    }

    return tem_2;


}

uchar Search_zero(uchar Table[])
{
    uchar pzero;
    uchar loop_E;


    for(loop_E = 0; loop_E < 20; loop_E++)
    {
        if(Table[loop_E] == 0)
           {
            pzero = loop_E;
            break;
           }
    }

    return pzero;

}
/*---------------------------------------------------------------------
  Function Name: Print_Table_Text_WAT89EC_8
  Description:   None
  Inputs:        None
  Returns:       None
-----------------------------------------------------------------------*/
void Print_Table_Text_Alcovisor_Mercury(void)
{
 uchar Loop_H;

#ifdef TradeMark_Version

 unsigned char Table_Text_Alcovisor_Jupiter[]= //"               Alcovisor-Jupiter"
 {
    0x20,0x20,0x20,0x20,0x20,0x80,0x8B,0x8A,
	0x8E,0x92,0x85,0x8A,0x92,0x8E,0x90,0x20, 	
 };
/*
    for(Loop_H = 0; Loop_H < 29; Loop_H++)
        {
            if(Table_Text_Alcovisor_Jupiter[Loop_H] > 0x007c)       
                Table_Text_Alcovisor_Jupiter[Loop_H]=Search_Table(Table_Text_Alcovisor_Jupiter[Loop_H]);
       
        }
        */
 CopyXCHARtoCHAR(&PrintDataBuff[Datanum],Table_Text_Alcovisor_Jupiter,16);//30-57

 Datanum = Datanum+16;

 PrintDataBuff[Datanum++] = 0x0d;

 unsigned char Table_Text_Pro_touch[]= //"               Alcovisor-Jupiter"
 {
     0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x9E,
	 0xAF,0xA8,0xB2,0xA5,0xB0,
 };	


    for(Loop_H = 0; Loop_H < 26; Loop_H++)
        {
            if(Table_Text_Pro_touch[Loop_H] > 0x007c)       
                Table_Text_Pro_touch[Loop_H]=Search_Table(Table_Text_Pro_touch[Loop_H]);
       
        }
	
 CopyXCHARtoCHAR(&PrintDataBuff[Datanum],Table_Text_Pro_touch,13);//30-57

 Datanum = Datanum+13;

 PrintDataBuff[Datanum++] = 0x0d;


#else
 unsigned char Table_Text_Alcovisor_Jupiter[]= //"               Alcovisor-Jupiter"
 {
    0x20,0x20,0x20,0x20,0x20,0x80,0x8B,0x8A,
	0x8E,0x92,0x85,0x8A,0x92,0x8E,0x90,0x20, 	
 };

 CopyXCHARtoCHAR(&PrintDataBuff[Datanum],Table_Text_Alcovisor_Jupiter,16);//30-57

 Datanum = Datanum+16;

 PrintDataBuff[Datanum++] = 0x0d;

 unsigned char Table_Text_Pro_touch[]= //"               Alcovisor-Jupiter"
 {
     
	 0x20,0x20,0x20,'P','R','O','-','1','0','0',' ',' ','t','o','u','c','h'
 };	

 CopyXCHARtoCHAR(&PrintDataBuff[Datanum],Table_Text_Pro_touch,17);//30-57

 Datanum = Datanum+17;

  PrintDataBuff[Datanum++] = 0x0d;

#endif

}


/*---------------------------------------------------------------------
  Function Name: Print_Table_Text_YIQIHAO
  Description:   None
  Inputs:        None
  Returns:       None
-----------------------------------------------------------------------*/
void Print_Table_Text_YIQIHAO()
{
  
   unsigned char Table_Text_YIQIHAO_RU[]=  //Serial Number:
    {
     0x8D, 0xb0, 0xAe, 0xA7, 0xB2, 0x20, 0x8F, 
     0xB2, 0xAa, 0xA3, 0xb0, 0xB2, 0xA2, 0x3a,0x20
    };

    CopyXCHARtoCHAR(&PrintDataBuff[Datanum],Table_Text_YIQIHAO_RU,15);//30-57

	Datanum = Datanum+15;

	unsigned char Table_YIQIHAO[4] = {0x00};

	uchar *p_TestRecord = TestRecord;

    uchar Table_TestRecord[7]={0x00};

    *(p_TestRecord+n_Offset_JILUHAO+0) = EEPROMReadByte(ID_JIQIADDRESS);

    *(p_TestRecord+n_Offset_JILUHAO+1) = EEPROMReadByte(ID_JIQIADDRESS+1);
	

    *(p_TestRecord+n_Offset_JILUHAO+2) = EEPROMReadByte(ID_JIQIADDRESS+2);


    BCD_to_ASCII(Table_TestRecord,3,n_Offset_JILUHAO,0);

	Table_TestRecord[6]= 0x0d;
    
    CopyXCHARtoCHAR(&PrintDataBuff[Datanum],Table_TestRecord,7);//30-57

	Datanum = Datanum+7; 


}

/*---------------------------------------------------------------------
  Function Name: Print_Table_Text_JILUHAO
  Description:   None
  Inputs:        None
  Returns:       None
-----------------------------------------------------------------------*/
void Print_Table_Text_JILUHAO()
{
 uchar Loop_H;
 
    unsigned char Table_Text_JILUHAO_Ru[]=  //Record No.：
    {
     0x92, 0xA5, 0xB1, 0xB2, 0x20, 0x4E, 0x4F,
	 0x2e, 0x3a, 0x20
    };

    uchar *p_TestRecord = TestRecord;


    unsigned char Table_TestRecord[6]={0x00};

    *(p_TestRecord+n_Offset_JILUHAO+0) = Printbuff[0];

    *(p_TestRecord+n_Offset_JILUHAO+1) = Printbuff[1];

    *(p_TestRecord+n_Offset_JILUHAO+2) = Printbuff[2];

    ASCII_to_BCD_Eng(Table_TestRecord,3,n_Offset_JILUHAO,0);
    Table_TestRecord[5]= 0x0d;
 

   for(Loop_H = 0; Loop_H < 10; Loop_H++)
        {
            if(Table_Text_JILUHAO_Ru[Loop_H] > 0x007c)       
                Table_Text_JILUHAO_Ru[Loop_H]=Search_Table(Table_Text_JILUHAO_Ru[Loop_H]);
       
        }	

    CopyXCHARtoCHAR(&PrintDataBuff[Datanum],Table_Text_JILUHAO_Ru,10);//30-57

	Datanum = Datanum+10;
	
	CopyXCHARtoCHAR(&PrintDataBuff[Datanum],Table_TestRecord,6);

	Datanum = Datanum+6;

 
}



void Print_Table_Text_RIQI()
{

    unsigned char Table_Text_RIQI_RU[]=   //Date：
    {
     0x84,0xa2,0xb4,0xa2,0x3a,0x20,0x20,0x20
    };	

	unsigned char MonthtempH,MonthtempL;
	
    uchar *p_TestRecord = TestRecord;

    *(p_TestRecord+n_Offset_RIQI+0) = Printbuff[3];

    *(p_TestRecord+n_Offset_RIQI+1) = Printbuff[4];

    *(p_TestRecord+n_Offset_RIQI+2) = Printbuff[5];

    unsigned char Table_TestRecord[3]={0x00};

	MonthtempH = (Printbuff[4]>>4)&0x0f;
	MonthtempL =  Printbuff[4]&0x0f;
	
   
    CopyXCHARtoCHAR(&PrintDataBuff[Datanum],Table_Text_RIQI_RU,8);

	Datanum = Datanum+8;



    BCD_to_ASCII(Table_TestRecord,1,n_Offset_RIQI,2);

    CopyXCHARtoCHAR(&PrintDataBuff[Datanum],Table_TestRecord,2);

	Datanum = Datanum+2;

	PrintDataBuff[Datanum] = '/';

	Datanum++;	
	
    Byte_to_PRN_TextBuffer(MonthtempH+'0',11);

	Byte_to_PRN_TextBuffer(MonthtempL+'0',12);

	
	
    PrintDataBuff[Datanum] = '/';//Byte_to_PRN_TextBuffer(',',14);

	Datanum++;

    PrintDataBuff[Datanum] = '2';//Byte_to_PRN_TextBuffer('2',15);

	Datanum++;

    PrintDataBuff[Datanum] = '0';//Byte_to_PRN_TextBuffer('0',16);

	Datanum++;

    BCD_to_ASCII(Table_TestRecord,1,n_Offset_RIQI,0);

    CopyXCHARtoCHAR(&PrintDataBuff[Datanum],Table_TestRecord,2);

	Datanum = Datanum+2;

    PrintDataBuff[Datanum] = 0x0d;

	Datanum++;
	
 
}

 
/*---------------------------------------------------------------------
  Function Name: Print_Table_Text_SHIJIAN
  Description:   None
  Inputs:        None
  Returns:       None
-----------------------------------------------------------------------*/
void Print_Table_Text_SHIJIAN()
{

    unsigned char Table_Text_SHIJIAN_Ru[]=  //Time:
    {
     0x82,0xb2,0xa7,0xae,0xc1, 0x3a,0x20,0x20,0x20
    };
	
	
    uchar *p_TestRecord = TestRecord;

    *(p_TestRecord+n_Offset_SHIJIAN+0) = Printbuff[6];

    *(p_TestRecord+n_Offset_SHIJIAN+1) = Printbuff[7];

    uchar Table_TestRecord[3]={0x0000};


	CopyXCHARtoCHAR(&PrintDataBuff[Datanum],Table_Text_SHIJIAN_Ru,9);

	Datanum = Datanum + 9;

    BCD_to_ASCII(Table_TestRecord,1,n_Offset_SHIJIAN,0);

	CopyXCHARtoCHAR(&PrintDataBuff[Datanum],Table_TestRecord,2);

	Datanum = Datanum + 2;

    PrintDataBuff[Datanum] = ':';//Byte_to_PRN_TextBuffer(':',10);

	Datanum++;

    BCD_to_ASCII(Table_TestRecord,1,n_Offset_SHIJIAN,1);


	CopyXCHARtoCHAR(&PrintDataBuff[Datanum],Table_TestRecord,2);

	Datanum = Datanum + 2;

    PrintDataBuff[Datanum] = 0x0d;

	Datanum++;

}

/*---------------------------------------------------------------------
  Function Name: Print_Table_Text_GUILING归零
  Description:   None
  Inputs:        None
  Returns:       None
-----------------------------------------------------------------------*/
void Print_Table_Text_GUILING()
{

    unsigned char Table_Text_GUILING_Ru[]=  //Blank：
    {
     0x82,0xb0,0xa9,0xa6,0xb5,0xb7,0x3a,0x20,0x20
     };

    unsigned char Table_Text_mg_100ml[] = // "mg/100mL"
    {0x6d,0x67,0x2f,0x31,0x30,
      0x30,0x6d,0x4c,0x0d};
    unsigned char Table_Text_mg_l[] = // "mg/L"
    {0xae,0xa5,'/',0xad,0x0d};
    unsigned char Table_Text_mg_100[] = //"%"
    {0x25,0x0d};
    unsigned char Table_Text_mg_1000ml[] = //"%."
    {0x25,0x2e,0x0d};
    unsigned char Table_Text_g_100ml[] = //"g/100mL"
    {0x67, 0x2F, 0x31, 0x30, 0x30,
      0x6D, 0x4c, 0x0d};
    unsigned char Table_Text_g_210l[] = //"g/210L"
    {0x67, 0x2F, 0x32, 0x31,
      0x30, 0x4c, 0x0d};
    unsigned char Table_Text_ug_100ml[] = //"ug/100mL"
    {0x75, 0x67, 0x2F, 0x31,
     0x30, 0x30, 0x6D, 0x4c, 0x0d};
    unsigned char Table_Text_mg_ml[] = //"mg/mL"
    {0x6D, 0x67, 0x2F, 0x6D, 0x4c, 0x0d};
    unsigned char Table_Text_mg100[] = //"g/L"
    {0x67, 0x2F, 0x4c, 0x0d};


	CopyXCHARtoCHAR(&PrintDataBuff[Datanum],Table_Text_GUILING_Ru,9);

	Datanum = Datanum+9;

    PrintDataBuff[Datanum] = '0';//Byte_to_PRN_TextBuffer('0',8);

	Datanum++;

    PrintDataBuff[Datanum] = '.';//Byte_to_PRN_TextBuffer('.',9);

	Datanum++;

    PrintDataBuff[Datanum] = '0';//Byte_to_PRN_TextBuffer('0',10);

	Datanum++;

    PrintDataBuff[Datanum] = '0';//Byte_to_PRN_TextBuffer('0',11);

	Datanum++;

    PrintDataBuff[Datanum] = ' ';//Byte_to_PRN_TextBuffer(' ',12);

	Datanum++;

	switch(Printbuff[9])
    {

         case 0://mg/100mL

		CopyXCHARtoCHAR(&PrintDataBuff[Datanum],Table_Text_mg_100ml,9);

	    Datanum = Datanum+9;
         break;

         case 1://mg/L
 
		 CopyXCHARtoCHAR(&PrintDataBuff[Datanum],Table_Text_mg_l,5);

	    Datanum = Datanum+5;
         break;

         case 2://%

		 CopyXCHARtoCHAR(&PrintDataBuff[Datanum],Table_Text_mg_100,2);

	    Datanum = Datanum+2;
         break;       

         case 3://g/100mL

		 CopyXCHARtoCHAR(&PrintDataBuff[Datanum],Table_Text_g_100ml,8);

	    Datanum = Datanum+8;
         break;

         case 4://%.

		 CopyXCHARtoCHAR(&PrintDataBuff[Datanum],Table_Text_mg_1000ml,3);

	    Datanum = Datanum+3;
 
         break;         

         case 5://ug/100mL

		 CopyXCHARtoCHAR(&PrintDataBuff[Datanum],Table_Text_ug_100ml,9);

	    Datanum = Datanum+9;
         break;

         case 6://mg/mL

		 CopyXCHARtoCHAR(&PrintDataBuff[Datanum],Table_Text_mg_ml,6);

	    Datanum = Datanum+6;	 
         break;

         case 7://mg%

		 CopyXCHARtoCHAR(&PrintDataBuff[Datanum],Table_Text_mg100,4);

	    Datanum = Datanum+4;
         break;

         case 8://g/210L

		 CopyXCHARtoCHAR(&PrintDataBuff[Datanum],Table_Text_g_210l,7);

	    Datanum = Datanum+7;		 
         break;

         default: 
         break;
		}
    //PrintDataBuff[Datanum] = 0x0d;//Byte_to_PRN_TextBuffer(0x0a,14);
 
    //Datanum++;
}



/*---------------------------------------------------------------------
  Function Name: Last_Calibration_Date最后一次校准时间
  Description:   None
  Inputs:        None
  Returns:       None
-----------------------------------------------------------------------*/
void Last_Calibration_Date()
{

    unsigned char Table_Last_Calibration_Date[]= //Last Calibration Date：
    {
      0x84, 0xA0, 0xB2, 0xA0, 0x20, 0xB0, 0xA5, 0xA3, 
	  0xB3, 0xAB, 0xA8, 0xB0, 0xAE, 0xA2, 0xAA, 0xA8
	  
    };
   uchar Loop_H;
   for(Loop_H = 0; Loop_H < 16; Loop_H++)
        {
            if(Table_Last_Calibration_Date[Loop_H] > 0x007c)
            	{
                Table_Last_Calibration_Date[Loop_H]=Search_Table(Table_Last_Calibration_Date[Loop_H]);
            	}
        }


    CopyXCHARtoCHAR(&PrintDataBuff[Datanum],Table_Last_Calibration_Date,16);

	Datanum = Datanum+16;
  
    PrintDataBuff[Datanum] = 0x0d;

	Datanum++;
}



void Print_Last_Calibration_Date()
{
     uint Table_TestRecord[3]={0};

   unsigned char MonthtempH,MonthtempL;	 

unsigned char datetemp;
unsigned char TempCalibrationBuff[]={0x05,0x03,0x09,0};//月 日 年
//datetemp=EEPROMReadByte(MarkYrDataAddress);
TempCalibrationBuff[2]=Flashbuff[46];//((datetemp/10)&0x0f)<<4+(datetemp%10);
//datetemp=EEPROMReadByte(MarkMthDataAddress);
TempCalibrationBuff[0]=Flashbuff[47];//((datetemp/10)&0x0f)<<4+(datetemp%10);
//datetemp=EEPROMReadByte(MarkDayDataAddress);
TempCalibrationBuff[1]=Flashbuff[48];//((datetemp/10)&0x0f)<<4+(datetemp%10);



   	MonthtempH = (TempCalibrationBuff[0]>>4)&0x0f;
	MonthtempL =  TempCalibrationBuff[0]&0x0f;

	uchar *p_TestRecord = TestRecord;

    *(p_TestRecord+n_Offset_RIQI+0) = TempCalibrationBuff[1];

    *(p_TestRecord+n_Offset_RIQI+1) = TempCalibrationBuff[0];

    *(p_TestRecord+n_Offset_RIQI+2) = TempCalibrationBuff[2];

	
    BCD_to_ASCII(Table_TestRecord,1,n_Offset_RIQI,0);

    CopyXCHARtoCHAR(&PrintDataBuff[Datanum],Table_TestRecord,2);

	Datanum = Datanum+2;//TextBlock_to_PRN_TextBuffer(Table_TestRecord,2,0);//日    

    Byte_to_PRN_TextBuffer('/',2);


    Byte_to_PRN_TextBuffer(MonthtempH+'0',3);

	Byte_to_PRN_TextBuffer(MonthtempL+'0',4);

    Byte_to_PRN_TextBuffer('/',5);

    Byte_to_PRN_TextBuffer('2',6);

    Byte_to_PRN_TextBuffer('0',7);

    BCD_to_ASCII(Table_TestRecord,1,n_Offset_RIQI,2);

    CopyXCHARtoCHAR(&PrintDataBuff[Datanum],Table_TestRecord,2);//TextBlock_to_PRN_TextBuffer(Table_TestRecord,2,8);// 年

	Datanum = Datanum+2;

    PrintDataBuff[Datanum] = 0x0d;

	Datanum++;	

	PrintDataBuff[Datanum] = 0x0d;

	Datanum++;
}

/*---------------------------------------------------------------------
  Function Name: Timeto_Calibration
  Description:   None
  Inputs:        None
  Returns:       None
-----------------------------------------------------------------------*/
void Timeto_Calibration()
{

    unsigned char Table_Last_Calibration_Date[]= //Last Calibration Date：
    {
      0x84, 0xA0, 0xB2, 0xA0, 0x20, 0xAF, 0xAE, 0xA2, 0xA5, 0xB0, 0xAA, 
	  0xA8, 0x3A,
	  
    };
   uchar Loop_H;
   for(Loop_H = 0; Loop_H < 13; Loop_H++)
        {
            if(Table_Last_Calibration_Date[Loop_H] > 0x007c)
            	{
                Table_Last_Calibration_Date[Loop_H]=Search_Table(Table_Last_Calibration_Date[Loop_H]);
            	}
        }


    CopyXCHARtoCHAR(&PrintDataBuff[Datanum],Table_Last_Calibration_Date,13);

	Datanum = Datanum+13;
  
    PrintDataBuff[Datanum] = 0x0d;

	Datanum++;





   uint Table_TestRecord[3]={0};

   unsigned char MonthtempH,MonthtempL;	 

   unsigned char datetemp;
   unsigned char TempCalibrationBuff[]={0x05,0x03,0x09,0};//月 日 年

   
  // datetemp=EEPROMReadByte(inspectYearAddress);
   TempCalibrationBuff[2]=Flashbuff[253];//datetemp;//((datetemp/10)&0x0f)<<4+(datetemp%10);
   //datetemp=EEPROMReadByte(inspectMonthAddress);
   TempCalibrationBuff[1]=Flashbuff[254];//datetemp;//((datetemp/10)&0x0f)<<4+(datetemp%10);
   //datetemp=EEPROMReadByte(inspectDayAddress);
   TempCalibrationBuff[0]=Flashbuff[255];//datetemp;//((datetemp/10)&0x0f)<<4+(datetemp%10);


    PrintDataBuff[Datanum] = TempCalibrationBuff[0]/10+'0';

	Datanum++;

	PrintDataBuff[Datanum] = TempCalibrationBuff[0]%10+'0';

	Datanum++;
/*
   	MonthtempH = (TempCalibrationBuff[1]>>4)&0x0f;
	MonthtempL =  TempCalibrationBuff[1]&0x0f;

	uchar *p_TestRecord = TestRecord;

    *(p_TestRecord+n_Offset_RIQI+0) = TempCalibrationBuff[0];

    *(p_TestRecord+n_Offset_RIQI+1) = TempCalibrationBuff[1];

    *(p_TestRecord+n_Offset_RIQI+2) = TempCalibrationBuff[2];

	
    BCD_to_ASCII(Table_TestRecord,1,n_Offset_RIQI,0);

    CopyXCHARtoCHAR(&PrintDataBuff[Datanum],Table_TestRecord,2);

	Datanum = Datanum+2;//TextBlock_to_PRN_TextBuffer(Table_TestRecord,2,0);//日    
*/
    Byte_to_PRN_TextBuffer('/',2);

    PrintDataBuff[Datanum] = TempCalibrationBuff[1]/10+'0';

	Datanum++;

	PrintDataBuff[Datanum] = TempCalibrationBuff[1]%10+'0';

	Datanum++;
/*
    Byte_to_PRN_TextBuffer(MonthtempH+'0',3);

	Byte_to_PRN_TextBuffer(MonthtempL+'0',4);
	*/

    Byte_to_PRN_TextBuffer('/',5);

    Byte_to_PRN_TextBuffer('2',6);

    Byte_to_PRN_TextBuffer('0',7);

	/*

    BCD_to_ASCII(Table_TestRecord,1,n_Offset_RIQI,2);

    CopyXCHARtoCHAR(&PrintDataBuff[Datanum],Table_TestRecord,2);//TextBlock_to_PRN_TextBuffer(Table_TestRecord,2,8);// 年

	Datanum = Datanum+2;
*/
    PrintDataBuff[Datanum] = TempCalibrationBuff[2]/10+'0';

	Datanum++;

	PrintDataBuff[Datanum] = TempCalibrationBuff[2]%10+'0';

	Datanum++;


    PrintDataBuff[Datanum] = 0x0d;

	Datanum++;	

	PrintDataBuff[Datanum] = 0x0d;

	Datanum++;	
}

/*---------------------------------------------------------------------
  Function Name: Print_Table_Text_RefuseTest
  Description:   None
  Inputs:        None
  Returns:       None
-----------------------------------------------------------------------*/
void Print_Table_Text_RefuseTest()
{

    unsigned char Table_Text_JUJUECESHI_Eng[]= //Refuse Test
    {
     0x8E, 0xB2, 0xAA, 0xA0, 0xA7, 0x20, 0xAE,
	 0xB2, 0x20, 0xB2, 0xA5, 0xB1, 0xB2, 0xA0, 
    };
   uchar Loop_H;
   for(Loop_H = 0; Loop_H < 14; Loop_H++)
        {
            if(Table_Text_JUJUECESHI_Eng[Loop_H] > 0x007c)
            	{
                Table_Text_JUJUECESHI_Eng[Loop_H]=Search_Table(Table_Text_JUJUECESHI_Eng[Loop_H]);
            	}
        }


	

	CopyXCHARtoCHAR(&PrintDataBuff[Datanum],Table_Text_JUJUECESHI_Eng,14);

    Datanum = Datanum+14;

    PrintDataBuff[Datanum] = 0x0d;

	Datanum++;	

    PrintDataBuff[Datanum] = 0x0d;

	Datanum++;

	PrintDataBuff[Datanum] = 0x0d;

	Datanum++;

	PrintDataBuff[Datanum] = 0x0d;

	Datanum++;	
}
/*---------------------------------------------------------------------
  Function Name: Print_Table_Text_Discontiued
  Description:   None
  Inputs:        None
  Returns:       None
-----------------------------------------------------------------------*/
void Print_Table_Text_Discontiued()
{

    unsigned char Table_Text_Discontiued[]= //test mode:discontinued
    {
     
	 0x82, 0xBB, 0xA4, 0xAE, 0xB5, 0x20, 0x08F, 0xB0, 0xA5,
	 0xB0, 0xA2, 0xA0, 0xAD
    };

   uchar Loop_H;
   for(Loop_H = 0; Loop_H < 14; Loop_H++)
        {
            if(Table_Text_Discontiued[Loop_H] > 0x007c)
            	{
                Table_Text_Discontiued[Loop_H]=Search_Table(Table_Text_Discontiued[Loop_H]);
            	}
        }
   
	CopyXCHARtoCHAR(&PrintDataBuff[Datanum],Table_Text_Discontiued,13);

    Datanum = Datanum+13;

    PrintDataBuff[Datanum] = 0x0d;

	Datanum++;

	PrintDataBuff[Datanum] = 0x0d;

	Datanum++;

	PrintDataBuff[Datanum] = 0x0d;

	Datanum++;	

	PrintDataBuff[Datanum] = 0x0d;

	Datanum++;	

}
/*---------------------------------------------------------------------
  Function Name: Print_Table_Text_TestModePassive
  Description:   None
  Inputs:        None
  Returns:       None
-----------------------------------------------------------------------*/
void Print_Table_Text_TestModePassive()
{
    unsigned char Table_Text_TestModePassive_Eng[]=  //Test mode: Passive
    {
     0x90, 0xA7, 0xA8, 0xAa, 0xAe, 0x3a, 0x20,
	 0x90, 0x93, 0x97, 0x8D, 0x8E, 0x89, 0x20, 
	 0x87, 0x80, 0x81, 0x8E, 0x90
    };
	

	CopyXCHARtoCHAR(&PrintDataBuff[Datanum],Table_Text_TestModePassive_Eng,19);

   Datanum = Datanum +19;

    PrintDataBuff[Datanum] = 0x0d;

	Datanum++;   
 
}

/*---------------------------------------------------------------------
  Function Name: Print_Table_Text_TestModeManual
  Description:   None
  Inputs:        None
  Returns:       None
-----------------------------------------------------------------------*/
void Print_Table_Text_TestModeManual()
{
    unsigned char Table_Text_TestModeManual_Eng[]=  //Test mode: Auto
    {
      0x90, 0xA5, 0xA6, 0xA8, 0xAC, 0x3A, 0x20, 0x80, 
	  0xA2, 0xB2, 0xAE, 0xAC, 0xA0, 0xB2, 0xA8, 0xB7, 
	  0xA5, 0xB1, 0xAA, 0xA8, 0xA9 
    };//
	
   uchar Loop_H;
   for(Loop_H = 0; Loop_H < 21; Loop_H++)
        {
            if(Table_Text_TestModeManual_Eng[Loop_H] > 0x007c)
            	{
                Table_Text_TestModeManual_Eng[Loop_H]=Search_Table(Table_Text_TestModeManual_Eng[Loop_H]);
            	}
        }



	CopyXCHARtoCHAR(&PrintDataBuff[Datanum],Table_Text_TestModeManual_Eng,21);

    Datanum = Datanum +21;

    PrintDataBuff[Datanum] = 0x0d;

	Datanum++;	
}

/*---------------------------------------------------------------------
  Function Name: Print_Table_Text_JIUJINGHANLIANG
  Description:   None
  Inputs:        None
  Returns:       None
-----------------------------------------------------------------------*/
void Print_Table_Text_JIUJINGHANLIANG()
{
    unsigned char Table_Text_JIUJINGHANLIANG_Eng[]= //Alcohol Content：
    {
     0x90,0xa7,0xa9,0xb5,0xad,0xbe,0xb4,0xa2,0xb4,0x3a,0x0d
    };


	

	CopyXCHARtoCHAR(&PrintDataBuff[Datanum],Table_Text_JIUJINGHANLIANG_Eng,11);

    Datanum = Datanum + 11;

}

/*---------------------------------------------------------------------
  Function Name: Print_Table_Text_NONGDU
  Description:   None
  Inputs:        None
  Returns:       None
-----------------------------------------------------------------------*/
void Print_Table_Text_NONGDU()
{
    unsigned char Table_Text_mg_100ml[] = // "mg/100mL"
    {0x6d,0x67,0x2f,0x31,0x30,
      0x30,0x6d,0x4c,0x0d};
    unsigned char Table_Text_mg_l[] = // "mg/L"
    {0xae, 0xa5, 0x2F, 0xad, 0x0d};
    unsigned char Table_Text_mg_100[] = //"%"
    {0x25,0x0d};
    unsigned char Table_Text_mg_1000ml[] = //"%."
    {0x25,0x2e,0x0d};
    unsigned char Table_Text_g_100ml[] = //"g/100mL"
    {0x67, 0x2F, 0x31, 0x30, 0x30,
      0x6D, 0x4c, 0x0d};
    unsigned char Table_Text_g_210l[] = //"g/210L"
    {0x67, 0x2F, 0x32, 0x31,
      0x30, 0x4c, 0x0d};
    unsigned char Table_Text_ug_100ml[] = //"ug/100mL"
    {0x75, 0x67, 0x2F, 0x31,
     0x30, 0x30, 0x6D, 0x4c, 0x0d};
    unsigned char Table_Text_mg_ml[] = //"mg/mL"
    {0x6D, 0x67, 0x2F, 0x6D, 0x4c, 0x0d};
    unsigned char Table_Text_mg100[] = //"g/L"
    {0x67, 0x2F, 0x4c, 0x0d};

    unsigned char temp_nongduH,temp_nongduL;
    
  
    switch(Printbuff[9])
    {
        case 0://mg/100mL
        temp_nongduH = (Printbuff[10]>>4)&0x0f;
      	 temp_nongduL = Printbuff[10]&0x0f;
      
        Byte_to_PRN_TextBuffer(temp_nongduH+'0',0);

        Byte_to_PRN_TextBuffer(temp_nongduL+'0',1);
      
        temp_nongduH = (Printbuff[11]>>4)&0x0f;
        temp_nongduL = Printbuff[11]&0x0f;

		
        Byte_to_PRN_TextBuffer(temp_nongduH+'0',2);

		Byte_to_PRN_TextBuffer('.',3);

		Byte_to_PRN_TextBuffer(temp_nongduL+'0',4);

        //TextBlock_to_PRN_TextBuffer(Table_Text_mg_100ml,9,3);

		Byte_to_PRN_TextBuffer(' ',5);

		Byte_to_PRN_TextBuffer(' ',6);

		Byte_to_PRN_TextBuffer(' ',7);

		CopyXCHARtoCHAR(&PrintDataBuff[Datanum],Table_Text_mg_100ml,9);

        Datanum = Datanum + 9;
		
         break;

         case 1://mg/L

         if(Flashbuff[15] == 0xaa)
         	{
           Byte_to_PRN_TextBuffer('>',0);
		   Byte_to_PRN_TextBuffer('2',1);
		   Byte_to_PRN_TextBuffer('.',2);
		   Byte_to_PRN_TextBuffer('5',3);
		   Byte_to_PRN_TextBuffer('0',4);
		   Byte_to_PRN_TextBuffer('0',5);

		 }
		 else
		 	{
         temp_nongduH = (Printbuff[10]>>4)&0x0f;
         temp_nongduL = Printbuff[10]&0x0f;
      
         Byte_to_PRN_TextBuffer(temp_nongduH+'0',0);

         Byte_to_PRN_TextBuffer('.',1);

         Byte_to_PRN_TextBuffer(temp_nongduL+'0',2);
      
         temp_nongduH = (Printbuff[11]>>4)&0x0f;
         temp_nongduL = Printbuff[11]&0x0f;
      
         Byte_to_PRN_TextBuffer(temp_nongduH+'0',3);

		 Byte_to_PRN_TextBuffer(temp_nongduL+'0',4);

		 Byte_to_PRN_TextBuffer(' ',5); 
		 	}
		 CopyXCHARtoCHAR(&PrintDataBuff[Datanum],Table_Text_mg_l,5);

		 Datanum = Datanum+5;

         break;

         case 2://%
         temp_nongduH = (Printbuff[10]>>4)&0x0f;
         temp_nongduL = Printbuff[10]&0x0f;
      
         Byte_to_PRN_TextBuffer(temp_nongduH+'0',0);

         Byte_to_PRN_TextBuffer('.',1);

         Byte_to_PRN_TextBuffer(temp_nongduL+'0',2);
      
         temp_nongduH = (Printbuff[11]>>4)&0x0f;
         temp_nongduL = Printbuff[11]&0x0f;
      
         Byte_to_PRN_TextBuffer(temp_nongduH+'0',3);

         Byte_to_PRN_TextBuffer(temp_nongduL+'0',4);
		 CopyXCHARtoCHAR(&PrintDataBuff[Datanum],Table_Text_mg_100,2);

         Datanum = Datanum + 2;
         break;       

         case 3://g/100mL
         temp_nongduH = (Printbuff[10]>>4)&0x0f;
         temp_nongduL = Printbuff[10]&0x0f;
      
         Byte_to_PRN_TextBuffer(temp_nongduH+'0',0);

         Byte_to_PRN_TextBuffer('.',1);

         Byte_to_PRN_TextBuffer(temp_nongduL+'0',2);
      
         temp_nongduH = (Printbuff[11]>>4)&0x0f;
         temp_nongduL = Printbuff[11]&0x0f;
      
         Byte_to_PRN_TextBuffer(temp_nongduH+'0',3);

         //TextBlock_to_PRN_TextBuffer(Table_Text_g_100ml,9,4);

		 CopyXCHARtoCHAR(&PrintDataBuff[Datanum],Table_Text_g_100ml,8);

         Datanum = Datanum + 8;
         break;

         case 4://%.
         temp_nongduH = (Printbuff[10]>>4)&0x0f;
         temp_nongduL = Printbuff[10]&0x0f;
      
         Byte_to_PRN_TextBuffer(temp_nongduH+'0',0);

         Byte_to_PRN_TextBuffer('.',1);

         Byte_to_PRN_TextBuffer(temp_nongduL+'0',2);
      
         temp_nongduH = (Printbuff[11]>>4)&0x0f;
         temp_nongduL = Printbuff[11]&0x0f;
      
         Byte_to_PRN_TextBuffer(temp_nongduH+'0',3);

         Byte_to_PRN_TextBuffer(temp_nongduL+'0',4);

		 CopyXCHARtoCHAR(&PrintDataBuff[Datanum],Table_Text_mg_1000ml,3);

         Datanum = Datanum +3;
         break;         

         case 5://ug/100mL
         temp_nongduH = (Printbuff[10]>>4)&0x0f;
      	  temp_nongduL = Printbuff[10]&0x0f;

      	  Byte_to_PRN_TextBuffer(temp_nongduH+'0',0);

         Byte_to_PRN_TextBuffer(temp_nongduL+'0',1);
      
         temp_nongduH = (Printbuff[11]>>4)&0x0f;
         temp_nongduL = Printbuff[11]&0x0f;
      
         Byte_to_PRN_TextBuffer(temp_nongduH+'0',2);

		 Byte_to_PRN_TextBuffer('.',3);

         Byte_to_PRN_TextBuffer(temp_nongduL+'0',4);

		 CopyXCHARtoCHAR(&PrintDataBuff[Datanum],Table_Text_ug_100ml,9);

         Datanum = Datanum + 9;
				 
         break;

         case 6://mg/mL
         temp_nongduH = (Printbuff[10]>>4)&0x0f;
         temp_nongduL = Printbuff[10]&0x0f;
      
         Byte_to_PRN_TextBuffer(temp_nongduH+'0',0);

         Byte_to_PRN_TextBuffer('.',1);

         Byte_to_PRN_TextBuffer(temp_nongduL+'0',2);
      
         temp_nongduH = (Printbuff[11]>>4)&0x0f;
         temp_nongduL = Printbuff[11]&0x0f;
      
         Byte_to_PRN_TextBuffer(temp_nongduH+'0',3);

         Byte_to_PRN_TextBuffer(temp_nongduL+'0',4);

		 CopyXCHARtoCHAR(&PrintDataBuff[Datanum],Table_Text_mg_ml,6);

         Datanum = Datanum + 6;         	 
         break;

         case 7://mg%
         temp_nongduH = (Printbuff[10]>>4)&0x0f;
      	  temp_nongduL = Printbuff[10]&0x0f;

      	  Byte_to_PRN_TextBuffer(temp_nongduH+'0',0);

		  Byte_to_PRN_TextBuffer('.',1);

         Byte_to_PRN_TextBuffer(temp_nongduL+'0',2);
      
         temp_nongduH = (Printbuff[11]>>4)&0x0f;
         temp_nongduL = Printbuff[11]&0x0f;
      
         Byte_to_PRN_TextBuffer(temp_nongduH+'0',3);

		 

		 Byte_to_PRN_TextBuffer(temp_nongduL+'0',4);

		 Byte_to_PRN_TextBuffer(' ',5);

		 CopyXCHARtoCHAR(&PrintDataBuff[Datanum],Table_Text_mg100,4);

         Datanum = Datanum + 4;
         break;

         case 8://g/210L
         temp_nongduH = (Printbuff[10]>>4)&0x0f;
      	  temp_nongduL = Printbuff[10]&0x0f;

      	  Byte_to_PRN_TextBuffer(temp_nongduH+'0',0);

         Byte_to_PRN_TextBuffer(temp_nongduL+'0',1);
      
         temp_nongduH = (Printbuff[11]>>4)&0x0f;
         temp_nongduL = Printbuff[11]&0x0f;
      
         Byte_to_PRN_TextBuffer(temp_nongduH+'0',2);

		 Byte_to_PRN_TextBuffer('.',3);

		 Byte_to_PRN_TextBuffer(temp_nongduL+'0',4);

		 Byte_to_PRN_TextBuffer(' ',5);

		 CopyXCHARtoCHAR(&PrintDataBuff[Datanum],Table_Text_g_210l,7);

         Datanum = Datanum + 7;
         break;

         default: 
         break;
    }

	 PrintDataBuff[Datanum] = 0x0d;

	Datanum++;



}
/*
void Print_Table_Text_Department_Eng()
{
    Print_Font0816_2xHV_Start();

    TextBlock_to_PRN_TextBuffer(Table_Text_Department_Eng,12,0);

    PRN_Print_TextBuffer();

    Print_Offset();

    Print_Font0816_1xHV_Start();

    Print_1xHV_HyphenBlankLine();
}
*/
/*---------------------------------------------------------------------
  Function Name: Print_Table_Text_GPS
  Description:   None
  Inputs:        None
  Returns:       None
-----------------------------------------------------------------------*/
void Print_Table_Text_GPS()
{
    unsigned char Table_Longitude[15] =    {   0x00 }; //
    unsigned char Table_Latitude[14] =    {   0x00 }; 
    unsigned char *p_Longitude = Table_Longitude;
    unsigned char *p_Latitude = Table_Latitude;
    uchar Loop_H;
	/*
    unsigned char Table_Text_GPS[]= //GPS：
    {
       'G','P','S',0x3a
    }; 
    */
    unsigned char Table_Text_GPS[]= //GPS：
    {
       0x8A, 0xAE, 0xAE, 0xB0, 0xA4, 0xA8, 0xAD, 0xA0, 0xB2, 0xBB,0x3a
    };
    for(Loop_H=0;Loop_H<41;Loop_H++)
    {
        textbuffer[Loop_H]=0x00;
    }

   for(Loop_H = 0; Loop_H < 11; Loop_H++)
        {
            if(Table_Text_GPS[Loop_H] > 0x007c)
            	{
                Table_Text_GPS[Loop_H]=Search_Table(Table_Text_GPS[Loop_H]);
            	}
        }
	
    CopyXCHARtoCHAR(&PrintDataBuff[Datanum],Table_Text_GPS,11);
    //TextBlock_to_PRN_TextBuffer(Table_Text_GPS,4,0);
    Datanum = Datanum+11;

    PrintDataBuff[Datanum] = 0x0d;

	Datanum++;

    if(Printbuff[17]==0)
    {
       	 PrintDataBuff[Datanum] = 0x0d;

	     Datanum++;   
    }
    else
    {
        CopyXCHARtoCHAR(Table_Longitude, &Printbuff[17], 14);

		//Datanum = Datanum+14;

		for(Loop_H = 0; Loop_H < 15; Loop_H++)
        {
            if(*(p_Longitude+Loop_H) == 0x7b)
            	{
                *(p_Longitude+Loop_H)=0xc4;
            	}
        }

        CopyXCHARtoCHAR(&PrintDataBuff[Datanum],Table_Longitude,14);

       Datanum = Datanum+14;

	   PrintDataBuff[Datanum] = 0x0d;

	   Datanum++;
    }
	
    if(Printbuff[32]<=0x20)
    {
       	 PrintDataBuff[Datanum] = 0x0d;

	     Datanum++;   
    }
    else
    {
        //CopyChar(Table_Latitude, &Printbuff[32], 13);
        CopyXCHARtoCHAR(Table_Latitude, &Printbuff[32], 13);
	    for(Loop_H = 0; Loop_H < 14; Loop_H++)
        {
            if(*(p_Latitude+Loop_H) == 0x007b)
            	{
                *(p_Latitude+Loop_H)=0x00c4;
            	}
        }

        //TextBlock_to_PRN_TextBuffer(Table_Latitude,14,0);
        CopyXCHARtoCHAR(&PrintDataBuff[Datanum],Table_Latitude,13);
		
        Datanum = Datanum+13;

		PrintDataBuff[Datanum] = 0x0d;

	    Datanum++;
    }
	
   Print_UnderLine();
}

/*---------------------------------------------------------------------
  Function Name: Print_Table_Text_teperature
  Description:   None
  Inputs:        None
  Returns:       None
-----------------------------------------------------------------------*/
void Print_Table_Text_temperature()
{
  //XCHAR  recordtem;
  
    unsigned char Table_Text_temperature[]= //Refuse Test
    {
      0x92, 0xA5, 0xAC, 0xAF,0x2e,0x3a,0x020,0x020,0x020,0x020,0xc4,0x91
    };
  
   uchar Loop_H;
   for(Loop_H = 0; Loop_H < 4; Loop_H++)
        {
            if(Table_Text_temperature[Loop_H] > 0x007c)
            	{
                Table_Text_temperature[Loop_H]=Search_Table(Table_Text_temperature[Loop_H]);
            	}
        }
	
    

   	Table_Text_temperature[9]=(Printbuff[13]%10+'0');
                
	Table_Text_temperature[8]=(Printbuff[13]/10+'0');

	if(Flashbuff[14]==1)
		Table_Text_temperature[7]=0x2D;
	else
		Table_Text_temperature[7]=0x20;
	
    CopyXCHARtoCHAR(&PrintDataBuff[Datanum],Table_Text_temperature,12);

	Datanum = Datanum + 12;

	PrintDataBuff[Datanum] = 0x0d;

	Datanum++;

	PrintDataBuff[Datanum] = 0x0a;

	Datanum++;



	
}

/*---------------------------------------------------------------------
  Function Name: Print_Table_Text_SignatureCheck
  Description:   None
  Inputs:        None
  Returns:       None
-----------------------------------------------------------------------*/
void Print_Table_Text_SignatureCheck()
{
    unsigned char Print_Table_Text_SignatureCheck[]= //"被测试人："
    {
     0x8F, 0xAE, 0xA4, 0xAF, 0xA8, 0xB1, 0xBC,
	 0x20, 0x8E, 0xA1, 0xB1, 0xAB, 0xA5, 0xA4,
	 0x2E, 0x3A
    };

	   uchar Loop_H;
   for(Loop_H = 0; Loop_H < 16; Loop_H++)
        {
            if(Print_Table_Text_SignatureCheck[Loop_H] > 0x007c)
            	{
                Print_Table_Text_SignatureCheck[Loop_H]=Search_Table(Print_Table_Text_SignatureCheck[Loop_H]);
            	}
        }


    CopyXCHARtoCHAR(&PrintDataBuff[Datanum],Print_Table_Text_SignatureCheck,16);
               Datanum = Datanum+16;

   // TextBlock_to_PRN_TextBuffer(Print_Table_Text_SignatureCheck,17,0);

      //	 PrintDataBuff[Datanum] = 0x0d;

	  //   Datanum++;

       	 PrintDataBuff[Datanum] = 0x0d;

	     Datanum++;

		 Print_UnderLine();

}


/*---------------------------------------------------------------------
  Function Name: Print_Table_Text_Signature
  Description:   None
  Inputs:        None
  Returns:       None
-----------------------------------------------------------------------*/
void Print_Table_Text_Signature()
{
    unsigned char Print_Table_Text_Signature[]= //"被测试人："
    {
     0x8F, 0xAE, 0xA4, 0xAF, 0xA8, 0xB1, 0xBC, 0x3A
    };

		   uchar Loop_H;
   for(Loop_H = 0; Loop_H < 8; Loop_H++)
        {
            if(Print_Table_Text_Signature[Loop_H] > 0x007c)
            	{
                Print_Table_Text_Signature[Loop_H]=Search_Table(Print_Table_Text_Signature[Loop_H]);
            	}
        }

    CopyXCHARtoCHAR(&PrintDataBuff[Datanum],Print_Table_Text_Signature,8);
               Datanum = Datanum+8;
    //TextBlock_to_PRN_TextBuffer(Print_Table_Text_Signature,9,0);



      	 PrintDataBuff[Datanum] = 0x0d;

	     Datanum++;

       	// PrintDataBuff[Datanum] = 0x0a;

	    // Datanum++;

		 Print_UnderLine();


}

void Print_UnderLine(void)
{
/*
    int m;


	
	for(m=0;m<30;m++)
  
      PrintDataBuff[Datanum+m]=95;
   
    Datanum = Datanum + 30;
	
	PrintDataBuff[Datanum] = 0x0d;

	Datanum++;
*/
	//PrintDataBuff[Datanum] = 0x0d;

	//Datanum++;
     PrintDataBuff[Datanum] = 0x0d;

	 Datanum++;
	 
	 PrintDataBuff[Datanum] = 0x0d;

	  Datanum++;
	  
	 PrintDataBuff[Datanum] = 0x0d;

	 Datanum++;
	
	PrintDataBuff[Datanum] = 0x0d;

	Datanum++;

	PrintDataBuff[Datanum] = 0x0d;

	Datanum++;

	PrintDataBuff[Datanum] = 0x0d;

	Datanum++;

	PrintDataBuff[Datanum] = 0x0a;

	Datanum++;
	//PrintDataBuff[Datanum] = 0x0d;

	//Datanum++;	
	
}

void Print_datareturn(void)
{


	PrintDataBuff[Datanum] = 0x0a;

	Datanum++;
	
	
}

void Printdata(XCHAR *Str)

{

	uchar  pz;
		
    int m;
	
    uchar ItemString[20] = {0x00}; 

	for(m=0;m<20;m++)
    {
        ItemString[m]=0;
    }
	
	
    CopyXCHARtoCHAR(ItemString, Str, 19);

    pz=Search_zero(ItemString);			   
        		
	
	CopyXCHARtoCHAR(&PrintDataBuff[Datanum],ItemString,pz);

	Datanum = Datanum + pz;
/*
	for(m=0;m<(30-pz);m++)
    {
        PrintDataBuff[Datanum+m]=0x20;
    }

	Datanum = Datanum+m;
*/
//	PrintDataBuff[Datanum] = 0x0d;

//	Datanum++;


}

void Print_ITEM(WORD ItemAddress)
{ 

	uchar *p_buffer = textbuffer;

	uchar  pz;
		
    int m;
     uchar Loop_H;

				
	DelayMs(50);
	
    for(m=0;m<41;m++)
    {
        textbuffer[m]=0x00;
    }

    EEPROMReadArray(ItemAddress,p_buffer,20); 

	   for(Loop_H = 0; Loop_H < 19; Loop_H++)
        {
            if(*(p_buffer+Loop_H) > 0x7c)
            	{
                *(p_buffer+Loop_H)=Search_Table(*(p_buffer+Loop_H));
            	}			
        }
    	 for(Loop_H = 0; Loop_H < 20; Loop_H++)
        {
		 if(*(p_buffer+Loop_H) == 0)
			{
				*(p_buffer+Loop_H) = 0x3a;
				 Loop_H++;
				*(p_buffer+Loop_H) = 0;
				break;
				}
    	 }

	pz=Search_zero(textbuffer);

	CopyXCHARtoCHAR(&PrintDataBuff[Datanum],textbuffer,pz);

	Datanum = Datanum+pz;

	PrintDataBuff[Datanum] = 0x0d;
	
    Datanum++;

	PrintDataBuff[Datanum] = 0x0d;

	Datanum++;

	PrintDataBuff[Datanum] = 0x0d;

	Datanum++;
}

/*---------------------------------------------------------------------
  Function Name:  Print_Table_Text_Eng
  Description:   None
  Inputs:        None
  Returns:       None
-----------------------------------------------------------------------*/
void Print_Table_Text_Eng(WORD ItemAddress)
{   

  uchar *p_buffer = textbuffer;
    int m;
    XCHAR ItemString[17] = {0x00}; 

    for(m=0;m<41;m++)
    {
        textbuffer[m]=0x00;
    }

    //EEPROMReadArray(&PrintDataBuff[Datanum],p_buffer,16); 
    Print_ITEM(ItemAddress);

    switch(ItemAddress)// ItemState
    {
        case Item0Address:

			if(Printbuff[49]==0)
				
        		    Print_UnderLine(); 



        	else
        		{
                  Printdata(&Printbuff[49]);
                  Print_datareturn();
        		}
                break;
				
        case Item1Address:


			if(Printbuff[68]==0)
				
				{
				
        			 Print_UnderLine();    
                      
				}
                else
                	{
                       Printdata(&Printbuff[68]);
					   Print_datareturn();

	                   
        		    }
                break;
				
        case Item2Address:

    			if(Printbuff[87]==0)

        			Print_UnderLine();    

    			else
    				{
                   Printdata(&Printbuff[87]);
                   Print_datareturn(); 

				}
                break;
				
        case Item3Address:

    			if(Printbuff[106]==0)

        			Print_UnderLine();    

    			else
    				{
                   Printdata(&Printbuff[106]);
                   Print_datareturn();
				}
                break;
        case Item4Address:

    			if(Printbuff[125]==0)

        			Print_UnderLine();    

    			else
    				{
                   Printdata(&Printbuff[125]);
                   Print_datareturn();
				}
                break;	
        case Item5Address:

    			if(Printbuff[144]==0)

        			Print_UnderLine();    

    			else
    				{
                   Printdata(&Printbuff[144]);
				   Print_datareturn();
    				}
                break;
        case Item6Address:

    			if(Printbuff[163]==0)

        			Print_UnderLine();    

    			else
    				{
                   Printdata(&Printbuff[163]);
                   Print_datareturn();
				}
                break;
        case Item7Address:

    			if(Printbuff[182]==0)

        			Print_UnderLine();    

    			else
    				{
                   Printdata(&Printbuff[182]);
                   Print_datareturn();
				}
				break; 	
        case Item8Address:

    			if(Printbuff[201]==0)

        			Print_UnderLine();    

    			else
    				{
                   Printdata(&Printbuff[201]);
                   Print_datareturn();
				}
				break; 	
        case Item9Address:

    			if(Printbuff[220]==0)

        			Print_UnderLine();    

    			else
    				{
                   Printdata(&Printbuff[220]);
                   Print_datareturn();

				}
				break; 	                       	
        default:
                break;
    }

}

void Inputdata(void)

{

      		memset(&tx1_buff[0],0,250);
			
			memset(&rx1_buff[0],0,250);
			
      		Datanum = 0;
		 
#ifndef No_Print_Title
        Print_Table_Text_Alcovisor_Mercury();
#endif          
          


       // Datanum  = 38;
            Print_Table_Text_YIQIHAO();
		  


       // Datanum  = 52;
            Print_Table_Text_JILUHAO();
		  
 

            Print_Table_Text_RIQI();


            Print_Table_Text_SHIJIAN();

#ifdef PRINTBLANK
            Print_Table_Text_GUILING();
#endif
            Last_Calibration_Date();

			Print_Last_Calibration_Date();

#ifdef TradeMark_Version

			Timeto_Calibration();

#endif

            if(Printbuff[16]&(ST_REFUSE))
                Print_Table_Text_RefuseTest();
            else if(Printbuff[16]&(ST_Discontinued))
				Print_Table_Text_Discontiued();
            else 
				{
					if(Printbuff[16]&(ST_Passive))

               		Print_Table_Text_TestModePassive();

            		else 

               			Print_Table_Text_TestModeManual();


               			Print_Table_Text_JIUJINGHANLIANG();


               			Print_Table_Text_NONGDU();
            	}
			   
                 if(EEPROMReadByte(ID_CHECKBOXADDRESS)==1)
				
                    Print_Table_Text_Eng(Item0Address);
	 
			 
                 if(EEPROMReadByte(ID_CHECKBOXADDRESS+1)==1)
				
                    Print_Table_Text_Eng(Item1Address);

	  	
                 if(EEPROMReadByte(ID_CHECKBOXADDRESS+2)==1)
				
                    Print_Table_Text_Eng(Item2Address);

		
                 if(EEPROMReadByte(ID_CHECKBOXADDRESS+3)==1)

			
                    Print_Table_Text_Eng(Item3Address);


                 if(EEPROMReadByte(ID_CHECKBOXADDRESS+4)==1)
				 	
				
                   Print_Table_Text_Eng(Item4Address);


		
                  if(EEPROMReadByte(ID_CHECKBOXADDRESS+5)==1)
				  	
				
                    Print_Table_Text_Eng(Item5Address);

		
                  if(EEPROMReadByte(ID_CHECKBOXADDRESS+6)==1)
				
                    Print_Table_Text_Eng(Item6Address);


   	
                 if(EEPROMReadByte(ID_CHECKBOXADDRESS+7)==1)
				
                    Print_Table_Text_Eng(Item7Address);


   	
                if(EEPROMReadByte(ID_CHECKBOXADDRESS+8)==1)
				
                    Print_Table_Text_Eng(Item8Address);


   	
                if(EEPROMReadByte(ID_CHECKBOXADDRESS+9)==1)
				
                    Print_Table_Text_Eng(Item9Address);
				
          if(EEPROMReadByte(ID_CHECKBOXADDRESS + 28))
              Print_Table_Text_GPS();

	//	  Print_Table_Text_temperature();

			Print_Table_Text_SignatureCheck();

			Print_Table_Text_Signature();

			   memset(&rx1_buff[0],0,200);
			   rx1_ptr   = &rx1_buff[0];

}
/*---------------------------------------------------------------------
  Function Name: Print_TestRecord
  Description:   None
  Inputs:        None
  Returns:       None
-----------------------------------------------------------------------*/
void Print_TestRecord()
{

if(rx1_buff[3]==240)
   SPrint = TRUE;

if(SPrint)
  {


if(Datanum-Sprinnum>=240)
  {

  	p_n++;
	
    memset(&tx1_buff[0],0,250); 

	
	U1STAbits.UTXEN=1;
	
    tx1_ptr       = &tx1_buff[0];

   
    tx1_ptrend    = tx1_ptr; 

	tx1_buff[0] = 0x1b;

	tx1_buff[1] = 0x10;

	tx1_buff[2] = p_n;

	tx1_buff[3] = 240;
	
    CopyXCHARtoCHAR(&tx1_buff[4],&PrintDataBuff[Sprinnum],240);

	
    Sprinnum = Sprinnum+ 240;



   	SPrint=FALSE;
	
    tx1_ptrend   += 244; 

	UART1transtart();


	  }	

 else if((Datanum-Sprinnum>=1)&&(Datanum-Sprinnum<240))
	{

  	p_n++;
	
    memset(&tx1_buff[0],0,250); 

	
	U1STAbits.UTXEN=1;
	
    tx1_ptr       = &tx1_buff[0];

   
    tx1_ptrend    = tx1_ptr; 

	tx1_buff[0] = 0x1b;

	tx1_buff[1] = 0x10;

	tx1_buff[2] = p_n;

	tx1_buff[3] = Datanum-Sprinnum;
	
    CopyXCHARtoCHAR(&tx1_buff[4],&PrintDataBuff[Sprinnum],Datanum-Sprinnum);

	
    Sprinnum = Datanum;

	//p_n++;

   	SPrint=FALSE;
	
    tx1_ptrend   += tx1_buff[3]+4;

	UART1transtart();

}
 else if(Sprinnum == Datanum)
  {

   // endprint = TRUE;

    StartPrint();

    SPrint=FALSE;


    Sprinnum =0;

    Datanum = 0;

   }
memset(&rx1_buff[0],0,20);
rx1_ptr   = &rx1_buff[0];


}

 if(endprint )
   	{
				ChoicePrint = FALSE;

				SPrint=FALSE;

				p_n =0;

				 Sprinnum =0;

				 Datanum = 0;

				 // _INT2IE = 1;
         //if(!EEPROMReadByte(ID_CHECKBOXADDRESS+26))		 	
				// endprint = FALSE;



   	}





/*
switch(Printturn)	
	
    {

	    case  0x01:

			 memset(&rx1_buff[0],0,200);
		    rx1_ptr = &rx1_buff[0];

		//	FFPrinter();

			AwakenPrinter();

			DelayMs(100);

			ESCMnPrinter(1);

			DelayMs(100);

			ESCSPnPrinter(1);

			DelayMs(100);

			ESC_nPrinter(0);

			DelayMs(50);

		   Printturn = 0xa0;
		
		   break;
		

         case 0xa0:
            Print_Table_Text_Alcovisor_Mercury();
          
            Printturn++;

			SPrint=FALSE;

			break;

		 case 0xa1:	

		   if(SPrint)
         	{
            Print_Table_Text_YIQIHAO();
		  
            Printturn++;

			SPrint=FALSE;
		   	}
		   break;

		 case  0xa2:  

		   if(SPrint)
         	{
            Print_Table_Text_JILUHAO();
		  
            Printturn++;

			SPrint=FALSE;			  	
         	}
		   break;
        case  0xa3: 

		   if(SPrint)
         	{
            Print_Table_Text_RIQI();
            Printturn++;

			SPrint=FALSE;	  	
         	}
		    break;
       case   0xa4:

		   if(SPrint)
         	{
            Print_Table_Text_SHIJIAN();

            Printturn++;

			SPrint=FALSE;
         	}
		    break;

	   case  0xa5:	   
    
		   if(SPrint)
         	{
            Print_Table_Text_GUILING();

            Printturn++;

			SPrint=FALSE;

          	}
		    break;

       case   0xa6:

              if(SPrint)
         	{

            if(Printbuff[16]&(ST_REFUSE))
            {
                Print_Table_Text_RefuseTest();

			    Printturn++;

			    SPrint=FALSE;

            }
			else
			 Printturn++;
              	}
			 break;

       case	 0xa7:		  
              if(SPrint)
         	{
                if(Printbuff[16]&(ST_Passive))
                {

                    Print_Table_Text_TestModePassive();

				    Printturn++;

			        SPrint=FALSE;

                }
                else
					Printturn++;
             }
			   break;

	   case   0xa8:		  
              if(SPrint)
         	{

                if(Printbuff[16]&(ST_AUTO))
                  {
                    Print_Table_Text_TestModeManual();

				    Printturn++;

			        SPrint=FALSE;

			       }
                else
					Printturn++;
                }
			   break;

	  case	0xa9:	  

              if(SPrint)
         	{

                Print_Table_Text_JIUJINGHANLIANG();

				    Printturn++;

			        SPrint=FALSE;

           	}	
			  break;

			  
       case  0xaa:

              if(SPrint)
         	{
                Print_Table_Text_NONGDU();

				    Printturn++;

			        SPrint=FALSE;
           
            }
			  break;

	   case   0xab:	

		    if(SPrint)
		    	{
        			ESCJnPrinter(1); 
					DelayMs(50);
					Printturn++;

		    	}
			break;
       case   0xac:
	   	
                 if(EEPROMReadByte(ID_CHECKBOXADDRESS)==1)
				
                    Print_Table_Text_Eng(Item0Address);

			     else  Printturn++;
				 
			     break;

	  case	 0xad:		 
			 
                 if(EEPROMReadByte(ID_CHECKBOXADDRESS+1)==1)
				
                    Print_Table_Text_Eng(Item1Address);

			     else  Printturn++;
				 
			     break;

	  case  0xae:	
	  	
            if(EEPROMReadByte(ID_CHECKBOXADDRESS+2)==1)
				
                    Print_Table_Text_Eng(Item2Address);

			     else  Printturn++;
				 
			     break;

	case    0xaf:
		
            if(EEPROMReadByte(ID_CHECKBOXADDRESS+3)==1)

			
                    Print_Table_Text_Eng(Item3Address);

			     else  Printturn++;
				 
			     break;

	case    0xb0:
		
            if(EEPROMReadByte(ID_CHECKBOXADDRESS+4)==1)
				
                    Print_Table_Text_Eng(Item4Address);

			     else  Printturn++;
				 
			     break;

	case   0xb1:
		
            if(EEPROMReadByte(ID_CHECKBOXADDRESS+5)==1)
				
                    Print_Table_Text_Eng(Item5Address);

			     else  Printturn++;
				 
			     break;

	case  0xb2:
		
            if(EEPROMReadByte(ID_CHECKBOXADDRESS+6)==1)
				
                    Print_Table_Text_Eng(Item6Address);

			     else  Printturn++;
				 
			     break;

   case  0xb3:
   	
            if(EEPROMReadByte(ID_CHECKBOXADDRESS+7)==1)
				
                    Print_Table_Text_Eng(Item7Address);

			     else  Printturn++;
				 
			     break;

   case  0xb4:
   	
            if(EEPROMReadByte(ID_CHECKBOXADDRESS+8)==1)
				
                    Print_Table_Text_Eng(Item8Address);

			     else  Printturn++;
				 
			     break;

   case  0xb5:
   	
            if(EEPROMReadByte(ID_CHECKBOXADDRESS+9)==1)
				
                    Print_Table_Text_Eng(Item9Address);

			     else  Printturn++;
				 
			     break;

   case  0xb6:
			    
			 if(SPrint)
         	{	

			    ESC_nPrinter(0);
				
	            DelayMs(50);
				
				LFPrinter();

				DelayMs(50);

			    CRPrinter();

			    FFPrinter();

				Printturn = 0x01;

				ChoicePrint = FALSE;

				SPrint=FALSE;

				 ClrState(pbtn, BTN_DISABLED);
				 
	             SetState(pbtn, BTN_DRAW);
			 }
                break;

    default	:

		     break;
			
      }
         


	pn=0;
	Printercounter=0;
	*/
}

