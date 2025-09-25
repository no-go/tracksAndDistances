## Process this file with automake to produce Makefile.in

foxtrotgps_datadir = $(datadir)/applications
foxtrotgps_data_in_files = foxtrotgps.desktop.in
foxtrotgps_data_DATA = $(foxtrotgps_data_in_files:.desktop.in=.desktop)

@INTLTOOL_DESKTOP_RULE@


gsettings_SCHEMAS = org.foxtrotgps.gschema.xml

data_convertdir = $(datadir)/GConf/gsettings
dist_data_convert_DATA = org.foxtrotgps.convert

@GSETTINGS_RULES@


dist_pkgdata_DATA = foxtrotgps.glade

EXTRA_DIST = $(foxtrotgps_data_in_files) $(gsettings_SCHEMAS)
DISTCLEANFILES = $(foxtrotgps_data_in_files:.desktop.in=.desktop)
