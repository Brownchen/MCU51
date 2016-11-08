#include<reg52.h>

#define LCD1602_DB P0
sbit LCD1602_RS = P1^0;
sbit LCD1602_RW = P1^1;
sbit LCD1602_E = P1^5;

bit flag500ms = 0;
unsigned char T0RH = 0;
unsigned char T0RL = 0;
unsigned char code str1[] = "WHUT XX1401 CM";
unsigned char code str2[] = "Let's go...";

void ConfigTimer0(unsigned int ms);
void InitLcd1602();
void LcdShowStr(unsigned char x,unsigned char y,unsigned char *str,unsigned char len);

void main()
{
   unsigned char i;
   unsigned char index = 0;
   unsigned char pdata bufMove1[16+sizeof(str1)+16];
   unsigned char pdata bufMove2[16+sizeof(str2)+16];

   EA = 1;
   ConfigTimer0(10);
   InitLcd1602();

   for(i=0;i<16;i++)
   {
      bufMove1[i] = ' ';
	  bufMove2[i] = ' ';
   }

   for(i=0;i<sizeof(str1)-1;i++)
   {
      bufMove1[16+i] = str1[i];
	  bufMove2[16+i] = str2[i];
   }

   for(i=(16+sizeof(str1)-1);i<sizeof(bufMove1);i++)
   {
      bufMove1[i] = ' ';
	  bufMove2[i] = ' ';
   }
  
   while(1)
   {
      if(flag500ms)
	  {
	     flag500ms = 0;

		 LcdShowStr(0,0,bufMove1+index,16);
		 LcdShowStr(0,1,bufMove2+index,16);

		 index++;
		 if(index>=(16+sizeof(str1)-1))
		 {
		    index = 0;
		 }
	  }
   }
}

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

void LcdWaitReady()
{
   unsigned char sta;

   LCD1602_DB = 0xFF;
   LCD1602_RS = 0;
   LCD1602_RW = 1;
   do
   {
      LCD1602_E = 1;
	  sta = LCD1602_DB;
	  LCD1602_E = 0;
   } while(sta & 0x80);
}

void LcdWriteCmd(unsigned char cmd)
{
   LcdWaitReady();
   LCD1602_RS = 0;
   LCD1602_RW = 0;
   LCD1602_DB = cmd;
   LCD1602_E = 1;
   LCD1602_E = 0;      
}

void LcdSetCursor(unsigned char x,unsigned char y)
{
   unsigned char addr;

   if(y == 0)
   {
      addr = 0x00 + x;
   }
   else
   {
      addr = 0x40 + x;
   }
   LcdWriteCmd(addr | 0x80);
}

void LcdWriteDat(unsigned char dat)
{
   LcdWaitReady();
   LCD1602_RS = 1;
   LCD1602_RW = 0;
   LCD1602_DB = dat;
   LCD1602_E = 1;
   LCD1602_E = 0;
}

void LcdShowStr(unsigned char x,unsigned char y,unsigned char *str,unsigned char len)
{
   LcdSetCursor(x,y);
   while(len--)
   {
       LcdWriteDat(*str++);
   }
}

void InitLcd1602()
{
   LcdWriteCmd(0x38);
   LcdWriteCmd(0x0C);
   LcdWriteCmd(0x06);
   LcdWriteCmd(0x01);
}

void InterruptTimer0() interrupt 1
{
    static unsigned char tmr500ms = 0;

    TH0 = T0RH;  //重新加载重载值
    TL0 = T0RL;
	tmr500ms++;
	if(tmr500ms>=50)
	{
	    tmr500ms = 0;
		flag500ms = 1;
	}
}