#include <reg52.h>		   		// ͷ�ļ�����
#include <intrins.h>

#define uchar unsigned char	// �Ժ�unsigned char�Ϳ�����uchar����
#define uint  unsigned int	// �Ժ�unsigned int �Ϳ�����uint ����
/*******************************������**********************************/
typedef unsigned char  U8;       /* defined for unsigned 8-bits integer variable           �޷���8λ���ͱ���  */
typedef unsigned int   U16;      /* defined for unsigned 16-bits integer variable           �޷���16λ���ͱ��� */
#define uchar unsigned char
#define uint unsigned int
//----------------IO�ڶ�����--------------------//
sbit  P2_0  = P2^0 ;

//----------------������--------------------//
U8  U8temp;
U8  humidity_H,humidity_L,temperature_H,temperature_L,checkdata;
U8  str[5]= {"RS232"};

/*******************************������**********************************/
sfr ISP_DATA  = 0xe2;			// ���ݼĴ���
sfr ISP_ADDRH = 0xe3;			// ��ַ�Ĵ����߰�λ
sfr ISP_ADDRL = 0xe4;			// ��ַ�Ĵ����Ͱ�λ
sfr ISP_CMD   = 0xe5;			// ����Ĵ���
sfr ISP_TRIG  = 0xe6;			// ������Ĵ���
sfr ISP_CONTR = 0xe7;			// ����Ĵ���

sbit Buzzer_P  = P1^6;    // ������
sbit DHT11_P   = P1^7;	 	// ��ʪ�ȴ�����DHT11���ݽ���
sbit LcdRs_P   = P2^5;    // 1602Һ����RS�ܽ�       
sbit LcdRw_P   = P2^6;    // 1602Һ����RW�ܽ� 
sbit LcdEn_P   = P2^7;    // 1602Һ����EN�ܽ�
sbit KeySet_P  = P3^2;		// �����á������Ĺܽ�
sbit KeyDown_P = P3^4;		// �����������Ĺܽ�
sbit KeyUp_P   = P3^3;		// ���ӡ������Ĺܽ� 
sbit LedHL_P   = P1^0;		// ʪ�ȹ���
sbit LedHH_P   = P1^1;		// ʪ�ȹ���
sbit LedTL_P   = P1^2;		// �¶ȹ���
sbit LedTH_P   = P1^3;		// �¶ȹ���

uchar temp;					// �����¶�
uchar humi;					// ����ʪ��

uchar AlarmTL;			// �¶����ޱ���ֵ
uchar AlarmTH;			// �¶����ޱ���ֵ
uchar AlarmHL;			// ʪ�����ޱ���ֵ
uchar AlarmHH;			// ʪ�����ޱ���ֵ


/*******************************������**********************************/


void Ble_Delay(U16 j)
{   U8 i;
    for(; j>0; j--)
    {
        for(i=0; i<27; i++);
    }
}

/**
*���ڷ���һ���ֽ�
*/
void sendOneChar(uchar a)
{
    SBUF = a;
    while(TI==0);
    TI=0;
}


/**
*��ʱ��������λ ms
*/
void Ble_delayms(uint x)
{
    uint i,j;
    for(i=x; i>0; i--)
        for(j=110; j>0; j--);
}
/**
*��ʱ������ ��ʱ10us
*/
void  Ble_Delay_10us(void)
{
    U8 i;
    i--;
    i--;
    i--;
    i--;
    i--;
    i--;
}

/**
*��ʱ������ ��ʱ10us
*/
void Ble_delay13us()
{
	U8 i;
	i--;
	i--;
	i--;
	i--;
	i--;
	i--;
	i--;
	i--;
}
/**
*����ʱ�������ʪ��ֵ
*/
U8 computeData()
{
    U8 i,U8comdata;
    for(i=0; i<8; i++)
    {
        while(P2_0==0);				//�ȴ�54us�͵�ƽ
        Ble_Delay_10us();					//��ʱ30usȷ�ϸߵ�ƽ�Ƿ����
        Ble_Delay_10us();
        Ble_Delay_10us();
        U8temp=0;
        if(P2_0==1)						//����ߵ�ƽ�߹�Ԥ��0�ߵ�ƽ
        {											//ֵ������λΪ 1
            U8temp=1;
        }
        while(P2_0==1);			//�ȴ��ߵ�ƽ����
        U8comdata<<=1;
        U8comdata|=U8temp;
    }
    return U8comdata;
}

