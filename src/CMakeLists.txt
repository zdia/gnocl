# This is the Gnocl configuration file for building native shared libraries
# on Unix, Windows and MacOSX with CMake

# For building a Windows dll try:
# cmake -DCMAKE_TOOLCHAIN_FILE=~/Toolchain-mingw32.cmake

# Last Update: Di 22 Feb 2011 08:16:29 CET

# The name of the project is "GNOCL". CMakeLists files in this project can
# refer to the root source directory of the project as ${GNOCL_SOURCE_DIR} and
# to the root binary directory of the project as ${GNOCL_BINARY_DIR}.

# Test version for Windows Development
# Dependencies:
# Gtk+ development package
# MinGW
# CMake

# Build call on Windows systems: cmake -G "MinGW Makefiles" ../src
# Build call on Linux systems: cmake ../src

cmake_policy(VERSION 2.8)
# explicit setting of cmake's macro policy else we get warning messages
cmake_policy ( SET CMP0011 NEW )
cmake_minimum_required (VERSION 2.6)

project (GNOCL)
set (VERSION 0.9.95)

if (UNIX)
	set ( GET_TCL_VERSION echo "puts $tcl_version" )
	set ( CMAKE_C_FLAGS "-pedantic -fPIC -w -Os" )
	
  if (APPLE)
    SET(GUI "Cocoa")
  else ()
    SET(GUI "X11")
  endif (APPLE)
  
else (UNIX)
  if (WIN32)
		set (CMAKE_C_COMPILER mingw32-gcc)
		set (CMAKE_CXX_COMPILER mingw32-g++)
		cmake_policy ( SET CMP0015 NEW )

		set ( GET_TCL_VERSION "cmd.exe /c echo puts $tcl_version" )
		set ( CMAKE_C_FLAGS "-pedantic -fPIC -w -Os -mms-bitfields" )
		
  else ()
    SET(GUI "Unknown host system")
  endif (WIN32)
  
endif (UNIX)

# --- toolchain file ----
# the name of the target operating system ( see Toolchain )
# SET(CMAKE_SYSTEM_NAME Windows)

# define all the target environment directories
# SET(CMAKE_FIND_ROOT_PATH  C:/MinGW C:/Tcl "C:/Programme/CMake 2.8" C:/Gtk )

# adjust the default behaviour of the FIND_XXX() commands:
# search headers and libraries in the target environment, search 
# programs in the host environment

# set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
# set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
# set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
# --- end of toolchain file ----

message ( ======================================================================= )
message ( STATUS "CMAKE_C_COMPILER:\t" ${CMAKE_C_COMPILER} )

# tell CMake the path for the not shipped modules
# set( CMAKE_MODULE_PATH ${CMAKE_ROOT}/Modules ${GNOCL_SOURCE_DIR}/cmake/Modules )
# message (STATUS "CMAKE_MODULE_PATH:\t" ${CMAKE_MODULE_PATH})
# message (STATUS "CMAKE_COMMAND:\t" ${CMAKE_COMMAND})

# --------- get Tcl related information -------------------------------
#
# find_library (TCL_LIBRARY  tcl85  PATHS C:/Tcl/lib )
# message ( STATUS "TCL_LIBRARY" ${TCL_LIBRARY})
find_package ( TCL REQUIRED )
find_package ( TclStub ${TCL_VERSION} REQUIRED ) 
# message (  "TCL_INCLUDE_PATH:\t" ${TCL_INCLUDE_PATH}  )
message ( STATUS "TCL_TCLSH:\t\t" ${TCL_TCLSH}  )
message ( STATUS "TCL_STUB_LIBRARY:\t" ${TCL_STUB_LIBRARY} )

# Note: ActiveState installs Tcl on Windows per default as "tclsh85.exe"

find_program (TCL_PROGRAM NAMES tclsh tclsh85 tclsh8.5 tclsh85.exe ENV PATH)

