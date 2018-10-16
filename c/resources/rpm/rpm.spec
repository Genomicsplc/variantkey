# SPEC file

%global c_vendor    %{_vendor}
%global gh_owner    %{_owner}
%global gh_cvspath  %{_cvspath}
%global gh_project  %{_project}

Name:      %{_package}
Version:   %{_version}
Release:   %{_release}%{?dist}
Summary:   Numerical Encoding for Human Genetic Variants.

Group:     Applications/Services
License:   %{_docpath}/LICENSE
URL:       https://%{gh_cvspath}/%{gh_project}

BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-%(%{__id_u} -n)

Provides:  %{gh_project} = %{version}

%description
Provides C header-only files for:
VariantKey, a reversible numerical encoding schema for human genetic variants.
RegionKey, a reversible numerical encoding schema for human genetic regions.
ESID, a reversible numerical encoding schema for genetic string identifiers.

%build
#(cd %{_current_directory} && make build)

%install
rm -rf $RPM_BUILD_ROOT
(cd %{_current_directory} && make install DESTDIR=$RPM_BUILD_ROOT)

#%clean
#rm -rf $RPM_BUILD_ROOT
#(cd %{_current_directory} && make clean)

%files
%attr(-,root,root) %{_libpath}
%attr(-,root,root) %{_docpath}
%docdir %{_docpath}

%changelog
* Tue Oct 16 2018 GENOMICS plc <support@genomicsplc.com> 5.1.1-1
- First RPM package.
