# $Id$

COMPSRCS += components/io/linux-io.c
COMPOBJS += linux-io.o

linux-io.o: components/io/linux-io.c $(HEADERS)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(OPTFLAGS) -c components/io/linux-io.c -o $@

