#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int
main(int argc, char **argv)
{
	return printf("%s\n", strerror(atoi(argv[1] ? argv[1] : "0")));
}
