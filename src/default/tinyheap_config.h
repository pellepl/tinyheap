/*
 * tinyheap_config.h
 *
 *  Created on: Oct 6, 2012
 *      Author: petera
 */

#ifndef TINYHEAP_CONFIG_H_
#define TINYHEAP_CONFIG_H_


/* number of bytes in heap that can be addressed:
   TH_BLOCKSIZE * (2^(TH_ADDRESSING_BITS-1) - 1) */
/* number of address bits in each block header */
#define TH_ADDRESSING_BITS              8
/* the size of a block in bytes */
#define TH_BLOCKSIZE                    32

/* keep track of free number of bytes in heap */
#define TH_CALC_FREE                    1
/* safety check heap boundary during free block scan */
#define TH_CHECK_BOUNDARY               1
/* safety check if freeing a freed block */
#define TH_CHECK_FREE_OF_FREE           1
/* calculate a parity bit in each block header */
#define TH_USE_PARITY                   1
/* when to check for parity */
#define TH_PARITY_CHECK_LEVEL           TH_PARITY_CHECK_ALL

/* check parity when freeing a block */
#define TH_PARITY_CHECK_DURING_FREE     1
/* check parity when mallocing and scanning for free blocks */
#define TH_PARITY_CHECK_DURING_MALLOC   2
/* check parity whenever possible */
#define TH_PARITY_CHECK_ALL             3

/* keep track of number of block search during free scan */
#define TH_COUNT_SEARCH_CYCLES          0

/* Free block tracking strategies */

/* simply starts looking for free blocks from start of heap */
#define TH_SEEK_STRAT_NONE              0
/* keeps note of last seen free block, small overhead in code */
#define TH_SEEK_STRAT_LAST_FREE         0
/* keeps pointers to min and max free blocks seen, split by
   TH_BLOCK_MEDIAN, somewhat bigger overhead */
#define TH_SEEK_STRAT_MIN_MAX           1
#if TH_SEEK_STRAT_MIN_MAX
/* smaller or equal requests indicates small block size, bigger
   requests big block size */
#define TH_BLOCK_MEDIAN                 3
#endif

/* Free block picking strategies */

/* pick first free fitting block found */
#define TH_PICK_STRAT_FIRST             0
/* pick the block that fits exactly or with minimum delta */
#define TH_PICK_STRAT_BEST_FIT          0
/* pick block that has a delta less than specified in
   TH_GOOD_ENOUGH_DELTA */
#define TH_PICK_STRAT_GOOD_ENOUGH_FIT   1
#if TH_PICK_STRAT_GOOD_ENOUGH_FIT
/* if nbr of delta blocks is equal or less, the free block
   is taken (0 means the same as BEST_FIT) */
#define TH_GOOD_ENOUGH_DELTA            1
#endif

/* in multithreading environments locking can be implemented here */
#define TH_LOCK(heapptr)
/* in multithreading environments unlocking can be implemented here */
#define TH_UNLOCK(heapptr)

/* called when a check fails, heap is corrupted */
#define TH_ASSERT(mustBeTrue)
/* called when memory is freed */
#define TH_ON_FREED(addr, len) //memset(addr, 0xff, len)
/* called when memory is allocated */
#define TH_ON_MALLOCED(addr, len) //memset(addr, 0xaa, len)
/* called for the remaining free part when free memory is allocated  */
#define TH_ON_SPLITFREED(addr, len) //memset(addr, 0x55, len)

#ifndef TH_ALIGNMENT
#define TH_ALIGNMENT (sizeof(void *) - (sizeof(th_block_h)&(sizeof(void *)-1)))
#endif


#define TH_PRINTF(...)


#endif /* TINYHEAP_CONFIG_H_ */
