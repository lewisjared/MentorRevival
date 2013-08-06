/*
 * joystick.c
 *
 *  Created on: 3/05/2013
 *      Author: Jared
 *
 *      This module interfaces with a PS2 joystick to provide 4 axes of movement.
 */

#include "ch.h"
#include "hal.h"

#include "adcThread.h"
#include "joystick.h"

#include <assert.h>
#include <stdlib.h>

#define MAX_ADC_VAL 4096
#define MIDPOINT MAX_ADC_VAL/2
#define VALUE_SPAN (MIDPOINT-JOY_THRES)

void joy_init(void)
{

	//Initialises the ADC and begins conversion
	ADC_init();

	//Check if the number of joysticks is less than or equal to the number of ADC values
	assert(NUM_JOYSTICKS <= ADC_NUM_CHANNELS);
}

void joy_getValues(joypos_t* buff)
{
	//Copy the ADC values into buff
	int i;
	adcsample_t rawVals[NUM_JOYSTICKS];
	chSysLock()
	for (i = 0; i < NUM_JOYSTICKS; i++)
	{
		rawVals[i] = currentPos[i];
	}
	chSysUnlock();

	for (i = 0; i < NUM_JOYSTICKS; i++)
	{
		buff[i].dir = (rawVals[i] > MIDPOINT);
		uint16_t mag = (uint16_t) abs(rawVals[i] - MIDPOINT);
		if (mag > JOY_THRES)
		{
			mag -= JOY_THRES;

			//Scale from 0 to JOY_FULL_SCALE
			buff[i].pos = (mag * JOY_FULL_SCALE) / VALUE_SPAN;
		} else
			buff[i].pos = 0;
	}
}

void joy_getValue(uint8_t axis, joypos_t* buff)
{
	//Copy the ADC values into buff
	adcsample_t rawVal;
	chSysLock()
	rawVal = currentPos[axis];
	chSysUnlock();

	buff->dir = (rawVal > MIDPOINT);
	uint16_t mag = (uint16_t) abs(rawVal - MIDPOINT);
	if (mag > JOY_THRES)
	{
		mag -= JOY_THRES;

		//Scale from 0 to JOY_FULL_SCALE
		buff->pos = (mag * JOY_FULL_SCALE) / VALUE_SPAN;
	} else
		buff->pos = 0;
}
