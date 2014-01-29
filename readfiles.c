#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "readfiles.h"
#include "cleanline.h"
#include "splitbuf.h"
#include "debug.h"

static void
memstrcat(char *dst, char *src)
{
	int dlen = strlen(dst);
	int slen = strlen(src);

	//debug(3, "memmove(%x, %x, %i)\n", dst + dlen, src, slen);
	debug(3, "memmove(%s, %s, %i)\n", dst + dlen, src, slen);
	memmove(dst + dlen, src, slen);
	dst[dlen + slen] = '\0';
}

char *
read_line(FILE *infile, int options)
{
	char buf[1024];
	char *rbuf;
	char *newbuf = NULL;
	char *tmp;

	while (1)
	{
		rbuf = fgets(buf, sizeof(buf), infile);
		if (!rbuf)
			return(NULL); // EOF, or i/o error, either way we're done here.

		// if realloc fails, we have a problem.
		if (newbuf)
			newbuf = realloc(newbuf, strlen(buf) + strlen(newbuf) + 1);
		else
			newbuf = calloc(strlen(buf) + 1, 1);

		memstrcat(newbuf, buf);

		tmp = strchr(newbuf, '\n');
		if (!tmp)
		{
			debug(2, "%s\n", "no newline found, reading more");
			continue; // no newline... line in file was longer than sizeof(buf), so we must read more
		}

		if (options & READFILES_NONL)
		{
			*tmp = '\0';
			tmp = strchr(newbuf, '\r'); // if file is CRLF formatted
			if (tmp)
				*tmp = '\0';
		}

		return(newbuf);
	}
}

static int
in_array(char **array, char *str)
{
	int i;

	for (i = 0; array[i]; i++)
	{
		if (array[i][0] && !strncmp(str, array[i], strlen(array[i])))
			return 1;
	}

	return 0;
}

char *
readwholefile(char *filename, char *commentsplit, int options)
{
	char *buf = NULL;
	FILE *infile;
	char *thisline;
	char **comments = NULL;

	infile = fopen(filename, "r");
	if (!infile)
		return(NULL);

	int inquote = 0;
	int escape = 0;

	splitbuf(SPLITBUF_COPY, commentsplit ? commentsplit : "#|//", "|", "", 0, &comments);

	while ((thisline = read_line(infile, 0)) != NULL)
	{
		if (options & READFILES_NOCOMMENTS)
		{
			char *eol = strchr(thisline, '\r');
			if (!eol)
				eol = strchr(thisline, '\n');

			char *x;
			
			for (x = thisline; x && *x; )
			{
				if ((*x == '"' || *x == '\'') && !escape)
				{
					if (*x == inquote)
						inquote = 0;
					else
						inquote = *x;
				}

				if (!inquote && in_array(comments, x))
				{
					*x = '\0';
					x = NULL; // will exit loop
				}
				else
				{
					if (*x == '\\')
						escape = 1;
					else
						escape = 0;
					x++;
				}
			}
#if 0
			char *tmp = strchr(thisline, '#');
			if (tmp)
				*tmp = '\0';
			char *tmp2 = strstr(thisline, "//");
			if (tmp2)
				*tmp2 = '\0';
#endif

			// if we're keeping newlines, and there was a comment in the line, put the newline back
			//if (eol && (tmp || tmp2) && !(options & READFILES_NONL))
			if (eol && x == NULL && !(options & READFILES_NONL))
				memstrcat(thisline, eol);
		}
		if (options & READFILES_CLEANLINE)
		{
			if (options & READFILES_NONL)
				cleanline(thisline, CLEAN_BEGIN|CLEAN_END|CLEAN_NL);
			else
				cleanline(thisline, CLEAN_BEGIN|CLEAN_END);
		}

		if (buf)
		{
			buf = realloc(buf, strlen(thisline) + strlen(buf) + 2);
			if ((options & READFILES_NONL) && *thisline && *buf) // skip blank line
				strcat(buf, " ");
		}
		else
		{
			buf = malloc(strlen(thisline) + 1);
			*buf = '\0';
		}
		// skip blank line 
		if (*thisline && strcmp(thisline, "\r\n") && *thisline != '\n')
			strcat(buf, thisline);
		free(thisline);
	}
	fclose(infile);
	if (comments)
		free(comments);
	return(buf);
}

#ifdef _TEST_
int
main(int argc, char **argv)
{
	char *filebuf;
	int option = atoi(argv[3]);

	filebuf = readwholefile(argv[1], argv[2], option);
	printf("read in:\n%s\n", filebuf);
	free(filebuf);
	
	return(0);
}	
#endif
