# NAME test.tcl
#
# AUTHOR Zbigniew Diaczyszyn
#
# Testsuite for Gnocl on MacOSX
#
# Version 0.0
#

set TESTPATH = /Users/zdia/gnocl-0.9.95/src

# initialization

cd $TESTPATH
lappend auto_path [pwd]
package require Gnocl
package require GnoclMacOSX


#
# Test gnocl::menubar
#
# set menuBar [gnocl::menuBar]
# set menu [gnocl::menu]
# 
# $menu add [gnocl::menuItem \
    # -text "%#Quit" \
    # -onClicked exit]
# 
# $menuBar add [gnocl::menuItem \
    # -text "%__File" -submenu $menu]

puts "test finished"

# // adding this line will freeze the application
# // we will have to do some event management
# // gnocl::mainLoop
# 
# // without exit command application waits for GUI exit command
# // exit

