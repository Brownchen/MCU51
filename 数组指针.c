#include<reg52.h>

bit cmdArrived = 0;
unsigned char cmdIndex = 0;
unsigned char cntTxd = 0;
unsigned char * ptrTxd ;

unsigned char array1[1] = {1};
unsigned char array2[2] = {1,2};
//unsigned char array3[3] = {1,2,3};
unsigned char array3[4] = {1,2,3,4};
unsigned char array4[8]	= {1,2,3,4,5,6,7,8};

void ConfigUART(unsigned int baud);

void main()
{
   EA = 1;
   ConfigUART(9600);

   while(1)
   {
      if(cmdArrived)
	  {
	     cmdArrived = 0;
	     switch(cmdIndex)
		 {
		    case 1:
			  ptrTxd = array1;
			  cntTxd = sizeof(array1);
			  TI = 1;
		   	  break;
			case 2:
			  ptrTxd = array2;
			  cntTxd = sizeof(array2);
			  TI = 1;
			  break;
			case 3:
			  ptrTxd = array3;
			  cntTxd = sizeof(array3);
			  TI = 1;
			  break;
			case 4:
		  	  ptrTxd = array4;
			  cntTxd = sizeof(array4);
			  TI = 1;
			  break;
			default:
			  break;

		 }
	  }
   }
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

void InterruptUART() interrupt 4
{
   if(RI)
   {
      RI = 0;
	  cmdIndex = SBUF;
	  cmdArrived = 1;
   }
   if(TI)
   {
      TI = 0;
	  if(cntTxd>0)
	  {
	     SBUF = *ptrTxd;
		 cntTxd--;
		 ptrTxd++;
	  }
   }
}