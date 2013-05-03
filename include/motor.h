/*
 * motor.h
 *
 * Created: 25/04/2013 11:12:40 a.m.
 *  Author: Jared
 *
 * This file declares the public interfaces for accessing the motors
 */ 


#ifndef MOTOR_H_
#define MOTOR_H_


#define NUM_MOTOR 2


/**
 * Initialises the motors ready to be used. Must be called before setting speed.
 */
void motor_init(void);

/**
 * Sets the direction of travel for a given motor
 * \param motorNum Motor Number to alter. In range [0:NUM_MOTORS-1]
 * \param direction Direction of travel. Direction > 0 moves in one direction <= 0  moves in the other
 */
void motor_setDir(int motorNum, int direction);

/**
 * Sets the speed of a given motor
 * \param motorNum Motor Number to alter. In range [0:NUM_MOTORS-1]
 * \param speed Sets the speed of the motor. [0..1024] maps to 0 to 100%
 */
void motor_setSpeed(int motorNum, uint16_t speed);


#endif /* MOTOR_H_ */
