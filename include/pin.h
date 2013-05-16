/*
 * pin.h
 *
 *  Created on: 17/05/2013
 *      Author: Jared
 *
 *  Defines a individual pin. encapusulates the Chibios definitions of pins
 */

#ifndef PIN_H_
#define PIN_H_

#include "hal.h"

typedef struct {
	GPIO_TypeDef* port; //Port definition. i.e. GPIOD
	uint8_t pin; //Pin number
} pin_t;


//Writes data to a single pin
#define setPin(pinStruct, val) \
	palWritePad(pinStruct.port, pinStruct.pin,val)

#define configPinOpenDrain(pinStruct) \
		palSetPadMode(pinStruct.port, pinStruct.pin, PAL_MODE_OUTPUT_OPENDRAIN);

#endif /* PIN_H_ */