//--------------------------------
//-----ʪ�ȶ�ȡ�ӳ��� ------------
//--------------------------------
//----���±�����Ϊȫ�ֱ���--------
//----�¶ȸ�8λ== temperature_H------
//----�¶ȵ�8λ== temperature_L------
//----ʪ�ȸ�8λ== humidity_H-----
//----ʪ�ȵ�8λ== humidity_L-----
//----У�� 8λ == checkdata-----
//--------------------------------
void readData()
{
    U8  humidity_H_temp,humidity_L_temp,temperature_H_temp,temperature_L_temp,checkdata_temp;
    //�������Ͳ�����18ms
    P2_0=0;
    Ble_delayms(20);
		//������������������ ������ʱ13us���20us �ȴ�DHT11�͵�ƽ��Ӧ
    P2_0=1;
		Ble_delay13us();
    //�жϴӻ��Ƿ��е͵�ƽ��Ӧ�ź�
    if(P2_0==0)
    {
        while(P2_0==0);				//�ж�DHT11���� 83us �ĵ͵�ƽ��Ӧ�ź��Ƿ����
        while(P2_0==1);				//�ж�DHT11���� 87us �ĸߵ�ƽ�Ƿ����
        //���ݽ���״̬
        humidity_H_temp = computeData();
        humidity_L_temp = computeData();
        temperature_H_temp = computeData();
        temperature_L_temp = computeData();
        checkdata_temp = computeData();
        P2_0 = 1;		//data�����ͷ�����
        //����У��
        if(checkdata_temp == humidity_H_temp + humidity_L_temp + temperature_H_temp + temperature_L_temp)
        {
            humidity_H = humidity_H_temp;
            humidity_L = humidity_L_temp;
            temperature_H = temperature_H_temp;
            temperature_L = temperature_L_temp;
            checkdata = checkdata_temp;
        }
    }

}

/*******************************������**********************************/

/*********************************************************/
// ��Ƭ���ڲ�EEPROM��ʹ��
/*********************************************************/
void ISP_Disable()
{
	ISP_CONTR = 0;
	ISP_ADDRH = 0;
	ISP_ADDRL = 0;
}


/*********************************************************/
// �ӵ�Ƭ���ڲ�EEPROM��һ���ֽڣ���0x2000��ַ��ʼ
/*********************************************************/
unsigned char EEPROM_Read(unsigned int add)
{
	ISP_DATA  = 0x00;
	ISP_CONTR = 0x83;
	ISP_CMD   = 0x01;
	ISP_ADDRH = (unsigned char)(add>>8);
	ISP_ADDRL = (unsigned char)(add&0xff);
	// ��STC89C51ϵ����˵��ÿ��Ҫд��0x46����д��0xB9,ISP/IAP�Ż���Ч
	ISP_TRIG  = 0x46;	   
	ISP_TRIG  = 0xB9;
	_nop_();
	ISP_Disable();
	return (ISP_DATA);
}


/*********************************************************/
// ����Ƭ���ڲ�EEPROMдһ���ֽڣ���0x2000��ַ��ʼ
/*********************************************************/
void EEPROM_Write(unsigned int add,unsigned char ch)
{
	ISP_CONTR = 0x83;
	ISP_CMD   = 0x02;
	ISP_ADDRH = (unsigned char)(add>>8);
	ISP_ADDRL = (unsigned char)(add&0xff);
	ISP_DATA  = ch;
	ISP_TRIG  = 0x46;
	ISP_TRIG  = 0xB9;
	_nop_();
	ISP_Disable();
}


/*********************************************************/
// ������Ƭ���ڲ�EEPROM��һ������
// д8�����������һ���ĵ�ַ���������������д��ǰҪ�Ȳ���
/*********************************************************/
void Sector_Erase(unsigned int add)	  
{
	ISP_CONTR = 0x83;
	ISP_CMD   = 0x03;
	ISP_ADDRH = (unsigned char)(add>>8);
	ISP_ADDRL = (unsigned char)(add&0xff);
	ISP_TRIG  = 0x46;
	ISP_TRIG  = 0xB9;
	_nop_();
	ISP_Disable();
}


/*********************************************************/
// ���뼶����ʱ������time��Ҫ��ʱ�ĺ�����
/*********************************************************/
void DelayMs(uint time)
{
	uint i,j;
	for(i=0;i<time;i++)
		for(j=0;j<112;j++);
}


