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
} Motor_struct;


static const Motor_struct motors[NUM_MOTOR] =
{
	{ //Motor 1
		GPIOD, 	//Pin Port
		0,		//PinA
		1,		//PinB
		&PWMD4, //PWM Driver
		0,		//PWM Channel
		GPIOD,	//PWM Pin Port
		12		//PWM Pin #
	}, { //Motor 2
		GPIOD, 	//Pin Port
		2,		//PinA
		3,		//PinB
		&PWMD4, //PWM Driver
		1,		//PWM Channel
		GPIOD,	//PWM Pin Port
		13		//PWM Pin #
	}, { //Motor 3
		GPIOE, 	//Pin Port
		0,		//PinA
		1,		//PinB
		&PWMD3, //PWM Driver
		0,		//PWM Channel
		GPIOC,	//PWM Pin Port
		6		//PWM Pin #
	}, 	{ //Motor 4
		GPIOE, 	//Pin Port
		7,		//PinA
		8,		//PinB
		&PWMD3, //PWM Driver
		1,		//PWM Channel
		GPIOC,	//PWM Pin Port
		7		//PWM Pin #
	}, { //Motor 5
		GPIOE, 	//Pin Port
		9,		//PinA
		10,		//PinB
		&PWMD3, //PWM Driver
		2,		//PWM Channel
		GPIOC,	//PWM Pin Port
		8		//PWM Pin #
	}, { //Motor 6
		GPIOE, 	//Pin Port
		11,		//PinA
		12,		//PinB
		&PWMD3, //PWM Driver
		3,		//PWM Channel
		GPIOC,	//PWM Pin Port
		9		//PWM Pin #
	}
};

void motor_init(void)
{
	pwmStart(&PWMD3, &pwmcfg3);
	pwmStart(&PWMD4, &pwmcfg4);

	//Pin Setup
	//M1
	palSetPadMode(GPIOD, 0, PAL_MODE_OUTPUT_OPENDRAIN);
	palSetPadMode(GPIOD, 1, PAL_MODE_OUTPUT_OPENDRAIN);
	//M2
	palSetPadMode(GPIOD, 2, PAL_MODE_OUTPUT_OPENDRAIN);
	palSetPadMode(GPIOD, 3, PAL_MODE_OUTPUT_OPENDRAIN);
	//M3
	palSetPadMode(GPIOE, 0, PAL_MODE_OUTPUT_OPENDRAIN);
	palSetPadMode(GPIOE, 1, PAL_MODE_OUTPUT_OPENDRAIN);
	//M4
	palSetPadMode(GPIOE, 7, PAL_MODE_OUTPUT_OPENDRAIN);
	palSetPadMode(GPIOE, 8, PAL_MODE_OUTPUT_OPENDRAIN);
	//M5
	palSetPadMode(GPIOE, 9, PAL_MODE_OUTPUT_OPENDRAIN);
	palSetPadMode(GPIOE, 10, PAL_MODE_OUTPUT_OPENDRAIN);
	//M6
	palSetPadMode(GPIOE, 11, PAL_MODE_OUTPUT_OPENDRAIN);
	palSetPadMode(GPIOE, 12, PAL_MODE_OUTPUT_OPENDRAIN);

	//Configure PWM pins
	palSetPadMode(GPIOD, 12, PAL_MODE_ALTERNATE(2)); //M1
	palSetPadMode(GPIOD, 13, PAL_MODE_ALTERNATE(2)); //M2
	palSetPadMode(GPIOC, 6, PAL_MODE_ALTERNATE(2)); //M3
	palSetPadMode(GPIOC, 7, PAL_MODE_ALTERNATE(2)); //M4
	palSetPadMode(GPIOC, 8, PAL_MODE_ALTERNATE(2)); //M5
	palSetPadMode(GPIOC, 9, PAL_MODE_ALTERNATE(2)); //M6

	int i = 0;
	for (i = 0; i< NUM_MOTOR; i++)
	{
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
