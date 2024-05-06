#ifndef PARKING_H
#define PARKING_H

#define TABSZ 3     /* default hash table size a prime number */
#define MINTABSZ 1  /* Smallest allowable hash table size */
#define CODES 100   /* number of fine codes + 1 */
#define FINE_FILE "in/Fines.csv" /* default fines table */

/*
 * fine description by code (table index is code)
 * given a summons code this maps to a description and fine amount
 */
struct fine {
    char *desc;             /* text description of code */
    unsigned int fine;      /* value of the fine in $ */
};

/*
 * a specific summons
 * Note: for this PA summons as a long is ok, but it should be bigger
 */
struct ticket {
    unsigned long summons;  /* summons or ticket id */
    unsigned int code;      /* fine code 1-99 */
    struct ticket *next;    /* pointer to next ticket */
};

/*
 * a vehicle that has at least one summons
 */
struct vehicle {
    char *state;            /* state on license plate */
    char *plate;            /* id on license plate */
    unsigned int tot_fine;  /* summary field; all tickets */
    unsigned int cnt_ticket;/* number of tickets unpaid */
    struct vehicle *next;   /* pointer to next vehicle on hash chain */
    struct ticket *head;    /* pointer to first ticket; add tickets to end! */
};

/* 
 * global variable declaration for error message printing
 */
extern char *argv0;                /* for error message printing */
extern struct vehicle **htable;    /* pointer to hash table */
extern struct fine fineTab[];      /* table of fines by code 1-99 */
extern uint32_t tabsz;             /* hash table size */
extern int silent;                 /* silent mode for autograder */
#endif
