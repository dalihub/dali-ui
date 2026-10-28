# Auto-generated from dali-ui.spec.in by makespec.sh
# dali2 is the same code built a second time under the dali2 name, for
# applications that still use it. To drop it, remove the "with dali2" blocks.
%bcond_without dali2

Name:       dali3-ui-foundation
Summary:    DALi UI Library
Version:    2.5.45.11629
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
BuildRequires:  pkgconfig(dali3-core)
BuildRequires:  pkgconfig(dali3-adaptor)
BuildRequires:  dali3-integration-devel
BuildRequires:  dali3-adaptor-integration-devel
%if %{with dali2}
BuildRequires:  pkgconfig(dali2-core)
BuildRequires:  pkgconfig(dali2-adaptor)
BuildRequires:  dali2-integration-devel
BuildRequires:  dali2-adaptor-integration-devel
%endif
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
%define dali_ui_components dali3-ui-components
%package -n %{dali_ui_components}
Summary:    DALi UI components
Group:      System/Libraries
License:    Apache-2.0 and BSD-3-Clause and MIT
Requires:   dali3-ui-foundation

%description -n %{dali_ui_components}
DALi UI components providing controls such as Button.

%package -n %{dali_ui_components}-devel
Summary:    Development components for dali-ui-components
Group:      Development/Building
Requires:   %{dali_ui_components} = %{version}-%{release}

%description -n %{dali_ui_components}-devel
Development components for dali-ui-components.

%if %{with dali2}
##############################
# dali2
##############################
%package -n dali2-ui-foundation
Summary:    DALi UI Library
Group:      System/Libraries
Requires(post): /sbin/ldconfig
Requires(postun): /sbin/ldconfig

%description -n dali2-ui-foundation
DALi library providing app development tools and UI components - the same code as %{name}, under the dali2 name

%package -n dali2-ui-foundation-devel
Summary:    Development components for DALi UI
Group:      Development/Building
Requires:   dali2-ui-foundation = %{version}-%{release}

%description -n dali2-ui-foundation-devel
Development components for DALi UI - public headers and package config

%package -n dali2-ui-foundation-integration-devel
Summary:    Integration development package for DALi UI
Group:      Development/Building
Requires:   dali2-ui-foundation = %{version}-%{release}

%description -n dali2-ui-foundation-integration-devel
Integration development package for DALi UI - headers for integration level.

%package -n dali2-ui-components
Summary:    DALi UI components
Group:      System/Libraries
License:    Apache-2.0 and BSD-3-Clause and MIT
Requires:   dali2-ui-foundation

%description -n dali2-ui-components
DALi UI components providing controls such as Button.

%package -n dali2-ui-components-devel
Summary:    Development components for dali-ui-components
Group:      Development/Building
Requires:   dali2-ui-components = %{version}-%{release}

%description -n dali2-ui-components-devel
Development components for dali-ui-components.
%endif

##############################
# Preparation
##############################
%prep
%setup -q

%define dev_include_path %{_includedir}
%if %{with dali2}
%define dali2_build_dir %{_builddir}/%{name}-%{version}/_build/dali2
%endif

##############################
# Build
##############################
%build
# TODO: temporary fix => will remove after included in gcc_warning_sr_pkgs list in build.conf
%{?gcc_unforce_options:%gcc_unforce_options}
PREFIX="/usr"
CXXFLAGS="$CXXFLAGS -Wall -g -Os -DNDEBUG -fPIC -fvisibility-inlines-hidden -fdata-sections -ffunction-sections "
LDFLAGS="$LDFLAGS -Wl,--rpath=$PREFIX/lib -Wl,--as-needed -Wl,--gc-sections -lgcc_s -lgcc -Wl,-Bsymbolic-functions "

%ifarch %{arm}
CXXFLAGS="$CXXFLAGS -D_ARCH_ARM_ -mfpu=neon"
%endif

%if 0%{?enable_coverage}
CXXFLAGS="$CXXFLAGS --coverage "
LDFLAGS="$LDFLAGS --coverage "
%endif

libtoolize --force
cd %{_builddir}/%{name}-%{version}/build/tizen

CFLAGS="${CFLAGS:-%optflags}" ;
CXXFLAGS="${CXXFLAGS:-%optflags}" ;
LDFLAGS="${LDFLAGS:-%optflags}" ;

%if "%{vd_asan}" == "1" || "%{asan}" == "1"
CFLAGS="$CFLAGS -fsanitize=address"
CXXFLAGS="$CXXFLAGS -fsanitize=address -Wno-maybe-uninitialized"
LDFLAGS="$LDFLAGS -fsanitize=address"
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
) > /dev/null 2>&1

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

%if %{with dali2}
# Keep the options the same as dali3 above; only DALI_API_MAJOR differs.
# -S and -B are required: a plain source path would reuse the dali3 in-source cache.
cmake -S . -B %{dali2_build_dir} \
%if 0%{?enable_debug}
      -DCMAKE_BUILD_TYPE=Debug \