/*********************************************************/
// 1602Һ��д�������cmd����Ҫд�������
/*********************************************************/
void LcdWriteCmd(uchar cmd)
{ 
	LcdRs_P = 0;
	LcdRw_P = 0;
	LcdEn_P = 0;
	P0=cmd;
	DelayMs(2);
	LcdEn_P = 1;    
	DelayMs(2);
	LcdEn_P = 0;	
}


/*********************************************************/
// 1602Һ��д���ݺ�����dat����Ҫд�������
/*********************************************************/
void LcdWriteData(uchar dat)
{
	LcdRs_P = 1; 
	LcdRw_P = 0;
	LcdEn_P = 0;
	P0=dat;
	DelayMs(2);
	LcdEn_P = 1;    
	DelayMs(2);
	LcdEn_P = 0;
}


/*********************************************************/
// 1602Һ����ʼ������
/*********************************************************/
void LcdInit()
{
	LcdWriteCmd(0x38);        // 16*2��ʾ��5*7����8λ���ݿ�
	LcdWriteCmd(0x0C);        // ����ʾ������ʾ���
	LcdWriteCmd(0x06);        // ��ַ��1����д�����ݺ�������
	LcdWriteCmd(0x01);        // ����
}


/*********************************************************/
// Һ����궨λ����
/*********************************************************/
void LcdGotoXY(uchar line,uchar column)
{
	// ��һ��
	if(line==0)        
		LcdWriteCmd(0x80+column); 
	// �ڶ���
	if(line==1)        
		LcdWriteCmd(0x80+0x40+column); 
}


/*********************************************************/
// Һ������ַ�������
/*********************************************************/
void LcdPrintStr(uchar *str)
{
	while(*str!='\0') 			// �ж��Ƿ��ַ����ľ�ͷ��
		LcdWriteData(*str++);
}


/*********************************************************/
// Һ���������
/*********************************************************/
void LcdPrintNum(uchar num)
{
	LcdWriteData(num/10+48);	// ʮλ
	LcdWriteData(num%10+48); 	// ��λ
}


/*********************************************************/
// Һ����ʾ���ݵĳ�ʼ��
/*********************************************************/
void LcdShowInit()
{
	LcdGotoXY(0,0);								// ��0�е���ʾ����
	LcdPrintStr("  DHT11 System  ");
	LcdGotoXY(1,0);						   	// ��1�е���ʾ����
	LcdPrintStr("T:   C   H:  %RH");
	LcdGotoXY(1,4);								// �¶ȵ�λ���϶������ԲȦ����
	LcdWriteData(0xdf);	
}



/*********************************************************/
// 10us����ʱ����
/*********************************************************/
void Delay10us()
{
	_nop_();	// ִ��һ��ָ���ʱ1΢��
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
}


/*********************************************************/
// ��ȡDHT11�������ϵ�һ���ֽ�
/*********************************************************/
uchar DhtReadByte(void)
{   
	bit bit_i; 
	uchar j;
	uchar dat=0;

	for(j=0;j<8;j++)    
	{
		while(!DHT11_P);	// �ȴ��͵�ƽ����	
		Delay10us();			// ��ʱ
		Delay10us();
		Delay10us();
		if(DHT11_P==1)		// �ж��������Ǹߵ�ƽ���ǵ͵�ƽ
		{
			bit_i=1; 
			while(DHT11_P);
		} 
		else
		{
			bit_i=0;
		}
		dat<<=1;		   		// ����λ��λ���浽dat������
		dat|=bit_i;    
	}
	return(dat);  
}


/*********************************************************/
// ��ȡDHT11��һ֡���ݣ�ʪ�ߡ�ʪ��(0)���¸ߡ��µ�(0)��У����
/*********************************************************/
void ReadDhtData()
{    	 
	uchar HumiHig;		// ʪ�ȸ߼��ֵ
	uchar HumiLow;		// ʪ�ȵͼ��ֵ 
	uchar TemHig;			// �¶ȸ߼��ֵ
	uchar TemLow;			// �¶ȵͼ��ֵ
	uchar check;			// У���ֽ� 
	
	DHT11_P=0;				// ��������
	DelayMs(20);			// ����20����
	DHT11_P=1;				// DATA������������������

	Delay10us();	 		// ��ʱ�ȴ�30us
	Delay10us();
	Delay10us();

	while(!DHT11_P);	// �ȴ�DHT�ĵ͵�ƽ����
	while(DHT11_P);		// �ȴ�DHT�ĸߵ�ƽ����

	//�������ݽ���״̬
	HumiHig = DhtReadByte(); 	// ʪ�ȸ�8λ
	HumiLow = DhtReadByte(); 	// ʪ�ȵ�8Ϊ����Ϊ0
	TemHig  = DhtReadByte(); 	// �¶ȸ�8λ 
	TemLow  = DhtReadByte(); 	// �¶ȵ�8Ϊ����Ϊ0 
	check   = DhtReadByte();	// 8λУ���룬��ֵ���ڶ������ĸ��ֽ����֮�͵ĵ�8λ

	while(!DHT11_P);
	DHT11_P=1;								// ��������

	if(check==HumiHig + HumiLow + TemHig + TemLow) 		// ����յ�����������
	{
		temp=TemHig; 			// ���¶ȵļ������ֵ��ȫ�ֱ���temp
		humi=HumiHig;			// ��ʪ�ȵļ������ֵ��ȫ�ֱ���humi
	}
}


