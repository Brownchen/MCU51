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
    ConfigTimer0(1);   //����T0��ʱ1ms
    ConfigUART(9600);  //���ò�����Ϊ9600
    
    while (1)
    {  
//	   P0 = LedChar[(RxdByte & 0x0F)];//�������ֽ������������ʮ��������ʽ��ʾ����
        LedBuff[0] = LedChar[RxdByte & 0x0F];
	    LedBuff[1] = LedChar[RxdByte>>4]; 
	   if (KeySta != backup)  //��ǰֵ��ǰ��ֵ�����˵����ʱ�����ж���
        {
            if (backup == 0)   //���ǰ��ֵΪ0����˵����ǰ�ǵ�����
            {
                cnt++;         //��������+1
                if (cnt >= 10)
                {              //ֻ��1���������ʾ�����Լӵ�10���������¿�ʼ
                    cnt = 0;
                }    
            }
            backup = KeySta;   //���±���Ϊ��ǰֵ���Ա������´αȽ�
        }
	        SBUF = cnt;		//��cnt��ֵ����SBUF�Ĵ�����Ȼ���ͳ�ȥ	
    }
}
/* ���ò�����T0��ms-T0��ʱʱ�� */
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
/* �������ú�����baud-ͨ�Ų����� */
void ConfigUART(unsigned int baud)
{
    SCON  = 0x50;  //���ô���Ϊģʽ1
    TMOD &= 0x0F;  //����T1�Ŀ���λ
    TMOD |= 0x20;  //����T1Ϊģʽ2
    TH1 = 256 - (11059200/12/32)/baud;  //����T1����ֵ
    TL1 = TH1;     //��ֵ��������ֵ
    ET1 = 0;       //��ֹT1�ж�
    ES  = 1;       //ʹ�ܴ����ж�
    TR1 = 1;       //����T1
}
/*  ��������   */
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

/* T0�жϷ����������LEDɨ�� */
void InterruptTimer0() interrupt 1
{
    TH0 = T0RH;  //���¼�������ֵ
    TL0 = T0RL;
	LedScan();
	abcde();   //������������
}
/* UART�жϷ����� */
void InterruptUART() interrupt 4
{
    if (RI)  //���յ��ֽ�
    {
        RI = 0;  //�������
		RxdByte = SBUF;	  //���յ�Ƭ��2���ص�����
    }
    if (TI)  //�ֽڷ������
    {
        TI = 0;  //�������
    }
}
