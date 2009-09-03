#!/usr/bin/python

### This is the external applet demo_bash.py for cairo-dock
### Author : Nochka85 (based on the demo.py by Fabounet)
### Contact : nochka85@cairo-dock.org
### Rev : 09/09/03

### We assume the name of this applet is "demo_bash"
### This Python script (demo_bash.py) should be placed in a folder with 3 files :
### "icon" (the default icon of the applet) , "preview" (a preview of this applet) and demo_script.sh (the script in bash)

### import ###
import sys
import gobject
import glib
import gtk
import dbus
import os.path
from dbus.mainloop.glib import DBusGMainLoop


### env variables ###
app_folder = os.getcwd()	# name of the script folder
app_name = os.popen("TEMP=`basename " + sys.argv[0] +"` && echo $TEMP | cut -f1 -d '.' ").read().rstrip()	# name of the script without th '.py' extension AND without the './' if running as a script


### let's connect to the dock.###
DBusGMainLoop(set_as_default=True)
bus = dbus.SessionBus()
try:
	dock_object = bus.get_object("org.cairodock.CairoDock",
			"/org/cairodock/CairoDock")
except dbus.DBusException:
	print app_name + " applet : Cairo-Dock not found on bus (did you activate its 'DBus' plug-in ?)"
	sys.exit(1)
dock_iface = dbus.Interface(dock_object, "org.cairodock.CairoDock")


### let's register our applet !###
return_from_bash = os.popen(app_folder + "/" + app_name + ".sh " + " register_the_applet").read().rstrip()
try:
	applet_object = bus.get_object("org.cairodock.CairoDock",
			"/org/cairodock/CairoDock/" + app_name)
except dbus.DBusException:
	print app_name + " applet : the module has not been registered"
	sys.exit(2)
applet_iface = dbus.Interface(applet_object, "org.cairodock.CairoDock.applet")
	

### init ###
def init():
	# register to the notifications on our applet
	applet_iface.connect_to_signal("on_click_icon", action_on_click)
	applet_iface.connect_to_signal("on_middle_click_icon", action_on_middle_click)
	applet_iface.connect_to_signal("on_scroll_icon", action_on_scroll_icon)
	applet_iface.connect_to_signal("on_drop_data", action_on_drop_data)
	applet_iface.connect_to_signal("on_init_module", action_on_init)
	applet_iface.connect_to_signal("on_stop_module", action_on_stop)
	applet_iface.connect_to_signal("on_reload_module", action_on_reload)
	applet_iface.connect_to_signal("on_build_menu", action_on_build_menu)
	applet_iface.connect_to_signal("on_menu_select", action_on_menu_select)
	action_on_init()


### stop ###
def stop():
	print "STOP"
	bus = dbus.SessionBus()
	remote_object = bus.get_object("org.cairodock.CairoDock", "/org/cairodock/CairoDock")
	iface = dbus.Interface(remote_object, "org.cairodock.CairoDock")
	iface.UnregisterModule(app_name)
	del iface
	del remote_object
	del bus
	

### callbacks ###
def action_on_click(iState):
	return_from_bash = os.popen(app_folder + "/" + app_name + ".sh " + " action_on_click").read().rstrip()
	print return_from_bash
	
def action_on_middle_click():
	return_from_bash = os.popen(app_folder + "/" + app_name + ".sh " + " action_on_middle_click").read().rstrip()
	print return_from_bash

def action_on_scroll_icon(bUpOrDown):
	if bUpOrDown:
		return_from_bash = os.popen(app_folder + "/" + app_name + ".sh " + " action_on_scroll_icon 0").read().rstrip()
		print return_from_bash
	else:
		return_from_bash = os.popen(app_folder + "/" + app_name + ".sh " + " action_on_scroll_icon 1").read().rstrip()
		print return_from_bash

def action_on_drop_data(sDataName):
	return_from_bash = os.popen(app_folder + "/" + app_name + ".sh " + " action_on_drop_data " + sDataName).read().rstrip()
	print return_from_bash

def action_on_init():
	return_from_bash = os.popen(app_folder + "/" + app_name + ".sh " + " action_on_init").read().rstrip()
	print return_from_bash
	
def action_on_stop():
	return_from_bash = os.popen(app_folder + "/" + app_name + ".sh " + " action_on_stop").read().rstrip()
	print return_from_bash
	
def action_on_reload(bConfigHasChanged):
	if bConfigHasChanged:
		return_from_bash = os.popen(app_folder + "/" + app_name + ".sh " + " action_on_reload").read().rstrip()
		print return_from_bash

def action_on_build_menu():
	return_from_bash = os.popen(app_folder + "/" + app_name + ".sh " + " action_on_build_menu").read().rstrip()
	print return_from_bash
	
def action_on_menu_select(iNumEntry):
	return_from_bash = os.popen(app_folder + "/" + app_name + ".sh " + " action_on_menu_select " + str(iNumEntry)).read().rstrip()
	print return_from_bash


### main ###
if __name__ == '__main__':
	init()
	gtk.main()
	stop()
	sys.exit(0)
