#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <inttypes.h>
//#include "inc/tm4c1294ncpdt.h"
#include "inc/hw_i2c.h"
#include "inc/hw_memmap.h"
#include "inc/hw_sysctl.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/i2c.h"
#include "driverlib/sysctl.h"
#include "inc/hw_gpio.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "utils/uartstdio.h"
#include "driverlib/uart.h"
#include "driverlib/pwm.h"

#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/pwm.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"

#include "myFiles/myHfile.h"
#include "myFiles/TRIAD.h"
#include "myFiles/QUEST.h"
#include "myFiles/Bdot.h"

#define BUF_SIZE 100

volatile int calcReq =0;

triadClass *triadClassPtr=NULL;
questClass *questClassPtr =NULL;
bdotClass * bdotPtr = NULL;

/*UART*/
void initUART();
int checkReceivedQt();
void analyzeString(char *);
void setUARTinterrupt();

/*graph function*/
void sendRandomData();
void sendFakeRed();
void sendFakeGreen();
void sendFakeBlue();
void sendFakeClear();


/*public vars interrupt*/
volatile int gotDataQt;
volatile int letter=0;



volatile int actualTab[6]={0};

volatile int algTRIAD = 0;
volatile int algQUEST = 0;
volatile int algQmethod = 0;

volatile float Si[3]={0,0,0};
volatile float So[3]={0,0,0};
volatile float Mi[3]={0,0,0};
volatile float Mo[3]={0,0,0};

volatile int windings = 0;
volatile float coilArea =0.0;
volatile float angle=0.0;


int main(void)
{

/*
	float Si[3]={0.2673, 0.5345, 0.8018};
	float So[3]={-0.3124, 0.9370, 0.1562};
	float Mi[3]={0.7749, 0.3448, 0.5297};
	float Mo[3]={0.6296, 0.6944, -0.3486};
*/

	float RM[9]={0};

    SYSCTL_RCGCGPIO_R = SYSCTL_RCGCGPIO_R12;
    GPIO_PORTN_DIR_R = 0xFF;
    GPIO_PORTN_DEN_R = 0xFF;


	initUART();
    setUARTinterrupt();

    triadClassPtr = initializeTRIADclass();
    questClassPtr = initializeQUESTclass();
    bdotPtr = initializeBdotClass();



  //  bdotPtr->setupPwm();

    calcReq = 0;
	while(1)
	{

//	testFunc();
//	includeQUEST();
//	includeTRIAD();
//	includeBdot();

	if(actualTab[0]==1)
	{
        if(calcReq == 1)
        {

            if(Mo[2]!=0) //indien alle inputs werden ingevuld
            {
            	if(algTRIAD==1)
            	{

            		triadClassPtr->TRIAD( Si, Mi, So, Mo, RM );
            		algTRIAD = 0;
            	}
            	else if(algQUEST==1)
            	{
            		questClassPtr->QUEST(Si,So,Mi,Mo,RM);
            		algQUEST = 0;
            	}
            }
            Mo[2]=0;
            calcReq = 0;
        }
	}
    else if (actualTab[0]==0)
    {
       	if(actualTab[1]==1)
       	{
       		sendRandomData();
       	}
   }

	if (actualTab[5]==1)
	{
		if(calcReq == 1)
		{
			bdotPtr->bdot();
			calcReq = 0;
		}
	}

}

}



/*************************************************************************************************************************************/
/*UART*/

void initUART()
{
		uint32_t g_ui32SysClock = SysCtlClockFreqSet((SYSCTL_OSC_MAIN | SYSCTL_XTAL_25MHZ | SYSCTL_USE_PLL | SYSCTL_CFG_VCO_320), 40000000);

	    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

	    // Configure GPIO Pins for UART mode.
	    GPIOPinConfigure(0x00000001); //rx
	    GPIOPinConfigure(0x00000401); //tx
	    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

	    UARTStdioConfig(0, 115200, g_ui32SysClock);
}