execute_process (	
	COMMAND ${GET_TCL_VERSION}
	COMMAND ${TCL_TCLSH}
	OUTPUT_VARIABLE TCL_VERSION
)
   
string ( STRIP ${TCL_VERSION} TCL_VERSION )
message ( STATUS "TCL_VERSION:\t \t" ${TCL_VERSION} )
# message ( STATUS "SYSTEM-PATH:\t \t" $ENV{PATH} )
# message ( STATUS "TCL_PROGRAM:\t \t" ${TCL_PROGRAM} )
# message ( STATUS "GNOCL_SOURCE_DIR:\t" ${GNOCL_SOURCE_DIR} )
add_definitions ( -DVERSION="${VERSION}" -DUSE_TCL_STUBS )
message ( STATUS "CMAKE_C_FLAGS:\t" ${CMAKE_C_FLAGS} )

# set (CMAKE_SHARED_LINKER_FLAGS "-Wl -shared --enable-auto-import -LC:/Tcl/lib -ltclstub8.5 -LC:/Gtk/lib -lglade-2.0-0 -ltclstub8.5" )
# message ( STATUS "CMAKE_SHARED_LINKER_FLAGS:\t" ${CMAKE_SHARED_LINKER_FLAGS} )

set (GNOCL_SOURCES
	aboutDialog.c 
	accelarator.c 
	action.c 
	arrowButton.c 
	assistant.c 
	box.c 
	button.c 
	builder.c 
	calendar.c 
	callback.c 
	checkButton.c 
	colorButton.c 
	colorSelection.c 
	colorSelectionDialog.c 
	colorWheel.c 
	comboBox.c 
	commands.c 
	curve.c 
	debug.c 
	./deprecated/combo.c
	./deprecated/fileSelection.c
	./deprecated/optionMenu.c
	drawingArea.c 
	entry.c 
	eventBox.c 
	expander.c 
	fileChooser.c 
	fileChooserDialog.c 
	fileChooserButton.c 
	fontButton.c 
	fontSelection.c 
	fontSelectionDialog.c 
	gammaCurve.c 
	gnocl.c 
	handleBox.c 
	helperFuncs.c 
	iconView.c 
	inputDialog.c 
	image.c 
	label.c 
	linkButton.c 
	menu.c 
	menuBar.c 
	menuCheckItem.c 
	menuItem.c 
	menuRadioItem.c 
	menuSeparator.c 
	messageDialog.c 
	notebook.c 
	paned.c 
	parseOptions.c 
	pixBuf.c 
	pixBufFilters.c 
	pixMap.c 
	plug.c 
	print.c 
	progressBar.c 
	radioButton.c 
	recentChooser.c 
	recentChooserDialog.c 
	ruler.c 
	scale.c 
	scaleButton.c 
	scroll.c 
	scrolledWindow.c 
	separator.c 
	socket.c 
	spinButton.c 
	statusBar.c 
	statusIcon.c 
	table.c 
	text.c 
	textBuffer.c 
	toggleButton.c 
	toolBar.c 
	treeList.c 
	volumeButton.c 
	window.c
)

# Include the directory itself as a path to include directories
# set(CMAKE_INCLUDE_CURRENT_DIR ON)

# use the predefined find modules instead of pkgconfig which is not always
# present on the host system
# FIXME: guarantee the cmake module path
# The following variables will be defined for our use:
   # GTK2_FOUND - Were all of your specified components found?
   # GTK2_INCLUDE_DIRS - All include directories
   # GTK2_LIBRARIES - All libraries
   # GTK2_VERSION - The version of GTK2 found (x.y.z)

# include(LibFindMacros)
# libfind_pkg_check_modules( GTK2 REQUIRED gtk+-2.0_PKGCONF )

