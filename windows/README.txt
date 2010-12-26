This directory contains the MS Windows port of Gnocl.
Gnocl is an interface between the programming language Tcl
and the GTK+ / Gnome toolkit. The homepage of Gnocl can
be found on the web at "http://www.dr-baum.net/gnocl/index.html".
Copyright © 2001, 2002, 2003, 2004, 2005, 2006 Peter G. Baum

HOW TO INSTALL THE BINARIES ON WINDOWS

1. Of course you need to install GTK+. A binary version for
   MS Windows can be downloaded from
   "http://gimp-win.sourceforge.net/stable.html".
   The zipfile you can download there contains
   an installer program. So it is quite easy to install.
   The version I used was 2.10.6-1.

2. Gnocl works with ActiveTcl. You can download ActiveTcl from
   "http://www.activestate.com/Products/ActiveTcl/". There
   you get an installer which makes installation easy.
   The version I used was 8.4.14.

3. As my tests showed, there is a problem with iconv.dll in
   the subdirectory "bin" of the ActiveTcl installation
   directory which prevents gnocl.dll to load. But there is
   a solution to this problem: Rename iconv.dll to something
   else and then copy the DLL iconv.dll from the installation
   directory of GTK to the "bin" directory of ActiveTcl.
   If you plan to use gnoclVFS, you need to replace libxml2.dll
   in the same way.

4. In the subdirectory "lib" of the installation directory of
   ActiveTcl create a directory "gnocl-0.9.91" and place the
   DLL files and pkgIndex.tcl there. Now you are ready
   to start wish84.exe and type in "package require Gnocl".

5. Gnocl can also be used with the package tcltk of cygwin.
   If the subdirectory "gnocl-0.9.91" is created under
   /usr/share, wish will find the package when
   "package require Gnocl" is typed in.

HOW TO COMPILE GNOCL ON WINDOWS

1. You need to install the include files, libraries and
   all other stuff for compiling plugins for Gimp on
   MS Windows. Read the information on
   "http://www.gimp.org/~tml/gimp/win32//", then
   download and install the stuff.

2. I compiled Gnocl with Cygwin's gcc. With this, one
   has to be careful to avoid a dependency of the
   resulting DLL on cygwin1.dll. The compiler option
   "-mno-cygwin" has to be used. Again, this is
   described on Tor Lillqvist's website
   "http://www.gimp.org/~tml/gimp/win32//".
   If you want to use Cygwin, you can download it
   from "http://www.cygwin.com/".

3. You also need include files and libraries from Tcl.
   ActiveTcl contains the include files, but only
   the libraries for the Microsoft C compiler (with
   file extension ".lib"). Linking with Cygwin
   requires the library libtclstub8.4.a. You can
   create this library by downloading the source
   files of Tcl
   ("http://www.tcl.tk/software/tcltk/downloadnow84.html")
   and compiling them with Cygwin's gcc. I used the
   following commands:
   cd tcl8.4.9/win
   CPPFLAGS="-mno-cygwin" CFLAGS="-mno-cygwin" ./configure
   make

4. Get the source of Gnocl from "http://www.dr-baum.net/gnocl/index.html"
   (the link "News and Download" will direct you to another
   web site where you can download the archive file gnocl-0.9.91.tar.bz2).
   Unpack it and then apply the patch gnocl-0.9.91.patch.
   This can be done in the directory where you unpacked the source
   by the command "patch -p0 gnocl-0.9.91.patch".

5. Call "make" to create the DLLs. I use the following flags to compile:
   CPPFLAGS="-mno-cygwin -IC:/Program\ Files/Tcl\ 8.4.14/include" \
   LDFLAGS="-mms-bitfields -mno-cygwin -mwindows" SHLIB_SUFFIX=.dll make