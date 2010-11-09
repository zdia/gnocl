#---------------
# NAME
#   makefile for package GnoclMacOSX-- Package build and installation
#---------------
# AUTHORS
#   Peter G Baum, 2005
#   William J Giddings. 2010
#		adapted to MacOSX: Zbigniew Diaczyszyn, 2010
#		see https://github.com/zdia/gnocl/wiki/gnocl-on-MacOSX
#---------------
# DEPENDENCIES
#		Gtk-osx installed
#		Gnocl installed
#		Git installed
#		ActiveState Tcl 8.5 for MacOSX
#		build is started in 'jhbuild shell' environment
#---------------

PACKAGE_NAME := gnoclmacosx
PACKAGE_REQUIRE_NAME := GnoclMacOSX
VERSION := 0.0

CFLAGS += -pedantic -fPIC -w -Os -dynamiclib -ObjC -arch i386

TCL_VERSION := $(shell echo 'puts $$tcl_version' | tclsh)
GNOCLLIB := "/Users/zdia/gnocl-0.9.95/src/gnocl.dylib"

# if not AS Tcl/Tk then:
# -F/Developer/SDKs/MacOSX10.6.sdk/System/Library/Frameworks/Tcl.framework

ADDCFLAGS := -DVERSION=\"$(VERSION)\" \
	-DUSE_TCL_STUBS $(shell pkg-config --cflags gtk+-2.0 ige-mac-integration) \
	-F/Library/Frameworks/Tcl.framework 

LIBS := $(shell pkg-config \
	--libs gtk+-2.0 ige-mac-integration) \
	-ltclstub$(TCL_VERSION) \

GTK_OBJ := \
	gnoclOSX.o

.PHONY: all $(PACKAGE_NAME) clean install uninstall reinstall devinstall devuninstall

%.o : %.c; $(CC) -c $(CFLAGS) $(ADDCFLAGS) -o $*.o $<

all: pkgIndex.tcl $(PACKAGE_NAME).dylib ;

$(PACKAGE_NAME): $(PACKAGE_NAME).dylib ;

# jhbuild's modules work with the autotools' libtool 
# for the moment we build the lib with Apple's native tool

$(PACKAGE_NAME).dylib: $(GTK_OBJ)
	/usr/bin/libtool -dynamic -arch_only i386 -o $@ $(GTK_OBJ) $(LIBS) \
		/Developer/SDKs/MacOSX10.6.sdk/usr/lib/libc.dylib $(GNOCLLIB)

clean:
	rm -f $(GTK_OBJ) $(PACKAGE_NAME).dylib

install:
	./install.tcl $(PACKAGE_REQUIRE_NAME) $(VERSION) $(PACKAGE_NAME) install


uninstall:
	./install.tcl $(PACKAGE_REQUIRE_NAME) $(VERSION) $(PACKAGE_NAME) uninstall

reinstall:
	./install.tcl $(PACKAGE_REQUIRE_NAME) $(VERSION) $(PACKAGE_NAME) uninstall
	./install.tcl $(PACKAGE_REQUIRE_NAME) $(VERSION) $(PACKAGE_NAME) install

test:
	( \
	cd ../demo; \
	./doTests; \
	cd ../src; \
	)

devinstall:
	ar cr libgnocl-$(VERSION).a $(GTK_OBJ)
	cp gnocl.h /usr/include
	cp libgnocl*.a /usr/lib
	rm libgnocl*.a

devuninstall:
	rm /usr/include/gnocl.h
	rm /usr/lib/libgnocl*.a

