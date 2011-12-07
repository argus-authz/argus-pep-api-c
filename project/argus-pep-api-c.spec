Summary: Argus PEP client library
Name: argus-pep-api-c
Version: 2.1.0
Release: 1%{?dist}
License: ASL 2.0
Group: System Environment/Libraries
URL: https://twiki.cern.ch/twiki/bin/view/EGEE/AuthorizationFramework
Source: %{name}-%{version}.tar.gz
#Source0: http://www.switch.ch/grid/authz/emi.argus.pep-api-c/%{name}-%{version}.tar.gz
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-buildroot
BuildRequires: curl-devel

%description
The Argus PEP client API for C is a multi-thread friendly client library used to 
communicate with the Argus PEP Server. It authorizes request and receives
authorization response back from Argus.

%package devel
Group: Development/Libraries
Summary: Argus PEP client development libraries
Requires: %{name} = %{version}-%{release}

%description devel
The Argus PEP client API for C is a multi-thread friendly client library used to 
communicate with the Argus PEP Server. It authorizes request and receives
authorization response back from Argus.

This package contains the development libraries.

%prep
%setup -q

%build
%configure

# The following two lines were suggested by
# https://fedoraproject.org/wiki/Packaging/Guidelines to prevent any
# RPATHs creeping in.
sed -i 's|^hardcode_libdir_flag_spec=.*|hardcode_libdir_flag_spec=""|g' libtool
sed -i 's|^runpath_var=LD_RUN_PATH|runpath_var=DIE_RPATH_DIE|g' libtool

make %{?_smp_mflags}

%install
rm -rf $RPM_BUILD_ROOT

make DESTDIR=$RPM_BUILD_ROOT install
find $RPM_BUILD_ROOT -name '*.la' -exec rm -f {} ';'

# clean up installed documentation, will be done using doc macro
rm -rf $RPM_BUILD_ROOT%{_datadir}/doc/%{name}-%{version}

%clean
rm -rf $RPM_BUILD_ROOT

%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig

%files
%defattr(-,root,root,-)
%{_libdir}/libargus-pep.so.2.0.3
%{_libdir}/libargus-pep.so.2

%doc AUTHORS LICENSE README CHANGELOG
%doc src/example

%files devel
%defattr(-,root,root,-)
%{_libdir}/libargus-pep.so
%{_libdir}/pkgconfig/libargus-pep.pc
%dir %{_includedir}/argus
%{_includedir}/argus/error.h
%{_includedir}/argus/xacml.h
%{_includedir}/argus/pep.h
%{_includedir}/argus/pip.h
%{_includedir}/argus/oh.h
%{_includedir}/argus/profiles.h

%changelog
* Tue Dec 6 2011 Valery Tschopp <valery.tschopp@switch.ch> 2.1.0-1
- New package for version 2.1.0
* Tue Oct 4 2011 Mischa Salle <msalle@nikhef.nl> 2.0.3-2
- Initial build.


