/*
 * packet.c
 *
 *  Created on: May 17, 2013
 *      Author: Jared
 *
 *  Implementation of the packet module.
 *  This module is used to send data between the host and the micro.
 */

#include "packet.h"

#include <string.h>

void pkt_init(packet_t* packet)
{
	//Zeros the struct
	memset(packet,0,sizeof(packet_t));
}

packet_t pkt_create(command_t command, uint8_t target, uint16_t data)
{
	packet_t packet;
	packet.command = command;
	packet.targetAxis = target;
	packet.data = data;
	packet._numBytes = 4;

	return packet;
}

void pkt_appendByte(packet_t* packet, uint8_t byte)
{
	//Check if the packet is already full
	if (pkt_isComplete(packet))
		return;

	switch(packet->_numBytes)
	{
	case 0:
		packet->command = byte >> 4;
		packet->targetAxis = 0xF & byte;
		break;
	case 1:
		packet->data = (uint16_t)byte << 8;
		break;
	case 2:
		packet->data = packet->data | (uint16_t) byte;
		break;
	case 3:
		//Do error check!!
		break;
	default:
		//Should not be in here!
		break;
	}
	packet->_numBytes++;
}

bool pkt_isComplete(packet_t* packet)
{
	return (packet->_numBytes >= 4);
}

void pkt_encode(const packet_t* packet, char* buff)
{
	buff[0] = (packet->command << 4) | (packet->targetAxis);
	//Upper byte of data
	buff[1] = (uint8_t) (packet->data >> 8);
	//Lower byte of data
	buff[2] = (uint8_t) (packet->data & 0xFF);
	buff[3] = (uint8_t) (0xAE);
}
