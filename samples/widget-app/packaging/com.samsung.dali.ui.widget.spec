%{!?dali_api_major:%global dali_api_major 3}

Name:       com.samsung.dali.ui.widget
Summary:    DALI UI widget sample provider
Version:    1.0.0
Release:    1
Group:      System/Libraries
License:    Apache-2.0
URL:        https://github.com/dalihub/dali-ui
Source0:    %{name}-%{version}.tar.gz

BuildRequires:  cmake
BuildRequires:  pkgconfig
BuildRequires:  pkgconfig(dali%{dali_api_major}-core)
BuildRequires:  pkgconfig(dali%{dali_api_major}-adaptor)
BuildRequires:  pkgconfig(dali%{dali_api_major}-ui-foundation)
BuildRequires:  dali%{dali_api_major}-integration-devel
BuildRequires:  dali%{dali_api_major}-adaptor-integration-devel
BuildRequires:  pkgconfig(bundle)
BuildRequires:  pkgconfig(dlog)
BuildRequires:  pkgconfig(message-port)

%description
DALI UI widget sample provider.
Demonstrates resize, ContentInfo, MessagePort IPC, input forwarding,
and fault simulation using Dali::WidgetApplication with Dali UI controls.

##############################
# Preparation
##############################
%prep
%setup -q

%define app_root_dir    samples/widget-app/
%define app_ro_dir      /usr/apps/%{name}
%define xml_file_dir    /usr/share/packages
%define app_res_dir     %{app_ro_dir}/res
%define app_exe_dir     %{app_ro_dir}/bin

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

cmake -DDALI_API_MAJOR=%{dali_api_major} \
      -DCMAKE_INSTALL_PREFIX=%{app_ro_dir} \
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

mkdir -p %{buildroot}%{app_ro_dir}/shared/res
cp -f %{_builddir}/%{name}-%{version}/%{app_root_dir}/shared/res/preview.png %{buildroot}%{app_ro_dir}/shared/res/preview.png

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
# Files
##############################
%files
%manifest %{app_root_dir}/%{name}.manifest
%defattr(-,root,root,-)
%{app_exe_dir}/widget-app
%{app_ro_dir}/shared/res/preview.png
%{xml_file_dir}/%{name}.xml
