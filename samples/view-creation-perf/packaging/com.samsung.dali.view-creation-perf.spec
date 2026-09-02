Name:       com.samsung.dali.view-creation-perf
Summary:    DALi UI View creation performance sample
Version:    2.0.0
Release:    1
Group:      System/Libraries
License:    Apache-2.0
URL:        https://github.com/dalihub/dali-ui
Source0:    %{name}-%{version}.tar.gz

BuildRequires:  cmake
BuildRequires:  pkgconfig
BuildRequires:  pkgconfig(capi-appfw-application)
BuildRequires:  pkgconfig(capi-appfw-app-control)
BuildRequires:  pkgconfig(dlog)
BuildRequires:  pkgconfig(dali2-core)
BuildRequires:  pkgconfig(dali2-adaptor)
BuildRequires:  pkgconfig(dali2-ui-foundation)
BuildRequires:  pkgconfig(dali2-ui-components)

%description
DALi UI sample measuring creation of 100 or 10000 View objects.

%prep
%setup -q

%define app_root_dir samples/view-creation-perf/
%define app_ro_dir   %TZ_SYS_RO_APP/%{name}/
%define xml_file_dir %TZ_SYS_RO_PACKAGES
%define app_res_dir  %{app_ro_dir}/res/
%define app_exe_dir  %{app_ro_dir}/bin/

%build
PREFIX="/usr"
CXXFLAGS+=" -Wall -g -O2"
LDFLAGS+=" -Wl,--rpath=$PREFIX/lib -Wl,--as-needed"

%ifarch %{arm}
CXXFLAGS+=" -D_ARCH_ARM_"
%endif

cd %{_builddir}/%{name}-%{version}/%{app_root_dir}
cmake -DCMAKE_INSTALL_PREFIX=%{app_ro_dir} \
      -DENABLE_TRACE:BOOL=OFF \
      -DINTERNATIONALIZATION:BOOL=OFF \
      -DTIZEN:BOOL=ON \
      .
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
%{app_exe_dir}/view-creation-perf.example
%{xml_file_dir}/%{name}.xml
