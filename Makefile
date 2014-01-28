all: test liberic.so
test: cleanline getpw splitbuf readfiles

OUTFILES=cleanline getpw splitbuf readfiles liberic.so

CFLAGS=-Wall -Wextra -Wno-unused-parameter -fPIC 
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

clean:
	rm -f *.o $(OUTFILES)

install: all
	cp *.h /usr/local/include
	cp *.so /usr/local/lib
	ldconfig

.c.o:
	gcc -c $(CFLAGS) $<
