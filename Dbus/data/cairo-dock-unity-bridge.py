#!/usr/bin/python
#
# This is a part of the Cairo-Dock plug-ins.
# Copyright : (C) 2012 by Fabrice Rey
# E-mail : fabounet@glx-dock.org
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 3
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
# http://www.gnu.org/licenses/licenses.html#GPL
#

import sys
import glib
import gobject
import re
import dbus, dbus.service
from dbus.mainloop.glib import DBusGMainLoop
from CairoDock import CairoDock

class Launcher:
	has_count = False
	count = 0
	count_visible = False
	has_progress = False
	progress = 0.
	progress_visible = False
	has_emblem = False
	emblem = 0
	emblem_visible = False
	menu_path = ''
	urgent = False
	
	def __init__(self, app_uri=None, dock=None, dbus_name=None):
		## initialize the properties of the launcher
		self.app_uri = app_uri
		self.dock = dock
		self.dbus_name = dbus_name
		desktop_file = re.sub ('application://', '', app_uri)
		app_name = re.sub ('.desktop', '', desktop_file)
		self.id = 'class='+app_name
		
		## watch for the deconnection of the distant app (we don't want to let an icon demanding attention or with an invalid menu).
		bus = dbus.SessionBus()
		bus.watch_name_owner(self.dbus_name, self.on_name_owner_changed)
	
	def update_dbus_name(self, dbus_name):
		if dbus_name != self.dbus_name:
			self.dbus_name = dbus_name
			bus = dbus.SessionBus()
			bus.watch_name_owner(self.dbus_name, self.on_name_owner_changed)
	
	def on_name_owner_changed(self,connection_name):
		print "launcher bus name changed:",connection_name
		if len(connection_name) == 0:  # no more distant app -> reset the launcher and its icon.
			print "-> the launcher "+self.dbus_name+" is no more connected to its app."
			if self.has_count and self.count_visible:
				self.dock.iface.SetQuickInfo('', self.id)
				self.has_count = False
				self.count_visible = False
			if self.has_progress and self.progress_visible:
				self.dock.iface.SetProgress (-1, self.id)
				self.has_progress = False
				self.progress_visible = False
			if self.has_emblem and self.emblem_visible:
				self.dock.iface.SetEmblem('', CairoDock.EMBLEM_TOP_RIGHT, self.id)
				self.has_emblem = False
				self.emblem_visible = False
			if self.urgent:
				self.dock.iface.DemandsAttention(False, 'default', self.id)
				self.urgent = False
			if self.menu_path != '':
				self.dock.iface.SetMenu(self.dbus_name, '', self.id)
				self.menu_path = ''
			self.dbus_name = None
			
	def set_count(self, x):
		print 'set_count (%d)'%x
		self.count = x
		self.has_count = True
		if self.count_visible:
			self.dock.iface.SetQuickInfo(str(self.count), self.id)
	
	def set_count_visible(self, x):
		print 'set_count_visible (%d)'%x
		self.count_visible = x
		if self.has_count:
			if x:
				self.dock.iface.SetQuickInfo(str(self.count), self.id)
			else:
				self.dock.iface.SetQuickInfo('', self.id)
	
	def set_progress(self, x):
		print 'set_progress (%.2f)'%x
		self.progress = x
		self.has_progress = True
		self.dock.iface.SetProgress (x, self.id)
	
	def set_progress_visible(self, x):
		print 'set_progress_visible (%d)'%x
		self.progress_visible = x
		if not x:
			self.dock.iface.SetProgress (-1, self.id)
		else:
			self.dock.iface.SetProgress (self.progress, self.id)
	
	def set_urgent(self, x):
		print 'set_urgent (%d)'%x
		self.urgent = x
		self.dock.iface.DemandsAttention(x, 'default', self.id)
	
	def set_menu(self, menu_path):
		print 'set_menu (%s)'%menu_path
		self.menu_path = menu_path
		self.dock.iface.SetMenu(self.dbus_name, menu_path, self.id)
	
	def set_emblem(self, x):
		print 'set_emblem (%s)'%x
		self.emblem = x
		self.has_emblem = True
		if self.emblem_visible:
			self.dock.iface.SetEmblem(self.emblem, CairoDock.EMBLEM_TOP_RIGHT, self.id)
	
	def set_emblem_visible(self, x):
		print 'set_emblem_visible (%d)'%x
		self.emblem_visible = x
		if self.has_emblem:
			if x:
				self.dock.iface.SetEmblem(self.emblem, CairoDock.EMBLEM_TOP_RIGHT, self.id)
			else:
				self.dock.iface.SetEmblem('', CairoDock.EMBLEM_TOP_RIGHT, self.id)
	

class ULWatcher:
	bus_iface_str = 'com.canonical.Unity.LauncherEntry'  # bus name to watch
	bus_name_str = 'com.canonical.Unity'  # bus name to claim, to signal our presence on the bus.
	launchers = {}  # list of launchers that have been seen on the bus at least once.
	
	def __init__(self):
		DBusGMainLoop(set_as_default=True)
		try: 
			self.bus = dbus.SessionBus()
		except Exception, exception:
			print 'Could not open dbus. Uncaught exception.'
			return
		     
		bus_name = dbus.service.BusName (self.bus_name_str, self.bus)
		print "registered as Unity:",bus_name
		
		self.bus.add_signal_receiver (self.on_launcher_entry_signal, dbus_interface=self.bus_iface_str, member_keyword='member', sender_keyword='sender')
		
		self.dock = CairoDock()
		
		self.loop = gobject.MainLoop()
		self.loop.run()
	
	def on_launcher_entry_signal(self, val1=None, val2=None, member=None, sender=None):  # application://evolution.desktop, dictionnary (sv)
		print '>>> signal',member,' has been received (',val1,val2,')'
		if (member == 'Update'):
			print '  update',val1,'with',val2
			
			launcher = None
			if val1 in self.launchers:
				launcher = self.launchers[val1]
				launcher.update_dbus_name(sender)
			else:
				launcher = Launcher(val1, self.dock, sender)
				self.launchers[val1] = launcher
			
			if 'count' in val2:
				count = val2['count']
				print ' set count to',count
				launcher.set_count (count)
			
			if 'progress' in val2:
				progress = val2['progress']
				print ' set progress to',progress
				launcher.set_progress (progress)
			
			if 'emblem' in val2:
				emblem = val2['emblem']
				print ' set emblem to',emblem
				launcher.set_emblem (emblem)
			
			if 'urgent' in val2:
				urgent = val2['urgent']
				print ' set urgent to',urgent
				launcher.set_urgent (urgent)
			
			if 'quicklist' in val2:
				menu_path = val2['quicklist']
				print ' set quicklist to',menu_path
				launcher.set_menu (menu_path)
			
			if 'count-visible' in val2:
				count_visible = val2['count-visible']
				print ' set count-visible to',count_visible
				launcher.set_count_visible (count_visible)
			
			if 'progress-visible' in val2:
				progress_visible = val2['progress-visible']
				print ' set progress-visible to',progress_visible
				launcher.set_progress_visible (progress_visible)
			
			if 'emblem-visible' in val2:
				emblem_visible = val2['emblem-visible']
				print ' set emblem-visible to',emblem_visible
				launcher.set_emblem_visible (emblem_visible)
			
		elif (member == 'Query'):
			print '  query',val2,'for',val1
		else:
			print 'unknown signal'
		
if __name__ == '__main__':
	ULWatcher()
