# Auto-generated from dali-ui.spec.in by makespec.sh
Name:       dali2-ui-foundation
Summary:    DALi UI Library
Version:    2.5.28.10810
Release:    1
Group:      System/Libraries
License:    Apache-2.0 and BSD-3-Clause and MIT
URL:        https://github.com/dalihub/dali-ui
Source0:    %{name}-%{version}.tar.gz

Requires(post): /sbin/ldconfig
Requires(postun): /sbin/ldconfig
BuildRequires:  cmake
BuildRequires:  pkgconfig
BuildRequires:  gawk
BuildRequires:  pkgconfig(dali2-core)
BuildRequires:  pkgconfig(dali2-adaptor)
BuildRequires:  dali2-integration-devel
BuildRequires:  dali2-adaptor-integration-devel
BuildRequires:  pkgconfig(gles20)
BuildRequires:  pkgconfig(glesv2)
BuildRequires:  pkgconfig(egl)
BuildRequires:  gettext
BuildRequires:  python3

%if 0%{?tizen_version_major} >= 3
BuildRequires:  pkgconfig(libtzplatform-config)
%endif

# For ASAN test
%if "%{vd_asan}" == "1" || "%{asan}" == "1"
BuildRequires: asan-force-options
BuildRequires: asan-build-env
BuildRequires: libasan
%endif

%description
DALi library providing app development tools and UI components

##############################
# devel
##############################
%package devel
Summary:    Development components for DALi UI
Group:      Development/Building
Requires:   %{name} = %{version}-%{release}

%description devel
Development components for DALi UI - public headers and package config

##############################
# integration-devel
##############################
%package integration-devel
Summary:    Integration development package for DALi UI
Group:      Development/Building
Requires:   %{name} = %{version}-%{release}

%description integration-devel
Integration development package for DALi UI - headers for integration level.


##############################
# dali-ui-components
##############################
%define dali_ui_components dali2-ui-components
%package -n %{dali_ui_components}
Summary:    DALi UI components
Group:      System/Libraries
License:    Apache-2.0 and BSD-3-Clause and MIT
Requires:   dali2-ui-foundation

%description -n %{dali_ui_components}
DALi UI components providing controls such as Button.

%package -n %{dali_ui_components}-devel
Summary:    Development components for dali-ui-components
Group:      Development/Building
Requires:   %{dali_ui_components} = %{version}-%{release}

%description -n %{dali_ui_components}-devel
Development components for dali-ui-components.

##############################
# Preparation
##############################
%prep
%setup -q

%define dev_include_path %{_includedir}

##############################
# Build
##############################
%build
PREFIX="/usr"
CXXFLAGS+=" -Wall -g -Os -DNDEBUG -fPIC -fvisibility-inlines-hidden -fdata-sections -ffunction-sections "
LDFLAGS+=" -Wl,--rpath=$PREFIX/lib -Wl,--as-needed -Wl,--gc-sections -lgcc_s -lgcc -Wl,-Bsymbolic-functions "

%ifarch %{arm}
CXXFLAGS+=" -D_ARCH_ARM_ -mfpu=neon"
%endif

%if 0%{?enable_coverage}
CXXFLAGS+=" --coverage "
LDFLAGS+=" --coverage "
%endif

libtoolize --force
cd %{_builddir}/%{name}-%{version}/build/tizen

CFLAGS="${CFLAGS:-%optflags}" ;
CXXFLAGS="${CXXFLAGS:-%optflags}" ;
LDFLAGS="${LDFLAGS:-%optflags}" ;

%if "%{vd_asan}" == "1" || "%{asan}" == "1"
CFLAGS+=" -fsanitize=address"
CXXFLAGS+=" -fsanitize=address -Wno-maybe-uninitialized"
LDFLAGS+=" -fsanitize=address"
%endif
export CFLAGS;
export CXXFLAGS;
export LDFLAGS;

# PO (Translations)
(
cd ../../dali-ui-foundation/po
for language in *.po
do
  language=${language%.po}
  msgfmt -o ${language}.mo ${language}.po
done
) &> /dev/null

