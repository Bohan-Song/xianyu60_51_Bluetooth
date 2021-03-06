#include <reg52.h>		   		// 头文件包含
#include <intrins.h>

#define uchar unsigned char	// 以后unsigned char就可以用uchar代替
#define uint  unsigned int	// 以后unsigned int 就可以用uint 代替
/*******************************蓝牙用**********************************/
typedef unsigned char  U8;       /* defined for unsigned 8-bits integer variable           无符号8位整型变量  */
typedef unsigned int   U16;      /* defined for unsigned 16-bits integer variable           无符号16位整型变量 */
#define uchar unsigned char
#define uint unsigned int
//----------------IO口定义区--------------------//
sbit  P2_0  = P2^0 ;

//----------------定义区--------------------//
U8  U8temp;
U8  humidity_H,humidity_L,temperature_H,temperature_L,checkdata;
U8  str[5]= {"RS232"};

/*******************************蓝牙用**********************************/
sfr ISP_DATA  = 0xe2;			// 数据寄存器
sfr ISP_ADDRH = 0xe3;			// 地址寄存器高八位
sfr ISP_ADDRL = 0xe4;			// 地址寄存器低八位
sfr ISP_CMD   = 0xe5;			// 命令寄存器
sfr ISP_TRIG  = 0xe6;			// 命令触发寄存器
sfr ISP_CONTR = 0xe7;			// 命令寄存器

sbit Buzzer_P  = P1^6;    // 蜂鸣器
sbit DHT11_P   = P1^7;	 	// 温湿度传感器DHT11数据接入
sbit LcdRs_P   = P2^5;    // 1602液晶的RS管脚       
sbit LcdRw_P   = P2^6;    // 1602液晶的RW管脚 
sbit LcdEn_P   = P2^7;    // 1602液晶的EN管脚
sbit KeySet_P  = P3^2;		// “设置”按键的管脚
sbit KeyDown_P = P3^4;		// “减”按键的管脚
sbit KeyUp_P   = P3^3;		// “加”按键的管脚 
sbit LedHL_P   = P1^0;		// 湿度过低
sbit LedHH_P   = P1^1;		// 湿度过高
sbit LedTL_P   = P1^2;		// 温度过低
sbit LedTH_P   = P1^3;		// 温度过高

uchar temp;					// 保存温度
uchar humi;					// 保存湿度

uchar AlarmTL;			// 温度下限报警值
uchar AlarmTH;			// 温度上限报警值
uchar AlarmHL;			// 湿度下限报警值
uchar AlarmHH;			// 湿度上限报警值


/*******************************蓝牙用**********************************/


void Ble_Delay(U16 j)
{   U8 i;
    for(; j>0; j--)
    {
        for(i=0; i<27; i++);
    }
}

/**
*串口发送一个字节
*/
void sendOneChar(uchar a)
{
    SBUF = a;
    while(TI==0);
    TI=0;
}


/**
*延时函数：单位 ms
*/
void Ble_delayms(uint x)
{
    uint i,j;
    for(i=x; i>0; i--)
        for(j=110; j>0; j--);
}
/**
*延时函数： 延时10us
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
*延时函数： 延时10us
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
*根据时序计算温湿度值
*/
U8 computeData()
{
    U8 i,U8comdata;
    for(i=0; i<8; i++)
    {
        while(P2_0==0);				//等待54us低电平
        Ble_Delay_10us();					//延时30us确认高电平是否结束
        Ble_Delay_10us();
        Ble_Delay_10us();
        U8temp=0;
        if(P2_0==1)						//如果高电平高过预定0高电平
        {											//值则数据位为 1
            U8temp=1;
        }
        while(P2_0==1);			//等待高电平结束
        U8comdata<<=1;
        U8comdata|=U8temp;
    }
    return U8comdata;
}

