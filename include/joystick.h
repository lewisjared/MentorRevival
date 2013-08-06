/*
 * joystick.h
 *
 *  Created on: 3/05/2013
 *      Author: Jared
 *  Declares the Joystick modules.
 *
 *  This module interfaces with a PS2 joystick to provide 4 axes of movement.
 */

#ifndef JOYSTICK_H_
#define JOYSTICK_H_

#include <stdint.h>

typedef struct {
	uint8_t dir; //Direction of joystick
	uint16_t pos; //Position of joystick. Value in range 0 to JOY_FULL_SCALE
} joypos_t;

// Number of Joysticks used
#define NUM_JOYSTICKS 4

// Sets the width of the null zone where movement results in no output.
#define JOY_THRES 400

// Full scale of output position
#define JOY_FULL_SCALE 10000

/**
 * Initialises the joystick module and begins reading the joystick positions
 */
void joy_init(void);

/**
 * Returns the joystick position for each axis.
 * The returned values are ready to be used as a PWM percentage.
 *
 * \param buff Buffer to hold positions. Must be at least NUM_JOYSTICKS elements long.
 */
void joy_getValues(joypos_t* buff);

/**
 * Returns the joystick position for a single axis
 * The returned values are ready to be used as a PWM percentage.
 *
 * \param axis Axis to sample.
 * \param buff Buffer to hold position.
 */
void joy_getValue(uint8_t axis, joypos_t* buff);

#endif /* JOYSTICK_H_ */
