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
 *  summ:
 *      summons string (ascii numbers)
 * Output variable:
 *  summid:
 *      summ converted to an unsigned long
 *
 * return: 
 *      0 if the conversion is ok
 *      -1 otherwise
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
 *  vhpt: pointer to vehicle
 */
unsigned int
printvehicle(struct vehicle *vhpt)
{
    struct ticket *ticket;
    unsigned int cnt_ticket = 0U;
    unsigned int tot_fine = 0U;

    if (vhpt == NULL)  {
        fprintf(stderr, "%s: printvehicle(), passed NULL\n", argv0);
        return cnt_ticket;
    }

    printf("  Plate: %s, State: %s, Tickets: %u, Total: $%u\n", vhpt->plate,
            vhpt->state, vhpt->cnt_ticket, vhpt->tot_fine);

    if ((ticket = vhpt->head) == NULL) {
        printf("     Error, no tickets found!\n");
        return cnt_ticket;
    }

    while (ticket != NULL) {
        cnt_ticket++;
        tot_fine += fineTab[ticket->code].fine;
        printf("     Ticket %2u Summons #: %6lu Code#: %2d Fine: $%3u %s\n",
                cnt_ticket, ticket->summons, ticket->code, fineTab[ticket->code].fine,
                fineTab[ticket->code].desc);
        ticket = ticket->next;
    }
    if (cnt_ticket != vhpt->cnt_ticket)
        fprintf(stderr, "\t%s Warning: Found %u tickets expected %u\n", argv0,
                cnt_ticket, vhpt->cnt_ticket);
    if (tot_fine != vhpt->tot_fine)
        fprintf(stderr, "\t%s Warning: Total fine $%u expected $%u\n", argv0,
                tot_fine, vhpt->tot_fine);

    fflush(stdout);
    return cnt_ticket;
}

/*
 * printsummons
 *     prints a specific summons for a vehicle
 * args
 *  vhpt: pointer to vehicle
 *  summid: the summons to print
 */
void
printsummons(struct vehicle *vhpt, unsigned long summid)
{
    struct ticket *ticket;
    unsigned int cnt_ticket = 0U;
    unsigned int tot_fine = 0U;

    if (vhpt == NULL)  {
        fprintf(stderr, "%s: printvehicle(), passed NULL\n", argv0);
        return;
    }

    printf("  Plate: %s, State: %s, Tickets: %u, Total: $%u\n", vhpt->plate,
            vhpt->state, vhpt->cnt_ticket, vhpt->tot_fine);

    if ((ticket = vhpt->head) == NULL) {
        printf("     Error, no tickets found!\n");
        return;
    }

    while (ticket != NULL) {
        cnt_ticket++;
        tot_fine += fineTab[ticket->code].fine;
        if (ticket->summons == summid) {
            printf("     Summons #: %6lu Code#: %2d Fine: $%3u %s\n",
                    ticket->summons, ticket->code, fineTab[ticket->code].fine,
                    fineTab[ticket->code].desc);
        }
        ticket = ticket->next;
    }
    if (cnt_ticket != vhpt->cnt_ticket)
        fprintf(stderr, "\t%s Warning: Found %u tickets expected %u\n", argv0,
                cnt_ticket, vhpt->cnt_ticket);
    if (tot_fine != vhpt->tot_fine)
        fprintf(stderr, "\t%s Warning: Total fine $%u expected $%u\n", argv0,
                tot_fine, vhpt->tot_fine);

    fflush(stdout);
    return;
}
