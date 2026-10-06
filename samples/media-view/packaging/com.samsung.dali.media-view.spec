Name:       com.samsung.dali.media-view
Summary:    DALi UI MediaView player and camera sample
Version:    2.0.0
Release:    1
Group:      Applications/Multimedia
License:    Apache-2.0
URL:        https://github.com/dalihub/dali-ui
Source0:    %{name}-%{version}.tar.gz

BuildRequires:  cmake
BuildRequires:  pkgconfig
BuildRequires:  pkgconfig(dali3-core)
BuildRequires:  pkgconfig(dali3-adaptor)
BuildRequires:  dali3-integration-devel
BuildRequires:  dali3-adaptor-integration-devel
BuildRequires:  pkgconfig(dali3-ui-foundation)
BuildRequires:  pkgconfig(dali3-ui-components)
BuildRequires:  pkgconfig(capi-media-player)
BuildRequires:  pkgconfig(capi-media-camera)
BuildRequires:  coregl
Requires:       dali3-extension-media-frame-source-plugin

%description
Interactive player and camera tests using Dali::Ui::MediaView and TextButton.
The application owns each producer and connects its media_frame_source_h to
MediaView. Controls exercise playback, crop, flips, size, corners and lifecycle.

%define app_root_dir samples/media-view
%define app_ro_dir   %{_prefix}/apps/%{name}
%define xml_file_dir %{_datadir}/packages

%prep
%setup -q

%build
# This sample does not use LTO. Avoid loading the host LTO plugin with the
# ARM linker in QEMU-based GBS rootstraps.
export CFLAGS="$CFLAGS -fno-use-linker-plugin"
export CXXFLAGS="$CXXFLAGS -fno-use-linker-plugin"
cmake -S %{app_root_dir} -B media-view-build \
      -DCMAKE_INSTALL_PREFIX=%{app_ro_dir} \
      -DCMAKE_BUILD_TYPE=Release \
      -DTIZEN:BOOL=ON
cmake --build media-view-build %{?jobs:--parallel %jobs}

%install
DESTDIR=%{buildroot} cmake --install media-view-build
mkdir -p %{buildroot}%{xml_file_dir}
cp %{app_root_dir}/%{name}.xml %{buildroot}%{xml_file_dir}/

%files
%manifest %{app_root_dir}/%{name}.manifest
%defattr(-,root,root,-)
%{app_ro_dir}/bin/media-view.example
%{xml_file_dir}/%{name}.xml
