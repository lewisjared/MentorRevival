/*
 * events.h
 *
 *  Created on: May 17, 2013
 *      Author: Jared
 *
 *  This file defines the events module. This module communicates via serial with a host computer. This computer
 *
 */

#ifndef EVENTS_H_
#define EVENTS_H_


/**
 * Initialises the event thread. This continues running after this call and
 * control of the UART.
 */
void evt_init(void);




#endif /* EVENTS_H_ */
