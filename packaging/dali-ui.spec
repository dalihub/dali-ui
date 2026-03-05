# Auto-generated from dali-ui.spec.in by makespec.sh
# Auto-generated from dali-ui.spec.in by makespec.sh
Name:       dali2-ui-foundation
Summary:    DALi UI Library
Version:    2.0.10078
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
BuildRequires:  pkgconfig(gles20)
BuildRequires:  pkgconfig(glesv2)
BuildRequires:  pkgconfig(egl)
BuildRequires:  gettext

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
# dali-ui-elements
##############################
%define dali_ui_elements dali2-ui-elements
%package -n %{dali_ui_elements}
Summary:    DALi UI elements
Group:      System/Libraries
License:    Apache-2.0 and BSD-3-Clause and MIT
Requires:   dali2-ui-foundation

%description -n %{dali_ui_elements}
DALi UI elements providing TextLabel and Image.

%package -n %{dali_ui_elements}-devel
Summary:    Development components for dali-ui-elements
Group:      Development/Building
Requires:   %{dali_ui_elements} = %{version}-%{release}

%description -n %{dali_ui_elements}-devel
Development components for dali-ui-elements.

##############################
# dali-ui-components
##############################
%define dali_ui_components dali2-ui-components
%package -n %{dali_ui_components}
Summary:    DALi UI components
Group:      System/Libraries
License:    Apache-2.0 and BSD-3-Clause and MIT
Requires:   dali2-ui-foundation
Requires:   dali2-ui-elements

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
  cp ${language}.mo %{buildroot}/%{_datadir}/locale/${language}/LC_MESSAGES/dali-toolkit.mo
done
) &> /dev/null

# Create directory and copy (style, images, sounds etc)
%define dali_data_ro_dir %TZ_SYS_RO_SHARE/dali/
%define dali_toolkit_style_files %{dali_data_ro_dir}/toolkit/styles/
%define dali_toolkit_image_files %{dali_data_ro_dir}/toolkit/images/
%define dali_toolkit_sound_files %{dali_data_ro_dir}/toolkit/sounds/

mkdir -p %{buildroot}%{dali_toolkit_style_files}/360x360
cp -r ../../dali-ui-foundation/styles/360x360/* %{buildroot}%{dali_toolkit_style_files}/360x360/

# 이미지와 사운드도 복사해야 %files 패키징 시 에러가 나지 않습니다.
mkdir -p %{buildroot}%{dali_toolkit_image_files}
cp -r ../../dali-ui-foundation/images-common/* %{buildroot}%{dali_toolkit_image_files}/ || true

mkdir -p %{buildroot}%{dali_toolkit_sound_files}
cp -r ../../dali-ui-foundation/sounds/* %{buildroot}%{dali_toolkit_sound_files}/ || true

cd ../../
cp dali-ui.manifest %{name}.manifest
cp dali-ui.manifest %{dali_ui_elements}.manifest
cp dali-ui.manifest %{dali_ui_components}.manifest
cp dali-ui.manifest-smack %{name}.manifest-smack
cp dali-ui.manifest-smack %{dali_ui_elements}.manifest-smack
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
%{_datadir}/dali/toolkit/styles/*
%{_datadir}/dali/toolkit/images/*
%{_datadir}/dali/toolkit/sounds/*


%files devel
%defattr(-,root,root,-)
%{_libdir}/pkgconfig/%{name}.pc
%{dev_include_path}/dali-ui-foundation/*.h
%{dev_include_path}/dali-ui-foundation/public-api/*
%{_bindir}/dali-shader-generator

%files integration-devel
%defattr(-,root,root,-)
%{_libdir}/pkgconfig/dali2-ui-foundation-integration.pc
%{dev_include_path}/dali-ui-foundation/integration-api/*

%files -n %{dali_ui_elements}
%if 0%{?enable_dali_smack_rules}
%manifest %{dali_ui_elements}.manifest-smack
%else
%manifest %{dali_ui_elements}.manifest
%endif
%defattr(-,root,root,-)
%{_libdir}/lib%{dali_ui_elements}.so*
%license LICENSE

%files -n %{dali_ui_elements}-devel
%defattr(-,root,root,-)
%{_libdir}/pkgconfig/%{dali_ui_elements}.pc
%{_includedir}/dali-ui-elements/*.h
%{_includedir}/dali-ui-elements/public-api/*

%files -n %{dali_ui_components}
%if 0%{?enable_dali_smack_rules}
%manifest %{dali_ui_components}.manifest-smack
%else
%manifest %{dali_ui_components}.manifest
%endif
%defattr(-,root,root,-)
%{_libdir}/lib%{dali_ui_components}.so*
%license LICENSE

%files -n %{dali_ui_components}-devel
%defattr(-,root,root,-)
%{_libdir}/pkgconfig/%{dali_ui_components}.pc
%{_includedir}/dali-ui-components/*.h
%{_includedir}/dali-ui-components/public-api/*
