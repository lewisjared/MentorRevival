/*
 * packet.h
 *
 *  Created on: May 17, 2013
 *      Author: Jared
 *
 *  This module implements a single packet that is communicated between the host and the micro.
 *  This definition should be the same on the host computer.
 *
 *  A packet is defined as 4 bytes. The first byte is used to identify the command and the axis id.
 *  There are 2 bytes for data therefore a packet can contain a single 16 bit number. The final byte
 *  contains CRC information and is currently unused.
 */

#ifndef PACKET_H_
#define PACKET_H_

#include <stdint.h>
#include <stdbool.h>

/**
 * Define all the possible commands.
 * This struct should be the same on matlab
 */
typedef enum {
	CMD_HALT,
	CMD_READ_CURRENT,
	CMD_READ_POS,
	CMD_SET_DUTY,
	CMD_SET_MODE
} command_t;


typedef struct {
	command_t command; 	// The parsed command
	uint8_t targetAxis; //The targeted axis
	uint16_t data;		//The data

	//Below are the private members of this packet. Please don't touch
	uint8_t _numBytes;
} packet_t;

/**
 * Initialises an empty packet.
 *
 * This function should be used to reset the packet before changing the data it contains
 */
void initPacket(packet_t* packet);

/**
 * Creates a new packet
 *
 * \param command The command to be sent
 * \param target The target axis. This should be [0..5] for the case of the mentor.
 * \param data The data the the packet is to contain
 */
packet_t createPacket(command_t command, uint8_t target, uint16_t data);


/**
 * Appends a byte to an existing packet.
 * This function does nothing if the packet is already full. The packet fills the command in the following order.:
 * command byte
 * upper byte of data
 * lower byte of data
 * error correction byte
 *
 * \param packet The packet to append to.
 * \param byte The char of data to append to the packet.
 */
void appendByte(packet_t* packet, uint8_t byte);

/**
 * Checks if the packet is full
 *
 * \returns True if the packet is full.
 */
bool isComplete(packet_t*);





#endif /* PACKET_H_ */
