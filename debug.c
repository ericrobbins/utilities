#include <stdio.h>
#include <stdlib.h>
#include "debug.h"

int debuglevel = 1;

void
debug_init()
{
	char *buf = getenv("DEBUGLEVEL");
	if (buf)
		debuglevel = atoi(buf);

	return;
}
