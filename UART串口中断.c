#include<reg52.h>

void ConfigUART(unsigned int baud);
   
void main()
   {
      EA = 1;
	  ConfigUART(9600);
	  while(1);
   }

void CONfigUART(unsigned int baud)
{
   SCON = 0x50;
   TMOD &= 0x0F;
   TH1 = 256-(11059200/12/32)/baud;
   TL1 = TH1;
   ET1 = 0;
   ES = 1;
   TR1 = 1;
}

void InterruptUART() interrupt 4
{
   if(RI)
   {
     RI = 0;
	 SBUF = SBUF +1;
   }
   if(TI)
   {
     TI = 0;
   }
}