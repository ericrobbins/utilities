/*
 * (c) 2012 Eric Robbins, all rights reserved, etc.
 *
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#include "cleanline.h"

/*
 * Cleans leading, trailing, and newlines from null terminated single line string. 
 * Flags can be ORed together to perform subsets of functions, 
 *  ie (CLEAN_BEGIN | CLEAN_END) for whitespace cleaning but leaving the newline.
 * Returns pointer to the cleaned string.
 * If you are using CLEAN_COPY, the returned pointer will be to malloc()ed memory 
 *  and you will have to free() it.
 * CLEAN_NL only cleans the LAST newline in the string, this is meant for single lines.
 *
 */

char *
cleanline(char *line, int flags)
{
	char *begin = line;
	char *end;
	char *result;
	char nl[3] = {0, 0, 0};

	// Skip past leading whitespace if CLEAN_BEGIN set
	while ((flags & CLEAN_BEGIN) && *begin && isspace(*begin))
		begin++;

	// if CLEAN_NL, find character before last newline or CR.. that's the end
	if (flags & CLEAN_NL)
	{
		char *e2;

		end = strchr(begin, '\n');
		if (end)
			end--;
		else
			end = begin + strlen(begin) - 1;

		e2 = strchr(begin, '\r');
		if (e2)
			e2--;

		if (e2 && e2 < end)
			end = e2;
	}
	else
	{
		char *tmp = strstr(begin, "\r\n");
		if (!tmp)
			tmp = strchr(begin, '\n');
		if (tmp)
		{
			if (*tmp == '\n')
				strcpy(nl, "\n");
			else
				strcpy(nl, "\r\n");
		}
		end = begin + strlen(begin) - 1;
	}

	// if CLEAN_END, back up from end until not whitespace, or end < begin
	while ((flags & CLEAN_END) && end >= begin)
	{
		//if (isspace(*end) && *end != '\r' && *end != '\n')
		if (isspace(*end))
			end--;
		else
			break;
	}

	if (end < begin) // happens if 0 length 
		end = begin;

	int newlen = end - begin + 1;
	if (flags & CLEAN_COPY)
	{
		result = malloc((newlen + strlen(nl) + 1) * sizeof(char));
		strncpy(result, begin, newlen);
		result[newlen] = '\0';
		strcat(result, nl);
	}
	else
	{
		memmove(line, begin, newlen);
		line[newlen] = '\0';
		strcat(line, nl);
		result = line;
	}

	return(result);
}

#ifdef _TEST_
int
main(int argc, char **argv)
{
	char *buf;
	char *result;
	int flags = atoi(argv[2]);

	buf = malloc(strlen(argv[1]) + 1);
	strcpy(buf, argv[1]);

	result = cleanline(buf, flags);
	printf("-%s-\t-%s-\t-%s-\n", argv[1], buf, result);
	free(buf);
	if (flags & CLEAN_COPY)
		free(result);

	return(0);
}
#endif
