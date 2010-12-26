# NAME test.tcl
#
# AUTHOR Zbigniew Diaczyszyn
#
# Testsuite for Gnocl on MacOSX
# Zbigniew Diaczyszyn z_dot_dia_at_gmx_dot_de
# Version 0.0
#
# Linux: tclsh ~/Projekte/git/gnocl/macosx/test.tcl
# Xfce 4.6.1

set macosx 0

if { $macosx } {
	set TESTPATH "/Users/zdia/gnocl-0.9.95/src"
} else {
	set TESTPATH /home/dia/Projekte/tcl/gnocl-0.9.95/src
}

cd $TESTPATH
lappend auto_path [pwd]
package require Gnocl
if { $macosx } { package require GnoclMacOSX }

# structure of windowOptions: option | value | ms_delay
# for detailed testing you can increase the delay or add
# an item in test-window's switch

set windowOptions [list \
-name GnoclWindow \
-title Gnocl-Window \
-backgroundColor blue \
-defaultWidth 640 \
-defaultHeight 480 \
-visible {0 1} \
-decorated {0 1} \
-x 100 \
-y 100 \
-width 100 \
-width 300 \
-height 100 \
-height 300 \
-resizable {0 1} \
-deletable {0 1} \
-allowGrow {0 1} \
-allowShrink {0 1} \
-sensitive {0 1} \
-data MyData \
-dropTargets MyWidget \
-onDelete exit \
-onDestroy Destroycommand \
-onDragData MyDragData \
-onDropData MyDropData \
-onKeyPress MyKeyCommnd \
-onKeyRelease MyKeyReleaseCommand \
-onPopupMenu ShowPopup \
-onShowHelp ShowHelp \
-tooltip MyToolTip \
-borderWidth 12 \
-heightRequest 400 \
-widthRequest 400 \
-onRealize widget \
-onRealize w% \
-onResize OnResizeCommand \
-onEnter OnEnterCommand \
-onLeave OnLeaveCommand \
-skipPagerHint {0 1} \
-skipTaskBarHint {0 1} \
-urgencyHint {0 1} \
-acceptFocus {0 1} \
-focusOnMap {0 1} \
-gravity 1 \
-onActivateDefault testCommand \
-onActivateFocus testCommand \
-onFrameEvent testCommand \
-onFocusIn testCommand \
-onFocusOut testCommand \
-onMoveFocus onMoveFocus \
-onKeysChanged testCommand \
-onSetFocus testCommand \
-destroyWithParent {0 1} \
-onMotion onMotion \
-onButtonPress onButtonPress \
-onButtonRelease onButtonRelease \
-modal {1 0} \
-child "" \
]

	# /* window specific signals */
	# /* "activate-default" , "activate-focus", "frame-event", "keys-changed", "set-focus" */
# 

set windowFunc [list ]

# "delete", "configure", "cget", "iconify",
# "center", "centre", "beep", "class", "reshow",
# "geometry", "pointer", "reposition", "grab", "ungrab",
# "hide", "show",

proc assert { opt val } {
	# check if the widget returns the expected value
	puts -nonewline "$opt $val"
	flush stdout
	if { [ catch { $::win cget $opt } val2] } {
		puts "\nError: $val2"
		return
	}
	if { $val != $val2 } {
		puts " Error: $opt: $val != $val2"
	} else {
		puts " ok"
	}
}

proc test-window-1 {  } {
	# checking all the functions and options available in window.c
	
	global windowOptions windowFunc
	
	puts "Automatic Testing of gnocl::window"
	
	set ::win [gnocl::window]
	
	foreach {opt values} $windowOptions {
		foreach val $values {
			if { [catch { $::win configure $opt $val} err] } {
				puts $err
			}
			gnocl::update
			after 200
			gnocl::update
			assert $opt $val
			# for special cases we use:
			# switch $opt {
				# -resizable { 
					# puts "we set -resizable to $val"
						# $::testWindow configure -resizable $val -height 300 -width 300 
				# }
				# default {  }
			# } ;# end switch
   }
	}
	
	$::win delete
	
} ;# end of proc

