/*
 * pwm.h
 *
 *  Created on: 8/04/2013
 *      Author: jared
 */

#ifndef PWM_H_
#define PWM_H_

#define NUM_PID 6
#define MAX_ERROR 5000
#define MULTIPLIER 1000

/**
 * Initialises the PID gains
 */
void PID_init();
void PID_setGains(int n, float kp, float ki, float kd);
void PID_setFreq(unsigned int frequency);
void PID_addValue(int n, unsigned int value);
float PID_getResponse(int n);

#endif /* PWM_H_ */
