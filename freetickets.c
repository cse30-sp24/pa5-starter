#include "Version.h"
#ifndef MYFREETICKETS
#warning("WARNING - USING THE SOLUTION freetickets.c NOT MY CODE")
#else
#include <limits.h>
#include <stdint.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include "parking.h"
#include "freetickets.h"

/*
 * freetickets
 *      tears down the database freeing all allocated memory
 *      by deleting all tickets and vehicles structs in the database
 *      count the number of tickets and vehicles freed in
 *      the database 
 *      prints the counts before returning
 */
void
freetickets(void)
{
    // unsigned long tcnt = 0UL;
    // unsigned long vcnt = 0UL;

    //printf("Total vehicles freed: %lu\n", vcnt);
    //printf("Total tickets freed: %lu\n", tcnt);
    return;
}
#endif
