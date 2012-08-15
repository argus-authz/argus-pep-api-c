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

#ACLOCAL_AMFLAGS = -I project
#ACLOCAL_AMFLAGS = -I $(srcdir)/project

SUBDIRS = src 

if ENABLE_DEVEL
exampledir = $(docdir)/example
example_DATA = $(srcdir)/src/example/pep_client_example.c $(srcdir)/src/example/README
pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libargus-pep.pc
endif

EXTRA_DIST = fedora debian LICENSE ChangeLog autotools.sh doc/Doxyfile $(example_DATA)

docdir = $(datadir)/doc/$(PACKAGE_NAME)-$(PACKAGE_VERSION)

if ENABLE_LIBRARY
doc_DATA = AUTHORS ChangeLog LICENSE README
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

#
# RPM/ETICS
#
name = argus-pep-api-c
spec_file = fedora/$(name)$(shell rpm --eval "%?dist").spec
version = $(shell grep "Version:" $(spec_file) | sed -e "s/Version://g" -e "s/[ \t]*//g")
release = 1
rpmbuild_dir = $(shell pwd)/rpmbuild
debbuild_dir = $(shell pwd)/debbuild
stage_dir = $(shell pwd)/stage

clean-local:
	@echo "Cleaning..."
	rm -fr rpmbuild debbuild stage tgz RPMS target *.tar.gz

rpm: dist
	rpm --version
	@echo "Building RPM with $(spec_file)"
	mv $(name)-$(version).tar.gz $(name)-$(version).src.tar.gz
	mkdir -p $(rpmbuild_dir)/BUILD $(rpmbuild_dir)/RPMS $(rpmbuild_dir)/SOURCES $(rpmbuild_dir)/SPECS $(rpmbuild_dir)/SRPMS
	cp $(name)-$(version).src.tar.gz $(rpmbuild_dir)/SOURCES/$(name)-$(version).tar.gz
	rpmbuild -v -ba $(spec_file) --define "_topdir $(rpmbuild_dir)"

debian: dist
	debuild --version
	mv $(name)-$(version).tar.gz $(name)-$(version).src.tar.gz
	@echo "Building Debian package in $(debbuild_dir)"
	mkdir -p $(debbuild_dir)
	cp $(name)-$(version).src.tar.gz $(debbuild_dir)/$(name)_$(version).orig.tar.gz
	tar -C $(debbuild_dir) -xf $(name)-$(version).src.tar.gz
	cp -r debian $(debbuild_dir)/$(name)-$(version)
	cd $(debbuild_dir)/$(name)-$(version) && debuild -us -uc 

etics: 
	@echo "Publishing RPMs and tarballs"
	mkdir -p RPMS tgz
	test -f $(rpmbuild_dir)/SRPMS/$(name)-$(version)-*.src.rpm
	cp -r $(rpmbuild_dir)/RPMS/* $(rpmbuild_dir)/SRPMS/* RPMS
	test -f $(name)-$(version).src.tar.gz && cp $(name)-$(version).src.tar.gz tgz
	test ! -f $(name)-$(version).bin.tar.gz || cp $(name)-$(version).bin.tar.gz tgz/$(name)-$(version).tar.gz


