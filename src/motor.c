/*
 * motor.c
 *
 * Created: 25/04/2013 11:12:31 a.m.
 *  Author: Jared
 */ 
#include "ch.h"
#include "hal.h"

#include "pin.h"
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
	pin_t phase;
	pin_t enable;
	pin_t mode1;
	PWMDriver *pwmDriver;
 	pwmchannel_t pwmChannel;
} Motor_struct;


static const Motor_struct motors[NUM_MOTOR] =
{
	{ //Motor 1
		{GPIOB,0},	//Phase
		{GPIOC,8},	//Enable
		{GPIOD,2},	//Mode1
		&PWMD3, //PWM Driver
		2,		//PWM Channel
	}
};

void motor_init(void)
{
	pwmStart(&PWMD3, &pwmcfg3);
	//pwmStart(&PWMD4, &pwmcfg4);

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
		configPinOpenDrain(motors[i].mode1);
		configPinOpenDrain(motors[i].phase);
		pwmcnt_t count = PWM_PERCENTAGE_TO_WIDTH(motors[i].pwmDriver, 0);
		pwmEnableChannel(motors[i].pwmDriver, motors[i].pwmChannel, count);
		setPin(motors[i].phase, PAL_HIGH);
	}
}


//Speed control is by appplying a pwm signal to the enable pin
void motor_setSpeed(int motorNum, uint16_t speed)
{
	if (motorNum < NUM_MOTOR && motorNum >= 0)
	{
		pwmcnt_t count = PWM_PERCENTAGE_TO_WIDTH(motors[motorNum].pwmDriver, speed);
		pwmEnableChannel(motors[motorNum].pwmDriver, motors[motorNum].pwmChannel, count);
	}
}

void motor_setDir(int motorNum, bool dir)
{
	if (motorNum < NUM_MOTOR && motorNum >= 0)
	{
		setPin(motors[motorNum].phase, dir);
	}
}

//Brakes are applied by setting mode1 high
// Note that the enable pin must also be low.
void motor_setBrakes(int motorNum, bool brake)
{
	if (motorNum < NUM_MOTOR && motorNum >= 0)
	{
		setPin(motors[motorNum].mode1, brake);
	}
}
