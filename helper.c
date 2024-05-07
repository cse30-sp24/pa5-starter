#include <limits.h>
#include <stdint.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include "parking.h"
#include "subs.h"

/*
 * strtosumid
 *      converts a summons id string to unsigned long integer
 *      saves space in the database
 * use example:
 *      unsigned long summid;
 *      if (strtosumid(summ, &summid) != 0)
 *          return -1;
 *      new_ticket->summons = summid;
 *
 * input:
 *  summ:summons string (ascii numbers)
 *
 * return: 0 is converted ok -1 otherwise
 */

int
strtosumid(char *summ, unsigned long *summid)
{
    char *endptr;

    if ((summ == NULL) || (summid == NULL)) {
        fprintf(stderr, "%s: strtoDate() passed a NULL\n", argv0);
        return -1;
    }
    errno = 0;
    *summid = strtoul(summ, &endptr, 10);
    if (((*endptr != '\0') && (*endptr != '\n')) || (errno != 0)) {
        fprintf(stderr, "%s: strtosumid() summons bad value %lu\n",
            argv0, *summid);
        return -1;
    }
    return 0;
}

/*
 *
 * hash
 *
 * Args:
 *  str     string to be hashed
 *
 * Operation: calculates a hash value for the string
 *        returns the hash value
 */
uint32_t
hash(char *str)
{
    uint32_t hash = 0;
    uint32_t c;

    if (str == NULL) {
        fprintf(stderr, "%s: hash() passed NULL\n", argv0);
        return 0UL;
    }

    while ((c = (unsigned char)*str++) != '\0')
        hash = c + (hash << 6) + (hash << 16) - hash;

    return hash;
}
/*
 * printvehicle
 *      prints the vehicle and all its tickets to stdout
 * args
 *  vhpt pointer to vehicle
 *
 *  unsigned int printvehicle(struct vehicle *vhpt);
 */

/*
 * printsummons
 *      prints the vehicle just one summons to stdout
 * args
 *  vhpt pointer to vehicle
 *  summid summons number
 *
 *  void printsummons(struct vehicle *vhpt, unsigned long summid);
 */
