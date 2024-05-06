#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include "parking.h"
#include "loaddb.h"
#include "splitrow.h"
#include "insticket.h"

/*
 * function for reading up the fines table. 
 * the fine table is a csv file with three fields per record 
 * code,description,fine
 * returns 0 on success, -1 on failure
 */
 int
 readfines(char *finenm)
 {
    struct fine *fineptr = fineTab; /* pointer to fine table */
    FILE *fp;               /* file handle */
    char *buf = NULL;       /* input buffer, getline() will allocate it */
    size_t bufsz = 0;       /* size of buffer altered by getline()*/
    unsigned long linecnt;  /* count of lines in finetable */
    int code;               /* code entry in record */
    char *endptr;           /* for strtol(); ensure entire argument is parsed */
    char *coltab[FCOLS];    /* pointers to fine table fields */
    int retval = 0;         /* return value */

    if ((fp = fopen(finenm, "r")) == NULL) {
        fprintf(stderr, "%s: unable to open %s for read\n", argv0, finenm);
        return -1;
    }

    /*
     * code 0 is not used, but set it to 0 dollars anyway
     */
    fineptr->desc = strdup("INVALID");
    fineptr->fine = 0U;
    /*
     * skip over the description
     */
    if (getline(&buf, &bufsz, fp) <= 0) {
        fprintf(stderr, "%s: error reading header in fine table\n", argv0);
        free(buf);
        fclose(fp);
        return -1;
    }
    fineptr++;
    linecnt = 1UL;
    /*
     * loop through the rest of the codes
     */
    while (getline(&buf, &bufsz, fp) > 0) {
        if (++linecnt > CODES) {
            fprintf(stderr, "%s: fine table, too many entries\n", argv0);
            retval = -1;
            break;
        }
        /*
         * find the fields in the table
         */
        if (splitrow(buf, FDELIM, FCOLS, coltab, linecnt, argv0) != 0) {
            retval = -1;
            break;
        }

        /*
         * validate the code is proper
         * a proper fine table is in sorted order
         */
        errno = 0;
        code = (int)strtoul(coltab[CODE_IN], &endptr, 10);
        if ((errno != 0) || (*endptr != '\0') || (code != (int)(linecnt -1))) {
            fprintf(stderr, "%s: fine table record %lu bad code field\n",
                 argv0, linecnt);
            retval = -1;
            break;
        }

        /*
         * check the description
         */
        if ((coltab[DESC_IN] == NULL) || (*coltab[DESC_IN] == '\0')) {
            fprintf(stderr, "%s: fine table record %lu bad description\n",
                 argv0, linecnt);
            retval = -1;
            break;
        }
       
        /*
         * copy the description
         */
        fineptr->desc = strdup(coltab[DESC_IN]);
        
        /*
         * get the fine
         */
        errno = 0;
        fineptr->fine = (unsigned int)strtoul(coltab[FINE_IN], &endptr, 10);
        if ((errno != 0) || ((*endptr != '\n') && (*endptr != '\0'))) {
            fprintf(stderr, "%s: fine table record %lu bad fine field\n",
                 argv0, linecnt);
            retval = -1;
            break;
        }
        fineptr++;
    }

    free(buf);
    fclose(fp);
    return retval;
}

/*
 * function to free fine table memory
 */
void
freefines(void)
{
    struct fine *fineptr = fineTab;
    struct fine *endptr = fineTab + CODES;

    if (fineptr == NULL)
        return;
    /*
     * free each fine description is allocated
     */
    while (fineptr < endptr) {
        if (fineptr->desc != NULL)
            free(fineptr->desc);
        fineptr++;
    }
    return;
}

/*
 * function for reading up the ticket table. 
 * the ticket fine table is a csv file with four fields per record
 * summons_id,plate,state,code
 * returns 0 on success, -1 on failure
 */
 int
 readtickets(char *datanm, int fromcmd)
 {
    FILE *fp;
    char *buf = NULL;       /* input buffer, getline() will allocate it */
    size_t bufsz = 0;       /* size of buffer altered by getline()*/
    unsigned long linecnt;  /* count of lines in ticket file */
    unsigned long goodcnt;  /* count of good lines in ticket file */
    int code;               /* code entry in record */
    char *endptr;           /* for strtol(); ensure entire argument is parsed */
    char *coltab[DCOLS];    /* pointers to ticket table fields */

    if ((datanm == NULL) || (*datanm == '\0')) {
        if (fromcmd)
            printf("Empty Database\n");
        return 0;
    }

    if ((fp = fopen(datanm, "r")) == NULL) {
        fprintf(stderr, "%s: unable to open %s for reading ticket data\n", argv0, datanm);
        return -1;
    }

    /*
     * skip over the description
     */
    if (getline(&buf, &bufsz, fp) <= 0) {
        fprintf(stderr, "%s: empty ticket file\n", argv0);
        fclose(fp);
        free(buf);
        return 0;
    }
    linecnt = 1UL;
    goodcnt = 0UL;
    /*
     * loop through the rest of the codes
     */
    while (getline(&buf, &bufsz, fp) > 0) {
        linecnt++;
        /*
         * find the fields in the table
         */
        if (splitrow(buf, DDELIM, DCOLS, coltab, linecnt, argv0) != 0)
            continue;

        /*
         * translate the code to an int
         */
        errno = 0;
        code = (int)strtoul(coltab[TCODE_IN], &endptr, 10);
        if ((errno != 0) || (*endptr != '\0') || (code >= CODES)) {
            fprintf(stderr, "%s: ticket file record %lu bad code field %d\n",
                 argv0, linecnt, code);
            continue;
        }
        if (insertticket(coltab[SUMM_IN], coltab[PLATE_IN], coltab[STATE_IN], code) != 0) {
            fprintf(stderr, "%s: ticket file record %lu summons:%s insertticket failed \n",
                 argv0, linecnt, coltab[SUMM_IN]);
        } else
            goodcnt++;
    }
    if (fromcmd)
        printf("Total tickets loaded: %lu\n", goodcnt);

    free(buf);
    fclose(fp);
    return 0;
}
