#---------------
# NAME
#   makefile -- test Gtk Initialisation on mac
#---------------
# AUTHORS
#		Zbigniew Diaczyszyn, 2010
#---------------

PACKAGE_NAME := gnocl
PACKAGE_REQUIRE_NAME := Gnocl
VERSION := 0.9.95

CFLAGS += -pedantic -fPIC -w -Os -ObjC -arch i386

TCL_VERSION := $(shell echo 'puts $$tcl_version' | tclsh)

# Die gedit Hilfsdateien finiden sich in /home/dia/Projekte/git/gedit/gedit
ADDCFLAGS := -DVERSION=\"$(VERSION)\" \
	-DUSE_TCL_STUBS $(shell pkg-config --cflags gtk+-2.0) \
	-F/Developer/SDKs/MacOSX10.6.sdk/System/Library/Frameworks/Tcl.framework \
	-I/Users/zdia/gtk/inst/include/libglade-2.0 \
	-I/Users/zdia/gtk/inst/include/gio-unix-2.0 \
	-I/Users/zdia/gtk/inst/include/igemacintegration \
	-I/Users/zdia/gnocl-0.9.95/src/macosx \
	-isysroot /Developer/SDKs/MacOSX10.6.sdk \
	-isystem /Users/zdia/gtk/inst/include
	#	-I/usr/include/libglade-2.0

LIBS := $(shell pkg-config \
	--libs gtk+-2.0 libglade-2.0 ) \
	-ltclstub$(TCL_VERSION) \
	-ligemacintegration

#	/System/Library/Frameworks/Tcl.framework/Versions/8.5/libtclstub8.5.a 

# Definiere das Quellmakro
GTK_OBJ := \
	osxmenu.o

.PHONY: all testosxmenu clean install uninstall reinstall devinstall devuninstall

# Regel für alle *.o Dateien
%.o : %.c
	$(CC) -c $(CFLAGS) $(ADDCFLAGS) $<

testosxmenu: $(GTK_OBJ)
	$(CC) -arch i386 -o $@ $(GTK_OBJ) $(LIBS)
# /Developer/SDKs/MacOSX10.6.sdk/usr/lib/libc.dylib

clean:
	rm -f $(GTK_OBJ)
