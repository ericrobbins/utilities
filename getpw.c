/*
 * (c) 2012 Eric Robbins, all rights reserved, etc.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cleanline.h"

/* 
 * get password for specified username out of specified file. 
 * usernames/passwords stored as user:pass, one per line. 
 *
 */

char *
getpw(char *file, char *user, char **pass)
{
	char buf[1024];

	FILE *infile = fopen(file, "r");
	if (!infile)
		return(NULL);
	while (fgets(buf, sizeof(buf), infile))
	{
		cleanline(buf, CLEAN_BEGIN|CLEAN_END|CLEAN_NL);
		char *tmp = strchr(buf, ':');
		if (!tmp)
			continue;
		*tmp++ = '\0';
		if (!strcmp(buf, user))
		{
			*pass = malloc(strlen(tmp) + 1);
			strcpy(*pass, tmp);
			fclose(infile);
			return(*pass);
		}
	}
	fclose(infile);
	return(NULL);
}

#ifdef _TEST_
int
main(int argc, char **argv)
{
	char *pw;

	pw = getpw(argv[1], argv[2], &pw);

	printf("user %s pw %s\n", argv[2], pw ? pw : "NOTFOUND");

	return(0);
}
#endif
