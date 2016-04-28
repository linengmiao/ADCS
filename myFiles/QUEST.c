/*
 * QUEST.c
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

#include "QUEST.h"

void includeQUEST();

/*QUEST*/
void QUEST( volatile float *V1i, volatile float *V2i, volatile float *V1b, volatile float *V2b, float *RM);

void multi3x1Mtrx(volatile float *a, int widthA, int weightA, volatile float *b, int widthB, int weightB, float *output);
void sumB(float *B, float *Bt,float *output, int width, int height);
void createZ(float *,float*);
void calcSigma(float*, float *);
void setupK(float*S, float *sigma, float *Z, float *K);
void calcP(float *sigma, float *lOpt, float *S, float *Z, float *P);
void inverseMatrix(float *input, float *output);
void quaternionToMatrix(float * input, float *output);


void transposeMatrix2(float* array2, int height, int width, float * output, int height2, int width2);

extern volatile int calcReq;
extern questClass *questClassPtr;

questClass* initializeQUESTclass()
{
	questClass * questPtr = malloc(sizeof(questClass));

	questPtr->QUEST = QUEST;
	questPtr->multi3x1Mtrx = multi3x1Mtrx;
	questPtr->sumB = sumB;
	questPtr->createZ = createZ;
	questPtr->calcSigma = calcSigma;
	questPtr->setupK = setupK;
	questPtr->calcP = calcP;
	questPtr->inverseMatrix = inverseMatrix;
	questPtr->quaternionToMatrix = quaternionToMatrix;

	questPtr->transposeMatrix2 = transposeMatrix2;

	return questPtr;

}


/*************************************************************************************************************************************/
/*QUEST*/

void QUEST(volatile float * V1i, volatile float *V2i, volatile float *V1b, volatile float *V2b, float *RM)
{


	float V1ib[9]={0};
	float V2ib[9]={0};


	float B[9]={0};
	float Bt[9]={0};
	float S[9]={0};
	float Z[3]={0};
	float sigma=0;
	float K[16]={0};
	float P[4] = {0.0};
	float lOpt = 2;
	int i,j,n;
	int k=0;
	int iValue;
	int value;

	float finalRotationMatrix[3][3]={0};

	questClassPtr->multi3x1Mtrx(V1b,3,3,V1i,3,3,V1ib);

	questClassPtr->multi3x1Mtrx(V2b,3,3,V2i,3,3,V2ib);

	questClassPtr->sumB(V1ib,V2ib,B,3,3);
	questClassPtr->transposeMatrix2(B,3,3,Bt,3,3);
	questClassPtr->sumB(B,Bt,S,3,3);
	questClassPtr->createZ(B,Z);
	questClassPtr->calcSigma(B,&sigma);
	questClassPtr->setupK(S, &sigma, Z,K);
	questClassPtr->calcP(&sigma, &lOpt, S, Z, P);
	questClassPtr->quaternionToMatrix(P, RM);


	finalRotationMatrix[0][0] = RM[0] ;  //zie p 4-16 exacte Rtriad
	finalRotationMatrix[0][1] = RM[1];
	finalRotationMatrix[0][2] = RM[2];
	finalRotationMatrix[1][0] = RM[3];
	finalRotationMatrix[1][1] = RM[4];
	finalRotationMatrix[1][2] = RM[5];
	finalRotationMatrix[2][0] = RM[6];
	finalRotationMatrix[2][1] = RM[7];
	finalRotationMatrix[2][2] = RM[8];

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

	UARTprintf("QUEST\n");
	UARTprintf("QUEST\n");
	UARTprintf("QUEST\n");
}

void transposeMatrix2(float* array2, int height, int width, float * output, int height2, int width2)
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

void quaternionToMatrix(float * input, float *output)
{


    output[8] = input[0]*input[0] + input[1]*input[1] - input[2]*input[2] - input[3]*input[3];
    output[7] = (2 * ((input[1])*(input[2]))) - (2*((input[0]) * (input[3])));
    output[6] = (2 * ((input[1]) *(input[3]))) + (2*((input[0]) *(input[2])));

    output[5] = (2 * ((input[1])*(input[2]))) + (2*((input[0]) * (input[3])));
    output[4] = ((input[0])*(input[0])) - ((input[1])*(input[1])) + ((input[2])*(input[2])) - ((input[3])*(input[3]));
    output[3] = (2*(input[2] * (input[3]))) - (2*(input[0] * (input[1])));

    output[2] = (2 * (input[1] * (input[3]))) - (2*(input[0] * (input[2])));
    output[1] = (2 * (input[2] * (input[3]))) + (2*(input[0] * (input[1])));
    output[0] = (input[0]*(input[0])) - (input[1]*(input[1])) - (input[2]*(input[2])) + (input[3]*(input[3]));



}

void calcP(float *sigma, float *lOpt, float *S, float *Z, float *P)
{
	float p=0;
 	int i;
	float tussenP[9]={0};
	float output[9]={0};

	float Yconcat[4]={0.0};
	float pInvXp=0.0;



	p = (*lOpt) + (*sigma);
	//printf("in func: %f


	for(i=0;i<9;i++)
	{
		tussenP[i] = 0;
		if(i==0)
		{
			tussenP[i] = p;
		}
		else if(i==4)
		{
			tussenP[i] = p;
		}
		else if(i==8)
		{
			tussenP[i] = p;
		}
	}

	for(i=0;i<9;i++)
	{
		tussenP[i] = tussenP[i] - S[i];
	}

	inverseMatrix(tussenP,output);


	tussenP[0]= (Z[0]*output[0]) + (Z[1]*output[1]) + (Z[2]*output[2]);
	tussenP[1]= (Z[0]*output[3]) + (Z[1]*output[4]) + (Z[2]*output[5]);
	tussenP[2]= (Z[0]*output[6]) + (Z[1]*output[7]) + (Z[2]*output[8]);


	for(i=0;i<3;i++)
	{
		Yconcat[i] = tussenP[i];
	}
	Yconcat[3] = 1;


	/*
	Alex 0.411 ng iutrekenen hier, zie website
	*/

	pInvXp = (tussenP[0] * tussenP[0]) + (tussenP[1]*tussenP[1]) + (tussenP[2]*tussenP[2]);

	for(i=0;i<4;i++)
	{
		P[i] = (1/sqrt(1+pInvXp)) * Yconcat[i];
	}





}