%endif
      -DDALI_API_MAJOR=2 \
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

make -C %{dali2_build_dir} %{?jobs:-j%jobs}

# The dali2 build rewrites the generated shader sources in the source tree,
# so bring the dali3 build up to date again here rather than at install time.
make %{?jobs:-j%jobs}
%endif

##############################
# Installation
##############################
%install
rm -rf %{buildroot}
cd build/tizen

cd %{_builddir}/%{name}-%{version}/build/tizen
%make_install

%if %{with dali2}
(cd %{dali2_build_dir} && %make_install)
%endif

# PO install
(
cd ../../dali-ui-foundation/po
for language in *.mo
do
  language=${language%.mo}
  mkdir -p %{buildroot}/%{_datadir}/locale/${language}/LC_MESSAGES/
  cp ${language}.mo %{buildroot}/%{_datadir}/locale/${language}/LC_MESSAGES/dali-ui-foundation.mo
done
) > /dev/null 2>&1

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
%if %{with dali2}
cp dali-ui.manifest dali2-ui-foundation.manifest
cp dali-ui.manifest dali2-ui-components.manifest
cp dali-ui.manifest-smack dali2-ui-foundation.manifest-smack
cp dali-ui.manifest-smack dali2-ui-components.manifest-smack
%endif

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

%if %{with dali2}
%post -n dali2-ui-foundation
/sbin/ldconfig
exit 0

%postun -n dali2-ui-foundation
/sbin/ldconfig
exit 0
%endif

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
%{_libdir}/libdali3-ui-foundation.so*
%license LICENSE

%{_datadir}/locale/*/LC_MESSAGES/*
%{dali_ui_image_files}/*.*

%files devel
%defattr(-,root,root,-)
%{_libdir}/pkgconfig/%{name}.pc
%{dev_include_path}/dali-ui-foundation/dali-ui-foundation.h
%{dev_include_path}/dali-ui-foundation/dali-ui-foundation-extension.h
%{dev_include_path}/dali-ui-foundation/extension-api/*
%{dev_include_path}/dali-ui-foundation/public-api/*
%{_bindir}/dali-ui-shader-generator

%files integration-devel
%defattr(-,root,root,-)
%{_libdir}/pkgconfig/dali3-ui-foundation-integration.pc
%{dev_include_path}/dali-ui-foundation/dali-ui-foundation-integ.h
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
%license third-party/md4c/LICENSE.md
%{dali_ui_image_files}/components/*

%files -n %{dali_ui_components}-devel
%defattr(-,root,root,-)
%{_libdir}/pkgconfig/%{dali_ui_components}.pc
%{_includedir}/dali-ui-components/*.h
%{_includedir}/dali-ui-components/public-api/*

%if %{with dali2}
# Both builds install the same headers, tools and resources, so dali2 shares them with dali3.
%files -n dali2-ui-foundation
%if 0%{?enable_dali_smack_rules}
%manifest dali2-ui-foundation.manifest-smack
%else
%manifest dali2-ui-foundation.manifest
%endif
%defattr(-,root,root,-)
%{_libdir}/libdali2-ui-foundation.so*
%license LICENSE

%{_datadir}/locale/*/LC_MESSAGES/*
%{dali_ui_image_files}/*.*

%files -n dali2-ui-foundation-devel
%defattr(-,root,root,-)
%{_libdir}/pkgconfig/dali2-ui-foundation.pc
%{dev_include_path}/dali-ui-foundation/dali-ui-foundation.h
%{dev_include_path}/dali-ui-foundation/dali-ui-foundation-extension.h
%{dev_include_path}/dali-ui-foundation/extension-api/*
%{dev_include_path}/dali-ui-foundation/public-api/*
%{_bindir}/dali-ui-shader-generator

%files -n dali2-ui-foundation-integration-devel
%defattr(-,root,root,-)
%{_libdir}/pkgconfig/dali2-ui-foundation-integration.pc
%{dev_include_path}/dali-ui-foundation/dali-ui-foundation-integ.h
%{dev_include_path}/dali-ui-foundation/integration-api/*

%files -n dali2-ui-components
%if 0%{?enable_dali_smack_rules}
%manifest dali2-ui-components.manifest-smack
%else
%manifest dali2-ui-components.manifest
%endif
%defattr(-,root,root,-)
%{_libdir}/libdali2-ui-components.so*
%license LICENSE
%license third-party/md4c/LICENSE.md
%{dali_ui_image_files}/components/*

%files -n dali2-ui-components-devel
%defattr(-,root,root,-)
%{_libdir}/pkgconfig/dali2-ui-components.pc
%{_includedir}/dali-ui-components/*.h
%{_includedir}/dali-ui-components/public-api/*
%endif
