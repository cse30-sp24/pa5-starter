/*
 * Programming assignment 6 sample solution
 * this is excessively commented for quick reads...
 *
 * Usage ./parking -c cvs_file -f fine_file
 *
 * reads the fine table up to get the fines for
 * each type of parking ticket 1-99
 *
 * fine table (csv):
 * code,description,fine
 *
 * read the ticket data in the cvs file into a
 * into a in memory hashed database
 *
 * ticket file (csv):
 * summons_id,state,plate,code
 *
 * assumes the above file formats
 * 
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include "parking.h"
#include "loaddb.h"
#include "commands.h"
#include "freetickets.h"

/*
 * global variable of program name for error message printing
 */
char *argv0;                /* point to argv0 for error messages */
struct vehicle **htable;    /* pointer to hash table */
struct fine fineTab[CODES]; /* table of fines by code 0-99 */
uint32_t tabsz = TABSZ;     /* hash table size */
int silent = 0;             /* silent mode for autograder */

/*
 * main
 *
 * Arguments: argc, argv
 * 
 * Operation:  driver for in memory ticket database
 *  parses options
 *  open files, then reads up fines, read up ticket data, closes files
 *
 *  Has a simple command line interface; runs until EOF on stdio or q command
 *  see commands.c
 */ 
int 
main(int argc, char **argv)
{
    char *ticknm = NULL;    // name of ticket data file name
    char *finenm = NULL;    // name of fines data file name
    int opt;                // option flag returned by getopt
    char *endptr;           // for strtol(); ensure entire argument is parsed
    int retval = EXIT_SUCCESS; // return value

    argv0 = *argv;          // set global with command name for error printing

    /*
     * parse the command line args.
     * if there was an error, return with EXIT_FAILURE
     */
    while ((opt = getopt(argc, argv, "+d:f:t:s")) != -1) {
        switch (opt) {
        case 'd':
            /*
             * name of ticket data to read up
             */
            ticknm  = optarg;
            break;
        case 'f':
            /*
             * name of fine file to read up
             */
            finenm  = optarg;
            break;
        case 's':
            silent = 1;
            break;
        case 't':
            /*
             * over-ride of hash table size
             */
            errno = 0;
            tabsz = (uint32_t)strtoul(optarg, &endptr, 10);
            if ((*endptr != '\0') || (errno != 0) || (tabsz < MINTABSZ)) {
                fprintf(stderr, "%s: -t operand bad value\n", argv0);
                retval = EXIT_FAILURE;
            }
            break;
        case '?':
        default:
            /*
             * this is the case for error message handle directly by getopt()
             * we just mark we got an error
             */
            retval = EXIT_FAILURE;
            break;
        }
    }

    /*
     * make sure that there are no more args
     */
    if (optind != argc) {
        fprintf(stderr, "%s: Too many args\n", argv0);
        retval = EXIT_FAILURE;
    }

    /*
     * if bad or missing required option, print usage and exit
     */
    if (retval != EXIT_SUCCESS) {
        fprintf(stderr,
            "Usage: %s [-d ticket_file] [-f fine_file] [-t size] [-s]\n", argv0);
        return retval;
    }

    /*
     * load the fine table. The fine codes run 1-99
     */
    if (finenm == NULL)
        finenm = FINE_FILE;
    if (readfines(finenm) != 0)
        return EXIT_FAILURE;

    /*
     * load the ticket table
     * allocate the hash table, then fill it
     */
    if ((htable = calloc(tabsz, sizeof(struct vehicle *))) == NULL) {
        fprintf(stderr,"%s: calloc of ticket table failed\n", argv0);
        return EXIT_FAILURE;
    }

    if (readtickets(ticknm, 0) != 0) {
        freetickets();
        free(htable);
        return EXIT_FAILURE;
    }

    /*
     * interactive commands
     */
    commands(ticknm);

    /*
     * freeup allocated space for valgrind leaking memory checks
     */
    freetickets();
    free(htable);
    return EXIT_SUCCESS;
}
