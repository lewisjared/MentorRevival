/*
 * adcThread.c
 *
 *  Created on: 22/04/2013
 *      Author: jared
 */

#include "hal.h"
#include "ch.h"

#include "adcThread.h"

#define ADC_BUFF_DEPTH 4


adcsample_t currentPos[ADC_NUM_CHANNELS];
//Raw samples from ADC
static adcsample_t rawSamples[ADC_NUM_CHANNELS * ADC_BUFF_DEPTH];


/*
 * ADC streaming callback.
 */
static void adccallback(ADCDriver *adcp, adcsample_t *buffer, size_t n ) {

  (void)adcp;

  //Average the data and put in CurrentPos
  int i,j;
  for (i = 0; i < ADC_NUM_CHANNELS; i++)
  {
	  uint32_t sum = 0;
	  for (j = 0; j < ADC_BUFF_DEPTH; j++)
		  sum += rawSamples[j*ADC_NUM_CHANNELS + i];
	  uint32_t res = sum / ADC_BUFF_DEPTH;

	  currentPos[i] = (adcsample_t) res;
  }
}

static void adcerrorcallback(ADCDriver *adcp, adcerror_t err) {

  (void)adcp;
  (void)err;
}

/**
 * Configuration for ADC
 * AIN1, AIN2, AIN3,AIN4
 */
static const ADCConversionGroup adcgrpcfg = {
  TRUE,
  ADC_NUM_CHANNELS,
  adccallback,
  adcerrorcallback,
  0,                        /* CR1 */
  ADC_CR2_SWSTART,          /* CR2 */
  0,
  ADC_SMPR2_SMP_AN0(ADC_SAMPLE_55P5) | ADC_SMPR2_SMP_AN1(ADC_SAMPLE_55P5) |
  ADC_SMPR2_SMP_AN2(ADC_SAMPLE_55P5) | ADC_SMPR2_SMP_AN3(ADC_SAMPLE_55P5),                        /* SMPR2 */
  ADC_SQR1_NUM_CH(ADC_NUM_CHANNELS), //SQR1
  0, // SQR2
  ADC_SQR3_SQ4_N(ADC_CHANNEL_IN3)   | ADC_SQR3_SQ3_N(ADC_CHANNEL_IN2) |
  ADC_SQR3_SQ2_N(ADC_CHANNEL_IN1)   | ADC_SQR3_SQ1_N(ADC_CHANNEL_IN0)
};


void ADC_init(void)
{

	//Set A1,A2,A3,A4 as analog inputs
	palSetGroupMode(GPIOA, PAL_PORT_BIT(0) | PAL_PORT_BIT(1) | PAL_PORT_BIT(2) | PAL_PORT_BIT(3),
			0,PAL_MODE_INPUT_ANALOG);

	//Start the ADC1
	adcStart(&ADCD1, NULL);

	//start the conversion
	adcStartConversion(&ADCD1, &adcgrpcfg, rawSamples, ADC_BUFF_DEPTH);

}
