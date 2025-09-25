#ifndef GFX_COMPAT_H
#define GFX_COMPAT_H

#include <cairo.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <glib.h>
#include <pango/pangocairo.h>
#include <gtk/gtkwidget.h>

#define COMPAT_LINE_SOLID 0
#define COMPAT_CAP_ROUND CAIRO_LINE_CAP_ROUND
#define COMPAT_JOIN_ROUND CAIRO_LINE_JOIN_ROUND

typedef struct {
    cairo_surface_t *surface;
    cairo_t *cr;
    int width;
    int height;
} CompatPixmap;

typedef struct {
    double r, g, b;
    double line_width;
    cairo_line_cap_t cap_style;
    cairo_line_join_t join_style;
} CompatGC;

CompatGC* compat_gc_new();
void compat_set_color(CompatGC *gc, double r, double g, double b);
void compat_gc_set_rgb_fg_color(CompatGC *gc, int red, int green, int blue);
void compat_draw_arc(CompatPixmap *pixmap,
	CompatGC *gc,
	gboolean filled,
	int x, int y, int width, int height,
	int angle1, int angle2);
void compat_draw_pixbuf(
    CompatPixmap *pixmap,
    CompatGC *gc,
    GdkPixbuf *pixbuf,
    int src_x, int src_y,
    int dest_x, int dest_y,
    int width, int height,
    int dither, int x_dither, int y_dither);
void compat_gc_free(CompatGC *gc);
void compat_draw_line(CompatPixmap *pixmap, CompatGC *gc, int x1, int y1, int x2, int y2);
void compat_gc_set_line_attributes(CompatGC *gc,
                                   double line_width,
                                   int line_style,
                                   cairo_line_cap_t cap_style,
                                   cairo_line_join_t join_style);
void compat_draw_layout(CompatPixmap *pixmap, CompatGC *gc, int x, int y, PangoLayout *layout);

void compat_draw_drawable(GtkWidget *target_widget,
                        CompatPixmap *source,
                        int src_x, int src_y,
                        int dest_x, int dest_y,
                        int width, int height);
#endif