/*********************************************************/
// �Ƿ���Ҫ�����ж�
/*********************************************************/
void AlarmJudge(void)
{
	// uchar i;

	if(temp>AlarmTH)				// �¶��Ƿ����
	{
		LedTH_P=0;
		LedTL_P=1;
	}
	else if(temp<AlarmTL)		// �¶��Ƿ����
	{
		LedTL_P=0;
		LedTH_P=1;
	}
	else										// �¶�����
	{
		LedTH_P=1;
		LedTL_P=1;
	}

	if(humi>AlarmHH)	   		// ʪ���Ƿ����
	{
		LedHH_P=0;
	  LedHL_P=1;
	}
	else if(humi<AlarmHL)		// ʪ���Ƿ����
	{
		LedHL_P=0;
		LedHH_P=1;
	}
	else				   					// ʪ������
	{
		LedHH_P=1;
		LedHL_P=1;
	}
}


/*********************************************************/
// ����ɨ�裬����������ʪ�ȱ�����Χ
/*********************************************************/
void KeyScanf()
{
	if(KeySet_P==0)		// �ж����ð����Ƿ񱻰���
	{
		/*��Һ����ʾ��Ϊ����ҳ���*******************************************************/

		LcdWriteCmd(0x01);				// ���ý������ʾ���
		LcdGotoXY(0,0);
		LcdPrintStr("Temp:   -       ");
		LcdGotoXY(1,0);
		LcdPrintStr("Humi:   -       ");
		
		LcdGotoXY(0,6);	 					// ��Һ��������¶ȵ�����ֵ	
		LcdPrintNum(AlarmTH);	
		LcdGotoXY(0,9);	 					// ��Һ��������¶ȵ�����ֵ
		LcdPrintNum(AlarmTL);

		LcdGotoXY(1,6);	 					// ��Һ�������ʪ�ȵ�����ֵ
		LcdPrintNum(AlarmHH);	
		LcdGotoXY(1,9);	 	 				// ��Һ�������ʪ�ȵ�����ֵ
		LcdPrintNum(AlarmHL);

		LcdGotoXY(0,7);	 					// ��궨λ
		LcdWriteCmd(0x0F);				// �����˸
		
		DelayMs(10);	  					// ȥ���������µĶ���
		while(!KeySet_P);	 				// �ȴ������ͷ�
		DelayMs(10);					  	// ȥ�������ɿ��Ķ���

		/*�����¶ȵ�����ֵ****************************************************************/

		while(KeySet_P)						// �����ü���û�б����£���һֱ�����¶����޵�����
		{
			if(KeyDown_P==0)				// �ж� ���������� �Ƿ񱻰���		
			{
				if(AlarmTH>AlarmTL+1)					// ֻ�е��¶�����ֵ��������ʱ�����ܼ�1
					AlarmTH--;
				LcdGotoXY(0,6);	 			// ����ˢ����ʾ���ĺ���¶�����ֵ	
				LcdPrintNum(AlarmTH);  		
				LcdGotoXY(0,7);				// ���¶�λ��˸�Ĺ��λ��
				DelayMs(350);					// ��ʱ
			}
			if(KeyUp_P==0)		  		// �ж� ���Ӱ����� �Ƿ񱻰���
			{
				if(AlarmTH<55)	  		// ֻ�е��¶�����ֵС��55ʱ�����ܼ�1
					AlarmTH++;
				LcdGotoXY(0,6);	 	 		// ����ˢ����ʾ���ĺ���¶�����ֵ
				LcdPrintNum(AlarmTH);
				LcdGotoXY(0,7);				// ���¶�λ��˸�Ĺ��λ��
				DelayMs(350);					// ��ʱ
			}	
		}

		LcdGotoXY(0,10);
		DelayMs(10);	  					// ȥ���������µĶ���
		while(!KeySet_P);	 				// �ȴ������ͷ�
		DelayMs(10);					  	// ȥ�������ɿ��Ķ���

		/*�����¶ȵ�����ֵ****************************************************************/
				
		while(KeySet_P)	  				// �����ü���û�б����£���һֱ�����¶����޵�����
		{
			if(KeyDown_P==0)				// �ж� ���������� �Ƿ񱻰���
			{
				if(AlarmTL>0)  				// ֻ�е��¶�����ֵ����0ʱ�����ܼ�1			
					AlarmTL--;
				LcdGotoXY(0,9);	 	  	// ����ˢ����ʾ���ĺ���¶�����ֵ
				LcdPrintNum(AlarmTL);
				LcdGotoXY(0,10);			// ���¶�λ��˸�Ĺ��λ��
				DelayMs(350);					// ��ʱ
			}
			if(KeyUp_P==0)			   	// �ж� ���Ӱ����� �Ƿ񱻰���
			{
				if(AlarmTL<AlarmTH-1)	 			// ֻ�е��¶�����ֵС������ʱ�����ܼ�1
					AlarmTL++;
				LcdGotoXY(0,9);				// ����ˢ����ʾ���ĺ���¶�����ֵ 	
				LcdPrintNum(AlarmTL);
				LcdGotoXY(0,10);			// ���¶�λ��˸�Ĺ��λ��
				DelayMs(350);					// ��ʱ
			}								 
		}

		LcdGotoXY(1,7);
		DelayMs(10);	  					// ȥ���������µĶ���
		while(!KeySet_P);	 				// �ȴ������ͷ�
		DelayMs(10);					  	// ȥ�������ɿ��Ķ���
		
		/*����ʪ�ȵ�����ֵ****************************************************************/

		while(KeySet_P)				 		// �����ü���û�б����£���һֱ����ʪ�����޵�����
		{
			if(KeyDown_P==0)				// �ж� ���������� �Ƿ񱻰���
			{
				if(AlarmHH>AlarmHL+1)	 				// ֻ�е�ʪ������ֵ��������ʱ�����ܼ�1
					AlarmHH--;
				LcdGotoXY(1,6);				// ����ˢ����ʾ���ĺ��ʪ������ֵ 	
				LcdPrintNum(AlarmHH);
				LcdGotoXY(1,7);				// ���¶�λ��˸�Ĺ��λ��
				DelayMs(350);
			}
			if(KeyUp_P==0)			 		// �ж� ���Ӱ����� �Ƿ񱻰���
			{
				if(AlarmHH<95)	  		// ֻ�е�ʪ������ֵС��95ʱ�����ܼ�1
					AlarmHH++;
				LcdGotoXY(1,6);	 		 	// ����ˢ����ʾ���ĺ��ʪ������ֵ
				LcdPrintNum(AlarmHH);
				LcdGotoXY(1,7);	  		// ���¶�λ��˸�Ĺ��λ��
				DelayMs(350);					// ��ʱ
			}	
		}

		LcdGotoXY(1,10);
		DelayMs(10);	  					// ȥ���������µĶ���
		while(!KeySet_P);	 				// �ȴ������ͷ�
		DelayMs(10);					  	// ȥ�������ɿ��Ķ���
		
		/*����ʪ�ȵ�����ֵ****************************************************************/

		while(KeySet_P)				 		// �����ü���û�б����£���һֱ����ʪ�����޵�����
		{
			if(KeyDown_P==0)		 		// �ж� ���������� �Ƿ񱻰���
			{
				if(AlarmHL>0)			  	// ֻ�е�ʪ������ֵ����0ʱ�����ܼ�1
					AlarmHL--;
				LcdGotoXY(1,9);	 		 	// ����ˢ����ʾ���ĺ��ʪ������ֵ
				LcdPrintNum(AlarmHL);
				LcdGotoXY(1,10);			// ���¶�λ��˸�Ĺ��λ��
				DelayMs(350);
			}
			if(KeyUp_P==0)				 	// �ж� ���Ӱ����� �Ƿ񱻰���
			{
				if(AlarmHL<AlarmHH-1)				// ֻ�е�ʪ������ֵС������ʱ�����ܼ�1
					AlarmHL++;
				LcdGotoXY(1,9);	 			// ����ˢ����ʾ���ĺ��ʪ������ֵ	
				LcdPrintNum(AlarmHL);
				LcdGotoXY(1,10);	 		// ���¶�λ��˸�Ĺ��λ��
				DelayMs(350);					// ��ʱ
			}	
		}

		LcdWriteCmd(0x0C);	  		// ȡ�������˸
		LcdShowInit();						// Һ����ʾΪ�������

		DelayMs(10);	  					// ȥ���������µĶ���
		while(!KeySet_P);	 				// �ȴ������ͷ�
		DelayMs(10);					  	// ȥ�������ɿ��Ķ���

		Sector_Erase(0x2000);			 			// �洢֮ǰ�����Ȳ���
		EEPROM_Write(0x2000,AlarmTH);		// ���¶����޴��뵽EEPROM��0x2000�����ַ
		EEPROM_Write(0x2001,AlarmTL);		// ���¶����޴��뵽EEPROM��0x2001�����ַ
		EEPROM_Write(0x2002,AlarmHH);		// ��ʪ�����޴��뵽EEPROM��0x2002�����ַ
		EEPROM_Write(0x2003,AlarmHL);		// ��ʪ�����޴��뵽EEPROM��0x2003�����ַ
	}	
}


