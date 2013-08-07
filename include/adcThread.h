/*
 * adcThread.h
 *
 * This file details the public interface of the ADC Thread of the MentorRevival project. This thread is used to
 * measure the position of the various axis of the arm.
 *
 */

#ifndef ADCTHREAD_H_
#define ADCTHREAD_H_

#include "hal.h"
#include "ch.h"

#define ADC_NUM_CHANNELS 6

extern adcsample_t currentPos[ADC_NUM_CHANNELS];

/**
 * Initialises the ADC and starts the thread
 */
void ADC_init(void);




#endif /* ADCTHREAD_H_ */
