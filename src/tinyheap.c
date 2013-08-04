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
 * tinyheap.c
 *
 *  Created on: Apr 5, 2012
 *      Author: petera
 */

#include "tinyheap.h"

#define TH_ASSERT_PAR_MALLOC(__x) \
	if (TH_USE_PARITY && (TH_PARITY_CHECK_LEVEL >= TH_PARITY_CHECK_DURING_MALLOC)) \
		TH_ASSERT(th_checkParity(__x));

#define TH_ASSERT_PAR_FREE(__x) \
  if (TH_USE_PARITY && (TH_PARITY_CHECK_LEVEL >= TH_PARITY_CHECK_DURING_FREE)) \
    TH_ASSERT(th_checkParity(__x));

#define TH_ASSERT_FREE_FREED(__x) \
  if (TH_CHECK_FREE_OF_FREE) \
    TH_ASSERT(!(__x)->free);

#define TH_CALC_PARITY(__x) \
		if (TH_USE_PARITY) \
			th_calcParity(__x);

#if TH_USE_PARITY
static void th_calcParity(th_block_h* h) {
	h->parity = ((~h->free) ^ h->next ^ h->prev);
}

static int th_checkParity(th_block_h* h) {
	return h->parity == (((~h->free) ^ h->next ^ h->prev) & 1);
}
#endif

static th_block_h* th_prev(tinyheap* heap, th_block_h* block) {
	if (block->prev > 0) {
		return (th_block_h*)((char*)block - block->prev * TH_BLOCKSIZE);
	} else {
		// there was no first block
		return 0;
	}
}

static th_block_h* th_next(tinyheap* heap, th_block_h* block) {
	th_block_h* nblock = (th_block_h*)((void*)block + block->next * TH_BLOCKSIZE);
	if (((void*)nblock - (void*)heap->p) < heap->len * TH_BLOCKSIZE ) {
		return nblock;
	} else {
		// there was no next block
		return 0;
	}
}

void th_init(tinyheap* heap, void* buffer, int len) {
	heap->p = buffer;
	heap->len = len/TH_BLOCKSIZE;
#if TH_CALC_FREE
  heap->free = heap->len;
#endif
#if TH_SEEK_STRAT_MIN_MAX
  heap->smallFree = 0;
	heap->bigFree = 0;
#endif
#if TH_SEEK_STRAT_LAST_FREE
	heap->lastFree = 0;
#endif
	heap->p[0].free = 1;
	heap->p[0].next = len/TH_BLOCKSIZE;
	heap->p[0].prev = 0;
	TH_ON_FREED((void*)&heap->p[0] + sizeof(th_block_h), heap->len * TH_BLOCKSIZE - sizeof(th_block_h));
	TH_CALC_PARITY(&heap->p[0]);
}

static th_block_h* th_findFree(tinyheap* heap, unsigned int bsize, th_block_h* block) {
	th_block_h* bcand = block;
	TH_ASSERT_PAR_MALLOC(bcand);
#if TH_CHECK_BOUNDARY
	unsigned int testlen = 0;
#endif
#if TH_PICK_STRAT_BEST_FIT || TH_PICK_STRAT_GOOD_ENOUGH_FIT
	unsigned int minD = heap->len;
	th_block_h* minDBlock = 0;
#endif
	do {
#if TH_CHECK_BOUNDARY
		TH_ASSERT(bcand->next > 0);
#endif
		if (bcand->free && bcand->next >= bsize) {
			// found one
#if TH_PICK_STRAT_FIRST
			return bcand;
#endif
#if TH_PICK_STRAT_BEST_FIT || TH_PICK_STRAT_GOOD_ENOUGH_FIT
			unsigned int d = bcand->next - bsize;
#if TH_PICK_STRAT_BEST_FIT
		  if (d <= 0)
#endif
#if TH_PICK_STRAT_GOOD_ENOUGH_FIT
	    if (d <= TH_GOOD_ENOUGH_DELTA)
#endif
	    {
			  return bcand;
			} else if (d < minD) {
			  minD = d;
			  minDBlock = bcand;
			}
#endif
		}
#if TH_COUNT_SEARCH_CYCLES
		heap->count++;
#endif
		// next block
#if TH_CHECK_BOUNDARY
		testlen += bcand->next;
		TH_ASSERT(testlen <= heap->len);
#endif
		bcand = th_next(heap, bcand);
		if (bcand == 0) {
			// wrap
			bcand = heap->p;
		}
		TH_ASSERT_PAR_MALLOC(bcand);
	} while (bcand != block);
#if TH_PICK_STRAT_FIRST
      return 0;
#endif
#if TH_PICK_STRAT_BEST_FIT || TH_PICK_STRAT_GOOD_ENOUGH_FIT
      return minDBlock;
#endif
}

