#
# $Id: Makefile.am,v 1.7 2009/02/20 14:25:34 vtschopp Exp $
#

SUBDIRS = src 
EXTRA_DIST = LICENSE CHANGELOG autotools.sh project/version.properties test/test_pep.c test/Makefile doc/html
doc_DATA = AUTHORS CHANGELOG LICENSE

# Generate doxygen doc
doxygen: doc/Doxyfile
	$(DOXYGEN) doc/Doxyfile

html-local: doxygen

# Install doxygen doc in $(prefix)/share/doc/glite-authz-pep-c/api
htmldir = $(datarootdir)/doc/$(PACKAGE)/api
install-data-local: install-html

uninstall-local: uninstall-html

install-html: 
	$(mkinstalldirs) $(htmldir)
	if [ -f "doc/html/index.html" ] ; then \
	for file in doc/html/*.html doc/html/*.css doc/html/*.png doc/html/*.gif ; do \
		$(INSTALL_DATA) $$file $(htmldir)/`basename $$file`; \
	done \
	fi

uninstall-html:
	$(RM) -fr $(htmldir) 

distclean-local:
	$(RM) -fr doc/html/*.html doc/html/*.css doc/html/*.png doc/html/*.gif doc/man
