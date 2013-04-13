/*
 * tinyheap.h
 *
 *  Created on: Apr 5, 2012
 *      Author: petera
 */

#ifndef TINYHEAP_H_
#define TINYHEAP_H_

#include "tinyheap_config.h"

/* heap block header */
typedef struct __attribute__((packed)) th_block_h {
  /* free flag */
	unsigned int free 	 : 1;
	/* pointer to next block header */
	unsigned int next 	 : TH_ADDRESSING_BITS-1;
  /* header parity bit */
	unsigned int parity  : 1;
  /* pointer to previous block header */
	unsigned int prev 	 : TH_ADDRESSING_BITS-1;
} th_block_h;

/* heap descriptor */
typedef struct tinyheap {
	th_block_h* p;
	int len;
#if TH_SEEK_STRAT_MIN_MAX
	th_block_h* smallFree;
	th_block_h* bigFree;
#endif
#if TH_SEEK_STRAT_LAST_FREE
  th_block_h* lastFree;
#endif
#if TH_CALC_FREE
  unsigned int free;
#endif
#if TH_COUNT_SEARCH_CYCLES
	unsigned int count;
#endif
} tinyheap;

/* initializes a heap of given size at given buffer address */
void th_init(tinyheap* heap, void* buffer, int len);
/* allocates memory in heap */
void* th_malloc(tinyheap* heap, unsigned int size);
/* frees memory in heap */
void th_free(tinyheap* heap, void* p);
#if TH_CALC_FREE
/* returns free amount of bytes in heap */
unsigned int th_freecount(tinyheap* heap);
#endif

void th_dump(tinyheap* heap);

#endif /* TINYHEAP_H_ */