void setupK(float*S, float *sigma, float *Z, float *K)
{

  	K[0] = S[0] - (*sigma);
    K[1] = S[1];
    K[2] = S[2];
    K[3] = Z[0];

    K[4] = S[3];
    K[5] = S[4] - (*sigma);
    K[6] = S[5];
    K[7] = Z[1];

    K[8] = S[6];
    K[9] = S[7];
    K[10] = S[8] - (*sigma);
    K[11] = Z[2];

    K[12] = Z[0];
    K[13] = Z[1];
    K[14] = Z[2];
    K[15] = (*sigma);

}
void calcSigma(float* B,float *sigma)
{

	*sigma = B[0] + B[4] + B[8];

}

void createZ(float *input, float *output)
{
	output[0] = input[5] - input[7];
	output[1] = input[6] - input[2];
	output[2] = input[1] - input[3];
}

void multi3x1Mtrx(volatile float *a, int widthA, int heightA, volatile float *b, int widthB, int heightB, float *output)
{
	int i,j,k=0;

	float **array; //ptr->ptr == array


	array = malloc(widthB * sizeof(int *)); //1 adress wordt geladen, die plaats heeft widthB keer ruimte vr een int ptr
	for (i = 0; i < widthB; i++)
  	{
		array[i] = malloc(heightA * sizeof(int)); //
	}

	for(i=1;i<widthB+1;i++)
	{
		for(j=1;j<heightA+1;j++)
		{
		 	array[i-1][j-1] = (a[i-1]) * (b[j-1]);
		}
	}

	for(i=0;i<widthB;i++)
	{
		for(j=0;j<heightA;j++)
		{
		 	output[k] = array[i][j];
			k++;
		}
	}

	for (i = 0; i < widthB; i++)
  	{
		free(array[i]);
	}
	free(array);


}

void sumB(float *B, float *Bt,float *output, int height, int width)
{

	int i;
	int iterMax;

	iterMax = height*width;

	for(i=0;i<iterMax;i++)
	{
	output[i] = B[i] + Bt[i];
	}


}



void inverseMatrix(float *input, float *inverse)
{

	int i=0;
	float output[9]={0.0};
	float outputt[9]={0.0};
	float tussenResult[9]={0.0};
	float Cinv=0.0;


	/*calculate matrix of minors*/
	output[0] = ((float)input[4]*(float)input[8])-((float)input[5]*(float)input[7]);
	output[1] = ((float)input[3]*(float)input[8])-((float)input[6]*(float)input[5]);
	output[2] = ((float)input[3]*(float)input[7])-((float)input[6]*(float)input[4]);
	output[3] = ((float)input[1]*(float)input[8])-((float)input[7]*(float)input[2]);
	output[4] = ((float)input[0]*(float)input[8])-((float)input[2]*(float)input[6]);
	output[5] = ((float)input[0]*(float)input[7])-((float)input[6]*(float)input[1]);
	output[6] = ((float)input[1]*(float)input[5])-((float)input[4]*(float)input[2]);
	output[7] = ((float)input[0]*(float)input[5])-((float)input[3]*(float)input[2]);
	output[8] = ((float)input[0]*(float)input[4])-((float)input[3]*(float)input[1]);


	/*calculate cofactor matrix*/
	output[0] = (float)output[0] * (+1.0);
	output[1] = (float)output[1] * (-1.0);
	output[2] = (float)output[2] * (+1.0);
	output[3] = (float)output[3] * (-1.0);
	output[4] = (float)output[4] * (+1.0);
	output[5] = (float)output[5] * (-1.0);
	output[6] = (float)output[6] * (+1.0);
	output[7] = (float)output[7] * (-1.0);
	output[8] = (float)output[8] * (+1.0);


	/*calculate determinant*/
	tussenResult[0] = (float)input[0] * (float)input[4] * (float)input[8];
	tussenResult[1] = (float)input[1] * (float)input[5] * (float)input[6];
	tussenResult[2] = (float)input[2] * (float)input[3] * (float)input[7];
	tussenResult[3] = (float)input[1] * (float)input[3] * (float)input[8];
	tussenResult[4] = (float)input[0] * (float)input[5] * (float)input[7];
	tussenResult[5] = (float)input[2] * (float)input[4] * (float)input[6];


	tussenResult[6] = ((float)tussenResult[0] + (float)tussenResult[1] + (float)tussenResult[2]);
	tussenResult[7] =   (-((float)tussenResult[3]) - ((float)tussenResult[4]) -((float)tussenResult[5])); //18
	tussenResult[8] = (float)tussenResult[6] + (float)tussenResult[7];


	transposeMatrix2(output,3,3,outputt,3,3);
	Cinv = (1.0/(float)tussenResult[8]);

	for(i=0;i<9;i++)
	{

		inverse[i] = (float)((float)outputt[i]*(float)Cinv); // first operation: 1/23  next 18/23
	}

}

void includeQUEST()
{
	UARTprintf("QUEST include\n");
	while(UARTBusy(UART0_BASE));


}
