#---------------
# NAME
#   Makefile -- Package build and installation for gnocl.dylib
#---------------
# AUTHORS
#   Peter G Baum, 2005
#   William J Giddings. 2010
#		adapted to MacOSX: Zbigniew Diaczyszyn, 2010
#		see https://github.com/zdia/gnocl/wiki/gnocl-on-MacOSX
#---------------
# DEPENDENCIES
#		Gtk+-2.0
#		Git installed
#		ActiveState Tcl 8.5 for MacOSX

PACKAGE_NAME := gnocl
PACKAGE_REQUIRE_NAME := Gnocl
VERSION := 0.9.95

CFLAGS += -pedantic -fPIC -w -Os -dynamiclib -ObjC -arch i386

TCL_VERSION := $(shell echo 'puts $$tcl_version' | tclsh)

# if not AS Tcl/Tk then:
# -F/Developer/SDKs/MacOSX10.6.sdk/System/Library/Frameworks/Tcl.framework

ADDCFLAGS := -DVERSION=\"$(VERSION)\" \
	-DUSE_TCL_STUBS $(shell pkg-config --cflags gtk+-2.0 libglade-2.0) \
	-F/Library/Frameworks/Tcl.framework 

LIBS := $(shell pkg-config \
	--libs gtk+-2.0 libglade-2.0 ige-mac-integration) \
	-ltclstub$(TCL_VERSION) \

GTK_OBJ := \
	aboutDialog.o \
	accelarator.o \
	action.o \
	arrowButton.o \
	assistant.o \
	box.o \
	button.o \
	builder.o \
	calendar.o \
	callback.o \
	checkButton.o \
	colorButton.o \
	colorSelection.o \
	colorSelectionDialog.o \
	colorWheel.o \
	comboBox.o \
	commands.o \
	curve.o \
	debug.o \
	./deprecated/combo.o \
	./deprecated/fileSelection.o \
	./deprecated/optionMenu.o \
	drawingArea.o \
	entry.o \
	eventBox.o \
	expander.o \
	fileChooser.o \
	fileChooserDialog.o \
	fileChooserButton.o \
	fontButton.o \
	fontSelection.o \
	fontSelectionDialog.o \
	gammaCurve.o \
	gnocl.o \
	handleBox.o \
	helperFuncs.o \
	iconView.o \
	inputDialog.o \
	image.o \
	label.o \
	linkButton.o \
	menu.o \
	menuBar.o \
	menuCheckItem.o \
	menuItem.o \
	menuRadioItem.o \
	menuSeparator.o \
	messageDialog.o \
	notebook.o \
	paned.o \
	parseOptions.o \
	pixBuf.o \
	pixBufFilters.o \
	pixMap.o \
	plug.o \
	print.o \
	progressBar.o \
	radioButton.o \
	recentChooser.o \
	recentChooserDialog.o \
	ruler.o \
	scale.o \
	scaleButton.o \
	scroll.o \
	scrolledWindow.o \
	separator.o \
	socket.o \
	spinButton.o \
	statusBar.o \
	statusIcon.o \
	table.o \
	text.o \
	textBuffer.o \
	toggleButton.o \
	toolBar.o \
	treeList.o \
	volumeButton.o \
	window.o

.PHONY: all $(PACKAGE_NAME) clean install uninstall reinstall devinstall devuninstall

%.o : %.c; $(CC) -c $(CFLAGS) $(ADDCFLAGS) -o $*.o $<
# darwin gcc is complaining about the -o output
# %.o : %.c;	$(CC) -c $(CFLAGS) $(ADDCFLAGS) $<

all: pkgIndex.tcl $(PACKAGE_NAME).dylib ;

$(PACKAGE_NAME): $(PACKAGE_NAME).dylib ;

$(PACKAGE_NAME).dylib: $(GTK_OBJ)
	# mv combo.o fileSelection.o optionMenu.o deprecated/
	# $(CC) -dynamic -arch i386 -o $@ $(GTK_OBJ) $(LIBS) \
		# /Developer/SDKs/MacOSX10.6.sdk/usr/lib/libc.dylib
		
	# jhbuild's modules work with the autotools' libtool 
	# for the moment we build the lib with Apple's native tool
	/usr/bin/libtool -dynamic -arch_only i386 -o $@ $(GTK_OBJ) $(LIBS) \
		/Developer/SDKs/MacOSX10.6.sdk/usr/lib/libc.dylib

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

