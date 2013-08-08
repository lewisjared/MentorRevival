/*
 * events.c
 *
 *  Created on: May 17, 2013
 *      Author: Jared
 */

#include "hal.h"
#include "ch.h"

#include "common.h"
#include "motor.h"
#include "packet.h"
#include "circularBuffer.h"
#include "usbSerial.h"
#include "adcThread.h"

#define PACKET_BUFFER_LEN 5

static packet_t s_packet;
static circ_t packetBuf;
static packet_t _packetData[PACKET_BUFFER_LEN];
static Semaphore packetSem;

static WORKING_AREA(waSerialThread,100);
static msg_t serialThread(void *arg)
{
	UNUSED(arg);
	bool start = true;

	while(TRUE)
	{
		//Blocking call to get next char
		char val = chSequentialStreamGet(&SDU1);

		if (start)
		{
			if (val != 254)
			{
				start = false;
				pkt_appendByte(&s_packet, val);
			}
		} else {
			//Append the char to the current packet.
			pkt_appendByte(&s_packet, val);

			//Check if the packet is full to pass to the application
			if (pkt_isComplete(&s_packet))
			{
				//Queue up for application
				circ_write(&packetBuf, &s_packet);

				//Signal the semaphore that there is more data
				chSemSignal(&packetSem);

				//Clear the current packet
				pkt_init(&s_packet);
			}
		}
	}
	return -1;
}


/********************************************************************
 * Event Functions
 ********************************************************************/
void evt_halt(packet_t* packet);
void evt_readCurrent(packet_t* packet);
void evt_readPos(packet_t* packet);
void evt_setMode(packet_t* packet);
void evt_setDuty(packet_t* packet);
void evt_badCommand(packet_t* packet);

void evt_sendPacket(packet_t packet);

static WORKING_AREA(waEvtThread,256);
static msg_t eventThread(void *arg)
{
	(void)arg;
	chRegSetThreadName("eventThread");

	chSemInit(&packetSem,0);

	packet_t currentPacket;

	while (TRUE) {
		//Wait for command
		chSemWait(&packetSem);

		//Extract command
		circ_read(&packetBuf, &currentPacket);

		switch(currentPacket.command)
		{
		case CMD_HALT:
			evt_halt(&currentPacket);
			break;
		case CMD_READ_CURRENT:
			evt_readCurrent(&currentPacket);
			break;
		case CMD_READ_POS:
			evt_readPos(&currentPacket);
			break;
		case CMD_SET_DUTY:
			evt_setDuty(&currentPacket);
			break;
		case CMD_SET_MODE:
			evt_setMode(&currentPacket);
			break;
		default:
			evt_badCommand(&currentPacket);
			break;
		}

		//Buffer output
		//For now just repeat the command back to the host
		evt_sendPacket(currentPacket);
	}
	return -1;
}

void evt_init(void)
{
	// initialises the temp packet
	pkt_init(&s_packet);

	/*
	* Activates the USB driver and then the USB bus pull-up on D+.
	* Note, a delay is inserted in order to not have to disconnect the cable
	* after a reset.
	*/
	palSetPadMode(GPIOA, 12, PAL_MODE_OUTPUT_PUSHPULL);
	palClearPad(GPIOA, 12);
	chThdSleepMilliseconds(1500);
	usbStart(serusbcfg.usbp, &usbcfg);
	palSetPad(GPIOA, 12);
	palSetPadMode(GPIOA, 12, PAL_MODE_ALTERNATE(14));

	/*
	* Initializes a serial-over-USB CDC driver.
	*/
	sduObjectInit(&SDU1);
	sduStart(&SDU1, &serusbcfg);

	//initialise the packet buffer
	circ_init(&packetBuf,_packetData,sizeof(packet_t), PACKET_BUFFER_LEN);

	// Create Event Thread. This thread is a basic state machine
	chThdCreateStatic(waEvtThread, sizeof(waEvtThread), NORMALPRIO, eventThread, NULL);

	//Start reading data of the USB Serial
	chThdCreateStatic(waSerialThread, sizeof(waSerialThread), NORMALPRIO, serialThread, NULL);

	//Now the state machine operates in its own thread while main thread continues
}


/******************************
 * Event Definitions
 *****************************/
void evt_halt(packet_t* packet)
{
	UNUSED(packet);
	int i = 0;
	//Set all duty cycles to 0% and apply breaks
	for (i = 0; i < 6; i++)
	{
		motor_setSpeed(i,0);
		motor_setBrakes(i, 1);
	}
}

void evt_readCurrent(packet_t* packet)
{
	UNUSED(packet);
}

void evt_readPos(packet_t* packet)
{
	chSysLock();
	packet->data = currentPos[packet->targetAxis];
	chSysUnlock();
}

void evt_setMode(packet_t* packet)
{
	motor_setBrakes(packet->targetAxis, packet->data);
}

void evt_setDuty(packet_t* packet)
{
	int16_t speed = packet->data;
	speed -= 10000;
	motor_setSpeed(packet->targetAxis,speed);
}

void evt_badCommand(packet_t* packet)
{
	UNUSED(packet);
	//Do Nothing
}

/**
 * Sends a packet to the host
 */
void evt_sendPacket(packet_t packet)
{
	//Get the encoded version
	char buf[4];
	pkt_encode(&packet, buf);

	//add to UART buffer
	chSequentialStreamWrite(&SDU1, (const uint8_t*)buf, 4);
}