static void th_internalAlloc(tinyheap* heap, unsigned int bsize, th_block_h* block) {
	unsigned int fullblocksize = block->next;
	block->free = 0;
	block->next = bsize;
	TH_CALC_PARITY(block);
	th_block_h* nblock = th_next(heap, block);
#if TH_CALC_FREE
	heap->free -= bsize;
#endif
	TH_ON_MALLOCED((void*)block + sizeof(th_block_h), block->next * TH_BLOCKSIZE - sizeof(th_block_h));
	// split block if free is larger than requested
	if ((nblock - heap->p)/TH_BLOCKSIZE < heap->len && fullblocksize > bsize) {
		nblock->free = 1;
		nblock->next = fullblocksize - bsize;
		nblock->prev = bsize;
		TH_CALC_PARITY(nblock);
#if TH_SEEK_STRAT_MIN_MAX
		if (nblock->next <= TH_BLOCK_MEDIAN) {
			if (heap->smallFree == 0 || !heap->smallFree->free) {
				heap->smallFree = nblock;
			}
		} else {
			if (heap->bigFree == 0 || !heap->bigFree->free) {
				heap->bigFree = nblock;
			}
		}
#endif
#if TH_SEEK_STRAT_LAST_FREE
		heap->lastFree = nblock;
#endif
	  TH_ON_SPLITFREED((void*)nblock + sizeof(th_block_h), nblock->next * TH_BLOCKSIZE - sizeof(th_block_h));
		nblock = th_next(heap, nblock);
		if (nblock != 0) {
			nblock->prev = fullblocksize - bsize;
			TH_CALC_PARITY(nblock);
		}
	}
}

void* th_malloc(tinyheap* heap, unsigned int size) {
  if (size == 0) return 0;
	unsigned int bsize = (size + sizeof(th_block_h) + TH_ALIGNMENT + TH_BLOCKSIZE - 1) / TH_BLOCKSIZE;
	th_block_h* b = 0;

	TH_LOCK(heap);

#if TH_SEEK_STRAT_MIN_MAX
	if (bsize <= TH_BLOCK_MEDIAN) {
		b = heap->smallFree;
		if (!b) {
			b = heap->bigFree;
		}
	} else {
		b = heap->bigFree;
		if (!b) {
			b = heap->smallFree;
		}
	}
#endif
#if TH_SEEK_STRAT_LAST_FREE
  b = heap->lastFree;
#endif

	if (b == 0) {
		b = heap->p;
	}
	b = th_findFree(heap, bsize, b);
	if (b) {
		th_internalAlloc(heap, bsize, b);

		TH_UNLOCK(heap);

		return (void*)b + sizeof(th_block_h) + TH_ALIGNMENT;
	} else {
    TH_UNLOCK(heap);
	  return 0;
	}
}

void th_free(tinyheap* heap, void* p) {
	th_block_h* block = (th_block_h*)(p - sizeof(th_block_h) - TH_ALIGNMENT);
	TH_ASSERT_PAR_FREE(block);
	TH_ASSERT_FREE_FREED(block);

	TH_LOCK(heap);

#if TH_CALC_FREE
  heap->free += block->next;
#endif
	th_block_h* nblock = th_next(heap, block);	// next block
	th_block_h* pblock = th_prev(heap, block);	// prev block
	// check next block, if not this is last
	if (nblock != 0) {
		TH_ASSERT_PAR_FREE(nblock);
		if (nblock->free) {
			// next was free, so bang this and next together
			block->next += nblock->next;
#if TH_SEEK_STRAT_MIN_MAX
			if (nblock == heap->smallFree) heap->smallFree = 0;
			if (nblock == heap->bigFree) heap->bigFree = 0;
#endif
#if TH_SEEK_STRAT_LAST_FREE
			if (nblock == heap->lastFree) heap->lastFree = 0;
#endif
			nblock = th_next(heap, nblock); // nblock is now next next block
		}
	}
	// check previous block, if not this is first
	if (pblock != 0 && pblock->free) {
		// prev was free, so bang previous and this together
		TH_ASSERT_PAR_FREE(pblock);
		pblock->next += block->next;
		TH_CALC_PARITY(pblock);
#if TH_SEEK_STRAT_MIN_MAX
		if (block == heap->smallFree) heap->smallFree = 0;
		if (block == heap->bigFree) heap->bigFree = 0;
#endif
#if TH_SEEK_STRAT_LAST_FREE
      if (block == heap->lastFree) heap->lastFree = 0;
#endif
		block = pblock; // current block is now previous
	} else {
		// previous wasn't free, so mark this one free now
		block->free = 1;
		TH_CALC_PARITY(block);
	}
#if TH_SEEK_STRAT_MIN_MAX
	if (block->next <= TH_BLOCK_MEDIAN) {
		if (heap->smallFree == 0 || !heap->smallFree->free) {
			heap->smallFree = block;
		}
	} else {
		if (heap->bigFree == 0 || !heap->bigFree->free) {
			heap->bigFree = block;
		}
	}
#endif
#if TH_SEEK_STRAT_LAST_FREE
  heap->lastFree = block;
#endif
  TH_ON_FREED((void*)block + sizeof(th_block_h), block->next * TH_BLOCKSIZE - sizeof(th_block_h));
	// if block after this exists (might be next next if next was merged), update prev ptr
	if (nblock != 0) {
		TH_ASSERT_PAR_FREE(nblock); // might be doubled checked if there is a next ALLOCATED block
		nblock->prev = block->next;
		TH_CALC_PARITY(nblock);
	}

  TH_UNLOCK(heap);
}

