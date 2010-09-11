// testfile for package gnoclMacOSX

// commands are working only interactively from terminal shell
// running the script either doesn't show the user menu

cd ..
lappend auto_path [pwd]
package require Gnocl
package require GnoclMacOSX

set menuBar [gnocl::menuBar]
set menu [gnocl::menu]

$menu add [gnocl::menuItem \
    -text "%#Quit" \
    -onClicked exit]

$menuBar add [gnocl::menuItem \
    -text "%__File" -submenu $menu]

puts "test finished"

// adding this line will freeze the application
// we will have to do some event management
// gnocl::mainLoop

// without exit command application waits for GUI exit command
// exit