void analyzeString(char * receivedString)
{
	int i=0;
	int j=0;
	int n=0;
	int k=0;

	char number[3]={0};
	int printfloatval;

	UARTprintf("analyzing string\n");
	UARTprintf("mc received: \n");
	for(n=0;n<20;n++)
	{
		UARTprintf("%c \n", receivedString[n]);
	}


	for(i=0;i<BUF_SIZE;i++)
	{

		if(receivedString[i]=='T')
		{
			algTRIAD = 1;
			algQUEST = 0;
			UARTprintf("T ontvangen\n");
			for (i=0;i<BUF_SIZE;i++)
			{receivedString[i] = '\0';}
        	actualTab[0] = 1;
        	actualTab[1] = 0;
        	actualTab[2] = 0;
        	actualTab[3] = 0;

		}
		else if(receivedString[i]=='Q')
		{
			UARTprintf("Q ontvangen\n");
			algQUEST = 1;
			algTRIAD = 0;
			for (i=0;i<BUF_SIZE;i++)
			{receivedString[i] = '\0';}
        	actualTab[0] = 1;
        	actualTab[1] = 0;
        	actualTab[2] = 0;
        	actualTab[3] = 0;

		}

		else if(receivedString[i] == 'S') //Six:
		{
			calcReq = 1;
			if(receivedString[i+1] == 'i')
			{
				UARTprintf("analyzing Si\n");
				j=(i+4); //'ix:' overslaan
				while(receivedString[j] !='\n')
				{
					number[k] = receivedString[j];
					UARTprintf("number transmtted char Si: %c\n", number[k]);
					while(!UARTBusy(UART0_BASE));
					while(UARTBusy(UART0_BASE));
					j++;
					if(receivedString[j]!='\n'){
					k++;}
				}
				if(receivedString[i+2]=='1'){
					Si[0] = atof(number);} //t ganse getal wordt gevormd door de verschillende chars die hier verzameld zijn
				else if(receivedString[i+2]=='2'){
					Si[1] = atof(number);}
				else if(receivedString[i+2]=='3'){
					Si[2] = atof(number);}

				for(j=0;j<3;j++){
					printfloatval = Si[j]*100000;
					UARTprintf("int Si - het nummer%d: %d\n", j,printfloatval);
					while(!UARTBusy(UART0_BASE));
					while(UARTBusy(UART0_BASE));
					}
				for(j=0;j<10000;j++)
				{k++;}
				UARTprintf("ack\n");
				while(!UARTBusy(UART0_BASE));
				while(UARTBusy(UART0_BASE));
			}
			else if(receivedString[i+1] == 'o')
			{
				k=0;
				//UARTprintf("analyzing So\n");
				j=(i+4);
				while(receivedString[j] !='\n')
				{
					number[k] = receivedString[j];
					UARTprintf("number transmission So: %c\n", number[k]);
					j++;
					if(receivedString[j]!='\n'){
					k++;}
				}
				//i = i+2;
				if(receivedString[i+2]=='1'){
				So[0] = atof(number);}
				else if(receivedString[i+2]=='2'){
				So[1] = atof(number);}
				else if(receivedString[i+2]=='3'){
				So[2] = atof(number);}

				for(j=0;j<3;j++){
					printfloatval = So[j]*100000;
					UARTprintf("int So - het nummer%d: %d\n", j,printfloatval);
					while(!UARTBusy(UART0_BASE));
					while(UARTBusy(UART0_BASE));
					}
				for(j=0;j<10000;j++)
				{k++;}
				UARTprintf("ack\n");
				while(!UARTBusy(UART0_BASE));
				while(UARTBusy(UART0_BASE));

			}

		}
		else if(receivedString[i] == 'M') //Six:
				{

					calcReq = 1;
					if(receivedString[i+1] == 'i')
					{
						UARTprintf("analyzing Mi\n");
						j=(i+4); //'ix:' overslaan
						while(receivedString[j] !='\n')
						{
							number[k] = receivedString[j];
							//UARTprintf("number transmission Si: %c\n", number[k]);
							j++;
							if(receivedString[j]!='\n'){
							k++;}
						}
						//i = i+2;
						if(receivedString[i+2]=='1'){
							Mi[0] = atof(number);} //t ganse getal wordt gevormd door de verschillende chars die hier verzameld zijn
						else if(receivedString[i+2]=='2'){
							Mi[1] = atof(number);}
						else if(receivedString[i+2]=='3'){
							Mi[2] = atof(number);}


						for(j=0;j<3;j++){
							printfloatval = Mi[j]*100000;
							UARTprintf("int Mi - het nummer%d: %d\n", j,printfloatval);
							while(!UARTBusy(UART0_BASE));
							while(UARTBusy(UART0_BASE));
							}
						for(j=0;j<10000;j++)
						{k++;}
						UARTprintf("ack\n");
						while(!UARTBusy(UART0_BASE));
						while(UARTBusy(UART0_BASE));
					}

					else if(receivedString[i+1] == 'o')
					{
						k=0;
							UARTprintf("analyzing Mo\n");
							j=(i+4); //'ix:' overslaan
							while(receivedString[j] !='\n')
							{
								number[k] = receivedString[j];
								//UARTprintf("number transmission Si: %c\n", number[k]);
								j++;
								if(receivedString[j]!='\n'){
									k++;}
							}
								//i = i+2;
								if(receivedString[i+2]=='1'){
									Mo[0] = atof(number);} //t ganse getal wordt gevormd door de verschillende chars die hier verzameld zijn
								else if(receivedString[i+2]=='2'){
									Mo[1] = atof(number);}
								else if(receivedString[i+2]=='3'){
									Mo[2] = atof(number);}

								for(j=0;j<3;j++){
									printfloatval = Mo[j]*100000;
									UARTprintf("int Mo - het nummer%d: %d\n", j,printfloatval);
									while(!UARTBusy(UART0_BASE));
									while(UARTBusy(UART0_BASE));
									}
								for(j=0;j<10000;j++)
								{k++;}
								UARTprintf("ack\n");
								while(!UARTBusy(UART0_BASE));
								while(UARTBusy(UART0_BASE));
					}


				}

		else if(receivedString[i]=='z') //zijde -t is al genomen
			   {

				   	switch(receivedString[i+1])
				   	{
				   	     //tab 0
				   		case('0'):
				   			UARTprintf("blz0 geopend\n");
							actualTab[0]=1;
				   			actualTab[1]=0;
				   			actualTab[2]=0;
				   			actualTab[3]=0;
				   			actualTab[4]=0;
				   			actualTab[5]=0;
				   			break;
				   		//gain
				   		case('1'):
							UARTprintf("blz1 geopend\n");
							actualTab[0]=0;
							actualTab[1]=1;
							actualTab[2]=0;
							actualTab[3]=0;
							actualTab[4]=0;
							actualTab[5]=0;
				   			break;
				   		case('2'):
							UARTprintf("blz2 geopend\n");
							actualTab[0]=0;
							actualTab[1]=0;
							actualTab[2]=1;
							actualTab[3]=0;
							actualTab[4]=0;
							actualTab[5]=0;
				   			break;

				   		case('x'):
							UARTprintf("blzx geopend\n");
							actualTab[0]=0;
							actualTab[1]=0;
							actualTab[2]=0;
							actualTab[3]=1;
							actualTab[4]=0;
							actualTab[5]=0;
							break;
				   		case('4'):
				   			UARTprintf("blz4 geopend\n");
				   			actualTab[0]=0;
				   			actualTab[1]=0;
				   			actualTab[2]=0;
				   			actualTab[3]=0;
				   			actualTab[4]=1;
				   			actualTab[5]=0;
				   		break;

				   		case('5'):
				   			UARTprintf("blz5 geopend\n");
				   			actualTab[0]=0;
				   			actualTab[1]=0;
				   			actualTab[2]=0;
				   			actualTab[3]=0;
				   			actualTab[4]=0;
				   			actualTab[5]=1;
						break;
				   		}
		   			}



		else if(receivedString[i]=='a') //	angle BDOT
		{
			j = i+2; //we pass the semicolumn
			k=0;
			calcReq =1;
			while(receivedString[j] !='\n')
			{
				number[k] = receivedString[j];
				j++;
				if(receivedString[j]!='\n')
				{k++;}
			}
			angle = atof(number);
			printfloatval = angle*1000;
			//UARTprintf("bdot angle: %d\n",printfloatval);
			UARTprintf("bdotA\n");
			while(!UARTBusy(UART0_BASE));
			while(UARTBusy(UART0_BASE));
			UARTprintf("bdotA\n");
			calcReq = 0;

		}
		else if(receivedString[i] == 'w') //windings
		{
			k=0;
			j=(i+2); //'i:' overslaan
			while(receivedString[j] !='\n')
			{
				number[k] = receivedString[j];
				j++;
				if(receivedString[j]!='\n')
				{k++;}
			}
			windings = atoi(number);
			UARTprintf("amount of windings: %d\n", windings);
			while(!UARTBusy(UART0_BASE));
			while(UARTBusy(UART0_BASE));
			UARTprintf("bdotA\n");
			calcReq = 0;
		}

		else if(receivedString[i] == 's') //surface
		{
			calcReq = 0;
			k=0;
			j=(i+2); //'i:' overslaan
			while(receivedString[j] !='\n')
			{
				number[k] = receivedString[j];
				//UARTprintf("number transmission Surface coil: %c\n", number[k]);
				j++;
				if(receivedString[j]!='\n'){
					k++;}
			}
			coilArea = atof(number);
			UARTprintf("surface coil: %d\n", coilArea);
			while(!UARTBusy(UART0_BASE));
			while(UARTBusy(UART0_BASE));
		/*	UARTprintf("bdtdddd\n");

			UARTprintf("final bdot values\n");
			while(UARTBusy(UART0_BASE));
			printfloatval = angle * 1000000;
			UARTprintf("bdot angle: %d\n",printfloatval);
			while(UARTBusy(UART0_BASE));
			UARTprintf("amount of windings: %d\n", windings);
			while(UARTBusy(UART0_BASE));
			printfloatval = coilArea * 10000000;
			UARTprintf("surface coil: %d\n", printfloatval);
			while(UARTBusy(UART0_BASE));
*/
			calcReq = 1;
		}
	}


		for (i=0;i<BUF_SIZE;i++)
		{receivedString[i] = '\0';}


}

