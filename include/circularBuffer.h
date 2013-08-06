/*
 * circularBuffer.h
 *
 *  Created on: May 17, 2013
 *      Author: Jared
 * 	This module implements a circular buffer.
 */

#ifndef CIRCULARBUFFER_H_
#define CIRCULARBUFFER_H_


#include <stdint.h>
#include <string.h>

typedef struct {
     void* buf;
     int head;
     int tail;
     int size;
     int itemSize;
} circ_t;


/**
 * Initialises a circular buffer
 * \param circ The circular buffer instance.
 * \param buf The allocated data buffer.
 * \param itemSize The size of an individual item in bytes.
 * \param numItems The number of items in the buffer.
 */
void circ_init(circ_t* circ, void* buf, unsigned int itemSize, int numItems)
{
	circ->buf = buf;
	circ->head = 0;
	circ->tail = 0;
	circ->itemSize = itemSize;
	circ->size = numItems;
}

/**
 * Reads an item from the buffer.
 * \param circ The circular buffer instance.
 * \param buf The buffer where the data will be written into.
 * \returns The number of items read. If this is 0 it indicates there is no data available.
 */
int circ_read(circ_t* circ, void * buf)
{
	uint8_t count = 0;
	if (circ->tail != circ->head)
	{
		memcpy(buf,(circ->buf + circ->tail*circ->itemSize), circ->itemSize);

		circ->tail++;

		//Check for wraparound
		if (circ->tail == circ->size)
			circ->tail = 0;
		count++;
	}

	return count;
}

/*
 * Writes an item to the buffer.
 * \param circ The circular buffer instance.
 * \param buf The buffer where the data will be read from.
 *
 * \returns The number of items written. Should be 1
 */
int circ_write(circ_t* circ, const void * buf)
{
	//If there is no room increase the tail. Overwriting the oldest data
	if ((circ->head + 1 == circ->tail) ||
			((circ->head + 1 == circ->size) && (circ->tail == 0))) {
		circ->tail++;
		//Check for wraparound
		if (circ->tail == circ->size)
			circ->tail = 0;
	}
	memcpy((circ->buf + circ->itemSize*circ->head),buf, circ->itemSize);

	//Increment head
	circ->head++;
	//Check for wraparound
	if (circ->head == circ->size)
		circ->head = 0;

	return 1;
}

#endif /* CIRCULARBUFFER_H_ */
