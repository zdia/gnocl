#---------------
# NAME
#   makefile for package GnoclMacOSX-- Package build and installation
#---------------
# AUTHORS
#   Peter G Baum, 2005
#   William J Giddings. 2010
#		adapted to MacOSX: Zbigniew Diaczyszyn, 2010
#---------------

PACKAGE_NAME := gnoclmacosx
PACKAGE_REQUIRE_NAME := GnoclMacOSX
VERSION := 0.0

CFLAGS += -pedantic -fPIC -w -Os -dynamiclib -ObjC -arch i386

TCL_VERSION := $(shell echo 'puts $$tcl_version' | tclsh)

ADDCFLAGS := -DVERSION=\"$(VERSION)\" \
	-DUSE_TCL_STUBS $(shell pkg-config --cflags gtk+-2.0) \
	-F/Developer/SDKs/MacOSX10.6.sdk/System/Library/Frameworks/Tcl.framework \
	-I/Users/zdia/gtk/inst/include/libglade-2.0 \
	-I/Users/zdia/gtk/inst/include/igemacintegration \
	-isysroot /Developer/SDKs/MacOSX10.6.sdk \
	-isystem /Users/zdia/gtk/inst/include
	#	-I/usr/include/libglade-2.0

LIBS := $(shell pkg-config \
	--libs gtk+-2.0 libglade-2.0 ) \
	-ltclstub$(TCL_VERSION) \
	-ligemacintegration \
# 	/System/Library/Frameworks/Tcl.framework/Versions/8.5/libtclstub8.5.a 

GTK_OBJ := \
	gnoclOSX.o

.PHONY: all $(PACKAGE_NAME) clean install uninstall reinstall devinstall devuninstall

# %.o : %.c; $(CC) -c $(CFLAGS) $(ADDCFLAGS) -o $*.o $<
# darwin gcc is complaining about the -o output
%.o : %.c;	$(CC) -c $(CFLAGS) $(ADDCFLAGS) $<

all: pkgIndex.tcl $(PACKAGE_NAME).dylib ;

$(PACKAGE_NAME): $(PACKAGE_NAME).dylib ;

$(PACKAGE_NAME).dylib: $(GTK_OBJ)
	# mv combo.o fileSelection.o optionMenu.o deprecated/
	# $(CC) -dynamic -arch i386 -o $@ $(GTK_OBJ) $(LIBS)
	libtool -dynamic -arch_only i386 -o $@ $(GTK_OBJ) $(LIBS) \
		/Developer/SDKs/MacOSX10.6.sdk/usr/lib/libc.dylib \
		/Users/zdia/gnocl-0.9.95/src/gnocl.dylib

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

