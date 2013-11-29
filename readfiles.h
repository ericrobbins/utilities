#ifndef _READFILES_H_
#define _READFILES_H_

#define READFILES_NOCOMMENTS 1
#define READFILES_CLEANLINE 2
#define READFILES_NONL 4
#define READFILES_ALL 255

char *read_line(FILE *, int);
char *readwholefile(char *, int);

#endif
