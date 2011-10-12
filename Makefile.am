#
# Copyright (c) Members of the EGEE Collaboration. 2004-2010.
# See http://www.eu-egee.org/partners/ for details on the copyright holders. 
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
# $Id$
#

#ACLOCAL_AMFLAGS = -I project
ACLOCAL_AMFLAGS = -I $(srcdir)/project

SUBDIRS = src 

if ENABLE_DEVEL
exampledir = $(docdir)/example
example_DATA = $(srcdir)/src/example/pep_client_example.c $(srcdir)/src/example/README
pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libargus-pep.pc
endif

#EXTRA_DIST = LICENSE CHANGELOG autotools.sh test/test_pep.c test/Makefile doc/html
EXTRA_DIST = LICENSE CHANGELOG autotools.sh doc/Doxyfile $(example_DATA)

docdir = $(datadir)/doc/$(PACKAGE_NAME)-$(PACKAGE_VERSION)

if ENABLE_LIBRARY
doc_DATA = AUTHORS CHANGELOG LICENSE README
endif


# Generate doxygen doc
doc: html
htmldir = $(docdir)/api
if ENABLE_DEVEL
doxygen: doc/Doxyfile
	$(DOXYGEN) $(srcdir)/doc/Doxyfile

html-local: doxygen

install-data-local: install-html
else
install-data-local:
endif

uninstall-local: uninstall-html

install-html:
	test -d "$(htmldir)" || $(mkdir_p) "$(DESTDIR)$(htmldir)"
	@if test -d "doc/html"; then \
	    for file in doc/html/* ; do \
	        if test -f "$$file"; then \
	            f=`basename $$file`; \
	            echo "$(INSTALL_DATA) '$$file' '$(DESTDIR)$(htmldir)/$$f'"; \
	            $(INSTALL_DATA) "$$file" "$(DESTDIR)$(htmldir)/$$f"; \
	        fi \
	    done \
	fi

uninstall-html:
	$(RM) -fr $(htmldir) 

distclean-local:
	$(RM) -fr doc/html doc/man