/*********************************************************/
// ������
/*********************************************************/
void main()
{
	uchar i;
    U8  temp_i;

	LcdInit();							// Һ�����ܵĳ�ʼ��			
	LcdShowInit(); 					// Һ����ʾ�ĳ�ʼ��

	AlarmTH=EEPROM_Read(0x2000);	// ��EEPROM��0x2000�����ַ��ȡ�¶ȵı�������
	AlarmTL=EEPROM_Read(0x2001);	// ��EEPROM��0x2001�����ַ��ȡ�¶ȵı�������
	AlarmHH=EEPROM_Read(0x2002);	// ��EEPROM��0x2002�����ַ��ȡʪ�ȵı�������	
	AlarmHL=EEPROM_Read(0x2003);	// ��EEPROM��0x2003�����ַ��ȡʪ�ȵı�������

	if((AlarmTL==0)||(AlarmTL>AlarmTH+1))	// ����¶����ޱ���ֵ�������쳣������0�����100���������¸�ֵ
		AlarmTL=20;
	if((AlarmTH==0)||(AlarmTH>55))	// ����¶����ޱ���ֵ�������쳣������0�����100���������¸�ֵ
		AlarmTH=35;
	if((AlarmHL==0)||(AlarmHL>AlarmHH+1))	// ����¶����ޱ���ֵ�������쳣������0�����100���������¸�ֵ
		AlarmHL=40;
	if((AlarmHH==0)||(AlarmHH>95))	// ����¶����ޱ���ֵ�������쳣������0�����100���������¸�ֵ
		AlarmHH=85;
		
	DelayMs(1200);					// �ϵ���һ���ٿ�ʼ��ȡ
/*******************************������**********************************/

    TMOD = 0x20;          //��ʱ��T1ʹ�ù�����ʽ2
    TH1 = 253;        // ���ó�ֵ
    TL1 = 253;
    TR1 = 1;          // ��ʼ��ʱ
    SCON = 0x50;          //������ʽ1��������9600bps���������
    ES = 1;
    EA = 1;           // �������ж�
    TI = 0;
    RI = 0;
    Ble_Delay(1);         //��ʱ100US��12M����)
/*******************************������**********************************/
	
	while(1)
	{
		ReadDhtData(); 				// �����ʪ������

		LcdGotoXY(1,2);	 			// ��λ��Ҫ��ʾ�¶ȵĵط�
		LcdPrintNum(temp);		// ��ʾ�¶�ֵ
		LcdGotoXY(1,11);			// ��λ��Ҫ��ʾʪ�ȵĵط�
		LcdPrintNum(humi);		// ��ʾʪ��ֵ
		
		AlarmJudge();					// �жϲ�������Ҫ����

		for(i=0;i<110;i++)
		{
			KeyScanf();					// ����ɨ��
			DelayMs(20);				// ��ʱ	
		}

/*******************************������**********************************/
        //������ʪ�ȶ�ȡ�ӳ���
        readData();
        str[0]=humidity_H;
        str[1]=humidity_L;
        str[2]=temperature_H;
        str[3]=temperature_L;
        str[4]=checkdata;
        //���͵�����
        for(temp_i=0; temp_i<5; temp_i++)
        {
            sendOneChar(str[temp_i]);
        }
        //��ȡģ���������ڲ���С�� 2S
        Ble_delayms(2000);
/*******************************������**********************************/
	}
}