proc test-window-2 {  } {
	gnocl::configure -defaultIcon "%/../demo/one.png"
	
	set box [gnocl::box -orientation vertical]
	set testWindow [ gnocl::window -child $box -title "Test Window" \
		-onShowHelp {puts "show help %w %h"} \
		-onPopupMenu {puts "%w popup menu"} \
		-tooltip "option -tooltip is activated" \
		-onKeyPress {puts [list KeyPress: %w %k %K %a]} \
		-onKeyRelease {puts [list KeyRelease: %w %k %K %a]} \
		-onDestroy { puts "destroying %w" } \
		-onDelete { exit } \
		-icon "%/../demo/c.png" \
		-defaultWidth 300 -defaultHeight 300 ]
		
	$box add [gnocl::checkButton -text "allowShrink"  \
		-onToggled "$testWindow configure -allowShrink %v"]
	$box add [gnocl::checkButton -text "allowGrow"  \
		-onToggled "$testWindow configure -allowGrow %v"]
	$box add [gnocl::checkButton -text "resizable" -active 1 \
		-onToggled "$testWindow configure -resizable %v"]
	$box add [gnocl::checkButton -text "modal" \
		-onToggled "$testWindow configure -modal %v"]
	$box add [gnocl::checkButton -text "decorated" -active 1 \
		-onToggled "$testWindow configure -decorated %v"]

	set box [gnocl::box -orientation vertical]
	$box add [gnocl::checkButton -text "visible" -active 1 \
		-onToggled "$testWindow configure -visible %v"]
	$box add [gnocl::checkButton -text "iconify" -active 0 \
		-onToggled "$testWindow iconify %v"]
	$box add [gnocl::checkButton -text "sensitive" -active 1 \
		-onToggled "$testWindow configure -sensitive %v"]
	$box add [gnocl::box -orientation horizontal -borderWidth 0 \
		-children [list \
		[gnocl::label -text "root x" -widthGroup labelGroup] \
		[gnocl::spinButton -lower 0 -upper 400 -stepInc 10 -digits 0 \
					-value 10 \
					-onValueChanged "$testWindow configure -x %v"]]]
	$box add [gnocl::box -orientation horizontal -borderWidth 0 \
		-children [list \
		[gnocl::label -text "root y" -widthGroup labelGroup] \
		[gnocl::spinButton -lower 0 -upper 400 -stepInc 10 -digits 0 \
					-value 10 \
					-onValueChanged "$testWindow configure -y %v"]]]
	$box add [gnocl::box -orientation horizontal -borderWidth 0 \
		-children [list \
		[gnocl::label -text "width" -widthGroup labelGroup] \
		[gnocl::spinButton -lower 10 -upper 400 -stepInc 10 -digits 0 \
					-value 200 \
					-onValueChanged "$testWindow configure -width %v"]]]
	$box add [gnocl::box -orientation horizontal -borderWidth 0 \
		-children [list \
		[gnocl::label -text "height" -widthGroup labelGroup] \
		[gnocl::spinButton -lower 10 -upper 400 -stepInc 10 -digits 0 \
					-value 200 \
					-onValueChanged "$testWindow configure -height %v"]]]
	$box add [gnocl::box -orientation horizontal -borderWidth 0 \
		-children [list \
		[gnocl::label -text "borderWidth" -widthGroup labelGroup] \
		[gnocl::spinButton -lower 1 -upper 40 -stepInc 1 -digits 0 \
					-value 1 \
					-onValueChanged "$testWindow configure -borderWidth %v"]]]
					
	set win [gnocl::window -child $box -onDestroy exit \
		-title "Modify Test Window" -width 300]
	# it is possible to embed this window into another by XEMBED protocol
	
} ;# end of proc

# ======================= main =============================

test-window-1
# test-window-2

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

# percent strings
# %w    widget name
# %v    value of the radio group
# %h    help type: either "whatsThis" or "tooltip"
# %t    type of event: always buttonRelease
# %x    x coordinate
# %y    y coordinate
# %b    button number
# %s    state of the buttons and modifiers (bitmask) 

