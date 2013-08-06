/*
 * events.c
 *
 *  Created on: May 17, 2013
 *      Author: Jared
 */

#include "hal.h"
#include "ch.h"


#include "motor.h"
#include "joystick.h"
#include "packet.h"
#include "circularBuffer.h"

#define PACKET_BUFFER_LEN 5

static packet_t s_packet;
static circ_t packetBuf;
static packet_t _packetData[PACKET_BUFFER_LEN];
static Semaphore packetSem;

/**********************************************************
 * UART interrupts
 *********************************************************/

/*
 * This callback is invoked when a transmission buffer has been completely
 * read by the driver.
 */
static void txend1(UARTDriver *uartp) {

  (void)uartp;
}

/*
 * This callback is invoked when a transmission has physically completed.
 */
static void txend2(UARTDriver *uartp) {

  (void)uartp;
}

/*
 * This callback is invoked on a receive error, the errors mask is passed
 * as parameter.
 */
static void rxerr(UARTDriver *uartp, uartflags_t e) {

  (void)uartp;
  (void)e;
}

/*
 * This callback is invoked when a character is received but the application
 * was not ready to receive it, the character is passed as parameter.
 */
static void rxchar(UARTDriver *uartp, uint16_t c)
{
	//Append the char to the current packet.
	pkt_appendByte(&s_packet, c);

	//Check if the packet is full to pass to the application
	if (pkt_isComplete(&s_packet))
	{
		//Queue up for application
		circ_write(&packetBuf, &s_packet);

		//Signal the semaphore that there is more data
		chSysLockFromIsr();
		chSemSignalI(&packetSem);
		chSysUnlockFromIsr();

		//Clear the current packet
		pkt_init(&s_packet);
	}
}

/*
 * This callback is invoked when a receive buffer has been completely written.
 */
static void rxend(UARTDriver *uartp) {

  (void)uartp;
}

/*
 * UART driver configuration structure.
 */
static UARTConfig uart_cfg_1 = {
  txend1,
  txend2,
  rxend,
  rxchar,
  rxerr,
  115200,
  0,
  USART_CR2_LINEN,
  0
};


/********************************************************************
 * Event Functions
 ********************************************************************/
void evt_halt(packet_t packet, packet_t* reply);
void evt_readCurrent(packet_t packet, packet_t* reply);
void evt_readPos(packet_t packet, packet_t* reply);
void evt_setMode(packet_t packet, packet_t* reply);
void evt_setDuty(packet_t packet, packet_t* reply);
void evt_badCommand(packet_t packet, packet_t* reply);

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

		packet_t reply;
		pkt_init(&reply);
		reply.command = currentPacket.command;
		reply.targetAxis = currentPacket.command;

		switch(currentPacket.command)
		{
		case CMD_HALT:
			evt_halt(currentPacket,&reply);
			break;
		case CMD_READ_CURRENT:
			evt_readCurrent(currentPacket,&reply);
			break;
		case CMD_READ_POS:
			evt_readPos(currentPacket,&reply);
			break;
		case CMD_SET_DUTY:
			evt_setDuty(currentPacket,&reply);
			break;
		case CMD_SET_MODE:
			evt_setMode(currentPacket,&reply);
			break;
		default:
			evt_badCommand(currentPacket,&reply);
			break;
		}

		//Buffer output
		evt_sendPacket(reply);
	}
}

void evt_init(void)
{
	// initialises the temp packet
	pkt_init(&s_packet);

	//initialse the packet buffer
	circ_init(&packetBuf,_packetData,sizeof(packet_t), PACKET_BUFFER_LEN);

	/*
	 * Activates the UART driver 2, PD5(TX) and PD6(RX) are routed to USART2.
	 */
	uartStart(&UARTD2, &uart_cfg_1);
	palSetPadMode(GPIOD, 5, PAL_MODE_ALTERNATE(7));
	palSetPadMode(GPIOD, 6, PAL_MODE_ALTERNATE(7));

	// Create Event Thread. This thread is a basic state machine
	chThdCreateStatic(waEvtThread, sizeof(waEvtThread), NORMALPRIO, eventThread, NULL);

	//Now the state machine operates in its own thread while main thread continues
}


/******************************
 * Event Definitions
 *****************************/
void evt_halt(packet_t packet, packet_t* reply)
{
	int i = 0;
	//Set all duty cycles to 0% and apply breaks
	for (i = 0; i < 6; i++)
	{
		motor_setSpeed(packet.targetAxis,0);
		motor_setBrakes(packet.targetAxis, 1);
	}
}

void evt_readCurrent(packet_t packet, packet_t* reply)
{

}

void evt_readPos(packet_t packet, packet_t* reply)
{
	joypos_t buff;
	joy_getValue(packet.targetAxis, &buff);

	reply->data = buff.pos;
}

void evt_setMode(packet_t packet, packet_t* reply)
{
	motor_setBrakes(packet.targetAxis, packet.data);
}

void evt_setDuty(packet_t packet, packet_t* reply)
{
	motor_setSpeed(packet.targetAxis,packet.data);
}

void evt_badCommand(packet_t packet, packet_t* reply)
{

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
	uartStartSend(&UARTD2, 4, buf);
}
