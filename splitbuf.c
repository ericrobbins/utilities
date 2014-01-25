/*
 * (c) 2012 Eric Robbins, all rights reserved, etc.
 *
 * given a buffer, split on tokens and return an allocated null terminated array of pointers.
 * 
 * if (flags & SPLITBUF_COPY) then copy string leaving original intact, else alter original string
 * if (flags & SPLITBUF_GROUP) then multiple sep characters in a row will be treated as 1
 *  ie splitbuf("a::b::c", ":") becomes "a", "b", "c" ..otherwise "a", "", "b", "", "c"
 * if (flags & SPLITBUF_STRING) then the "sep" argument is treated as a full string to split by 
 *  and not a list of characters.
 * if (flags & SPLITBUF_IGNORECASE) then comparisons are case insensitive
 *  ie splitbuf("abc123xyz", "123") becomes "abc", "xyz"
 * if (flags & STRIPBUF_STRIPENCLOSE) then enclose strings are stripped at beginning and end
 *  ie splitbuf('"Joe","Cool"', ",", "\"") becomes "Joe", "Cool".. otherwise ""Joe"", ""Cool""
 * return value is number of elements in array.
 *
 * When caller is done with the answer, it must free(*retmembers);
 *
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <ctype.h>

#include "splitbuf.h"

/* this is for a list of tokens.. pointer to beginning of token in original string, and length. 
   So I can make a copy without altering the original string.
*/
typedef struct part {
	char *start;
	int len;
} part;

/* strchr() but case insensitive */
static char *
strcasechr(const char *haystack, int needle)
{
	/* strchr is optimized assembly in glibc.. I can't hope to compete, only cooperate */
	/* was doing this before lowercasing the sep string.. might still be faster */
	/*
	char *x = strchr(haystack, toupper(needle));
	return(x ? x : strchr(haystack, tolower(needle)));
	*/
	return(strchr(haystack, tolower(needle)));
}

/* cpy len bytes from src to dest, skipping leading/trailing characters found in "ignore" */
static int
my_ignorecpy(char *dst, char *src, int len, const char *ignore, int ignorecase)
{
	int count = 0;
	int i, j;
	char *z = (char *)ignore;
	char *(*mystrchr)(const char *, int) = strchr;

//printf("my_ignorecpy(): src %s, len %i\n", src, len);

	// create lower case copy of "ignore" if necessary
	if (ignorecase) 
	{
		z = malloc(strlen(ignore) + 1);
		for (i = 0; ignore[i]; i++)
			z[i] = tolower(ignore[i]);
		z[i] = '\0';
		mystrchr = strcasechr;
	}

	/* copy string to dst.. if character is \ then skip and treat as escaping the next char.
       if first or last string of token and matches the "ignore" string, also skip */
	i = j = 0;
	int escaped = 0;
	while (i < len)
	{

		if (src[i] == '\\')
		{
			if (!escaped) // skip over first 
				count++, i++;
			else
				dst[j++] = src[i++];
			escaped = !escaped;
		}
		else 
		{
			escaped = 0;
			/* "Eric\"R" should become Eric"R not EricR 
			  however, "Eric" , "Robbins" will become Eric" "Robbins if sep is ",". Use " ," as 
			  string sep and SPLITBUF_GROUP to fix. */
			if ((i == 0 || i == (len - 1)) && mystrchr(z, src[i]))
				count++, i++;
			else
				dst[j++] = src[i++];
		}
	}
	dst[j] = '\0';

	if (ignorecase && z)
		free(z);

//printf("my_ignorecpy(): count %i, dst %s\n", count, dst);

	return(count);
}