#if TH_DUMP
void th_dump(tinyheap* heap) {
#if TH_SEEK_STRAT_MIN_MAX
  //heap->smallFree = 0;
  //heap->bigFree = 0;
#endif
#if TH_SEEK_STRAT_LAST_FREE
  //heap->lastFree = 0;
#endif


  TH_PRINTF("heap data:%p  size:%i  blocksize:%i  addr.bits:%i\n",
      heap->p,
      heap->len * TH_BLOCKSIZE,
      TH_BLOCKSIZE,
      TH_ADDRESSING_BITS);
#if TH_CALC_FREE
  TH_PRINTF("heap free:%i\n", heap->free * TH_BLOCKSIZE);
#endif
#if TH_USE_PARITY
  TH_PRINTF("heap PARITY check enable, check ");
#if TH_PARITY_CHECK_LEVEL == TH_PARITY_CHECK_DURING_FREE
  TH_PRINTF("during free\n");
#endif
#if TH_PARITY_CHECK_LEVEL == TH_PARITY_CHECK_DURING_MALLOC
  TH_PRINTF("during malloc\n");
#endif
#if TH_PARITY_CHECK_LEVEL == TH_PARITY_CHECK_ALL
  TH_PRINTF("always\n");
#endif

#endif
#if TH_SEEK_STRAT_MIN_MAX
  TH_PRINTF("heap seek stra:MIN_MAX  smallFree:%p  bigFree:%p  med:%i\n", heap->smallFree, heap->bigFree, TH_BLOCK_MEDIAN * TH_BLOCKSIZE);
#endif
#if TH_SEEK_STRAT_LAST_FREE
  TH_PRINTF("heap seek stra:LAST_FREE  lastFree:%p\n", heap->lastFree);
#endif
#if TH_SEEK_STRAT_NONE
  TH_PRINTF("heap seek stra:NONE\n");
#endif
#if TH_PICK_STRAT_FIRST
  TH_PRINTF("heap pick stra:FIRST FOUND\n");
#endif
#if TH_PICK_STRAT_BEST_FIT
  TH_PRINTF("heap pick stra:BEST FIT\n");
#endif
#if TH_PICK_STRAT_GOOD_ENOUGH_FIT
  TH_PRINTF("heap pick stra:GOOD ENOUGH +-%i\n", TH_GOOD_ENOUGH_DELTA*TH_BLOCKSIZE);
#endif

  TH_PRINTF("heap traverse:\n");

  th_block_h* block = heap->p;
  TH_ASSERT_PAR_MALLOC(block);
  do {
    TH_PRINTF("  %p %s prv:%04i nxt:%04i len:%04i ",
        block,
        block->free ? "FREE" : "BUSY",
        block->prev, block->next, block->next * TH_BLOCKSIZE);
#if TH_USE_PARITY
    TH_PRINTF("%s", th_checkParity(block) ? "OK" : "BAD");
#endif
    TH_PRINTF("\n  ");
    {
      int len = block->next * TH_BLOCKSIZE;
      if (len > 16) len = 16;
      char *d = (char*)((void*)block + sizeof(th_block_h));
      while (len--) {
        TH_PRINTF("%02x ", *d++);
      }
    }
    TH_PRINTF("\n");
    // next block
    block = th_next(heap, block);
  } while (block != 0);
}
#endif

#if TH_CALC_FREE
unsigned int th_freecount(tinyheap* heap) {
  return heap->free * TH_BLOCKSIZE;
}
#endif
