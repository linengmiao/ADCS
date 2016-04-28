/*
 * Bdot.h
 *
 *  Created on: 6 juin 2015
 *      Author: yalishanda dao
 */

#ifndef BDOT_H_
#define BDOT_H_

void includeBdot();

typedef struct{

	void (*bdot)();
	float (*calcCurrent)();
	void (*setupPwm)(int highTime);
	void (*testFunc)();


}bdotClass;

bdotClass* initializeBdotClass();

#endif /* BDOT_H_ */
