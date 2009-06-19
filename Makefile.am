#
# $Id: Makefile.am,v 1.11 2009/02/27 12:39:27 vtschopp Exp $
#

SUBDIRS = src 
EXTRA_DIST = LICENSE CHANGELOG autotools.sh test/test_pep.c test/Makefile doc/html

docdir = $(datadir)/doc/$(PACKAGE)
doc_DATA = AUTHORS CHANGELOG LICENSE

# Generate doxygen doc
doxygen: doc/Doxyfile
	$(DOXYGEN) doc/Doxyfile

html-local: doxygen

# Install doxygen doc in $(prefix)/share/doc/glite-authz-pep-c/api
htmldir = $(docdir)/api
install-data-local: install-html

uninstall-local: uninstall-html

install-html: 
	[ -d "$(htmldir)" ] || $(mkinstalldirs) $(htmldir)
	if [ -d "doc/html" ] ; then \
	for file in doc/html/* ; do \
		[ -f "$$file" ] && $(INSTALL_DATA) $$file $(htmldir)/`basename $$file`; \
	done \
	fi

uninstall-html:
	$(RM) -fr $(htmldir) 

distclean-local:
	$(RM) -fr doc/html doc/man
