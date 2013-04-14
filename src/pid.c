/*
 * pid.c
 *
 *  Created on: 8/04/2013
 *      Author: jared\
 *      www.atmel.com/images/doc2558.pdf
 */

#include "pid.h"

typedef struct {
	float kp;
	float ki;
	float kd;
	float sum;
	unsigned int target;
	unsigned int lastValue;
	int sumError;
	float result;
} TPID;

static TPID pid[NUM_PID];
static unsigned int s_frequency;


void PID_init()
{
	for (int i = 0; i < NUM_PID; i++)
	{
		pid[NUM_PID].lastValue = 0;
		pid[NUM_PID].sum = 0.0f;
		pid[NUM_PID].target = 0;

		// Zero gains
		PID_setGains(i,0,0,0);
	}
}

void PID_setGains(int n, float kp, float ki, float kd)
{
	pid[n].kp = kp;
	pid[n].ki = ki;
	pid[n].kd = kd;
}

void PID_setFreq(unsigned int frequency)
{
	s_frequency = frequency;
}

void PID_addValue(int n, unsigned int value)
{
	unsigned int e = pid[n].target - value;
	unsigned int d = value - pid[n].lastValue;
	pid[n].sumError += value;

	if (pid[n].sumError > MAX_ERROR)
		pid[n].sumError = MAX_ERROR;

	if (pid[n].sumError < -MAX_ERROR)
		pid[n].sumError = -MAX_ERROR;

	float result = pid[n].kp*e + pid[n].ki*pid[n].sum + pid[n].kd*d;
	pid[n].result = result;
}

float PID_getResult(int n)
{
	return pid[n].result;
}
