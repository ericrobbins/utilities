#ifndef _DEBUG_H_
#define _DEBUG_H_

#define debug(lev, x, ...)  \
	{if (debuglevel >= lev) {fprintf(stderr, "%s: ", __FUNCTION__); \
	fprintf(stderr, (x), ##__VA_ARGS__); \
	if (*(x + strlen(x) - 1) != '\n') {fprintf(stderr, "\n");}}}

#define fail(x, ...)    {fprintf(stderr, x, ##__VA_ARGS__); exit(1);}

extern int debuglevel = 1;

#endif
