#include <xinu.h>

/* Memory bounds */

void	*minheap;		/* Start of heap			*/
void	*maxheap;		/* Highest valid heap address		*/

/*------------------------------------------------------------------------
 * meminit - initialize memory bounds and the free memory list
 *------------------------------------------------------------------------
 */
void	meminit(void) {

       struct	memblk	*memptr;	/* Ptr to memory block		*/

       /* Initialize the free memory list */

       /* Note: we pre-allocate  the "hole" between 640K and 1024K */

       maxheap = (void *)0x600000;	/* Heap upper bound: 6 Mbytes */
       minheap = &end;

       memptr = memlist.mnext = (struct memblk *)roundmb(minheap);
       if ((char *)minheap < HOLESTART && (char *)maxheap > HOLEEND) {
       	/* create two blocks that straddle the hole */
       	memptr->mnext = (struct memblk *)HOLEEND;
       	memptr->mlength = (int) truncmb((unsigned) HOLESTART -
			 (unsigned)memptr);
       	memptr = (struct memblk *) HOLEEND;
       	memptr->mnext = (struct memblk *) NULL;
       	memptr->mlength = (int) truncmb( (uint32)maxheap - 
       			(uint32)HOLEEND - NULLSTK);
       } else {
       	/* initialize free memory list to one block */
       	memlist.mnext = memptr = (struct memblk *) roundmb(&end);
       	memptr->mnext = (struct memblk *) NULL;
       	memptr->mlength = (uint32) truncmb((uint32)maxheap -
			(uint32)memptr - NULLSTK);
       }

       /* Initialize the total used by getmem/getstk/freemem. */
       memlist.mlength = 0;
       for (memptr = memlist.mnext; memptr != NULL; memptr = memptr->mnext) {
           memlist.mlength += memptr->mlength;
       }

       return;
}