void
UARTIntHandler(void)
{
	int i,ui32Loop;

	for(i=0;i<10;i++)
	{
		GPIO_PORTN_DATA_R |= 0xff;
		for(ui32Loop = 0; ui32Loop < 200000; ui32Loop++)
		{
		}

		GPIO_PORTN_DATA_R &= ~(0xff);
		for(ui32Loop = 0; ui32Loop < 200000; ui32Loop++)
		{
		}
	}


	UARTprintf("receiving data\n");
	int ulStatus=0;
	static char receivedDataQt[BUF_SIZE]={0};

	  ulStatus = UARTIntStatus(UART0_BASE, true);
	  while(UARTCharsAvail(UART0_BASE))
	  {
	     char c = UARTCharGet(UART0_BASE);
	     UARTprintf("%c\n",c);
	     if (letter < BUF_SIZE)
	     {
	    	 receivedDataQt[letter++] = c;
	     }
	     if (c =='\n')
	     {
	    	 UARTprintf("carriage return mc\n");
	    	 letter = 0;
	    	 analyzeString(receivedDataQt);
	     }
	     if(letter>=BUF_SIZE)
	     {	 UARTprintf("overflow\n");
	    	 analyzeString(receivedDataQt);
	    	 break;}
	  }
	    UARTIntClear(UART0_BASE, ulStatus);

}


