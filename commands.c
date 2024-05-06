#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <errno.h>
#include "parking.h"
#include "loaddb.h"
#include "commands.h"
#include "verifydb.h"
#include "dumpdb.h"
#include "vehlookup.h"
#include "sumlookup.h"
#include "delticket.h"
#include "subs.h"
#include "insticket.h"
#include "freetickets.h"
#include "largest.h"
#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

/*
 * command "shell" routines used to test the database
 */
static void
menu(void)
{
    printf("***** Command Summary *****\n");
    printf("Debug commands:\n");
    printf("\tQ                                 Quit exit and free up memory\n");
    printf("\tV                                 Verify database \n");
    printf("\tD                                 Dump (print) entire database\n");
    printf("\tC index                           Print the vehicles on a hash chain[index]\n");
    printf("\tE                                 Erase the vehicle database (delete database contents)\n");
    printf("\tR [filename]                      Reload the vehicle database optional filename\n");
    printf("Query commands:\n");
    printf("\tL                                 Print largest fine and largest tickets count\n");
    printf("\tF PLATE STATE                     Print the tickets for a vehicle with id PLATE STATE\n");
    printf("\tS SUMMONS                         Print vehicle information with the SUMMONS\n");
    printf("\tP SUMMONS PLATE STATE             Pay SUMMONS number for vehicle with id  PLATE STATE\n");
    printf("\tI SUMMONS PLATE STATE CODE#       Insert a SUMMONS\n");
    return;
}

/*
 * findcmd
 *          finds and prints all tickets for specified vehicle
 * input    plate and state from stdim
 * output   vehicle summary plus all tickets unpaid
 */
static void
findcmd(char *buf)
{
    char cmd[3];
    char plate[12];         /* wired in limits for sscanf safety */
    char state[3];          /* wired in limits for sscanf safety */
    struct vehicle *found;
    char *ptr;

    if (sscanf(buf,"%1s %11s %2s", cmd, plate, state) != 3) {
        printf("Useage: F PLATE STATE\n");
        return;
    }

    for (ptr = plate; *ptr != '\0'; ptr++) {
        if (islower(*ptr))
            *ptr = toupper(*ptr);
    }

    for (ptr = state; *ptr != '\0'; ptr++) {
        if (islower(*ptr))
            *ptr = toupper(*ptr);
    }

    found = vehiclelookup(plate, state);
    if (found == NULL) {
        printf("   Plate: %s, State: %s \n", plate, state);
        printf("      No tickets\n");
    } else
        (void)printvehicle(found);
    return;
}

/*
 * findsumm
 *          finds vehicle for a summons
 * input    summons from stdim
 * output   vehicle summary
 */
static void
findsumm(char *buf)
{
    char cmd[3];
    char summ[14];          /* wired in limits for sscanf safety */
    unsigned long summid;
    struct vehicle *found;

    if (sscanf(buf, "%1s %13s", cmd, summ) != 2) {
        printf("Useage: S SUMMONS\n");
        return;
    }

    if (strtosumid(summ, &summid) != 0) {
        printf("Invalid summons number %s\n", summ);
        return;
    }
    found = sumlookup(summ);
    if (found == NULL) {
        printf("   Summons: %s\n", summ);
        printf("      Not found!\n");
    } else 
        printsummons(found, summid);
    return;
}

/*
 * paycmd
 *          pays a fine for a ticket (summons) specified
 *          will cause the ticket to be removed and if
 *          no tickets remain, remove the vehicle info
 * input    plate state and summos_is from stdin
 * output   summons paid or not
 */
static void
paycmd(char *buf)
{
    char cmd[3];
    char plate[12];         /* wired in limits for sscanf safety */
    char state[3];          /* wired in limits for sscanf safety */
    char summ[14];          /* wired in limits for sscanf safety */
    char *ptr;

    if (sscanf(buf,"%1s %13s %11s %2s", cmd, summ, plate, state) != 4) {
        printf("Useage: P SUMMONS PLATE STATE\n");
        return;
    }

    for (ptr = plate; *ptr != '\0'; ptr++) {
        if (islower(*ptr))
            *ptr = toupper(*ptr);
    }

    for (ptr = state; *ptr != '\0'; ptr++) {
        if (islower(*ptr))
            *ptr = toupper(*ptr);
    }

    if (delticket(plate, state, summ) == 0)
        printf("Plate: %s, State %s, Summons %s paid\n", plate, state, summ);
    else
        printf("Plate: %s, State %s, Summons %s not found\n", plate, state,
             summ);
    return;
}

/*
 * insertcmd
 *          inserts a summons into the database
 * input    summons,plate,state,code from stdin
 * output   none 
 */
