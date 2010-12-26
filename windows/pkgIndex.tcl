# $Id: pkgIndex.tcl,v 1.14 2005/04/12 19:21:46 baum Exp $
# Tcl package index file, version 1.1
# This file is sourced either when an application starts up or
# by a "package unknown" script.  It invokes the
# "package ifneeded" command to set up package-related
# information so that packages will be loaded automatically
# in response to "package require" commands.  When this
# script is sourced, the variable $dir must contain the
# full path name of this file's directory.

package ifneeded Gnocl 0.9.91 [list load [file join $dir gnocl[info sharedlibextension]]]
package ifneeded GnoclCanvas 0.9.91 "package require -exact Gnocl 0.9.91 
   [list load [file join $dir gnoclCanvas[info sharedlibextension]]]"
package ifneeded GnoclGconf 0.9.91 "package require -exact Gnocl 0.9.91 
   [list load [file join $dir gnoclGconf[info sharedlibextension]]]"
package ifneeded GnoclVFS 0.9.91 "package require -exact Gnocl 0.9.91 
   [list load [file join $dir gnoclVFS[info sharedlibextension]]]"
package ifneeded GnoclGnome 0.9.91 "package require -exact Gnocl 0.9.91 
   [list load [file join $dir gnoclGnome[info sharedlibextension]]]"
