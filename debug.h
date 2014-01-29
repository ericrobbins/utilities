#ifndef _DEBUG_H_
#define _DEBUG_H_

#define debug(lev, x, ...)  \
	do {if (debuglevel >= lev) {fprintf(stderr, "%s: ", __FUNCTION__); \
	fprintf(stderr, (x), ##__VA_ARGS__); \
	if (*(x + strlen(x) - 1) != '\n') {fprintf(stderr, "\n");}}} while (0);

#define fail(x, ...)    do {fprintf(stderr, x, ##__VA_ARGS__); exit(1);} while (0);

extern int debuglevel;
void debug_init();

#endif