void setUARTinterrupt()
{
	IntMasterEnable();
	IntRegister(INT_UART0,UARTIntHandler );
	IntEnable(INT_UART0);
	UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_RT);

}





/******************************************************************************************************************************/
//graphs UI in stead of sensors

void sendRandomData()
{
	sendFakeRed();
	sendFakeBlue();
	sendFakeGreen();
	sendFakeClear();

}


void sendFakeClear()
{
	int k,n=0;
	static int i=0;
	static int j=6000;
	static int invertI=0;
	static int invertJ = 0;

	UARTprintf("s1 c: %u\n" ,i);
	while(!UARTBusy(UART0_BASE));
	while(UARTBusy(UART0_BASE));

	UARTprintf("s0 c: %u\n" ,j);
	while(!UARTBusy(UART0_BASE));
	while(UARTBusy(UART0_BASE));


	if (invertI==0)
	{i+=30;}
	else if(invertI==1)
	{i-=52;}
	if (i>=6000)
	{invertI=1;}
	if(i<=100)
	{invertI=0;}


	if(invertJ==0)
	{j-=10;}
	else if(invertJ==1)
	{j+=44;}
	if (j>=6000)
	{invertJ=0;}
	if(j<=100)
	{invertJ=1;}


	for(k=0;k<100000;k++)
	{n++;}
	n=0;

}