//--------------------------------
//-----湿度读取子程序 ------------
//--------------------------------
//----以下变量均为全局变量--------
//----温度高8位== temperature_H------
//----温度低8位== temperature_L------
//----湿度高8位== humidity_H-----
//----湿度低8位== humidity_L-----
//----校验 8位 == checkdata-----
//--------------------------------
void readData()
{
    U8  humidity_H_temp,humidity_L_temp,temperature_H_temp,temperature_L_temp,checkdata_temp;
    //主机拉低不少于18ms
    P2_0=0;
    Ble_delayms(20);
		//总线由上拉电阻拉高 主机延时13us最多20us 等待DHT11低电平响应
    P2_0=1;
		Ble_delay13us();
    //判断从机是否有低电平响应信号
    if(P2_0==0)
    {
        while(P2_0==0);				//判断DHT11发出 83us 的低电平响应信号是否结束
        while(P2_0==1);				//判断DHT11发出 87us 的高电平是否结束
        //数据接收状态
        humidity_H_temp = computeData();
        humidity_L_temp = computeData();
        temperature_H_temp = computeData();
        temperature_L_temp = computeData();
        checkdata_temp = computeData();
        P2_0 = 1;		//data拉高释放总线
        //数据校验
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

/*******************************蓝牙用**********************************/

/*********************************************************/
// 单片机内部EEPROM不使能
/*********************************************************/
void ISP_Disable()
{
	ISP_CONTR = 0;
	ISP_ADDRH = 0;
	ISP_ADDRL = 0;
}


/*********************************************************/
// 从单片机内部EEPROM读一个字节，从0x2000地址开始
/*********************************************************/
unsigned char EEPROM_Read(unsigned int add)
{
	ISP_DATA  = 0x00;
	ISP_CONTR = 0x83;
	ISP_CMD   = 0x01;
	ISP_ADDRH = (unsigned char)(add>>8);
	ISP_ADDRL = (unsigned char)(add&0xff);
	// 对STC89C51系列来说，每次要写入0x46，再写入0xB9,ISP/IAP才会生效
	ISP_TRIG  = 0x46;	   
	ISP_TRIG  = 0xB9;
	_nop_();
	ISP_Disable();
	return (ISP_DATA);
}


/*********************************************************/
// 往单片机内部EEPROM写一个字节，从0x2000地址开始
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
// 擦除单片机内部EEPROM的一个扇区
// 写8个扇区中随便一个的地址，便擦除该扇区，写入前要先擦除
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
// 毫秒级的延时函数，time是要延时的毫秒数
/*********************************************************/
void DelayMs(uint time)
{
	uint i,j;
	for(i=0;i<time;i++)
		for(j=0;j<112;j++);
}


/*********************************************************/
// 1602液晶写命令函数，cmd就是要写入的命令
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
// 1602液晶写数据函数，dat就是要写入的数据
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
// 1602液晶初始化函数
/*********************************************************/
void LcdInit()
{
	LcdWriteCmd(0x38);        // 16*2显示，5*7点阵，8位数据口
	LcdWriteCmd(0x0C);        // 开显示，不显示光标
	LcdWriteCmd(0x06);        // 地址加1，当写入数据后光标右移
	LcdWriteCmd(0x01);        // 清屏
}


/*********************************************************/
// 液晶光标定位函数
/*********************************************************/
void LcdGotoXY(uchar line,uchar column)
{
	// 第一行
	if(line==0)        
		LcdWriteCmd(0x80+column); 
	// 第二行
	if(line==1)        
		LcdWriteCmd(0x80+0x40+column); 
}


/*********************************************************/
// 液晶输出字符串函数
/*********************************************************/
void LcdPrintStr(uchar *str)
{
	while(*str!='\0') 			// 判断是否到字符串的尽头了
		LcdWriteData(*str++);
}


/*********************************************************/
// 液晶输出数字
/*********************************************************/
void LcdPrintNum(uchar num)
{
	LcdWriteData(num/10+48);	// 十位
	LcdWriteData(num%10+48); 	// 个位
}


/*********************************************************/
// 液晶显示内容的初始化
/*********************************************************/
void LcdShowInit()
{
	LcdGotoXY(0,0);								// 第0行的显示内容
	LcdPrintStr("  DHT11 System  ");
	LcdGotoXY(1,0);						   	// 第1行的显示内容
	LcdPrintStr("T:   C   H:  %RH");
	LcdGotoXY(1,4);								// 温度单位摄氏度上面的圆圈符号
	LcdWriteData(0xdf);	
}



/*********************************************************/
// 10us级延时程序
/*********************************************************/
void Delay10us()
{
	_nop_();	// 执行一条指令，延时1微秒
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
// 读取DHT11单总线上的一个字节
/*********************************************************/
uchar DhtReadByte(void)
{   
	bit bit_i; 
	uchar j;
	uchar dat=0;

	for(j=0;j<8;j++)    
	{
		while(!DHT11_P);	// 等待低电平结束	
		Delay10us();			// 延时
		Delay10us();
		Delay10us();
		if(DHT11_P==1)		// 判断数据线是高电平还是低电平
		{
			bit_i=1; 
			while(DHT11_P);
		} 
		else
		{
			bit_i=0;
		}
		dat<<=1;		   		// 将该位移位保存到dat变量中
		dat|=bit_i;    
	}
	return(dat);  
}


/*********************************************************/
// 读取DHT11的一帧数据，湿高、湿低(0)、温高、温低(0)、校验码
/*********************************************************/
void ReadDhtData()
{    	 
	uchar HumiHig;		// 湿度高检测值
	uchar HumiLow;		// 湿度低检测值 
	uchar TemHig;			// 温度高检测值
	uchar TemLow;			// 温度低检测值
	uchar check;			// 校验字节 
	
	DHT11_P=0;				// 主机拉低
	DelayMs(20);			// 保持20毫秒
	DHT11_P=1;				// DATA总线由上拉电阻拉高

	Delay10us();	 		// 延时等待30us
	Delay10us();
	Delay10us();

	while(!DHT11_P);	// 等待DHT的低电平结束
	while(DHT11_P);		// 等待DHT的高电平结束

	//进入数据接收状态
	HumiHig = DhtReadByte(); 	// 湿度高8位
	HumiLow = DhtReadByte(); 	// 湿度低8为，总为0
	TemHig  = DhtReadByte(); 	// 温度高8位 
	TemLow  = DhtReadByte(); 	// 温度低8为，总为0 
	check   = DhtReadByte();	// 8位校验码，其值等于读出的四个字节相加之和的低8位

	while(!DHT11_P);
	DHT11_P=1;								// 拉高总线

	if(check==HumiHig + HumiLow + TemHig + TemLow) 		// 如果收到的数据无误
	{
		temp=TemHig; 			// 将温度的检测结果赋值给全局变量temp
		humi=HumiHig;			// 将湿度的检测结果赋值给全局变量humi
	}
}


/*********************************************************/
// 是否需要报警判断
/*********************************************************/
void AlarmJudge(void)
{
	// uchar i;

	if(temp>AlarmTH)				// 温度是否过高
	{
		LedTH_P=0;
		LedTL_P=1;
	}
	else if(temp<AlarmTL)		// 温度是否过低
	{
		LedTL_P=0;
		LedTH_P=1;
	}
	else										// 温度正常
	{
		LedTH_P=1;
		LedTL_P=1;
	}

	if(humi>AlarmHH)	   		// 湿度是否过高
	{
		LedHH_P=0;
	  LedHL_P=1;
	}
	else if(humi<AlarmHL)		// 湿度是否过低
	{
		LedHL_P=0;
		LedHH_P=1;
	}
	else				   					// 湿度正常
	{
		LedHH_P=1;
		LedHL_P=1;
	}
}


/*********************************************************/
// 按键扫描，用于设置温湿度报警范围
/*********************************************************/
void KeyScanf()
{
	if(KeySet_P==0)		// 判断设置按键是否被按下
	{
		/*将液晶显示改为设置页面的*******************************************************/

		LcdWriteCmd(0x01);				// 设置界面的显示框架
		LcdGotoXY(0,0);
		LcdPrintStr("Temp:   -       ");
		LcdGotoXY(1,0);
		LcdPrintStr("Humi:   -       ");
		
		LcdGotoXY(0,6);	 					// 在液晶上填充温度的上限值	
		LcdPrintNum(AlarmTH);	
		LcdGotoXY(0,9);	 					// 在液晶上填充温度的下限值
		LcdPrintNum(AlarmTL);

		LcdGotoXY(1,6);	 					// 在液晶上填充湿度的上限值
		LcdPrintNum(AlarmHH);	
		LcdGotoXY(1,9);	 	 				// 在液晶上填充湿度的下限值
		LcdPrintNum(AlarmHL);

		LcdGotoXY(0,7);	 					// 光标定位
		LcdWriteCmd(0x0F);				// 光标闪烁
		
		DelayMs(10);	  					// 去除按键按下的抖动
		while(!KeySet_P);	 				// 等待按键释放
		DelayMs(10);					  	// 去除按键松开的抖动

		/*设置温度的下限值****************************************************************/

		while(KeySet_P)						// “设置键”没有被按下，则一直处于温度上限的设置
		{
			if(KeyDown_P==0)				// 判断 “减按键“ 是否被按下		
			{
				if(AlarmTH>AlarmTL+1)					// 只有当温度上限值大于下限时，才能减1
					AlarmTH--;
				LcdGotoXY(0,6);	 			// 重新刷新显示更改后的温度上限值	
				LcdPrintNum(AlarmTH);  		
				LcdGotoXY(0,7);				// 重新定位闪烁的光标位置
				DelayMs(350);					// 延时
			}
			if(KeyUp_P==0)		  		// 判断 “加按键“ 是否被按下
			{
				if(AlarmTH<55)	  		// 只有当温度上限值小于55时，才能加1
					AlarmTH++;
				LcdGotoXY(0,6);	 	 		// 重新刷新显示更改后的温度上限值
				LcdPrintNum(AlarmTH);
				LcdGotoXY(0,7);				// 重新定位闪烁的光标位置
				DelayMs(350);					// 延时
			}	
		}

		LcdGotoXY(0,10);
		DelayMs(10);	  					// 去除按键按下的抖动
		while(!KeySet_P);	 				// 等待按键释放
		DelayMs(10);					  	// 去除按键松开的抖动

		/*设置温度的上限值****************************************************************/
				
		while(KeySet_P)	  				// “设置键”没有被按下，则一直处于温度下限的设置
		{
			if(KeyDown_P==0)				// 判断 “减按键“ 是否被按下
			{
				if(AlarmTL>0)  				// 只有当温度下限值大于0时，才能减1			
					AlarmTL--;
				LcdGotoXY(0,9);	 	  	// 重新刷新显示更改后的温度下限值
				LcdPrintNum(AlarmTL);
				LcdGotoXY(0,10);			// 重新定位闪烁的光标位置
				DelayMs(350);					// 延时
			}
			if(KeyUp_P==0)			   	// 判断 “加按键“ 是否被按下
			{
				if(AlarmTL<AlarmTH-1)	 			// 只有当温度下限值小于上限时，才能加1
					AlarmTL++;
				LcdGotoXY(0,9);				// 重新刷新显示更改后的温度下限值 	
				LcdPrintNum(AlarmTL);
				LcdGotoXY(0,10);			// 重新定位闪烁的光标位置
				DelayMs(350);					// 延时
			}								 
		}

		LcdGotoXY(1,7);
		DelayMs(10);	  					// 去除按键按下的抖动
		while(!KeySet_P);	 				// 等待按键释放
		DelayMs(10);					  	// 去除按键松开的抖动
		
		/*设置湿度的下限值****************************************************************/

		while(KeySet_P)				 		// “设置键”没有被按下，则一直处于湿度上限的设置
		{
			if(KeyDown_P==0)				// 判断 “减按键“ 是否被按下
			{
				if(AlarmHH>AlarmHL+1)	 				// 只有当湿度上限值大于下限时，才能减1
					AlarmHH--;
				LcdGotoXY(1,6);				// 重新刷新显示更改后的湿度上限值 	
				LcdPrintNum(AlarmHH);
				LcdGotoXY(1,7);				// 重新定位闪烁的光标位置
				DelayMs(350);
			}
			if(KeyUp_P==0)			 		// 判断 “加按键“ 是否被按下
			{
				if(AlarmHH<95)	  		// 只有当湿度上限值小于95时，才能加1
					AlarmHH++;
				LcdGotoXY(1,6);	 		 	// 重新刷新显示更改后的湿度下限值
				LcdPrintNum(AlarmHH);
				LcdGotoXY(1,7);	  		// 重新定位闪烁的光标位置
				DelayMs(350);					// 延时
			}	
		}

		LcdGotoXY(1,10);
		DelayMs(10);	  					// 去除按键按下的抖动
		while(!KeySet_P);	 				// 等待按键释放
		DelayMs(10);					  	// 去除按键松开的抖动
		
		/*设置湿度的上限值****************************************************************/

		while(KeySet_P)				 		// “设置键”没有被按下，则一直处于湿度下限的设置
		{
			if(KeyDown_P==0)		 		// 判断 “减按键“ 是否被按下
			{
				if(AlarmHL>0)			  	// 只有当湿度下限值大于0时，才能减1
					AlarmHL--;
				LcdGotoXY(1,9);	 		 	// 重新刷新显示更改后的湿度下限值
				LcdPrintNum(AlarmHL);
				LcdGotoXY(1,10);			// 重新定位闪烁的光标位置
				DelayMs(350);
			}
			if(KeyUp_P==0)				 	// 判断 “加按键“ 是否被按下
			{
				if(AlarmHL<AlarmHH-1)				// 只有当湿度下限值小于上限时，才能加1
					AlarmHL++;
				LcdGotoXY(1,9);	 			// 重新刷新显示更改后的湿度下限值	
				LcdPrintNum(AlarmHL);
				LcdGotoXY(1,10);	 		// 重新定位闪烁的光标位置
				DelayMs(350);					// 延时
			}	
		}

		LcdWriteCmd(0x0C);	  		// 取消光标闪烁
		LcdShowInit();						// 液晶显示为检测界面的

		DelayMs(10);	  					// 去除按键按下的抖动
		while(!KeySet_P);	 				// 等待按键释放
		DelayMs(10);					  	// 去除按键松开的抖动

		Sector_Erase(0x2000);			 			// 存储之前必须先擦除
		EEPROM_Write(0x2000,AlarmTH);		// 把温度上限存入到EEPROM的0x2000这个地址
		EEPROM_Write(0x2001,AlarmTL);		// 把温度下限存入到EEPROM的0x2001这个地址
		EEPROM_Write(0x2002,AlarmHH);		// 把湿度上限存入到EEPROM的0x2002这个地址
		EEPROM_Write(0x2003,AlarmHL);		// 把湿度下限存入到EEPROM的0x2003这个地址
	}	
}


/*********************************************************/
// 主函数
/*********************************************************/
void main()
{
	uchar i;
    U8  temp_i;

	LcdInit();							// 液晶功能的初始化			
	LcdShowInit(); 					// 液晶显示的初始化

	AlarmTH=EEPROM_Read(0x2000);	// 从EEPROM的0x2000这个地址读取温度的报警上限
	AlarmTL=EEPROM_Read(0x2001);	// 从EEPROM的0x2001这个地址读取温度的报警下限
	AlarmHH=EEPROM_Read(0x2002);	// 从EEPROM的0x2002这个地址读取湿度的报警上限	
	AlarmHL=EEPROM_Read(0x2003);	// 从EEPROM的0x2003这个地址读取湿度的报警下限

	if((AlarmTL==0)||(AlarmTL>AlarmTH+1))	// 如果温度下限报警值读出来异常（等于0或大于100），则重新赋值
		AlarmTL=20;
	if((AlarmTH==0)||(AlarmTH>55))	// 如果温度上限报警值读出来异常（等于0或大于100），则重新赋值
		AlarmTH=35;
	if((AlarmHL==0)||(AlarmHL>AlarmHH+1))	// 如果温度下限报警值读出来异常（等于0或大于100），则重新赋值
		AlarmHL=40;
	if((AlarmHH==0)||(AlarmHH>95))	// 如果温度上限报警值读出来异常（等于0或大于100），则重新赋值
		AlarmHH=85;
		
	DelayMs(1200);					// 上电后等一下再开始读取
/*******************************蓝牙用**********************************/

    TMOD = 0x20;          //定时器T1使用工作方式2
    TH1 = 253;        // 设置初值
    TL1 = 253;
    TR1 = 1;          // 开始计时
    SCON = 0x50;          //工作方式1，波特率9600bps，允许接收
    ES = 1;
    EA = 1;           // 打开所以中断
    TI = 0;
    RI = 0;
    Ble_Delay(1);         //延时100US（12M晶振)
/*******************************蓝牙用**********************************/
	
	while(1)
	{
		ReadDhtData(); 				// 检测温湿度数据

		LcdGotoXY(1,2);	 			// 定位到要显示温度的地方
		LcdPrintNum(temp);		// 显示温度值
		LcdGotoXY(1,11);			// 定位到要显示湿度的地方
		LcdPrintNum(humi);		// 显示湿度值
		
		AlarmJudge();					// 判断并根据需要报警

		for(i=0;i<110;i++)
		{
			KeyScanf();					// 按键扫描
			DelayMs(20);				// 延时	
		}

/*******************************蓝牙用**********************************/
        //调用温湿度读取子程序
        readData();
        str[0]=humidity_H;
        str[1]=humidity_L;
        str[2]=temperature_H;
        str[3]=temperature_L;
        str[4]=checkdata;
        //发送到串口
        for(temp_i=0; temp_i<5; temp_i++)
        {
            sendOneChar(str[temp_i]);
        }
        //读取模块数据周期不易小于 2S
        Ble_delayms(2000);
/*******************************蓝牙用**********************************/
	}
}
