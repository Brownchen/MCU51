#include<reg52.h>

sbit ADDR0 = P1^0;
sbit ADDR1 = P1^1;
sbit ADDR2 = P1^2;
sbit ADDR3 = P1^3;
sbit ENLED = P1^4;

void ShiftLeft(unsigned char *p);

void main()
{
   unsigned int i;
   unsigned char buf = 0x01;


   ENLED = 0;
   ADDR3 = 1;
   ADDR2 = 1;
   ADDR1 = 1;
   ADDR0 = 0;

   while(1)
   {
      P0 = ~buf;
	  for(i=0;i<30000;i++);
	  ShiftLeft(&buf);
	  if(buf == 0)
	  {
	    buf = 0x01;
	  }
   }
}

void ShiftLeft(unsigned char *p)
{
    *p = *p<<1;
}
