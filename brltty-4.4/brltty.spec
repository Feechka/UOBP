# brltty.spec.  Generated from brltty.spec.in by configure.
###############################################################################
# BRLTTY - A background process providing access to the console screen (when in
#          text mode) for a blind person using a refreshable braille display.
#
# Copyright (C) 1995-2012 by The BRLTTY Developers.
#
# BRLTTY comes with ABSOLUTELY NO WARRANTY.
#
# This is free software, placed under the terms of the
# GNU General Public License, as published by the Free Software
# Foundation; either version 2 of the License, or (at your option) any
# later version. Please see the file LICENSE-GPL for details.
#
# Web Page: http://mielke.cc/brltty/
#
# This software is maintained by Dave Mielke <dave@mielke.cc>.
###############################################################################

Name: brltty
Version: 4.4
Release: 1
Group: System Environment/Daemons
License: GPL

Vendor: The BRLTTY Developers
Packager: Dave Mielke <dave@mielke.cc>
URL: http://mielke.cc/brltty/
Source: http://mielke.cc/brltty/releases/%{name}-%{version}.tar.gz

BuildRoot: %{_tmppath}/%{name}-%{version}-InstallRoot
%define _bindir /bin
%define _sbindir /sbin
%define _libdir /lib
%define _sysconfdir /etc

BuildRequires: autoconf >= 2.53
BuildRequires: make
BuildRequires: gcc
BuildRequires: /bin/sh
BuildRequires: coreutils
BuildRequires: binutils
BuildRequires: ldconfig
BuildRequires: /usr/bin/awk
BuildRequires: glibc-devel

BuildRequires: gcc-java

BuildRequires: ocaml
BuildRequires: ocaml-findlib

BuildRequires: Pyrex

BuildRequires: tcl
BuildRequires: tcl-devel

AutoProv: no
Provides: brlapi-server
AutoReq: yes

Summary: Braille display driver for Linux/Unix.
%description
BRLTTY is a background process (daemon) which provides access to
the console screen (when in text mode) for a blind person using a
refreshable braille display.  It drives the braille display, and
provides complete screen review functionality.  Some speech capability
has also been incorporated.

Install this package if you use a refreshable braille display.


%package -n brltty-braille-brlapi
Version: 4.4
Release: 1
Group: System Environment/Daemons
License: GPL

AutoProv: no
AutoReq: yes

Summary: BrlAPI braille driver for BRLTTY.
%description -n brltty-braille-brlapi
This package provides the BrlAPI braille driver for BRLTTY.

Install this package if you need to communicate with
another instance of BRLTTY which is actually in control
of the braille device.


%package -n brltty-braille-xwindow
Version: 4.4
Release: 1
Group: System Environment/Daemons
License: GPL

AutoProv: no
AutoReq: yes

Summary: XWindow braille driver for BRLTTY.
%description -n brltty-braille-xwindow
This package provides the XWindow braille driver for BRLTTY.
This driver presents a virtual braille device within a graphical
user interface. It is primarily intended for sighted developers
who don't have access to a real braille device but still wish to
assess the accessibility of their applications.

Install this package if you would like to see how usable an
application would be by a blind person using a braille device.


%package -n brltty-speech-espeak
Version: 4.4
Release: 1
Group: System Environment/Daemons
License: GPL

AutoProv: no
AutoReq: yes
Requires: espeak

Summary: eSpeak speech driver for BRLTTY.
%description -n brltty-speech-espeak
This package provides the eSpeak speech driver for BRLTTY.

Install this package if you would like to be able to use the
eSpeak text-to-speech engine.


%package -n brltty-speech-festival
Version: 4.4
Release: 1
Group: System Environment/Daemons
License: GPL

AutoProv: no
AutoReq: yes
Requires: festival

Summary: Festival speech driver for BRLTTY.
%description -n brltty-speech-festival
This package provides the Festival speech driver for BRLTTY.

Install this package if you would like to be able to use the
Festival text-to-speech engine.


