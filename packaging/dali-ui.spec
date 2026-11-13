# Auto-generated from dali-ui.spec.in by makespec.sh
# This source builds DALi 3 (dali3-ui-foundation, dali3-ui-components). On
# Tizen 10.1 and 11, where applications are still moving from DALi 2.x, it
# also produces the DALi 2.x package names (dali2-ui-*) as alias packages:
# links to the dali3 files plus the Provides that 2.x binaries and specs
# depend on. On Tizen 10.0 and below the real DALi 2.x is installed from its
# own source, so the aliases must never be built there. --with/--without
# dali2_aliases overrides the default for a local build.
#
# Nested %if, not %elif: Tizen's rpm (4.14) predates %elif and silently
# skips such a branch.
%define legacy_tizen (0%{?tizen_version_major} < 10 || (0%{?tizen_version_major} == 10 && 0%{?tizen_version_minor} == 0))
%if %{legacy_tizen}
%bcond_with    dali2_aliases
%else
%bcond_without dali2_aliases
%endif
%if %{with dali2_aliases} && %{legacy_tizen}
%{error:Tizen <= 10.0 ships the real DALi 2.x; dali2 aliases must not be built there}
%endif

# rpm derives no Provides from a symbolic link, so the alias packages declare
# the ELF ones themselves. 64-bit ELF provides carry a suffix.
%if "%{__isa_bits}" == "64"
%define elf_suffix ()(64bit)
%else
%define elf_suffix %{nil}
%endif

Name:       dali3-ui-foundation
Summary:    DALi UI Library
Version:    3.0.0.11709
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

%if %{with dali2_aliases}
##############################
# DALi 2.x names (aliases)
##############################
%package -n dali2-ui-foundation
Summary:    DALi 2.x library name for the DALi UI Library
Group:      System/Libraries
Requires:   %{name} = %{version}-%{release}
Requires:   dali2-adaptor
Requires(post): /sbin/ldconfig
Requires(postun): /sbin/ldconfig
Provides:   libdali2-ui-foundation.so.2%{elf_suffix}

%description -n dali2-ui-foundation
libdali2-ui-foundation.so.2 as a link to the dali3 library, for binaries built against DALi 2.x. Goes away once the applications have moved to dali3.

%package -n dali2-ui-foundation-devel
Summary:    DALi 2.x development names for the DALi UI Library
Group:      Development/Building
Requires:   dali2-ui-foundation = %{version}-%{release}
Requires:   %{name}-devel = %{version}-%{release}

%description -n dali2-ui-foundation-devel
include/dali-ui-foundation, dali2-ui-foundation.pc and dali-ui-shader-generator as links to their dali3 counterparts.

%package -n dali2-ui-foundation-integration-devel
Summary:    DALi 2.x integration development name for the DALi UI Library
Group:      Development/Building
Requires:   dali2-ui-foundation-devel = %{version}-%{release}
Requires:   %{name}-integration-devel = %{version}-%{release}

%description -n dali2-ui-foundation-integration-devel
dali2-ui-foundation-integration.pc as a link to its dali3 counterpart.

%package -n dali2-ui-components
Summary:    DALi 2.x library name for the DALi UI components
Group:      System/Libraries
License:    Apache-2.0 and BSD-3-Clause and MIT
Requires:   %{dali_ui_components} = %{version}-%{release}
Requires:   dali2-ui-foundation = %{version}-%{release}
Requires(post): /sbin/ldconfig
Requires(postun): /sbin/ldconfig
Provides:   libdali2-ui-components.so.2%{elf_suffix}

%description -n dali2-ui-components
libdali2-ui-components.so.2 as a link to the dali3 library.

%package -n dali2-ui-components-devel
Summary:    DALi 2.x development names for the DALi UI components
Group:      Development/Building
Requires:   dali2-ui-components = %{version}-%{release}
Requires:   %{dali_ui_components}-devel = %{version}-%{release}

%description -n dali2-ui-components-devel
include/dali-ui-components and dali2-ui-components.pc as links to their dali3 counterparts.
%endif

##############################
# Preparation
##############################
%prep
%setup -q

# Paths below mirror what CMake derives from DALI_API_MAJOR.
%define dali3_data_ro_dir   %TZ_SYS_RO_SHARE/dali3/
%define dali3_ui_image_files %{dali3_data_ro_dir}/ui/images/

##############################
# Build
##############################
%build
# TODO: temporary fix => will remove after included in gcc_warning_sr_pkgs list in build.conf
%{?gcc_unforce_options:%gcc_unforce_options}
# Keep shadow diagnostics enabled independently of the build server policy.
export CFLAGS="${CFLAGS:-%optflags} -Werror=shadow"
export CXXFLAGS="${CXXFLAGS:-%optflags} -Werror=shadow"

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

# The data directory goes in as a cache variable, not the environment, so
# that a re-run of cmake inside the build directory keeps it.
cmake -DDALI_DATA_RO_DIR="%{dali3_data_ro_dir}" \
      -DDALI_LEGACY_ALIASES=%{?with_dali2_aliases:ON}%{!?with_dali2_aliases:OFF} \
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
      -DCMAKE_INSTALL_INCLUDEDIR=%{_includedir} \
      .

make %{?jobs:-j%jobs}

##############################
# Installation
##############################
%install
rm -rf %{buildroot}
cd build/tizen
%make_install