int
splitbuf(int flags, char *line, char *sep, char *enclose, int pieces, char ***retmembers)
{
	char **members;
	char *p;
	char *element;
	part *parts = NULL;
	int count = 0;
	int totallen = 0;
	int (*mystrncmp)(const char *, const char *, size_t) = strncmp;
	char *(*mystrchr)(const char *, int) = strchr;
	int jumplen = (flags & SPLITBUF_STRING) ? strlen(sep) : sizeof(char);
	char *mysep;
	int escaped = 0;
	int enclosed = 0;
	int stripchars = 0;

	if (!line || !sep || !retmembers)
		return(-1);

	if (flags & SPLITBUF_IGNORECASE)
	{
		/* use case insensitive str* versions if we're ignoring case */
		mystrncmp = strncasecmp;
		mystrchr = strcasechr;

		/* lower case the seperator array, if we're ignoring case */
		/* I don't know which is faster, doing this once or calling strchr twice with lower and upper *p. 
		   My gut tells me this is faster, or at least more constant. Maybe I should benchmark it. */
		mysep = malloc(strlen(sep) + 1);
		char *mysepcp = mysep;
		while ((*mysepcp++ = tolower(*sep++)) != 0)
			; /* nothing */
	}
	else
		mysep = sep;

	p = element = line; 

	if (pieces == 1)
		p += strlen(p); // go to end

	while (1)
	{
		//printf("current char is: %c\n", *p);
		//printf("p = %c escaped = %i enclosed = %i\n", *p, escaped, enclosed);
		/* If we are at the end of the string, or if the current token matches a seperator, tokenize */
		if (!*p || (!enclosed && ((flags & SPLITBUF_STRING) ? !mystrncmp(p, mysep, jumplen) : (mystrchr(mysep, *p) != NULL))))
		{
			int nobreak = 0;

			if (count)
				members = realloc(members, sizeof(char *) * (count + 2)); // count + 2 because last NULL.. 
			else
				members = malloc(sizeof(char *) * (count + 2));

			// if we're copying the string, create an array entry with this element's starting location and length for later use
			if (flags & SPLITBUF_COPY)
			{
				parts = realloc(parts, sizeof(part) * (count + 2));
				parts[count].start = element;
				parts[count].len = p - element;
				totallen += parts[count].len + 1;
			}

			if (*p)
			{
				if (!(flags & SPLITBUF_COPY))
					*p = '\0';
				p += jumplen;
				// If we're grouping seperators, keep advancing forward to next non seperator
				if (flags & SPLITBUF_GROUP)
				{
					while (*p && ((flags & SPLITBUF_STRING) ? !mystrncmp(p, mysep, jumplen) : (mystrchr(mysep, *p) != NULL)))
						p += jumplen;
				}

				/* if we've reached the end of the string, we still need one more 
				   pass through the loop to create the final empty field:
				   "a:b:c:" should be {a, b, c, ""} */
				if (!*p)
					nobreak = 1; 
			}

			members[count] = element;
			element = p;
			count++;

			if (!*p && !nobreak)
				break;

			if (pieces > 0 && count + 1 >= pieces && *p)
				p += strlen(p);
		}
		else
		{
			if (*p && enclose && *enclose)
			{
				if (!escaped && strchr(enclose, *p))
				{
					stripchars++;
					enclosed = !enclosed;
				}
				if (enclosed && *p == '\\')
				{
					if (!escaped)
						stripchars++;
					escaped = !escaped;
				}
				else
					escaped = 0;
			}
			p++;
		}
		//printf("p = %c escaped = %i enclosed = %i\n", *p, escaped, enclosed);
	}

/*
	int x;
	for (x = 0; x < count; x++)
		printf("members[%i] = %s\n", x, members[x]);
*/

	if (flags & SPLITBUF_COPY)
	{
		int memberslen = (sizeof(char *) * (count + 1)) +
			(totallen * sizeof(char)) - 
			(((flags & SPLITBUF_STRIPENCLOSE) ? stripchars : 0) * sizeof(char));
/*
printf("memberslen %i = count %i (%li) + totallen %i (%li) - stripchars %i\n", 
	memberslen, 
	count + 1, (sizeof(char *) * (count + 1)), 
	totallen, (totallen * sizeof(char)), 
	stripchars);
*/

		members = realloc(members, memberslen);
	
		int i;
		// have to cast this beacuse I'm being tricky
		char *data = (char *)members + ((count + 1) * sizeof(char *));

		for (i = 0; i < count; i++)
		{
//printf("strcpy %i bytes from %s to %s\n", parts[i].len, parts[i].start, data);
			members[i] = data;
			//strncpy(data, parts[i].start, parts[i].len);
			int stripped = my_ignorecpy(data, parts[i].start, parts[i].len, 
					(flags & SPLITBUF_STRIPENCLOSE) ? enclose : "", flags & SPLITBUF_IGNORECASE);
			data += parts[i].len - stripped;
			*data++ = '\0';
		}

	}
	else if (flags & SPLITBUF_STRIPENCLOSE)
	{
		// TODO - strip enclose chars
		int x;
		for (x = 0; x < count; x++)
			my_ignorecpy(members[x], members[x], strlen(members[x]), enclose, flags & SPLITBUF_IGNORECASE);
	}

	members[count] = NULL; // create { "A", "B", NULL } null terminated array format
	*retmembers = members;
	if (parts && (flags & SPLITBUF_COPY))
		free(parts);
	if (mysep && (flags & SPLITBUF_IGNORECASE))
		free(mysep);

	return(count);
}

#ifdef _TEST_
int
main(int argc, char **argv)
{
	int rval;
	int flags, pieces;
	char **answer = NULL;

	if (argc != 6)
		printf("usage: %s <flags> <string> <sepchars> <enclosechars> <parts>\n", argv[0]), exit(0);

	printf("original: %s\n", argv[2]);

	flags = atoi(argv[1]);
	pieces = atoi(argv[5]);
	rval = splitbuf(flags, argv[2], argv[3], argv[4], pieces, &answer);

	//printf("rval %i\n", rval);

	int i;
	for (i = 0; i <= rval; i++)
		printf("field %i: %s\n", i, answer[i] ? answer[i] : "NULL");
		//printf("field %i (%X): %s\n", i, answer[i], answer[i] ? answer[i] : "NULL");
	printf("original now: %s\n", argv[2]);

	free(answer);

	return(rval);
}
#endif
