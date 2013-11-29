all: test liberic.so
test: cleanline getpw splitbuf readfiles

OUTFILES=cleanline getpw splitbuf readfiles liberic.so

CFLAGS=-Wall -Wextra -Wno-unused-parameter -fPIC
LDFLAGS=
LIBS=

liberic.so: cleanline.o getpw.o splitbuf.o searchreplace.o readfiles.o
	gcc -shared cleanline.o getpw.o splitbuf.o searchreplace.o readfiles.o -o liberic.so

cleanline: cleanline.c
	gcc -D_TEST_ $(CFLAGS) -o cleanline cleanline.c 

getpw: getpw.c cleanline.o
	gcc -D_TEST_ $(CFLAGS) -o getpw getpw.c cleanline.o

splitbuf: splitbuf.c
	gcc -D_TEST_ $(CFLAGS) -o splitbuf splitbuf.c

readfiles: readfiles.c cleanline.o
	gcc -D_TEST_ $(CFLAGS) -o readfiles readfiles.c cleanline.o

clean:
	rm -f *.o $(OUTFILES)

install: all
	cp *.h /usr/local/include
	cp *.so /usr/local/lib
	ldconfig

.c.o:
	gcc -c $(CFLAGS) $<
