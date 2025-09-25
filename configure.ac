dnl Process this file with autoconf to produce a configure script.

AC_INIT([FoxtrotNoGPS], [1.2.2+], [foss-gps@lists.osgeo.org])
AM_INIT_AUTOMAKE([foreign no-define -Wall -Werror -Wno-portability])
AC_CONFIG_SRCDIR([src/main.c])
AC_CONFIG_HEADERS([config.h])
AM_MAINTAINER_MODE([enable])

AC_PROG_CC_C99
AC_HEADER_STDC

AM_GLIB_GNU_GETTEXT
GETTEXT_PACKAGE=foxtrotnogps
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE, "$GETTEXT_PACKAGE", [Package name for gettext])

AC_PROG_LN_S
LT_INIT

PKG_CHECK_MODULES(GTK, [gtk+-3.0 gdk-2.0 gthread-2.0])
PKG_CHECK_MODULES(GLESV2, [glesv2 egl])
PKG_CHECK_MODULES(LIBCURL, [libcurl])
PKG_CHECK_MODULES(SQLITE, [sqlite3])
PKG_CHECK_MODULES(LIBEXIF, [libexif])
PKG_CHECK_MODULES(LIBXML, [libxml-2.0])
PKG_CHECK_MODULES(BLUETOOTH, [bluez],
                  [AC_DEFINE(HAVE_BLUEZ, [],
                             [We have the BlueZ bluetooth library])
                   AC_DEFINE(ENABLE_HRM, [],
                             [Heartrate-monitor support is enabled])],
                  [AC_MSG_WARN([Building without heartrate-monitor support])])

GLIB_GSETTINGS

AC_CONFIG_FILES([
Makefile
src/Makefile
])
AC_OUTPUT
