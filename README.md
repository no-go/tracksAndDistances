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

## My Comments to the mailing list of foxtrotgps

### 1. june 2020, 16:47

Hello, everyone,

I use foxtrotgps to measure the tracks I have driven - without gps data. Although 
foxtrotgps has this function, but there is no possibility to save it. I would also 
like to be able to load these tracks with a distance measurement. For this I have 
implemented the following ideas in the code:

- with the distance measurement the coordinates are attached to the content of 
  the clipboard instead of being exchanged
- these coordinates are not appended to the clipboard with "lat - lon", but 
  with "lat,lon,distance distunit".
- when loading tracks, the distance per coordinate is calculated and (additively) 
  output in the console with g_printf().

These small adjustments make it possible for me to measure tracks with foxtrotgps
by hand and save it as track via clipboard in a text file. Furthermore I can read 
these files afterwards with foxtrotgps (and other lists with coordinates) and 
have the track measured (distance).

Since I'm not sure if this changed behaviour of foxtrotgps is also desired by 
other users, I don't know if (and how) I should transmit these changes with 
Bazaar.

So at this point I wanted to ask how I should proceed and whether there is any 
interest at these changes.

### 1. june 2020, 22:01

Hi everyone,

These are my first attempts with bazaar and launchpad, but this way you can 
see how I improvised the described procedure of creating track files by
hand and a distance measurement:

https://bazaar.launchpad.net/~deadlockz/+junk/jochen/revision/325

You are welcome to adopt the change if you also think it makes sense for other users.

kindly regards,
Jochen

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

That is hard, but ok. I remove some stuff, but the `src\foxtrotgps` binary should be build on (arch)linux.

