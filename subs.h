#ifndef SUBS_H
#define SUBS_H

/*
 * function prototypes
 */
int strtosumid(char *, unsigned long *);
uint32_t hash(char *);
unsigned int printvehicle(struct vehicle *);
void printsummons(struct vehicle *, unsigned long);
#endif
