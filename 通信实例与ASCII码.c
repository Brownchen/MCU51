#include<reg52.h>

sbit ADDR3 = P1^3;
sbit ENLED = P1^4;

unsigned char code LedChar[] = {
  0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,
  0x80,0x90,0x88,0x83,0xC6,0xA1,0x86,0x8E
} ;

unsigned char LedBuff[7] = {
  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF
};

unsigned char T0RH = 0;
unsigned char T0RL = 0;
unsigned char RxdByte = 0;

void ConfigTimer0(unsigned int ms);
void ConfigUART(unsigned int baud);

void main()
{
   EA = 1;
   ENLED = 0;
   ADDR3 = 1;
   ConfigTimer0(1);
   ConfigUART(9600);

   while(1)
   {
      LedBuff[0] = LedChar[RxdByte & 0x0F];
	  LedBuff[1] = LedChar[RxdByte>>4];
   }
}

void ConfigTimer0(unsigned int ms)
{
   unsigned long tmp;

   tmp = 11059200/12;
   tmp = (tmp*ms)/1000;
   tmp = 65536-tmp;
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

void ConfigUART(unsigned int baud)
{
   SCON = 0x50;
   TMOD &= 0x0F;
   TMOD |= 0x20;
   TH1 = 256-(11059200/12/32)/baud;
   TL1 = TH1;
   ET1 = 0;
   ES = 1;
   TR1 = 1;
}

void LedScan()
{
   static unsigned char i = 0;

   P0 = 0xFF;
   P1 = (P1 &0xF8) | i;
   P0 = LedBuff[i];
   if(i<6)
      i++;
   else
      i=0;
}

void InterruptTimer0() interrupt 1
{
  TH0 = T0RH;
  TL0 = T0RL;
  LedScan();
}

void InterruptUART() interrupt 4
{
  if(RI)
  {
     RI = 0;
	 RxdByte = SBUF;
	 SBUF = RxdByte;
  }
  if(TI)
  {
     TI = 0;
  }
}