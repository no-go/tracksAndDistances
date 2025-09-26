
#include <string.h>
#include <glib.h>
#include <glib/gprintf.h>
#include "support.h"
#include "converter.h"
#include "callbacks.h"
#include "wp.h"
#include "globals.h"
#include "gfx_compat.h"

static GdkPixbuf	*wp_icon = NULL;
static GdkPixbuf	*myposition_icon = NULL;
static CompatGC		*gc_map = NULL;



void
do_paint_myposition();



void
set_current_wp(double lat, double lon)
{
	global_wp_on = TRUE;
	global_wp.lat = lat;
	global_wp.lon = lon;


	repaint_all();
}



GdkPixbuf *
load_wp_icon ()
{
	GdkPixbuf *wp_icon = NULL;
	GError *error = NULL;

	wp_icon = gdk_pixbuf_new_from_file_at_size (
			PACKAGE_PIXMAPS_DIR "/foxtrotgps-wp.png", 36, 36,
			&error);
	if (error)
	{
		g_print ("%s%s(): loading pixbuf failure. %s\n", __FUNCTION__,__FILE__,
		         error->message);
		g_error_free (error);
	}

	return wp_icon;
}

void
paint_wp()
{
	if(global_wp_on) do_paint_wp();
}

void
do_paint_wp()
{
	int pixel_x, pixel_y, x,y;
	float lat, lon;
	CompatGC *gc;

	gc = compat_gc_new();
	compat_gc_set_rgb_fg_color(gc, 10000, 60000, 0);


	if(!wp_icon)
		wp_icon = load_wp_icon ();

	if (pixmap && !gc_map)
		gc_map = compat_gc_new();

	lat = global_wp.lat;
	lon = global_wp.lon;




	pixel_x = lon2pixel(global_zoom, lon);
	pixel_y = lat2pixel(global_zoom, lat);

	x = pixel_x - global_x;
	y = pixel_y - global_y;



	if(!wp_icon)
	{
		compat_draw_arc (
			pixmap,
			gc,
			TRUE,
			x-4, y-4,
			8,8,
			0,23040);
	}
	else
	{
		compat_draw_pixbuf (
			pixmap,
			gc_map,
			wp_icon,
			0,0,
			x,y-36,
			36,36,
			CAIRO_DITHER_NONE, 0, 0);

		gtk_widget_queue_draw_area (
			map_drawable,
			x, y-36,
			36,36);
	}
}




void
osd_wp()
{
	PangoLayout		*layout  = NULL;
	PangoFontDescription	*desc    = NULL;

	CompatGC *gc;

	gchar *buffer;
	static gchar distunit[3];


	static int width = 0, height = 0;

	float distance;
	double unit_conv = 1;

	if(gpsdata && mouse_dx == 0 && mouse_dy == 0)
	{
		switch (global_speed_unit)
		{
			case 0:
				unit_conv = 1.0;
				g_sprintf(distunit, "%s", "km");
				break;
			case 1 :
				unit_conv = 1.0 / 1.609344;
				g_sprintf(distunit, "%s", "m");
				break;
			case 2 :
				unit_conv = 1.0 / 1.852;
				g_sprintf(distunit, "%s", "NM");
				break;
		}


		distance = get_distance(deg2rad(gpsdata->fix.latitude),
					deg2rad(gpsdata->fix.longitude),
					global_wp.lat,global_wp.lon);
		buffer = g_strdup_printf("%.3f\n%.1f°",
					distance*unit_conv,

					rad2deg(gpsdata->fix.bearing));


		layout  = pango_cairo_create_layout(pixmap->cr);
		desc    = pango_font_description_new();

		pango_font_description_set_size (desc, 20 * PANGO_SCALE);
		pango_layout_set_font_description (layout, desc);
		pango_layout_set_text (layout, buffer, strlen(buffer));

		//gc = compat_gc_new (map_drawable->window);
		gc = compat_gc_new ();
		compat_set_color(gc, 0,0,0);

		compat_draw_drawable (
			map_drawable,
			pixmap,
			global_drawingarea_width - width - 10,
			global_drawingarea_height - height - 10,
			global_drawingarea_width - width - 10,
			global_drawingarea_height - height - 10,
			width+10,height+10);



		pango_layout_get_pixel_size(layout, &width, &height);



		compat_draw_layout(pixmap,
					gc,
					global_drawingarea_width - width - 10,
					global_drawingarea_height - height -10,
					layout);




		g_free(buffer);
		pango_font_description_free (desc);
		g_object_unref (layout);
		g_object_unref (gc);
	}
}





void
paint_myposition()
{
	if(global_myposition.lat) do_paint_myposition();
}

void
do_paint_myposition()
{
	int pixel_x, pixel_y, x,y;
	float lat, lon;
	CompatGC *gc;
	GError	*error = NULL;

	gc = compat_gc_new();
	compat_gc_set_rgb_fg_color(gc, 10000, 60000, 0);

	if(!myposition_icon)
	{
		myposition_icon = gdk_pixbuf_new_from_file_at_size (
			PACKAGE_PIXMAPS_DIR "/" PACKAGE_NAME "-myposition.png", 36,36,
			&error);
		if (error)
		{
			g_print ("%s(): loading pixbuf failure. %s\n", __FUNCTION__,
			error->message);
			g_error_free (error);


		}
	}
	if (pixmap && !gc_map)
		gc_map = compat_gc_new();

	lat = deg2rad(global_myposition.lat);
	lon = deg2rad(global_myposition.lon);




	pixel_x = lon2pixel(global_zoom, lon);
	pixel_y = lat2pixel(global_zoom, lat);

	x = pixel_x - global_x;
	y = pixel_y - global_y;

	if(!myposition_icon)
	{
		compat_draw_arc (
			pixmap,
			gc,
			TRUE,
			x-4, y-4,
			8,8,
			0,23040);
	}
	else
	{
		compat_draw_pixbuf (
			pixmap,
			gc_map,
			myposition_icon,
			0,0,
			x,y-36,
			36,36,
			CAIRO_DITHER_NONE, 0, 0);

		gtk_widget_queue_draw_area (
			map_drawable,
			x, y-36,
			36,36);
	}
}
