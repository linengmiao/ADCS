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

#include "triad.h"


extern void TRIAD(volatile float *Mi, volatile float *Mo, volatile float *Si, volatile float *So, float *RM);

void createFirstRotMatrix(float firstRotMatrix[3][3]);
void multiplyMatrices(float* sunvectorInertial, int, int, float *transposedRotMatrix, int, int, float* orbitalSunvectors);
void multiplyCrossVectors( volatile float * first, volatile float * second, float *output);
void normalizeVector(volatile float *input, volatile float *normalizedVec);
void regroupMatrix(float *pi,  float *pi2, float *pi3,float *outputMatrix);
void workaroundMallocFail3x3(float a[3][3], float b[3][3], float output[3][3]);
void calcAnglesRM(float RM[3][3]);
void transposeMatrix(float* array2, int height, int width, float * output, int height2, int width2);

void includeTRIAD();


extern volatile int calcReq;
extern triadClass *triadClassPtr;


triadClass* initializeTRIADclass()
{

	triadClass * structPtr  = malloc(sizeof(triadClass));

	structPtr->TRIAD = TRIAD;

	structPtr->crossMultiplyVector = multiplyCrossVectors;//functionAdresses[0];
	structPtr->normalizeVector = normalizeVector;
	structPtr->regroupMatrices = regroupMatrix;
	structPtr->transPoseMatrix = transposeMatrix;
	structPtr->multiplyMatrices = multiplyMatrices;
	structPtr->calcAnglesRM = calcAnglesRM;

	return structPtr;
}

/****************************************************************************************************************************/
/*TRIAD*/



void multiplyMatrices(float *transposedMatrix, int height1, int width1, float *iSunVector,int height2, int width2, float *orbitalSunVector)
{

	int y=0;
	int x = 0;
	int row=0;
	int column =0;
	int k=0;
	int k2=0;
	float result = 0;
	float *output2=NULL;

	int i=0;
	int j=0;

	i=0;
	k=0;
	k2 = 0;


	if(width1 != height2) //unmathcing matrices
	{
		return;
	}

	output2 = malloc(height1 * width2 * sizeof(float));


	while(k<width1) //aantal rijen 1ste matrix
	{
		for(j=0;j<height2;j++) //aantal rijen 2de matrix
		{
			result += (*((transposedMatrix+k*width1)+j)) * (*((iSunVector+j*width2)+k2));  //1ste var:aantal kolommen 2de matrix  --2de variabele na de plus = aantal kolommen 2de matrix
		}

		output2[row*width2 + column] = result;

        k2++;
		x++;
		column++;

		if(x==width2) //aantal kolommen 2de Matrix
		{
			k2=0;
			x=0;
			column=0;
			row++;
			y++;
			k++;
		}
		result = 0;

	}

	//tussenresultaat
 	for(i=0;i<height1;i++)
	{
		for(j=0;j<width2;j++)
		{
			orbitalSunVector[j*height1 + i] = output2[i*width2 + j];

		}
	}

 	free(output2);
}



void normalizeVector( volatile float *input, volatile float *normalizedVec)
{
	float magnitude;

	magnitude = pow(input[0],2) + pow(input[1],2) + pow(input[2],2);
	magnitude = sqrt(magnitude);

	//printf("magnitude: %f\n", magnitude);
	normalizedVec[0]=input[0]/magnitude;
	normalizedVec[1]=input[1]/magnitude;
	normalizedVec[2]=input[2]/magnitude;

}


void calcAnglesRM(float finalRotationMatrix[3][3])
{
	float testTheta=0.0;
	float testPhi = 0.0;
	float testEpsi = 0.0;

	testTheta = lround((acos(finalRotationMatrix[2][2])*180.0)/3.14);

	testEpsi = ((-(finalRotationMatrix[2][1]))/(sin(testTheta)));
	testEpsi = (acos(testEpsi));
	testEpsi = (testEpsi) * (180/3.14);
	testEpsi = lround(testEpsi);

	testPhi = ((finalRotationMatrix[1][2])/sin(testTheta));
	testPhi = (acos(testPhi))*180.0/3.14;
	testPhi = lround(testPhi);

}


void multiplyCrossVectors( volatile float * first, volatile float * second, float *output)
{

	output[0] = (first[1] * second[2]) - (first[2] * second[1]);
	output[1] = (first[2] * second[0]) - (first[0] * second[2]);
	output[2] = (first[0] * second[1]) - (first[1] * second[0]);

}

void transposeMatrix(float* array2, int height, int width, float * output, int height2, int width2)
{

	int i=0, j=0;

	for (i=0;i<height; i++)
	{
		for(j=0;j<width;j++)
		{
			output[j * height + i] = array2[i*width + j];

		}
	}

}

