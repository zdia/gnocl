#---------------
# NAME
#   makefile -- Package build and installation
#---------------
# AUTHORS
#   Peter G Baum, 2005
#   William J Giddings. 2010
#---------------

PACKAGE_NAME := gnocl
PACKAGE_REQUIRE_NAME := Gnocl
VERSION := 0.9.95

CFLAGS += -pedantic -fPIC -w -Os

TCL_VERSION := $(shell echo 'puts $$tcl_version' | tclsh)

ADDCFLAGS := -DVERSION=\"$(VERSION)\" \
	-DUSE_TCL_STUBS $(shell pkg-config --cflags gtk+-2.0) \
	-I/usr/include/tcl$(TCL_VERSION) \
	-I/usr/include/libglade-2.0

LIBS := $(shell pkg-config \
	--libs gtk+-2.0 libglade-2.0 libnotify libcanberra) \
	-ltclstub$(TCL_VERSION)

GTK_OBJ := \
	aboutDialog.o \
	accelarator.o \
	action.o \
	arrowButton.o \
	aspectFrame.o \
	assistant.o \
	box.o \
	button.o \
	builder.o \
	cairo.o \
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
	fileChooserButton.o \
	fixed.o \
	fontButton.o \
	fontSelection.o \
	fontSelectionDialog.o \
	gammaCurve.o \
	gnocl.o \
	handleBox.o \
	helperFuncs.o \
	iconView.o \
	infoBar.o \
	inputDialog.o \
	image.o \
	keyfile.o \
	label.o \
	layout.o \
	linkButton.o \
	menu.o \
	menuBar.o \
	menuCheckItem.o \
	menuItem.o \
	menuRadioItem.o \
	menuRecentChooser.o \
	menuSeparator.o \
	messageDialog.o \
	notebook.o \
	notify.o \
	pageSetup.o \
	paned.o \
	parseOptions.o \
	pixBuf.o \
	pixBufFilters.o \
	pixMap.o \
	plug.o \
	print.o \
	printerDialog.o \
	printOperation.o \
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
	splashScreen.o \
	statusBar.o \
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

all: pkgIndex.tcl $(PACKAGE_NAME).so ;

$(PACKAGE_NAME): $(PACKAGE_NAME).so ;

$(PACKAGE_NAME).so: $(GTK_OBJ)
	$(CC) -shared -o $@ $(GTK_OBJ) $(LIBS)

clean:
	rm -f $(GTK_OBJ) $(PACKAGE_NAME).so

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
