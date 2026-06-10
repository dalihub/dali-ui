Name:       com.samsung.dali.chart-memory-probe
Summary:    DALi UI ChartView memory probe
Version:    1.0.0
Release:    1
Group:      System/Libraries
License:    Apache-2.0
URL:        https://github.com/dalihub/dali-ui
Source0:    %{name}-%{version}.tar.gz

Requires(post): /sbin/ldconfig
Requires(postun): /sbin/ldconfig

BuildRequires:  cmake
BuildRequires:  pkgconfig
BuildRequires:  pkgconfig(capi-appfw-application)
BuildRequires:  pkgconfig(capi-appfw-app-control)
BuildRequires:  pkgconfig(dlog)
BuildRequires:  pkgconfig(dali2-core)
BuildRequires:  pkgconfig(dali2-adaptor)
BuildRequires:  dali2-integration-devel
BuildRequires:  dali2-adaptor-integration-devel
BuildRequires:  pkgconfig(dali2-ui-foundation)
BuildRequires:  pkgconfig(dali2-ui-components)

%description
DALi UI ChartView memory usage measurement probe (18-phase).
Measures RSS, heap, and GPU memory across chart creation/destruction cycles.

##############################
# Preparation
##############################
%prep
%setup -q

%define app_root_dir     samples/chart-view/
%define app_ro_dir       %TZ_SYS_RO_APP/%{name}/
%define xml_file_dir     %TZ_SYS_RO_PACKAGES

%define app_exe_dir      %{app_ro_dir}/bin/

##############################
# Build
##############################
%build
PREFIX="/usr"
CXXFLAGS+=" -Wall -g -O2"
LDFLAGS+=" -Wl,--rpath=$PREFIX/lib -Wl,--as-needed"

%ifarch %{arm}
CXXFLAGS+=" -D_ARCH_ARM_"
%endif

cd %{_builddir}/%{name}-%{version}/%{app_root_dir}

cmake -DCMAKE_INSTALL_PREFIX=%{app_ro_dir}\
      -DENABLE_TRACE:BOOL=ON \
      -DINTERNATIONALIZATION:BOOL=OFF \
      -DTIZEN:BOOL=ON \
%if 0%{?enable_debug}
      -DCMAKE_BUILD_TYPE=Debug \
%endif
      .

make chart-memory-probe %{?jobs:-j%jobs}

##############################
# Installation
##############################
%install
rm -rf %{buildroot}

mkdir -p %{buildroot}%{app_exe_dir}
cp -f %{_builddir}/%{name}-%{version}/%{app_root_dir}/bin/chart-memory-probe \
      %{buildroot}%{app_exe_dir}

mkdir -p %{buildroot}%{xml_file_dir}
cp -f %{_builddir}/%{name}-%{version}/%{app_root_dir}/%{name}.xml \
      %{buildroot}%{xml_file_dir}

##############################
# Post Install
##############################
%post
/sbin/ldconfig
exit 0

##############################
# Post Uninstall
##############################
%postun
/sbin/ldconfig
exit 0

##############################
# Files in Binary Packages
##############################

%files
%manifest %{app_root_dir}/%{name}.manifest
%defattr(-,root,root,-)
%{app_exe_dir}/chart-memory-probe
%{xml_file_dir}/%{name}.xml