# FindTCL: TK_INTERNAL_PATH was removed.
# 
# This module finds if Tcl is installed and determines
# where the include files and libraries are. It also determines
# what the name of the library is.
# This code sets the following variables:
# 
  # TCL_FOUND              = Tcl was found
  # TK_FOUND               = Tk was found
  # TCLTK_FOUND            = Tcl and Tk were found
  # TCL_LIBRARY            = path to Tcl library (tcl tcl80)
  # TCL_INCLUDE_PATH       = path to where tcl.h can be found
  # TCL_TCLSH              = path to tclsh binary (tcl tcl80)
  # TK_LIBRARY             = path to Tk library (tk tk80 etc)
  # TK_INCLUDE_PATH        = path to where tk.h can be found
  # TK_WISH                = full path to the wish executable

# ----- pkg-config search ----------------------------------------------
#
# FindXXX.cmake doesn't follow link line with dependencies like pkgconfig
include (FindPkgConfig)
pkg_check_modules (GTK+ REQUIRED gtk+-2.0 libglade-2.0)

# <XPREFIX>_LIBRARIES      ... only the libraries (w/o the '-l')
# <XPREFIX>_LIBRARY_DIRS   ... the paths of the libraries (w/o the '-L')
# <XPREFIX>_LDFLAGS        ... all required linker flags
# <XPREFIX>_LDFLAGS_OTHER  ... all other linker flags
# <XPREFIX>_INCLUDE_DIRS   ... the '-I' preprocessor flags (w/o the '-I')
# <XPREFIX>_CFLAGS         ... all required cflags
# <XPREFIX>_CFLAGS_OTHER   ... the other compiler flags
message ( STATUS "GTK+_FOUND:\t\t" ${GTK+_FOUND} )

# find_library ( LIBGLADE libglade-2.0-0 REQUIRED HINTS C:/Gtk/lib )
# message ( STATUS "LIBGLADE:\t\t" ${LIBGLADE} )

# set (GTK2_DEBUG on)
set (GTK2_DEBUG off)
# set (GTK2_ADDITIONAL_SUFFIXES)
find_package ( GTK2 REQUIRED gtk glade )
message ( STATUS "GTK2_FOUND:\t\t" ${GTK2_FOUND} )
message ( STATUS "TCL_VERSION:\t\t" ${TCL_VERSION} )
message ( STATUS "GNOCL_SOURCE_DIR:\t" ${GNOCL_SOURCE_DIR} )
# set (TCL_STUB_LIBRARY c:/Tcl/lib/libtclstub8.5.a)
message ( STATUS "TCL_STUB_LIBRARY:\t" ${TCL_STUB_LIBRARY} )
# Variable GTK2 is already in use by a FindGTK2.cmake module
message ( STATUS "GTK+_LIBRARIES:\t" ${GTK+_LIBRARIES} )
message ( STATUS "GTK+_INCLUDE_DIRS:\t" ${GTK+_INCLUDE_DIRS} )

# set (GTK2_DEBUG on)
set (GTK2_DEBUG off)
# set (GTK2_ADDITIONAL_SUFFIXES)
# find_package ( GTK2 REQUIRED gtk glade )
# message ( STATUS "GTK2_FOUND:\t\t" ${GTK2_FOUND} )
# Variable GTK2 is already in use by a FindGTK2.cmake module
# message ( STATUS "GTK2_LIBRARIES:\t" ${GTK2_LIBRARIES} )
# message ( STATUS "GTK2_LIBRARY_DIRS:\t" ${GTK2_LIBRARY_DIRS} )
# message ( STATUS "GTK2_INCLUDE_DIRS:\t" ${GTK2_INCLUDE_DIRS} )

include_directories( ${GTK+_INCLUDE_DIRS} ${TCL_INCLUDE_PATH} )

add_library ( gnocl SHARED ${GNOCL_SOURCES} )
# Result will be: libgnocl.(so|dylib|dll)

# Tells the shared library to be linked against Gtk+ and TclStub ones
target_link_libraries( gnocl ${GTK+_LIBRARIES} ${TCL_STUB_LIBRARY} )

