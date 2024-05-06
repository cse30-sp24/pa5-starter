#ifndef SPLITROW_H
#define SPLITROW_H
/*
 * Constants
 */
#define MINCOL  1       /* min number of columns */
#define INDELIM ','     /* input column delimiter default */
#define OUTDELIM ','    /* input column delimiter default */

 /*
  * Function prototypes
  */
int splitrow(char *, char, int, char **, unsigned long, char *);
#endif
