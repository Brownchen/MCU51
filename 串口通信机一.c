#include <reg52.h>

sbit ADDR0 = P1^0;
sbit ADDR1 = P1^1;
sbit ADDR2 = P1^2;
sbit ADDR3 = P1^3;
sbit ENLED = P1^4;
sbit KEY1 = P2^4;
sbit KEY2 = P2^5;
sbit KEY3 = P2^6;
sbit KEY4 = P2^7;

unsigned char code LedChar[] = {  
    0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8,
    0x80, 0x90, 0x88, 0x83, 0xC6, 0xA1, 0x86, 0x8E
};

unsigned char LedBuff[6] = {
  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF
};

unsigned char T0RH = 0;  
unsigned char T0RL = 0; 
unsigned char RxdByte = 0; 
bit backup = 1; 
unsigned char cnt = 0;
unsigned char KeySta=1;


void ConfigTimer0(unsigned int ms);
void ConfigUART(unsigned int baud);

void main()
{
    EA = 1;      
    ENLED = 0;   
    ADDR3 = 1;
//	ADDR2=0; 
//	ADDR1=0; 
//	ADDR0=0;
	P2 = 0xF7; 
    ConfigTimer0(1);   //配置T0定时1ms
    ConfigUART(9600);  //配置波特率为9600
    
    while (1)
    {  
//	   P0 = LedChar[(RxdByte & 0x0F)];//将接收字节在数码管上以十六进制形式显示出来
        LedBuff[0] = LedChar[RxdByte & 0x0F];
	    LedBuff[1] = LedChar[RxdByte>>4]; 
	   if (KeySta != backup)  //当前值与前次值不相等说明此时按键有动作
        {
            if (backup == 0)   //如果前次值为0，则说明当前是弹起动作
            {
                cnt++;         //按键次数+1
                if (cnt >= 10)
                {              //只用1个数码管显示，所以加到10就清零重新开始
                    cnt = 0;
                }    
            }
            backup = KeySta;   //更新备份为当前值，以备进行下次比较
        }
	        SBUF = cnt;		//把cnt的值赋给SBUF寄存器，然后发送出去	
    }
}
/* 配置并启动T0，ms-T0定时时间 */
void ConfigTimer0(unsigned int ms)
{
    unsigned long tmp;  
    
    tmp = 11059200 / 12;     
    tmp = (tmp * ms) / 1000;  
    tmp = 65536 - tmp;        
    tmp = tmp + 13;           
    T0RH = (unsigned char)(tmp>>8); 
    T0RL = (unsigned char)tmp;
    TMOD &= 0xF0;   
    TMOD |= 0x01;   
    TH0 = T0RH;    
    TL0 = T0RL;
    ET0 = 1;        
    TR0 = 1;       
}
/* 串口配置函数，baud-通信波特率 */
void ConfigUART(unsigned int baud)
{
    SCON  = 0x50;  //配置串口为模式1
    TMOD &= 0x0F;  //清零T1的控制位
    TMOD |= 0x20;  //配置T1为模式2
    TH1 = 256 - (11059200/12/32)/baud;  //计算T1重载值
    TL1 = TH1;     //初值等于重载值
    ET1 = 0;       //禁止T1中断
    ES  = 1;       //使能串口中断
    TR1 = 1;       //启动T1
}
/*  消抖函数   */
void abcde()
{
   static unsigned char keybuf = 0xFF;  
    
    keybuf = (keybuf<<1) | KEY4;  
    if (keybuf == 0x00)
    {   
        KeySta = 0;
    }
    else if (keybuf == 0xFF)
    {   
        KeySta = 1;
    }
    else
    {}  
}

void LedScan()
{
   static unsigned char i = 0;

   P0 = 0xFF;
   P1 = (P1&0xF8) | i;
   P0 = LedBuff[i];
   if(i<6)
     i++;
   else
   i = 0;
}

/* T0中断服务函数，完成LED扫描 */
void InterruptTimer0() interrupt 1
{
    TH0 = T0RH;  //重新加载重载值
    TL0 = T0RL;
	LedScan();
	abcde();   //引用消抖函数
}
/* UART中断服务函数 */
void InterruptUART() interrupt 4
{
    if (RI)  //接收到字节
    {
        RI = 0;  //软件清零
		RxdByte = SBUF;	  //接收单片机2返回的数据
    }
    if (TI)  //字节发送完毕
    {
        TI = 0;  //软件清零
    }
}
