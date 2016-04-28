/*
 * QUEST.h
 *
 *  Created on: 6 juin 2015
 *      Author: yalishanda dao
 */

#ifndef QUEST_H_
#define QUEST_H_




typedef struct{

	void (*TRIAD)(volatile float *Mi, volatile float *Mo, volatile float *Si, volatile float *So, float *RM);
	void (*QUEST)( volatile float *V1i, volatile float *V2i, volatile float *V1b, volatile float *V2b, float *RM);
	void (*multi3x1Mtrx)(volatile float *a, int widthA, int weightA, volatile float *b, int widthB, int weightB, float *output);
	void (*sumB)(float *B, float *Bt,float *output, int width, int height);
	void (*createZ)(float *,float*);
	void (*calcSigma)(float *,float*);
	void (*setupK)(float*S, float *sigma, float *Z, float *K);
	void (*calcP)(float *sigma, float *lOpt, float *S, float *Z, float *P);
	void (*inverseMatrix)(float *input, float *output);
	void(*quaternionToMatrix)(float * input, float *output);
	void(*transposeMatrix2)(float* array2, int height, int width, float * output, int height2, int width2);


}questClass;

questClass* initializeQUESTclass();



#endif /* QUEST_H_ */
