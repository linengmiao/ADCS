/*
 * TRIAD.h
 *
 *  Created on: 6 juin 2015
 *      Author: yalishanda dao
 */

#ifndef TRIAD_H_
#define TRIAD_H_



typedef struct {

	//functionPointers
	void (*TRIAD)(volatile float *Mi, volatile float *Mo, volatile float *Si, volatile float *So, float *RM);

	void (*crossMultiplyVector)(volatile float *, volatile float*, float*);
	void (*normalizeVector)( volatile float * input, volatile float* normalizedVector);
	void (*regroupMatrices)(float *pi,  float *pi2, float *pi3,float *outputMatrix);
	void (*transPoseMatrix)(float* , int, int, float*, int ,int);
	void (*multiplyMatrices)(float* sunvectorInertial, int, int, float *transposedRotMatrix, int, int, float* orbitalSunvectors);
	void (*calcAnglesRM)(float RM[3][3]);



}triadClass;


triadClass* initializeTRIADclass();



//extern int calcReq=0;

#endif /* TRIAD_H_ */