# Translations and the images are not part of the CMake install. The gettext
# catalog and the data directory carry the API major (see
# build/tizen/CMakeLists.txt).
install_resources() {
  local domain=$1
  local imagedir=$2
  local src=%{_builddir}/%{name}-%{version}
  local mo lang

  # PO
  for mo in ${src}/dali-ui-foundation/po/*.mo
  do
    [ -e "$mo" ] || continue
    lang=$(basename "$mo" .mo)
    mkdir -p %{buildroot}%{_datadir}/locale/${lang}/LC_MESSAGES/
    cp "$mo" %{buildroot}%{_datadir}/locale/${lang}/LC_MESSAGES/${domain}.mo
  done

  # Images
  mkdir -p %{buildroot}${imagedir}
  mkdir -p %{buildroot}${imagedir}/components
  cp -r ${src}/dali-ui-foundation/images/* %{buildroot}${imagedir}/ || true
  cp -r ${src}/dali-ui-components/images/* %{buildroot}${imagedir}/components || true
}

cd %{_builddir}/%{name}-%{version}
install_resources dali3-ui-foundation %{dali3_ui_image_files}
cp dali-ui.manifest %{name}.manifest
cp dali-ui.manifest %{dali_ui_components}.manifest
cp dali-ui.manifest-smack %{name}.manifest-smack
cp dali-ui.manifest-smack %{dali_ui_components}.manifest-smack
%if %{with dali2_aliases}
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

%postun
/sbin/ldconfig
exit 0

%if %{with dali2_aliases}
%post -n dali2-ui-foundation
/sbin/ldconfig
exit 0

%postun -n dali2-ui-foundation
/sbin/ldconfig
exit 0

%post -n dali2-ui-components
/sbin/ldconfig
exit 0

%postun -n dali2-ui-components
/sbin/ldconfig
exit 0
%endif

##############################
# Files in Binary Packages
##############################
# The Lua scriptlets run up to the next section, so nothing but another
# scriptlet or %files may follow them. Every earlier package (DALi 2.x and
# dali3 3.0.0 alike) installed these include directories as real
# directories; rpm cannot replace a directory with a link, so the alias
# packages clear them before the transaction unpacks.
%if %{with dali2_aliases}
%pretrans -n dali2-ui-foundation-devel -p <lua>
local path = "%{_includedir}/dali-ui-foundation"
if posix.stat(path, "type") == "directory" then
  os.execute("rm -rf '" .. path .. "'")
end
%pretrans -n dali2-ui-components-devel -p <lua>
local path = "%{_includedir}/dali-ui-components"
if posix.stat(path, "type") == "directory" then
  os.execute("rm -rf '" .. path .. "'")
end
%endif
%files
%if 0%{?enable_dali_smack_rules}
%manifest %{name}.manifest-smack
%else
%manifest %{name}.manifest
%endif
%defattr(-,root,root,-)
%{_libdir}/libdali3-ui-foundation.so*
%license LICENSE

%{_datadir}/locale/*/LC_MESSAGES/dali3-ui-foundation.mo
%{dali3_ui_image_files}/*.*

%files devel
%defattr(-,root,root,-)
%{_libdir}/pkgconfig/%{name}.pc
%{_includedir}/dali3/dali-ui-foundation/dali-ui-foundation.h
%{_includedir}/dali3/dali-ui-foundation/dali-ui-foundation-extension.h
%{_includedir}/dali3/dali-ui-foundation/extension-api/*
%{_includedir}/dali3/dali-ui-foundation/public-api/*
%{_bindir}/dali3-ui-shader-generator

%files integration-devel
%defattr(-,root,root,-)
%{_libdir}/pkgconfig/dali3-ui-foundation-integration.pc
%{_includedir}/dali3/dali-ui-foundation/dali-ui-foundation-integ.h
%{_includedir}/dali3/dali-ui-foundation/integration-api/*

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
%{dali3_ui_image_files}/components/*

%files -n %{dali_ui_components}-devel
%defattr(-,root,root,-)
%{_libdir}/pkgconfig/%{dali_ui_components}.pc
%{_includedir}/dali3/dali-ui-components/*.h
%{_includedir}/dali3/dali-ui-components/public-api/*

%if %{with dali2_aliases}
%files -n dali2-ui-foundation
%if 0%{?enable_dali_smack_rules}
%manifest dali2-ui-foundation.manifest-smack
%else
%manifest dali2-ui-foundation.manifest
%endif
%defattr(-,root,root,-)
%{_libdir}/libdali2-ui-foundation.so.2
%license LICENSE

%files -n dali2-ui-foundation-devel
%defattr(-,root,root,-)
%{_includedir}/dali-ui-foundation
%{_libdir}/libdali2-ui-foundation.so
%{_libdir}/pkgconfig/dali2-ui-foundation.pc
%{_bindir}/dali-ui-shader-generator

%files -n dali2-ui-foundation-integration-devel
%defattr(-,root,root,-)
%{_libdir}/pkgconfig/dali2-ui-foundation-integration.pc

%files -n dali2-ui-components
%if 0%{?enable_dali_smack_rules}
%manifest dali2-ui-components.manifest-smack
%else
%manifest dali2-ui-components.manifest
%endif
%defattr(-,root,root,-)
%{_libdir}/libdali2-ui-components.so.2
%license LICENSE

%files -n dali2-ui-components-devel
%defattr(-,root,root,-)
%{_includedir}/dali-ui-components
%{_libdir}/libdali2-ui-components.so
%{_libdir}/pkgconfig/dali2-ui-components.pc
%endif
