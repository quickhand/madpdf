Name: madpdf
Version: 0
Release: alt0.0

Summary: PDF viewer for e-ink e-book readers
License: GPLv2
Group: Graphics
Packager: Alex V. Myltsev <avm@altlinux.ru>

Source: %name-%version.tar

BuildRequires: libewl-devel libepdf-devel libexpat-devel

%description
madpdf is a PDF e-book reader application.

%prep
%setup

%build
autoreconf -fisv
%configure
%make

%install
%make_install DESTDIR=%buildroot install

%files
%_bindir/madpdf


%changelog
* Tue Sep 18 2008 Alexander Myltsev <avm@altlinux.ru> 0-alt0.0
- Initial build for ALT Linux.

