#ifndef _SPLITBUF_H_

#define SPLITBUF_COPY 1
#define SPLITBUF_GROUP 2
#define SPLITBUF_STRING 4
#define SPLITBUF_IGNORECASE 8
#define SPLITBUF_STRIPENCLOSE 16
#define SPLITBUF_ALL 31

extern int splitbuf(int, char *, char *, char *, int, char ***);

#define _SPLITBUF_H_
#endif