%package -n brltty-speech-flite
Version: 4.4
Release: 1
Group: System Environment/Daemons
License: GPL

AutoProv: no
AutoReq: yes
Requires: flite

Summary: Festival Lite speech driver for BRLTTY.
%description -n brltty-speech-flite
This package provides the Festival Lite speech driver for BRLTTY.

Install this package if you would like to be able to use the
Festival Lite text-to-speech engine.


%package -n brltty-speech-speechd
Version: 4.4
Release: 1
Group: System Environment/Daemons
License: GPL

AutoProv: no
AutoReq: yes
Requires: speech-dispatcher

Summary: Speech Dispatcher speech driver for BRLTTY.
%description -n brltty-speech-speechd
This package provides the Speech Dispatcher speech driver for BRLTTY.

Install this package if you would like to be able to use the
Speech Dispatcher text-to-speech server.


%package -n brltty-screen-atspi
Version: 4.4
Release: 1
Group: System Environment/Daemons
License: GPL

AutoProv: no
AutoReq: yes
Requires: at-spi

Summary: AT-SPI screen driver for BRLTTY.
%description -n brltty-screen-atspi
This package provides the AT-SPI screen driver for BRLTTY.

Install this package if you would like to be able to
access GTK-based applications via the Assistive
Technology Service Provider Interface.


%package -n brltty-screen-atspi2
Version: 4.4
Release: 1
Group: System Environment/Daemons
License: GPL

AutoProv: no
AutoReq: yes
Requires: dbus

Summary: AT-SPI2 screen driver for BRLTTY.
%description -n brltty-screen-atspi2
This package provides the AT-SPI2 screen driver for BRLTTY.

Install this package if you would like to be able to
access graphical applications via the D-Bus port of
the Assistive Technology Service Provider Interface.


%package -n brltty-screen-screen
Version: 4.4
Release: 1
Group: System Environment/Daemons
License: GPL

AutoProv: no
AutoReq: yes
Requires: screen

Summary: Screen screen driver for BRLTTY.
%description -n brltty-screen-screen
This package provides the Screen screen driver for BRLTTY.

Install this package if you would like to be able to
directly access session windows maintained by the
screen program.


%package -n brltty-utils
Version: 4.4
Release: 1
Group: Development/Tools
License: GPL

AutoProv: no
AutoReq: yes

Summary: Commands for maintaining BRLTTY data files.
%description -n brltty-utils
This package provides a set of commands for maintaining BRLTTY's datafiles.


%package -n brlapi
Version: 0.5.7
Release: 1
Group: System Environment/Libraries
License: LGPL

AutoProv: yes
AutoReq: yes

Summary: Appliation Programming Interface for BRLTTY.
%description -n brlapi
This package provides the run-time support for the Application
Programming Interface to BRLTTY.

Install this package if you have an application
which directly accesses a refreshable braille display.


%package -n brlapi-devel
Version: 0.5.7
Release: 1
Group: Development/Libraries
License: LGPL

AutoProv: yes
AutoReq: yes

Summary: Headers, static archive, and documentation for BrlAPI.
%description -n brlapi-devel
This package provides the header files, static archive, shared object
linker reference, and reference documentation for BrlAPI (the
Application Programming Interface to BRLTTY).  It enables the
implementation of applications which take direct advantage of a
refreshable braille display in order to present information in ways
which are more appropriate for blind users and/or to provide user
interfaces which are more specifically atuned to their needs.

Install this package if you're developing or maintaining an application
which directly accesses a refreshable braille display.


%package -n brlapi-java
Version: 0.5.7
Release: 1
Group: System Environment/Libraries
License: LGPL

AutoProv: no
AutoReq: yes

Summary: Java bindings for BrlAPI.
%description -n brlapi-java
This package provides the Java bindings for BrlAPI,
which is the Application Programming Interface to BRLTTY.

Install this package if you have a Java application
which directly accesses a refreshable braille display.


