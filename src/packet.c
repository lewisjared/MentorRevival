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

void initPacket(packet_t* packet)
{
	//Zeros the struct
	memset(packet,0,sizeof(packet_t));
}

packet_t createPacket(command_t command, uint8_t target, uint16_t data)
{
	packet_t packet;
	packet.command = command;
	packet.targetAxis = target;
	packet.data = data;
	packet._numBytes = 4;

	return packet;
}

void appendByte(packet_t* packet, uint8_t byte)
{
	//Check if the packet is already full
	if (isComplete(packet))
		return;

	switch(packet->_numBytes)
	{
	case 0:
		packet->command = byte >> 4;
		packet->targetAxis = 0xFF & byte;
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

bool isComplete(packet_t* packet)
{
	return (packet->_numBytes >= 4);
}
