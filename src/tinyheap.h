//The MIT License (MIT)
//
//Copyright (c) 2012-2013 Peter Andersson (pelleplutt1976<at>gmail.com)
//
//Permission is hereby granted, free of charge, to any person obtaining a copy
//of this software and associated documentation files (the "Software"), to deal
//in the Software without restriction, including without limitation the rights
//to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//copies of the Software, and to permit persons to whom the Software is
//furnished to do so, subject to the following conditions:
//
//The above copyright notice and this permission notice shall be included in
//all copies or substantial portions of the Software.
//
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
//THE SOFTWARE.

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

#if TH_ADDRESS_ALIGN == 0
#define TH_ALIGNMENT (0)
#else
#define TH_ALIGNMENT (TH_ADDRESS_ALIGN - (sizeof(th_block_h)&(TH_ADDRESS_ALIGN-1)))
#endif

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

#if TH_DUMP
void th_dump(tinyheap* heap);
#endif

#endif /* TINYHEAP_H_ */
