all: test liberic.so errno
test: cleanline getpw splitbuf readfiles

OUTFILES=cleanline getpw splitbuf readfiles
OUTLIBS=liberic.so
OUTPROGS=errno

CFLAGS=-Wall -Wextra -Wno-unused-parameter -fPIC 
#CFLAGS=-Wall -Wextra -Wno-unused-parameter -fPIC -g3
LDFLAGS=
LIBS=

liberic.so: cleanline.o getpw.o splitbuf.o searchreplace.o readfiles.o debug.o
	gcc -shared cleanline.o getpw.o splitbuf.o searchreplace.o readfiles.o debug.o -o liberic.so

cleanline: cleanline.c debug.o
	gcc -D_TEST_ $(CFLAGS) -o cleanline cleanline.c  debug.o

getpw: getpw.c cleanline.o debug.o
	gcc -D_TEST_ $(CFLAGS) -o getpw getpw.c cleanline.o debug.o

splitbuf: splitbuf.c debug.o
	gcc -D_TEST_ $(CFLAGS) -o splitbuf splitbuf.c debug.o

readfiles: readfiles.c cleanline.o splitbuf.o debug.o
	gcc -D_TEST_ $(CFLAGS) -o readfiles readfiles.c cleanline.o splitbuf.o debug.o

errno: errno.c
	gcc $(CFLAGS) -o errno errno.c

clean:
	rm -f *.o $(OUTFILES) $(OUTLIBS) $(OUTPROGS)

install: all
	cp *.h /usr/local/include
	cp $(OUTLIBS) /usr/local/lib
	cp $(OUTPROGS) /usr/local/bin
	ldconfig

.c.o:
	gcc -c $(CFLAGS) $<
