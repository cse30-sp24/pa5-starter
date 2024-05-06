#ifndef LOADDB_H
#define LOADDB_H

#define FDELIM ','  /* delimiter in fine table */
#define DDELIM ','  /* delimiter in data table */
#define FCOLS 3     /* fine table column count */
#define DCOLS 4     /* data table column count */
#define CODE_IN 0   /* code field in fine table */
#define DESC_IN 1   /* description field in fine table */
#define FINE_IN 2   /* fine field in fine table */
#define SUMM_IN 0   /* summons field in ticket table */
#define PLATE_IN 1  /* plate field in ticket table */
#define STATE_IN 2  /* plate field in ticket table */
#define TCODE_IN 3  /* code field in ticket table */

/*
 * function prototypes
 */
int readfines(char *);
void freefines();
int readtickets(char *, int);
#endif
