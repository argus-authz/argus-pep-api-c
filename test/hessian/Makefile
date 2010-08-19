#
# Copyright (c) Members of the EGEE Collaboration. 2008.
# See http://www.eu-egee.org/partners for details on the copyright holders. 
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
# $Id: Makefile 1646 2010-03-22 14:10:12Z vtschopp $
#
ifndef GLITE_LOCATION
GLITE_LOCATION=/opt/glite
endif

CC=gcc 
CFLAGS=-Wall -I../../src -I$(GLITE_LOCATION)/include
LDFLAGS=-L$(GLITE_LOCATION)/lib -L$(GLITE_LOCATION)/lib64 -lpep-c

SOURCES=test_hessian.c
OBJECTS=$(SOURCES:.c=.o)
EXEC=test_hessian

all: $(EXEC)

$(EXEC): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

clean:
	rm -f $(OBJECTS) $(EXEC)


