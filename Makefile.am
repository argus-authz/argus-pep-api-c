#
# $Id: Makefile.am,v 1.4 2009/02/11 13:27:28 vtschopp Exp $
#

SUBDIRS = src 
EXTRA_DIST = LICENSE CHANGELOG autotools.sh project/version.properties test/test_pep.c test/Makefile
doc_DATA = AUTHORS CHANGELOG LICENSE

# Generate doxygen doc
doxygen: doc/Doxyfile
	$(DOXYGEN) doc/Doxyfile

# Install doxygen doc
install-data-local: install-html

uninstall-local: uninstall-html

install-html: doxygen
	for file in doc/html/*.html ; do \
		$(INSTALL_DATA) $$file $(docdir)/`basename $$file`; \
	done

uninstall-html:
	$(RM) $(docdir)/*.html

distclean-local:
	$(RM) -fr doc/html doc/man