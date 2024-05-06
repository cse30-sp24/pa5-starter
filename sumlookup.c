#include "Version.h"
#ifndef MYSUMLOOKUP
#warning("WARNING - USING THE SOLUTION sumlookup.c NOT MY CODE")
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
#include "sumlookup.h"
#include "subs.h"

/*
 * summlookup
 *     find the vehicle with a specified summons number
 *
 * Arguments:
 *
 * summ:
 *       string of summons number
 *       make sure to convert summ string to a summons number
 *       use strsumid() - see subs.c
 *
 * returns:
 *       a pointer to the vehicle if found
 *       NULL if not found
 */
struct vehicle *
sumlookup(char *summ)
{
    return NULL;
}
#endif
