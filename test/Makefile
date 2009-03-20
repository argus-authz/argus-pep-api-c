#
# $Id: Makefile,v 1.4 2009/03/20 09:32:12 vtschopp Exp $
#
GLITE_LOCATION ?= /opt/glite

CC = gcc 
CFLAGS = -Wall -I$(GLITE_LOCATION)/include
LDFLAGS = -L$(GLITE_LOCATION)/lib -L$(GLITE_LOCATION)/lib64 -lpep-c

SOURCES = test_pep.c
OBJECTS = $(SOURCES:.c=.o)
EXEC = test-pep

all: $(EXEC)

$(EXEC): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

clean:
	rm -f $(OBJECTS) $(EXEC)


