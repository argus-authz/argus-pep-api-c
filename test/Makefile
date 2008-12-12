#
# $Id: Makefile,v 1.2 2008/12/12 14:06:39 vtschopp Exp $
#
GLITE_LOCATION=/opt/glite

CC=gcc 
CFLAGS=-Wall -I$(GLITE_LOCATION)/include
LDFLAGS=-L$(GLITE_LOCATION)/lib -lpep-c

SOURCES=test_pep.c
OBJECTS=$(SOURCES:.c=.o)
EXEC=test-pep

all: $(EXEC)

$(EXEC): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

clean:
	rm -f $(OBJECTS) $(EXEC)


