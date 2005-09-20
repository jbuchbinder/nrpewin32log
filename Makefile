#	$Id$
#	$Author$

CFLAGS += -g -O2

all: clean win32log.exe

.c.o:
	gcc $(CFLAGS) -o $@ -c $<

clean:
	rm -f *.o *.exe *.offset *.stackdump test/*.o

win32log.exe: regex.o match_count.o read_file.o glob.o tail.o main.o
	gcc -static $(CFLAGS) -o win32log.exe main.o glob.o tail.o read_file.o match_count.o regex.o


test_config.exe: test/test_config.o
	gcc $(CFLAGS) -o test_config.exe test/test_config.o
