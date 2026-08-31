Name:       com.samsung.dali.window-frame
Summary:    DALi UI WindowFrame sample
Version:    2.0.0
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
BuildRequires:  coregl

%description
DALi UI sample application demonstrating WindowFrame with a customizable partial window border.

##############################
# Preparation
##############################
%prep
%setup -q

%define app_root_dir     samples/window-frame/
%define app_ro_dir       %TZ_SYS_RO_APP/%{name}/
%define xml_file_dir     %TZ_SYS_RO_PACKAGES

%define app_res_dir      %{app_ro_dir}/res/
%define app_exe_dir      %{app_ro_dir}/bin/

##############################
# Build
##############################
%build
PREFIX="/usr"
CXXFLAGS+=" -Wall -g -O2"
LDFLAGS+=" -Wl,--rpath=$PREFIX/lib -Wl,--as-needed"

%ifarch %{arm}
CFLAGS+=" -fno-use-linker-plugin"
CXXFLAGS+=" -D_ARCH_ARM_ -fno-use-linker-plugin"
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

make %{?jobs:-j%jobs}

##############################
# Installation
##############################
%install
rm -rf %{buildroot}
cd %{app_root_dir}
%make_install

mkdir -p %{buildroot}%{xml_file_dir}
cp -f %{_builddir}/%{name}-%{version}/%{app_root_dir}/%{name}.xml %{buildroot}%{xml_file_dir}

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
%{app_exe_dir}/window-frame.example
%{app_res_dir}/*
%{xml_file_dir}/%{name}.xml
