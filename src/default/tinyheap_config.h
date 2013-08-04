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
 * tinyheap_config.h
 * Copy and reconfigure this file to suit your own project.
 *
 *  Created on: Oct 6, 2012
 *      Author: petera
 */

#ifndef TINYHEAP_CONFIG_H_
#define TINYHEAP_CONFIG_H_


/* number of bytes in heap that can be addressed:
   TH_BLOCKSIZE * (2^(TH_ADDRESSING_BITS-1) - 1) */
/* number of address bits in each block header */
#ifndef TH_ADDRESSING_BITS
#define TH_ADDRESSING_BITS              8
#endif

/* the minimum heap entity in bytes */
#ifndef TH_BLOCKSIZE
#define TH_BLOCKSIZE                    32
#endif

/* keep track of free number of bytes in heap */
#ifndef TH_CALC_FREE
#define TH_CALC_FREE                    1
#endif

/* safety check heap boundary during free block scan */
#ifndef TH_CHECK_BOUNDARY
#define TH_CHECK_BOUNDARY               1
#endif

/* safety check if freeing a freed block */
#ifndef TH_CHECK_FREE_OF_FREE
#define TH_CHECK_FREE_OF_FREE           1
#endif

/* calculate a parity bit in each block header, check for corruption,
   pretty bad algo, finds only 50% of overwrites */
#ifndef TH_USE_PARITY
#define TH_USE_PARITY                   0
#endif

/* when to check for parity */
#ifndef TH_PARITY_CHCEK_LEVEL
#define TH_PARITY_CHECK_LEVEL           TH_PARITY_CHECK_ALL
#endif
/* check parity when freeing a block */
#define TH_PARITY_CHECK_DURING_FREE     1
/* check parity when mallocing and scanning for free blocks */
#define TH_PARITY_CHECK_DURING_MALLOC   2
/* check parity whenever possible */
#define TH_PARITY_CHECK_ALL             3

/* keep track of number of block search during free scan */
#ifndef TH_COUNT_SEARCH_CYCLES
#define TH_COUNT_SEARCH_CYCLES          0
#endif

/* Free block tracking strategies */

/* simply starts looking for free blocks from start of heap */
#ifndef TH_SEEK_STRAT_NONE
#define TH_SEEK_STRAT_NONE              0
#endif

/* keeps note of last seen free block, small overhead in code */
#ifndef TH_SEEK_STRAT_LAST_FREE
#define TH_SEEK_STRAT_LAST_FREE         0
#endif

/* keeps pointers to min and max free blocks seen, split by
   TH_BLOCK_MEDIAN, somewhat bigger overhead */
#ifndef TH_SEEK_STRAT_MIN_MAX
#define TH_SEEK_STRAT_MIN_MAX           1
#endif
#if TH_SEEK_STRAT_MIN_MAX
/* smaller or equal requests indicates small block size, bigger
   requests big block size */
#define TH_BLOCK_MEDIAN                 3
#endif

#if TH_SEEK_STRAT_NONE && (TH_SEEK_STRAT_LAST_FREE || TH_SEEK_STRAT_MIN_MAX)
#error "only one tracking strategy can be enabled"
#endif

#if TH_SEEK_STRAT_LAST_FREE && (TH_SEEK_STRAT_NONE || TH_SEEK_STRAT_MIN_MAX)
#error "only one tracking strategy can be enabled"
#endif

#if TH_SEEK_STRAT_MIN_MAX && (TH_SEEK_STRAT_LAST_FREE || TH_SEEK_STRAT_NONE)
#error "only one tracking strategy can be enabled"
#endif

/* Free block picking strategies */

/* pick first free fitting block found */
#ifndef TH_PICK_STRAT_FIRST
#define TH_PICK_STRAT_FIRST             0
#endif

/* pick the block that fits exactly or with minimum delta */
#ifndef TH_PICK_STRAT_BEST_FIT
#define TH_PICK_STRAT_BEST_FIT          0
#endif

/* pick block that has a delta less than specified in
   TH_GOOD_ENOUGH_DELTA */
#ifndef TH_PICK_STRAT_GOOD_ENOUGH_FIT
#define TH_PICK_STRAT_GOOD_ENOUGH_FIT   1
#endif
#if TH_PICK_STRAT_GOOD_ENOUGH_FIT
/* if nbr of delta blocks is equal or less, the free block
   is taken (0 means the same as BEST_FIT) */
#define TH_GOOD_ENOUGH_DELTA            1
#endif

#if TH_PICK_STRAT_FIRST && (TH_PICK_STRAT_BEST_FIT || TH_PICK_STRAT_GOOD_ENOUGH_FIT)
#error "only one picking strategy can be enabled"
#endif

#if TH_PICK_STRAT_BEST_FIT && (TH_PICK_STRAT_FIRST || TH_PICK_STRAT_GOOD_ENOUGH_FIT)
#error "only one picking strategy can be enabled"
#endif

#if TH_PICK_STRAT_GOOD_ENOUGH_FIT && (TH_PICK_STRAT_FIRST || TH_PICK_STRAT_BEST_FIT)
#error "only one picking strategy can be enabled"
#endif

/* in multithreading environments locking can be implemented here */
#ifndef TH_LOCK
#define TH_LOCK(heapptr)
#endif
/* in multithreading environments unlocking can be implemented here */
#ifndef TH_UNLOCK
#define TH_UNLOCK(heapptr)
#endif

/* called when a check fails, heap is corrupted */
#ifndef TH_ASSERT
#define TH_ASSERT(mustBeTrue) if (!(mustBeTrue)) TH_PRINTF("assert");
#endif

/* called when memory is freed */
#ifndef TH_ON_FREED
#define TH_ON_FREED(addr, len) //memset(addr, 0xff, len)
#endif
/* called when memory is allocated */
#ifndef TH_ON_MALLOCED
#define TH_ON_MALLOCED(addr, len) //memset(addr, 0xaa, len)
#endif
/* called for the remaining free part when free memory is allocated  */
#ifndef TH_ON_SPLITFREED
#define TH_ON_SPLITFREED(addr, len) //memset(addr, 0x55, len)
#endif

/* address alignment for returned pointers to heap */
#ifndef TH_ALIGNMENT
/* align size, default to size of a pointer on target platform */
#define TH_ADDRESS_ALIGN  sizeof(void *)
#endif

#define TH_ALIGNMENT (TH_ADDRESS_ALIGN - (sizeof(th_block_h)&(TH_ADDRESS_ALIGN-1)))

/* debug */
#ifndef TH_DUMP
#define TH_DUMP             1
#endif

/* debug */
#ifndef TH_PRINTF
#define TH_PRINTF(...)
#endif


#endif /* TINYHEAP_CONFIG_H_ */