cmake \
%if 0%{?enable_debug}
      -DCMAKE_BUILD_TYPE=Debug \
%endif
      -DENABLE_TRACE=ON \
      -DENABLE_BACKTRACE=ON \
      -DGBS_BUILD=ON \
%if 0%{?enable_low_spec_memory_management}
      -DENABLE_LOW_SPEC_MEMORY_MANAGEMENT=ON \
%endif
%if 0%{?enable_gpu_memory_profile}
      -DENABLE_GPU_MEMORY_PROFILE=ON \
%endif
      -DCMAKE_INSTALL_PREFIX=%{_prefix} \
      -DCMAKE_INSTALL_LIBDIR=%{_libdir} \
      -DCMAKE_INSTALL_INCLUDEDIR=%{_includedir}

make %{?jobs:-j%jobs}

##############################
# Installation
##############################
%install
rm -rf %{buildroot}
cd build/tizen

pushd %{_builddir}/%{name}-%{version}/build/tizen
%make_install

# PO install
(
cd ../../dali-ui-foundation/po
for language in *.mo
do
  language=${language%.mo}
  mkdir -p %{buildroot}/%{_datadir}/locale/${language}/LC_MESSAGES/
  cp ${language}.mo %{buildroot}/%{_datadir}/locale/${language}/LC_MESSAGES/dali-ui-foundation.mo
done
) &> /dev/null

# Create directory and copy (style, images, sounds etc)
%define dali_data_ro_dir %TZ_SYS_RO_SHARE/dali/
%define dali_ui_image_files %{dali_data_ro_dir}/ui/images/

mkdir -p %{buildroot}%{dali_ui_image_files}
mkdir -p %{buildroot}%{dali_ui_image_files}/components
cp -r ../../dali-ui-foundation/images/* %{buildroot}%{dali_ui_image_files}/ || true
cp -r ../../dali-ui-components/images/* %{buildroot}%{dali_ui_image_files}/components || true

cd ../../
cp dali-ui.manifest %{name}.manifest
cp dali-ui.manifest %{dali_ui_components}.manifest
cp dali-ui.manifest-smack %{name}.manifest-smack
cp dali-ui.manifest-smack %{dali_ui_components}.manifest-smack

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
%if 0%{?enable_dali_smack_rules}
%manifest %{name}.manifest-smack
%else
%manifest %{name}.manifest
%endif
%defattr(-,root,root,-)
%{_libdir}/libdali2-ui-foundation.so*
%license LICENSE

%{_datadir}/locale/*/LC_MESSAGES/*
%{dali_ui_image_files}/*.*

%files devel
%defattr(-,root,root,-)
%{_libdir}/pkgconfig/%{name}.pc
%{dev_include_path}/dali-ui-foundation/dali-ui-foundation.h
%{dev_include_path}/dali-ui-foundation/dali-ui-foundation-provider.h
%{dev_include_path}/dali-ui-foundation/provider-api/*
%{dev_include_path}/dali-ui-foundation/public-api/*
%{_bindir}/dali-shader-generator

%files integration-devel
%defattr(-,root,root,-)
%{_libdir}/pkgconfig/dali2-ui-foundation-integration.pc
%{dev_include_path}/dali-ui-foundation/dali-ui-foundation-integ.h
%{dev_include_path}/dali-ui-foundation/devel-api/*
%{dev_include_path}/dali-ui-foundation/integration-api/*

%files -n %{dali_ui_components}
%if 0%{?enable_dali_smack_rules}
%manifest %{dali_ui_components}.manifest-smack
%else
%manifest %{dali_ui_components}.manifest
%endif
%defattr(-,root,root,-)
%{_libdir}/lib%{dali_ui_components}.so*
%license LICENSE
%{dali_ui_image_files}/components/*

%files -n %{dali_ui_components}-devel
%defattr(-,root,root,-)
%{_libdir}/pkgconfig/%{dali_ui_components}.pc
%{_includedir}/dali-ui-components/*.h
%{_includedir}/dali-ui-components/public-api/*
