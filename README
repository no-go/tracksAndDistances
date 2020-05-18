# Not original FoxtrotGPS!

FoxtrotGPS is an easy-to-use graphical tool that can be used
to track the position of a GPS receiver on a map in relation to
user-defined points of interest (POIs), a destination and waypoints,
and tracks loaded from files or internet routing-services.

This code is a modified version of "1.2.2, revno 318, Wed 2019-07-10 02:40:54 -0400".
The original authors are:

    Joshua Judson Rosen <rozzin@geekspace.com>
    Paul Wise <pabs3@bonedaddy.net>
    
    Thanks to the original author of tangoGPS, from which FoxtrotGPS is descended:
    Marcus Bauer <marcus.bauer@gmail.com>
    
See https://www.foxtrotgps.org/ for more detail! If you want to contribute,
use their unknown version control system "Bazaar".

## Why did I modify it?

There are no good tools for drawing and displaying routes WITHOUT 
gps-log (including distance measurement). Various websites require 
an upload or fail to display.

FoxtrotGPS has a good function to load and display tracks, but I missed the 
possibility to create them manually. For this reason I have extended the 
function for distance measurement, where each measuring point (incl. distance) 
is added (and not replaced) in the clipboard.

The content of the clipboard can then easily be saved with a text editor and 
later be loaded and displayed as a track in FoxtrotGPS. Why other programs 
and websites can't do this and only create tracks with a GPS mouse is a 
mystery to me since many years.

## Patch?!

In file `src/callbacks.c:on_item4_activate()` I place a new Clipboard code:

```
    GtkClipboard * clipdummy;
    clipdummy = gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);
    gchar * cliptext = gtk_clipboard_wait_for_text(clipdummy);
    gchar clipnew[512];
    GString * toclipboard;
    toclipboard = g_string_new(cliptext);
    g_sprintf (clipnew, _("%s,%.2f%s\n"), latlon, overall_distance*unit_conv, distunit);
    g_string_append(toclipboard, clipnew);
    gtk_clipboard_set_text(clipdummy, toclipboard->str, -1);
```

In file `src\gps_functions.c` I comment out (line 756, because of a type trouble with double:

```
    gpsdata->fix.time = libgps_gpsdata.fix.time;
```

... and I did not need the gps stuff ;-)

## make fail

That is hard, but ok. I remove some stuff, but the `src\foxtrotgps` binary schould be build on (arch)linux.

