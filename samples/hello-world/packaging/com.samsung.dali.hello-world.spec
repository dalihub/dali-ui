Name:       com.samsung.dali.hello-world
Summary:    The DALi UI smaple
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
BuildRequires:  pkgconfig(dali2-ui-foundation)
BuildRequires:  pkgconfig(dali2-ui-elements)
BuildRequires:  pkgconfig(dali2-ui-components)

%description
The DALi UI smaple

##############################
# Preparation
##############################
%prep
%setup -q

%define app_root_dir     samples/hello-world/
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
cd %{app_root_dir}
%make_install

mkdir -p %{buildroot}%{xml_file_dir}
cp -f %{_builddir}/%{name}-%{version}/%{app_root_dir}/%{name}.xml %{buildroot}%{xml_file_dir}

mkdir -p %{buildroot}%{app_res_dir}
cp -fr  res/* %{buildroot}%{app_res_dir}

#mkdir -p %{buildroot}%{icon_dir}
#cp -rf %{buildroot}/res %{buildroot}%{app_res_dir}
#mv %{buildroot}/res/images/%{name}.png %{buildroot}%{icon_dir}
#mv %{buildroot}/%{app_res_dir}/images/dali-examples.png %{buildroot}%{icon_dir}
#mv %{buildroot}/%{app_res_dir}/images/dali-tests.png %{buildroot}%{icon_dir}

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

# Files in Binary Packages
##############################

%files
%manifest %{app_root_dir}/%{name}.manifest
%defattr(-,root,root,-)
%{app_exe_dir}/hello-world.example
%{app_res_dir}/*
%{xml_file_dir}/%{name}.xml
#%license LICENSE