%package -n brlapi-ocaml
Version: 0.5.7
Release: 1
Group: System Environment/Libraries
License: LGPL

AutoProv: no
AutoReq: yes

Summary: OCaml bindings for BrlAPI.
%description -n brlapi-ocaml
This package provides the OCaml bindings for BrlAPI,
which is the Application Programming Interface to BRLTTY.

Install this package if you have an OCaml application
which directly accesses a refreshable braille display.


%package -n brlapi-python
Version: 0.5.7
Release: 1
Group: System Environment/Libraries
License: LGPL

AutoProv: no
AutoReq: yes

Summary: Python bindings for BrlAPI.
%description -n brlapi-python
This package provides the Python bindings for BrlAPI,
which is the Application Programming Interface to BRLTTY.

Install this package if you have a Python application
which directly accesses a refreshable braille display.


%package -n brlapi-tcl
Version: 0.5.7
Release: 1
Group: System Environment/Libraries
License: LGPL

AutoProv: no
AutoReq: yes

Summary: Tcl bindings for BrlAPI.
%description -n brlapi-tcl
This package provides the Tcl bindings for BrlAPI,
which is the Application Programming Interface to BRLTTY.

Install this package if you have a Tcl application
which directly accesses a refreshable braille display.


%package -n brlapi-utils
Version: 0.5.7
Release: 1
Group: Applications/System
License: LGPL

AutoProv: no
AutoReq: yes

Summary: Appliation Programming Interface for BRLTTY.
%description -n brlapi-utils
This package provides a set of commands for maintaining BRLTTY's datafiles.


%prep
# %setup -n %{name}-%{version}
%setup -n brltty-4.4

%build
%configure --disable-relocatable-install --with-install-root="${RPM_BUILD_ROOT}" --disable-gpm --without-mikropuhe --without-swift --without-theta --without-viavoice --without-libbraille --with-braille-driver=-tt,-vr
make

%install
make install install-documents install-messages
install -m 644 Documents/brltty.conf "${RPM_BUILD_ROOT}%{_sysconfdir}"
%find_lang %{name}

%clean
rm -fr "${RPM_BUILD_ROOT}"


%files -f %{name}.lang
%defattr(-,root,root)
%{_bindir}/brltty
%{_libdir}/brltty
%exclude %{_libdir}/brltty/libbrlttybba.so
%exclude %{_libdir}/brltty/libbrlttybxw.so
%exclude %{_libdir}/brltty/libbrlttyses.so
%exclude %{_libdir}/brltty/libbrlttysfl.so
%exclude %{_libdir}/brltty/libbrlttysfv.so
%exclude %{_libdir}/brltty/libbrlttyssd.so
%exclude %{_libdir}/brltty/libbrlttyxas.so
%exclude %{_libdir}/brltty/libbrlttyxa2.so
%exclude %{_libdir}/brltty/libbrlttyxsc.so
%{_sysconfdir}/brltty
%doc %{_mandir}/man1/brltty.1.gz
%doc %{_docdir}/*/LICENSE*
%doc %{_docdir}/*/README*
%doc %{_docdir}/*/ChangeLog
%doc %{_docdir}/*/CONTRIBUTORS
%doc %{_docdir}/*/TODO
%doc %{_docdir}/*/brltty.conf
%doc %{_docdir}/*/KeyTables
%doc %{_docdir}/*/Manual-BRLTTY
%config(noreplace) %verify(not size md5 mtime) %{_sysconfdir}/brltty.conf

%files -n brltty-braille-brlapi
%{_libdir}/brltty/libbrlttybba.so

%files -n brltty-braille-xwindow
%{_libdir}/brltty/libbrlttybxw.so

%files -n brltty-speech-espeak
%{_libdir}/brltty/libbrlttyses.so

%files -n brltty-speech-festival
%{_libdir}/brltty/libbrlttysfv.so

%files -n brltty-speech-flite
%{_libdir}/brltty/libbrlttysfl.so

