
########### sources ###############

SET(cd-Scooby-Do_LIB_SRCS
	applet-init.c 			applet-init.h
	applet-config.c 			applet-config.h
	applet-notifications.c 		applet-notifications.h
	applet-draw.c 			applet-draw.h
	applet-session.c 			applet-session.h
	applet-listing.c 			applet-listing.h
	applet-icon-finder.c 		applet-icon-finder.h
	applet-appli-finder.c 		applet-appli-finder.h
	applet-search.c 			applet-search.h
	applet-backend-command.c 	applet-backend-command.h
	applet-backend-files.c 		applet-backend-files.h
	applet-backend-web.c 		applet-backend-web.h
	applet-backend-firefox.c 	applet-backend-firefox.h
	applet-backend-recent.c 	applet-backend-recent.h
	applet-struct.h
)

add_library(${PACKAGE_SCOOBY_DO} SHARED ${cd-Scooby-Do_LIB_SRCS})

########### compil ###############
add_definitions (-DMY_APPLET_SHARE_DATA_DIR="${scooby_dodatadir}")
add_definitions (-DMY_APPLET_PREVIEW_FILE="preview.jpg")
add_definitions (-DMY_APPLET_CONF_FILE="Scooby-Do.conf")
add_definitions (-DMY_APPLET_USER_DATA_DIR="Scooby-Do")
add_definitions (-DMY_APPLET_VERSION="${VERSION_SCOOBY_DO}")
add_definitions (-DMY_APPLET_GETTEXT_DOMAIN="${GETTEXT_SCOOBY_DO}")
add_definitions (-DMY_APPLET_DOCK_VERSION="${dock_version}")
add_definitions (-DMY_APPLET_ICON_FILE="icon.png")
add_definitions (-DGL_GLEXT_PROTOTYPES="1")

include_directories (
	${PACKAGE_INCLUDE_DIRS})

link_directories (
	${PACKAGE_LIBRARY_DIRS})

target_link_libraries (${PACKAGE_SCOOBY_DO}
	${PACKAGE_LIBRARIES})

########### install files ###############

install(TARGETS ${PACKAGE_SCOOBY_DO} DESTINATION ${pluginsdir})


########### install files ###############




#original Makefile.am contents follow:

# #dnl Process this file with automake to produce Makefile.in
#
#lib_LTLIBRARIES = 
#	libcd-Scooby-Do.la
#
#
#libcd_Scooby_Do_la_SOURCES = 
#	applet-init.c
#	applet-init.h
#	applet-config.c
#	applet-config.h
#	applet-notifications.c
#	applet-notifications.h
#	applet-draw.c
#	applet-draw.h
#	applet-session.c
#	applet-session.h
#	applet-listing.c
#	applet-listing.h
#	applet-icon-finder.c
#	applet-icon-finder.h
#	applet-appli-finder.c
#	applet-appli-finder.h
#	applet-search.c
#	applet-search.h
#	applet-backend-command.c
#	applet-backend-command.h
#	applet-backend-files.c
#	applet-backend-files.h
#	applet-backend-web.c
#	applet-backend-web.h
#	applet-backend-firefox.c
#	applet-backend-firefox.h
#	applet-backend-recent.c
#	applet-backend-recent.h
#	applet-struct.h
#
#
#dock_version = `pkg-config --modversion cairo-dock`
#
#libcd_Scooby_Do_la_CFLAGS = 
#	-I$(top_srcdir)/src
#	$(PACKAGE_CFLAGS)
#	-DMY_APPLET_SHARE_DATA_DIR=""${scooby_dodatadir)""
#	-DMY_APPLET_PREVIEW_FILE=""preview.jpg""
#	-DMY_APPLET_CONF_FILE=""Scooby-Do.conf""
#	-DMY_APPLET_USER_DATA_DIR=""Scooby-Do""
#	-DMY_APPLET_VERSION=""${VERSION_SCOOBY_DO)""
#	-DMY_APPLET_GETTEXT_DOMAIN=""${GETTEXT_SCOOBY_DO)""
#	-DMY_APPLET_DOCK_VERSION=""${dock_version)""
#	-DMY_APPLET_ICON_FILE=""icon.png""
#	-DGL_GLEXT_PROTOTYPES=""1""
#	-std=c99
#	-Werror-implicit-function-declaration
##	-g -ggdb -W -Wall
#
#
#libcd_Scooby_Do_la_LIBADD = 
#	$(PACKAGE_LIBS) -lm
#
#
#libcd_Scooby_Do_la_LDFLAGS =
#	-avoid-version -module
