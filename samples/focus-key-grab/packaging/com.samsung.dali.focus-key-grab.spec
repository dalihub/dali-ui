Name:       com.samsung.dali.focus-key-grab
Summary:    DALi UI inactive Window focus and key grab probe
Version:    2.1.0
Release:    1
Group:      System/Libraries
License:    Apache-2.0
URL:        https://github.com/dalihub/dali-ui
Source0:    %{name}-%{version}.tar.gz

BuildRequires: cmake
BuildRequires: pkgconfig
BuildRequires: pkgconfig(capi-appfw-application)
BuildRequires: pkgconfig(capi-appfw-app-control)
BuildRequires: pkgconfig(dlog)
BuildRequires: pkgconfig(dali2-core)
BuildRequires: pkgconfig(dali2-adaptor)
BuildRequires: dali2-integration-devel
BuildRequires: dali2-adaptor-integration-devel
BuildRequires: pkgconfig(dali2-ui-foundation)
BuildRequires: pkgconfig(dali2-ui-components)

%description
Observes native key delivery to two Windows and their Views while an inactive
sub Window grabs navigation keys exclusively and receives a View focus request.

%prep
%setup -q

%define app_root_dir samples/focus-key-grab
%define app_ro_dir   %TZ_SYS_RO_APP/%{name}
%define xml_file_dir %TZ_SYS_RO_PACKAGES

%build
cd %{app_root_dir}
cmake -S . -B build \
      -DCMAKE_INSTALL_PREFIX=%{app_ro_dir} \
      -DCMAKE_BUILD_TYPE=RelWithDebInfo \
      -DTIZEN:BOOL=ON
cmake --build build %{?jobs:--parallel %jobs}

%install
rm -rf %{buildroot}
cd %{app_root_dir}
DESTDIR=%{buildroot} cmake --build build --target install
mkdir -p %{buildroot}%{xml_file_dir}
cp %{name}.xml %{buildroot}%{xml_file_dir}/%{name}.xml

%files
%manifest %{app_root_dir}/%{name}.manifest
%defattr(-,root,root,-)
%{app_ro_dir}/bin/focus-key-grab.example
%{xml_file_dir}/%{name}.xml
