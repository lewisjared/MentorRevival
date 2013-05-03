/*
 * motor.c
 *
 * Created: 25/04/2013 11:12:31 a.m.
 *  Author: Jared
 */ 
#include "ch.h"
#include "hal.h"

#include "motor.h"

/** PWM clock frequency in Hz */
#define PWM_CLOCK_FREQUENCY	10000

/** Desired PWM output frequency */
#define PWM_FREQUENCY 		100

/** Required clock counts per period*/
#define PWM_PERIOD			PWM_CLOCK_FREQUENCY / PWM_FREQUENCY

/**
 * Configuration for PWM channel 3
 * 4 channels
 */
static PWMConfig pwmcfg3 = {
	PWM_CLOCK_FREQUENCY,		/* 10kHz PWM clock frequency.   */
	PWM_PERIOD,					/* Initial PWM period 1S.       */
	NULL,
	{
		{PWM_OUTPUT_ACTIVE_HIGH, NULL}, //PB10
		{PWM_OUTPUT_ACTIVE_HIGH, NULL}, //PB11
		{PWM_OUTPUT_ACTIVE_HIGH, NULL}, //PB12
		{PWM_OUTPUT_ACTIVE_HIGH, NULL}, //PB13
	},
	0,
};

/*
* Configuration for PWM channel 4
* 2 Channels
*/
static PWMConfig pwmcfg4 = {
	PWM_CLOCK_FREQUENCY,
	PWM_PERIOD,
	NULL,
	{
		{PWM_OUTPUT_ACTIVE_HIGH, NULL}, //PD12
		{PWM_OUTPUT_ACTIVE_HIGH, NULL},//PD13
		{PWM_OUTPUT_DISABLED, NULL},
		{PWM_OUTPUT_DISABLED, NULL}
	},
	0,
};


typedef struct {
	GPIO_TypeDef *port;
	uint8_t pinA;
	uint8_t pinB;
	PWMDriver *pwmDriver;
 	pwmchannel_t pwmChannel;
 	GPIO_TypeDef *pwmPort;
	uint8_t pwmPin;
	uint32_t pwmFlags;
} Motor_struct;


static const Motor_struct motors[NUM_MOTOR] =
{
	{
		GPIOD,
		0,
		1,
		&PWMD4,
		0,
		GPIOD,
		12,
		PAL_MODE_ALTERNATE(1)
	}, {
		GPIOD,
		2,
		3,
		&PWMD4,
		1,
		GPIOD,
		13,
		PAL_MODE_ALTERNATE(1)
	}
};

void motor_init(void)
{
	pwmStart(&PWMD3, &pwmcfg3);
	pwmStart(&PWMD4, &pwmcfg4);

	palSetPadMode(GPIOD, 0, PAL_MODE_OUTPUT_OPENDRAIN);
	palSetPadMode(GPIOD, 1, PAL_MODE_OUTPUT_OPENDRAIN);

	//Configure PWM pins
	palSetPadMode(GPIOD, 12, PAL_MODE_ALTERNATE(2));
	palSetPadMode(GPIOD, 13, PAL_MODE_ALTERNATE(2));
	int i = 0;
	for (i = 0; i< NUM_MOTOR; i++)
	{
		//
		pwmcnt_t count = PWM_PERCENTAGE_TO_WIDTH(motors[i].pwmDriver, 0);
		pwmEnableChannel(motors[i].pwmDriver, motors[i].pwmChannel, count);
		palSetPad(motors[i].port,motors[i].pinA);
		palClearPad(motors[i].port,motors[i].pinB);
	}
}

void motor_setSpeed(int motorNum, uint16_t speed)
{
	if (motorNum < NUM_MOTOR && motorNum >= 0)
	{
		pwmcnt_t count = PWM_PERCENTAGE_TO_WIDTH(motors[motorNum].pwmDriver, speed);
		pwmEnableChannel(motors[motorNum].pwmDriver, motors[motorNum].pwmChannel, count);
	}
}

void motor_setDir(int motorNum, int dir)
{
	if (motorNum < NUM_MOTOR && motorNum >= 0)
	{
		if (dir)
		{
			palSetPad(motors[motorNum].port,motors[motorNum].pinA);
			palClearPad(motors[motorNum].port,motors[motorNum].pinB);
		} else {
			palClearPad(motors[motorNum].port,motors[motorNum].pinA);
			palSetPad(motors[motorNum].port,motors[motorNum].pinB);
		}
	}
}