%files -n brltty-speech-speechd
%{_libdir}/brltty/libbrlttyssd.so

%files -n brltty-screen-atspi
%{_libdir}/brltty/libbrlttyxas.so

%files -n brltty-screen-atspi2
%{_libdir}/brltty/libbrlttyxa2.so

%files -n brltty-screen-screen
%{_libdir}/brltty/libbrlttyxsc.so

%files -n brltty-utils
%{_bindir}/brltty-*
%{_bindir}/vstp
%doc %{_mandir}/man1/vstp.1.gz

%files -n brlapi
%defattr(-,root,root)
%{_libdir}/libbrlapi.so.*
%doc %{_docdir}/*/Manual-BrlAPI

%files -n brlapi-java
/*
/*

%files -n brlapi-ocaml
/brlapi

%files -n brlapi-python
/usr/lib/python2.7/site-packages/[bB]rlapi[-.]*

%files -n brlapi-tcl
/usr/lib/brlapi-0.5.7/libbrlapi_tcl.so
/usr/lib/brlapi-0.5.7/pkgIndex.tcl

%files -n brlapi-devel
%defattr(-,root,root)
%{_libdir}/libbrlapi.a
%{_libdir}/libbrlapi.so
%{_includedir}/brlapi.h
%{_includedir}/brlapi_*.h
%{_includedir}/brltty
%doc %{_mandir}/man3/*
%doc %{_docdir}/*/BrlAPIref

%files -n brlapi-utils
%{_bindir}/xbrlapi
%doc %{_mandir}/man1/xbrlapi.1.gz


%changelog
* Thu Jun 7 2012 Dave Mielke <dave@mielke.cc> 4.4
+  Changes:
      DOS option syntax (/option[:value]) can only be used on DOS or Windows.
      The clipboard functions have been renamed to accurately reflect what they do:
         CUTBEGIN  -> CLIP_NEW
         CUTAPPEND -> CLIP_ADD
         CUTLINE   -> COPY_LINE
         CUTRECT   -> COPY_RECT
         COPYCHARS -> CLIP_COPY
         APNDCHARS -> CLIP_APPEND
      The Caml bindings for BrlAPI have been renamed to Ocaml.
      The tbltest program has been renamed to brltty-ttb and is installed.
      The ctbtest program has been renamed to brltty-ctb and is installed.
      The --with-compiler-prefix= configure option has been removed.
+  Fixes:
      Brltty keyboard key bindings no longer interfere with Orca key bindings.
      The updating of status lines no longer confuses the cursor routing algorithm.
      The same USB device can no longer be used by more than one brltty at a time.
+  New Features:
      The help function cycles through all relevant pages.
         For example, help for the current keyboard key table is viewable.
      The info-mode line contains a clock.
      The preferences menu has been divided into a set of category-based submenus.
         Open a submenu with any key that would normally alter a setting.
         Close a submenu either by scrolling to its Close item and activating it
         or via the new MENU_PREV_LEVEL function.
      The -l [--log-level=] option of brltty accepts log category names.
         The names are: ingio, inpkts, outpkts, brlkeys, kbdkeys, csrtrk, csrrtg.
         Specify the log level and/or category names as a comma-delimited list.
      The -C [--cancel-execution] option has been added to brltty.
         The same pid file must be specified by both the cancelling and the
         already-running brltty.
      The pid file may be specified within the configuration file (brltty.conf).
      The brltty-trtxt program has been added and is installed.