static void
insertcmd(char *buf)
{
    char cmd[3];
    int code;
    char plate[12];         /* wired in limits for sscanf safety */
    char state[3];          /* wired in limits for sscanf safety */
    char summ[14];          /* wired in limits for sscanf safety */
    char codstr[4];         /* wired in limits for sscanf safety */
    char *endptr;
    char *ptr;

    if (sscanf(buf,"%1s %13s %11s %2s %3s", cmd, summ, plate, state,
            codstr) != 5) {
        printf("Useage: I SUMMONS PLATE STATE CODE\n");
        return;
    }
    /*
     * translate the code to an int
     */
    errno = 0;
    code = (int)strtoul(codstr, &endptr, 10);
    if ((errno != 0) || (*endptr != '\0') || (code >= CODES) || (code <= 0)) {
        printf("%s: bad code value\n", codstr);
        return;
    }

    for (ptr = plate; *ptr != '\0'; ptr++) {
        if (islower(*ptr))
            *ptr = toupper(*ptr);
    }

    for (ptr = state; *ptr != '\0'; ptr++) {
        if (islower(*ptr))
            *ptr = toupper(*ptr);
    }

    if (insertticket(summ, plate, state, code) != 0)
        printf("Plate: %s, State %s, Summons %s insert failed\n", plate, state, summ);
    else
        printf("Plate: %s, State %s, Summons %s inserted\n", plate, state, summ);

    return;
}
/*
 * chaincmd debug command
 *          prints out all the vehicles and tickets on a hash chain
 * input    hash chain number 
 * output   dump of all data on a hash chain
 */
static void
chaincmd(char *buf)
{
    uint32_t index;
    char *endptr;
    unsigned long tickets;
    unsigned long vehicles;

    errno = 0;
    if ((*(buf+1) == '\n') || (*(buf+1) == '\0')) {
        printf("Specify a chain number between 0 and %u\n", tabsz - 1);
        printf("Useage: C index\n");
        return;
    }
    index = (uint32_t)strtoul(buf+1, &endptr, 10);
    if (((*endptr != '\n') && (*endptr != '\0')) || (errno != 0) || (index >= tabsz)) {
        printf("C command: index not between 0 and %u; bad value %u %s\n", tabsz - 1, 
                index, endptr);
        printf("Useage: C index\n");
        return;
    }

    if ((vehicles = dumpchain(index, &tickets))
            > 0)
        printf("index: %u vehicles: %lu tickets: %lu\n", index, vehicles, tickets);
    else
        printf("Empty Database\n");
    return;
}

/*
 * reload debug command
 * used to reload the current database or specify another
 * Warning it does a reload, it doe not empty the database prior
 * so is good for checking dup insert detection code.
 */
static void
reload(char *buf, char *ticknm)
{
    char path[PATH_MAX];
    char cmd[3];

    *path = '\0';
    if (sscanf(buf,"%1s %s", cmd, path) > 2) {
        printf("Useage: R  [file]\n");
        return;
    }
    if (*path == '\0') {
        (void)readtickets(ticknm, 1); 
        return;
   }
    (void)readtickets(path, 1); 
    return;
}

/*
 * simple command line interface just to test code
 * NOT an example of a good user interface
 * this is another type of test fixture where debugging code
 * is built-in.
 */
void
commands(char *ticknm)
{
    char *buf = NULL;       /* input buffer, getline() will allocate it */
    size_t bufsz = 0;       /* size of buffer altered by getline()*/

    /*
     * loop through the rest of the codes
     */
    if (!silent) {            // set when running non-interactive tests...
        menu();
        printf("Input command: ");
    }
    while (getline(&buf, &bufsz, stdin) > 0) {
        if (silent)
            printf("Command: %s\n", buf);
        switch(*buf) {
        case 'C':
            /* FALL THROUGH */
        case 'c':
            chaincmd(buf);
            break;
        case 'D':
            /* FALL THROUGH */
        case 'd':
            dumpdb();
            break;
        case 'E':
            /* FALL THROUGH */
        case 'e':
            freetickets();
            break;
        case 'F':
            /* FALL THROUGH */
        case 'f':
            findcmd(buf);
            break;
        case 'I':
            /* FALL THROUGH */
        case 'i':
            insertcmd(buf);
            break;
        case 'P':
            /* FALL THROUGH */
        case 'p':
            paycmd(buf);
            break;
        case 'R':
            /* FALL THROUGH */
        case 'r':
            reload(buf, ticknm);
            break;
        case 'L':
            /* FALL THROUGH */
        case 'l':
            largest();
            break;
        case 'V':
            /* FALL THROUGH */
        case 'v':
            verifydb();
            break;
        case 'Q':
            /* FALL THROUGH */
        case 'q':
            free(buf);
            return;
        case 's':
            /* FALL THROUGH */
        case 'S':
            findsumm(buf);
            break; 
        default:
            if (silent)
                break;
            if (*buf != '\n')
                printf("Unknown command %c\n", *buf);
            menu();
            break;
        }
        if (!silent)
            printf("Input command: ");
    }
    free(buf);
    return;
}
