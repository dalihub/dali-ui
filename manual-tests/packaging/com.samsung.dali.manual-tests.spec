Name:       com.samsung.dali.manual-tests
Summary:    DALi UI manual-test launcher app (dalium PoC packaging)
Version:    1.0.0
Release:    1
Group:      System/Libraries
License:    Apache-2.0
URL:        https://github.com/dalihub/dali-ui
Source0:    %{name}-%{version}.tar.gz

BuildRequires:  cmake
BuildRequires:  pkgconfig
BuildRequires:  pkgconfig(gles20)
BuildRequires:  pkgconfig(glesv2)
BuildRequires:  pkgconfig(egl)
BuildRequires:  pkgconfig(dali2-core)
BuildRequires:  pkgconfig(dali2-adaptor)
BuildRequires:  dali2-integration-devel
BuildRequires:  dali2-adaptor-integration-devel
BuildRequires:  pkgconfig(dali2-ui-foundation)
BuildRequires:  dali2-ui-foundation-integration-devel
BuildRequires:  pkgconfig(dali2-ui-components)

%description
The dali-ui manual-tests launcher (foundation module) packaged as a Tizen ui-app
so dalium can drive the manual TCs on an emulator/device.

%prep
%setup -q

%define TZ_SYS_RO_APP /usr/apps
%define TZ_SYS_RO_PACKAGES /usr/share/packages
%define app_src_dir      manual-tests/dali-ui-foundation/
%define app_ro_dir       %TZ_SYS_RO_APP/%{name}/
%define xml_file_dir     %TZ_SYS_RO_PACKAGES
%define app_res_dir      %{app_ro_dir}res/
%define app_exe_dir      %{app_ro_dir}bin/

# TEST_RESOURCE_DIR is baked at compile time to the SOURCE res/ path; point it
# at the installed on-device res dir instead.
sed -i 's|SET(TEST_RESOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/res")|SET(TEST_RESOURCE_DIR "%{app_res_dir}")|' \
    %{app_src_dir}CMakeLists.txt

%build
PREFIX="/usr"
CXXFLAGS+=" -Wall -g -O2"
LDFLAGS+=" -Wl,--rpath=$PREFIX/lib -Wl,--as-needed"

cd %{_builddir}/%{name}-%{version}/%{app_src_dir}
cmake -DCMAKE_INSTALL_PREFIX=%{app_ro_dir} -DTIZEN:BOOL=ON .
make %{?jobs:-j%jobs}

%install
rm -rf %{buildroot}
cd %{app_src_dir}
%make_install

mkdir -p %{buildroot}%{xml_file_dir}
cp -f %{_builddir}/%{name}-%{version}/manual-tests/%{name}.xml %{buildroot}%{xml_file_dir}

mkdir -p %{buildroot}%{app_res_dir}
cp -fr res/* %{buildroot}%{app_res_dir}

%files
%manifest manual-tests/%{name}.manifest
%defattr(-,root,root,-)
%{app_exe_dir}manual-test-dali-ui-foundation
%{app_res_dir}*
%{xml_file_dir}/%{name}.xml