+  New functions:
      MENU_PREV_LEVEL: Close the current submenu.
      TIME: Display the current time (format configurable via the menu).
      PWGEN: Set the clipboard to a random password (length set by routing key).
      Sticky input modifier functions:
         SHIFT: Add the shift modifier to the next typed character.
         UPPER: Add the uppercase modifier to the next typed character.
         CONTROL: Add the control modifier to the next typed character.
         META: Add the meta (left alt) modifier to the next typed character.
      Autospeak suboption toggles:
         ASPK_SEL_LINE: Set autospeak selected line on/off.
         ASPK_SEL_CHAR: Set autospeak selected character on/off.
         ASPK_INS_CHARS: Set autospeak inserted characters on/off.
         ASPK_DEL_CHARS: Set autospeak deleted characters on/off.
         ASPK_REP_CHARS: Set autospeak replaced characters on/off.
         ASPK_CMP_WORDS: Set autospeak completed words on/off.
      Many functions specifically for speech navigation:
         SPEAK_CURR_CHAR: Speak current character.
         SPEAK_PREV_CHAR: Go to and speak previous character.
         SPEAK_NEXT_CHAR: Go to and speak next character.
         SPEAK_CURR_WORD: Speak current word.
         SPEAK_PREV_WORD: Go to and speak previous word.
         SPEAK_NEXT_WORD: Go to and speak next word.
         SPEAK_CURR_LINE: Speak current line.
         SPEAK_PREV_LINE: Go to and speak previous line.
         SPEAK_NEXT_LINE: Go to and speak next line.
         SPEAK_FRST_CHAR: Go to and speak first non-blank character on line.
         SPEAK_LAST_CHAR: Go to and speak last non-blank character on line.
         SPEAK_FRST_LINE: Go to and speak first non-blank line on screen.
         SPEAK_LAST_LINE: Go to and speak last non-blank line on screen.
         DESC_CURR_CHAR: Describe current character.
         SPELL_CURR_WORD: Spell current word.
         ROUTE_CURR_LOCN: Bring cursor to speech location.
         SPEAK_CURR_LOCN: Speak speech location.
         SHOW_CURR_LOCN: Set speech location visibility on/off.
+  New braille drivers:
      hw: HumanWare
      ir: Iris
      np: NinePoint (Cebra)
+  Baum braille driver changes:
      The Inka is fully supported.
      New key bindings for models with six display keys:
         Display1+Display5 displays the current time.
+  EuroBraille braille driver changes:
      The latest models are supported.
      Key tables, rather than hard-coded bindings, are used.
      The display is updated when returning from the internal menu.
      The LCD (visual display) is supported.
      Large displays work reliably when connected via USB.
      The protocol= parameter takes a model (rather than a protocol) name.
+  FreedomScientific braille driver changes:
      New key bindings:
         LeftRockerUp goes up one line.
         LeftRockerDown goes down one line.
+  HandyTech braille driver changes:
      New key bindings:
         B2+B3+B5+B6 displays the current time.
         B1+B2+!RoutingKey sets the clipboard to a random password.
+  Papenmeier braille driver changes:
      A serious USB problem has been fixed.
      New key bindings for models with 13 front keys:
         Dot3, when in the menu, closes the current submenu.
         Dot1+Dot2+Dot3+Dot6 routes the cursor vertically.
         Dot1+Dot2+Dot3+Dot6+Dot7 switches to the previous virtual console.
         Dot1+Dot2+Dot3+Dot6+Dot8 switches to the next virtual console.
         Dot1+Cot2+Dot3+Dot7 displays the current time.
         Dot1+Dot2+Dot3+Dot7+!RoutingKey1 sets the clipboard to a random password.
+  Seika braille driver changes:
      The notetaker models are supported.
+  eSpeak speech driver changes:
      The maxrate= parameter has been added.
+  Text tables:
      The de (German) table has been updated.
      The hu (Hungarian) table has been updated.
      Support for writing JAWS (.jbt) text tables has been added to brltty-ttb.
+  Contraction tables:
      the en-us-g2 (US English - Grade 2) table has been updated.
      A table may be implemented as an executable program.
      The latex-access (braille math) executable table has been added.
      The -r [--reformat-input] option of brltty-ctb handles paragraphs better.
+  Keyboard key tables:
      Many bindings have been added to the keypad and laptop tables.
+  Documentation:
      A portuguese translation of the manual has been added.