void TRIAD(volatile float *Mi, volatile float *Mo, volatile float *Si, volatile float *So, float *RM)
{
	int value, iValue;

	int i,j,n,k=0;

	float to2[3] = {0};
	float ti2[3] = {0};

	float Pib[3] = {0};
	float Pob[3] = {0};
	float Pi2b[3] = {0};
	float Po2b[3] = {0};

	float Pi3b[3]={0};
	float Po3b[3]= {0};

	float PiMatrixb[9]={0};
	float PoMatrixb[9]={0};
	float PoMatrixtb[9] = {0};

	float finalRotationMatrix[3][3]={0};

		triadClassPtr->crossMultiplyVector(Mo,So,to2);
		triadClassPtr->crossMultiplyVector(Mi,Si,ti2);
		triadClassPtr->normalizeVector(Mo,Pob);
		triadClassPtr->normalizeVector(Mi,Pib);
		triadClassPtr->normalizeVector(ti2,Pi2b);
		triadClassPtr->crossMultiplyVector(Pib,Pi2b,Pi3b);
		triadClassPtr->regroupMatrices(Pib,Pi2b,Pi3b, PiMatrixb);
		triadClassPtr->normalizeVector(to2,Po2b);
		triadClassPtr->crossMultiplyVector(Pob,Po2b,Po3b);
		triadClassPtr->regroupMatrices(Pob,Po2b, Po3b, PoMatrixb);
		triadClassPtr->transPoseMatrix(PoMatrixb, 3 ,3, PoMatrixtb,3 ,3);
		triadClassPtr->multiplyMatrices(PiMatrixb,3, 3, PoMatrixtb, 3, 3, *finalRotationMatrix);


		RM[0] = (finalRotationMatrix[0][0]);  //zie p 4-16 exacte Rtriad
		RM[1] = (finalRotationMatrix[0][1]);
		RM[2] = (finalRotationMatrix[0][2]);
		RM[3] = (finalRotationMatrix[1][0]);
		RM[4] = (finalRotationMatrix[1][1]);
		RM[5] = (finalRotationMatrix[1][2]);
		RM[6] = (finalRotationMatrix[2][0]);
		RM[7] = (finalRotationMatrix[2][1]);
		RM[8] = (finalRotationMatrix[2][2]);

		for(i=0;i<3;i++)
		{
			for(j=0;j<3;j++)
			{
				value = (finalRotationMatrix[i][j] * 1000000000);
				iValue = value;
				UARTprintf("m: %d\t%d  = %d\r\n",i,j,iValue);
				while(!UARTBusy(UART0_BASE));
				while(UARTBusy(UART0_BASE));
				for(n=0;n<100000;n++)
				{k++;}

			}
		}

		k--;
        calcReq = 0;
		triadClassPtr->calcAnglesRM( finalRotationMatrix);

		UARTprintf("TRIAD\n");

}





void workaroundMallocFail3x3(float array0[3][3], float array1[3][3], float output[3][3])
{

	output[0][0] = (array0[0][0] * array1[0][0]) + (array0[0][1] * array1[1][0]) + (array0[0][2] * array1[2][0]);
	output[0][1] = (array0[0][0] * array1[0][1]) + (array0[0][1] * array1[1][1]) + (array0[0][2] * array1[2][1]);
	output[0][2] = (array0[0][0] * array1[0][2]) + (array0[0][1] * array1[1][2]) + (array0[0][2] * array1[2][2]);

	output[1][0] = (array0[1][0] * array1[0][0]) + (array0[1][1] * array1[1][0]) + (array0[1][2] * array1[2][0]);
	output[1][1] = (array0[1][0] * array1[0][1]) + (array0[1][1] * array1[1][1]) + (array0[1][2] * array1[2][1]);
	output[1][2] = (array0[1][0] * array1[0][2]) + (array0[1][1] * array1[1][2]) + (array0[1][2] * array1[2][2]);

	output[2][0] = (array0[2][0] * array1[0][0]) + (array0[2][1] * array1[1][0]) + (array0[2][2] * array1[2][0]);
	output[2][1] = (array0[2][0] * array1[0][1]) + (array0[2][1] * array1[1][1]) + (array0[2][2] * array1[2][1]);
	output[2][2] = (array0[2][0] * array1[0][2]) + (array0[2][1] * array1[1][2]) + (array0[2][2] * array1[2][2]);

}


void regroupMatrix(float *pi,  float *pi2, float *pi3,float *outputMatrix)
{

	outputMatrix[0] = pi[0];
	outputMatrix[1] = pi2[0];
	outputMatrix[2] = pi3[0];

	outputMatrix[3] = pi[1];
	outputMatrix[4] = pi2[1];
	outputMatrix[5] = pi3[1];

	outputMatrix[6] = pi[2];
	outputMatrix[7] = pi2[2];
	outputMatrix[8] = pi3[2];
}







void includeTRIAD()
{

 UARTprintf("TRIAD include\n");
	while(UARTBusy(UART0_BASE));

}




