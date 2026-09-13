Name:       com.samsung.dali.icon-button
Summary:    DALi UI IconButton sample
Version:    2.0.0
Release:    1
Group:      System/Libraries
License:    Apache-2.0
URL:        https://github.com/dalihub/dali-ui
Source0:    %{name}-%{version}.tar.gz

BuildRequires:  cmake
BuildRequires:  pkgconfig
BuildRequires:  pkgconfig(dali2-core)
BuildRequires:  pkgconfig(dali2-adaptor)
BuildRequires:  pkgconfig(dali2-ui-foundation)
BuildRequires:  pkgconfig(dali2-ui-components)

%description
DALi UI IconButton sample application.

%prep
%setup -q

%define app_root_dir samples/icon-button/
%define app_ro_dir %TZ_SYS_RO_APP/%{name}/
%define xml_file_dir %TZ_SYS_RO_PACKAGES

%build
cd %{_builddir}/%{name}-%{version}/%{app_root_dir}
cmake -DCMAKE_INSTALL_PREFIX=%{app_ro_dir} -DTIZEN:BOOL=ON .
make %{?jobs:-j%jobs}

%install
rm -rf %{buildroot}
cd %{app_root_dir}
%make_install
mkdir -p %{buildroot}%{xml_file_dir}
cp -f %{_builddir}/%{name}-%{version}/%{app_root_dir}/%{name}.xml %{buildroot}%{xml_file_dir}

%files
%manifest %{app_root_dir}/%{name}.manifest
%defattr(-,root,root,-)
%{app_ro_dir}/bin/icon-button.example
%{xml_file_dir}/%{name}.xml
