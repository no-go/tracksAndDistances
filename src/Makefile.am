## Process this file with automake to produce Makefile.in

AM_CPPFLAGS = \
	-DPACKAGE_DATA_DIR="\"$(datadir)\"" \
	-DPACKAGE_PIXMAPS_DIR="\"$(datadir)/pixmaps\"" \
	-DPACKAGE_LOCALE_DIR="\"$(localedir)\"" \
	-DPACKAGE_HTML_DIR="\"$(htmldir)\"" \
	-DPACKAGE_LIBEXEC_DIR="\"$(pkglibexecdir)\"" \
	\
	$(GTK_CFLAGS) \
	$(GLADE_CFLAGS) \
	$(LIBCURL_CFLAGS) \
	$(SQLITE_CFLAGS) \
	$(LIBEXIF_CFLAGS) \
	$(LIBXML_CFLAGS) \
	$(BLUETOOTH_CFLAGS)

bin_PROGRAMS = foxtrotnogps

foxtrotnogps_SOURCES = \
	support.h\
	support.c\
	main.c\
	interface.h\
	interface.c\
	callbacks.h\
	callbacks.c\
	map_management.c\
	map_management.h\
	globals.c\
	globals.h\
	converter.c\
	converter.h\
	gps_functions.c\
	gps_functions.h\
	init.c\
	init.h\
	tile_management.c\
	tile_management.h\
	friends.c\
	friends.h\
	geo_photos.c\
	geo_photos.h\
	poi.c\
	poi.h\
	util.c\
	util.h\
	wp.c\
	wp.h\
	tracks.c\
	tracks.h\
	route.c\
	route.h\
	exif.c\
	exif.h\
	hrm_functions.c\
	hrm_functions.h

foxtrotnogps_LDFLAGS = -export-dynamic

foxtrotnogps_LDADD =  \
	$(GTK_LIBS) \
	$(GLADE_LIBS) \
	$(LIBCURL_LIBS) \
	$(SQLITE_LIBS) \
	$(LIBEXIF_LIBS) \
	$(LIBXML_LIBS) \
	$(BLUETOOTH_LIBS) \
	-lm
