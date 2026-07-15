Name:       com.samsung.dali.markdown
Summary:    DALi UI Markdown sample
Version:    2.0.0
Release:    1
Group:      System/Libraries
License:    Apache-2.0
URL:        https://github.com/dalihub/dali-ui
Source0:    %{name}-%{version}.tar.gz

Requires(post): /sbin/ldconfig
Requires(postun): /sbin/ldconfig

BuildRequires:  cmake
BuildRequires:  gettext-tools
BuildRequires:  pkgconfig
BuildRequires:  pkgconfig(capi-appfw-application)
BuildRequires:  pkgconfig(capi-appfw-app-control)
BuildRequires:  pkgconfig(dlog)
BuildRequires:  pkgconfig(dali2-core)
BuildRequires:  pkgconfig(dali2-adaptor)
BuildRequires:  pkgconfig(dali2-ui-foundation)
BuildRequires:  pkgconfig(dali2-ui-components)

%description
DALi UI Markdown sample application.

##############################
# Preparation
##############################
%prep
%setup -q

%define app_root_dir     samples/markdown/
%define app_ro_dir       %TZ_SYS_RO_APP/%{name}/
%define xml_file_dir     %TZ_SYS_RO_PACKAGES

%define app_res_dir      %{app_ro_dir}/res/
%define app_exe_dir      %{app_ro_dir}/bin/
%define locale_dir       %{app_res_dir}/locale

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

make %{?jobs:-j%jobs}

##############################
# Installation
##############################
%install
rm -rf %{buildroot}
cd %{_builddir}/%{name}-%{version}/%{app_root_dir}
%make_install

mkdir -p %{buildroot}%{xml_file_dir}
cp -f %{_builddir}/%{name}-%{version}/%{app_root_dir}/%{name}.xml %{buildroot}%{xml_file_dir}

mkdir -p %{buildroot}%{app_res_dir}
cp -a %{_builddir}/%{name}-%{version}/%{app_root_dir}/res/. %{buildroot}%{app_res_dir}

# Source PO files are not needed at runtime. Runtime uses generated MO files.
rm -rf %{buildroot}%{app_res_dir}/po

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
%{app_exe_dir}/markdown-view.example
%{app_res_dir}/*
%{xml_file_dir}/%{name}.xml