void sendFakeGreen()
{
	int k,n=0;
	static int i=0;
	static int j=6000;
	static int invertI=0;
	static int invertJ = 0;

	UARTprintf("s1 g: %u\n" ,i);
	while(!UARTBusy(UART0_BASE));
	while(UARTBusy(UART0_BASE));

	UARTprintf("s0 g: %u\n" ,j);
	while(!UARTBusy(UART0_BASE));
	while(UARTBusy(UART0_BASE));


	if (invertI==0)
	{i+=14;}
	else if(invertI==1)
	{i-=50;}
	if (i>=6000)
	{invertI=1;}
	if(i<=100)
	{invertI=0;}


	if(invertJ==0)
	{j-=50;}
	else if(invertJ==1)
	{j+=125;}
	if (j>=6000)
	{invertJ=0;}
	if(j<=100)
	{invertJ=1;}


	for(k=0;k<100000;k++)
	{n++;}
	n=0;
}


void sendFakeBlue()
{
	int k,n=0;
	static int i=0;
	static int j=6000;
	static int invertI=0;
	static int invertJ = 0;

	UARTprintf("s1 b: %u\n" ,i);
	while(!UARTBusy(UART0_BASE));
	while(UARTBusy(UART0_BASE));

	UARTprintf("s0 b: %u\n" ,j);
	while(!UARTBusy(UART0_BASE));
	while(UARTBusy(UART0_BASE));


	if (invertI==0)
	{i+=50;}
	else if(invertI==1)
	{i-=50;}
	if (i>=6000)
	{invertI=1;}
	if(i<100)
	{invertI=0;}


	if(invertJ==0)
	{j-=100;}
	else if(invertJ==1)
	{j+=115;}
	if (j>=6000)
	{invertJ=0;}
	if(j<=100)
	{invertJ=1;}


	for(k=0;k<100000;k++)
	{n++;}
	n=0;

}

void sendFakeRed()
{
	int k,n=0;
	static int i=0;
	static int j=6000;
	static int invertI=0;
	static int invertJ = 0;

	UARTprintf("s1 r: %u\n" ,i);
	while(!UARTBusy(UART0_BASE));
	while(UARTBusy(UART0_BASE));

	UARTprintf("s0 r: %u\n" ,j);
	while(!UARTBusy(UART0_BASE));
	while(UARTBusy(UART0_BASE));


	if (invertI==0)
	{i+=50;}
	else if(invertI==1)
	{i-=50;}
	if (i>=6000)
	{invertI=1;}
	if(i<=100)
	{invertI=0;}


	if(invertJ==0)
	{j-=50;}
	else if(invertJ==1)
	{j+=50;}
	if (j>=6000)
	{invertJ=0;}
	if(j<=100)
	{invertJ=1;}


	for(k=0;k<100000;k++)
	{n++;}
	n=0;

}




