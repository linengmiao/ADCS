/*
 * myHfile.c
 *
 *  Created on: 5 juin 2015
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

void mijnHfunctie()
{
 UARTprintf("in mijnHfunctie\n");
	while(UARTBusy(UART0_BASE));

}
