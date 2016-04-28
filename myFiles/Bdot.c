/*
 * Bdot.c
 *
 *  Created on: 6 juin 2015
 *      Author: yalishanda dao
 */



#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <inttypes.h>
#include "inc/tm4c1294ncpdt.h"
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
#include "utils/uartstdio.h"
#include "driverlib/pwm.h"

#include "Bdot.h"


/* Bdot functions*/
void Bdot();
float calcCurrent();
void setupPwm(int highTime);
void testFunc();
void includeBdot();
float calcPWMdutyCycle(float neededCurrent);
int calculateHighTime(float dutyCycle);


extern bdotClass * bdotPtr;
extern int windings;
extern float coilArea;
extern float angle;

bdotClass* initializeBdotClass()
{

	bdotClass * bdotPtr = malloc(sizeof(bdotClass));
	bdotPtr->bdot = Bdot;
	bdotPtr->calcCurrent = calcCurrent;
	bdotPtr->setupPwm = setupPwm;
	bdotPtr->testFunc = testFunc;

	return bdotPtr;


}


void includeBdot ()
{
	UARTprintf("Bdot include\n");
	while(UARTBusy(UART0_BASE));


}

/***************************************************************************************************************/
/*B-dot*/

void Bdot()
{
	float necessaryI=0;
	float dutyCycle=0;
	int highTime=0;

	UARTprintf("calculating Bdot\n");
	necessaryI = calcCurrent();
	dutyCycle = calcPWMdutyCycle(necessaryI);
	highTime = calculateHighTime(dutyCycle);
	setupPwm(highTime);

}


float calcCurrent()
{
	float Bder;
	float M;
	float I;

	int printFloat;

	Bder = (-1) * angle * sin(angle);
	M = (-1) * Bder;
	I = M/(windings * coilArea);

	printFloat = I*100000;
	UARTprintf("bdot current = %d\n", printFloat);

	return I;
}

float calcPWMdutyCycle(float neededCurrent)
{

		float Bder;
		float M;
		float I;
		float noemer,teller;
		int printFloat;

		float dutyCycle;

		/*Maximal angle to calculate proportion*/
		Bder = (-1) * 1 * sin(1);
		M = (-1) * Bder;
		I = M/(windings * coilArea);

		noemer = I;
		teller = neededCurrent;

		dutyCycle = (teller/noemer)*100;
		printFloat = dutyCycle * 100000;
		UARTprintf("dutycycle: %d\n", printFloat);
		return dutyCycle;


}


void setupPwm(highTime)
{/*
	uint32_t ui32Loop;
    SYSCTL_RCGCGPIO_R = SYSCTL_RCGCGPIO_R9;
    GPIO_PORTK_DIR_R = 0xFF;
    GPIO_PORTK_DEN_R = 0xFF;
    unsigned long pwm0base = 0x40028000;
    */


  /*  SysCtlPWMClockSet(SYSCTL_PWMDIV_1);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM1);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    GPIOPinConfigure(0x00050406); //bron pin_map.h
    GPIOPinTypePWM(GPIO_PORTF_BASE, GPIO_PIN_1);
    PWMGenPeriodSet(PWM1_BASE, PWM_GEN_0, 64000);
    PWMGenConfigure(PWM1_BASE, PWM_GEN_0, PWM_GEN_MODE_UP_DOWN | PWM_GEN_MODE_NO_SYNC);

    PWMPulseWidthSet(PWM1_BASE, PWM_OUT_1,PWMGenPeriodGet(PWM1_BASE, PWM_OUT_1) / 4);

    PWMOutputState(PWM1_BASE, PWM_OUT_1_BIT, true);
    PWMGenEnable(PWM1_BASE, PWM_GEN_0);
    */

	//float highTime;

	//highTime = calculateHighTime(dutyCycle);

	 SysCtlPWMClockSet(SYSCTL_PWMDIV_1);
	 SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM0);
	 SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	 GPIOPinConfigure(0x00050406); //bron pin_map.h
	 GPIOPinTypePWM(GPIO_PORTF_BASE, GPIO_PIN_1);
	 PWMGenPeriodSet(PWM0_BASE, PWM_GEN_0, 64000);
	 PWMGenConfigure(PWM0_BASE, PWM_GEN_0, PWM_GEN_MODE_UP_DOWN | PWM_GEN_MODE_NO_SYNC);

	 PWMPulseWidthSet(PWM0_BASE, PWM_OUT_1,/*63990*/ highTime);

	 PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, true);
	 PWMGenEnable(PWM0_BASE, PWM_GEN_0);

	 UARTprintf("setting up PWM\n");

}

int calculateHighTime(float dutyCycle)
{

	int returnValueHigh;
	int maxRegValue = 1600;

	returnValueHigh = (dutyCycle * maxRegValue)/100;
	return returnValueHigh;

}


void testPWM()
{

    SysCtlDelay((SysCtlClockGet() * 5) / 3);

    PWMOutputInvert(PWM1_BASE, PWM_OUT_1_BIT, true);


    SysCtlDelay((SysCtlClockGet() * 5) / 3);
    PWMOutputInvert(PWM1_BASE, PWM_OUT_1_BIT, false);

}


void testFunc()
{
	int i,ui32Loop;

/*	for(i=0;i<10;i++){
	    GPIO_PORTK_DATA_R |= 0xff;
	    for(ui32Loop = 0; ui32Loop < 200000; ui32Loop++)
	    {
	    }

	    GPIO_PORTK_DATA_R &= ~(0xff);
	    for(ui32Loop = 0; ui32Loop < 200000; ui32Loop++)
	    {
	    }

	}*/

	for(i=0;i<10;i++)
	{
			GPIO_PORTF_AHB_DATA_R |= 0xff;
		    for(ui32Loop = 0; ui32Loop < 200000; ui32Loop++)
		    {
		    }

		    GPIO_PORTF_AHB_DATA_R &= ~(0xff);
		    for(ui32Loop = 0; ui32Loop < 200000; ui32Loop++)
		    {
		    }
	}
}
