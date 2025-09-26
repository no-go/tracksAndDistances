#include "gfx_compat.h"
#include <stdlib.h>
#include <glib.h>
#include <cairo.h>
#include <pango/pangocairo.h>
#include <gtk/gtkwidget.h>

CompatGC *compat_gc_new () {
	CompatGC *gc = g_new0(CompatGC, 1);
	gc->r = gc->g = gc->b = 0.0;
	gc->line_width = 1.0;
	return gc;
}

void compat_gc_free (CompatGC *gc) {
	if (gc) free(gc);
}

void compat_set_color (CompatGC *gc, double r, double g, double b) {
	gc->r = r;
	gc->g = g;
	gc->b = b;
}

void compat_gc_set_rgb_fg_color (
	CompatGC *gc,
	int red, int green, int blue
) {
	gc->r = red / 65535.0;
	gc->g = green / 65535.0;
	gc->b = blue / 65535.0;
}

void compat_draw_arc (CompatPixmap *pixmap, CompatGC *gc,
	gboolean filled,
	int x, int y, int width, int height,
	int angle1, int angle2
) {
	if (!pixmap || !pixmap->cr || !gc) return;

	cairo_t *cr = pixmap->cr;

	cairo_save(cr);

	cairo_set_source_rgb(cr, gc->r, gc->g, gc->b);
	cairo_set_line_width(cr, gc->line_width);
	cairo_set_line_cap(cr, gc->cap_style);
	cairo_set_line_join(cr, gc->join_style);

	// GDK gibt Winkel in 1/64 Grad an -> umrechnen in Radians
	double start_angle = (angle1 / 64.0) * (G_PI / 180.0);
	double sweep_angle = (angle2 / 64.0) * (G_PI / 180.0);

	double xc = x + width / 2.0;
	double yc = y + height / 2.0;
	double radius = width / 2.0;

	cairo_arc(cr, xc, yc, radius, start_angle, start_angle + sweep_angle);

	if (filled)
		cairo_fill(cr);
	else
		cairo_stroke(cr);

	cairo_restore(cr);
}

void compat_draw_pixbuf (
	CompatPixmap *pixmap,
	CompatGC *gc,
	GdkPixbuf *pixbuf,
	int src_x, int src_y,
	int dest_x, int dest_y,
	int width, int height,
	int dither, int x_dither, int y_dither
) {
	if (!pixmap || !pixmap->cr || !pixbuf) return;

	// valid area
	int pixbuf_width = gdk_pixbuf_get_width (pixbuf);
	int pixbuf_height = gdk_pixbuf_get_height (pixbuf);

	if (src_x < 0) src_x = 0;
	if (src_y < 0) src_y = 0;
	if (src_x + width > pixbuf_width) width = pixbuf_width - src_x;
	if (src_y + height > pixbuf_height) height = pixbuf_height - src_y;

	// Subregion
	GdkPixbuf *sub_pixbuf = gdk_pixbuf_new_subpixbuf(
		pixbuf, src_x, src_y, width, height);

	// draw
	gdk_cairo_set_source_pixbuf(pixmap->cr, sub_pixbuf, dest_x, dest_y);
	cairo_paint(pixmap->cr);

	g_object_unref(sub_pixbuf);
}

void compat_draw_line (
	CompatPixmap *pixmap, CompatGC *gc,
	int x1, int y1,
	int x2, int y2
) {
	if (!pixmap || !pixmap->cr || !gc) return;

	cairo_set_source_rgb(pixmap->cr, gc->r, gc->g, gc->b);
	cairo_set_line_width(pixmap->cr, gc->line_width);
	cairo_set_line_cap(pixmap->cr, gc->cap_style);
	cairo_set_line_join(pixmap->cr, gc->join_style);
	cairo_move_to(pixmap->cr, x1, y1);
	cairo_line_to(pixmap->cr, x2, y2);
	cairo_stroke(pixmap->cr);
}

void compat_gc_set_line_attributes (
	CompatGC *gc,
	double line_width,
	int line_style,
	cairo_line_cap_t cap_style,
	cairo_line_join_t join_style
) {
	gc->line_width = line_width;
	gc->cap_style = cap_style;
	gc->join_style = join_style;
	// @TODO
	// line_style (GDK_LINE_SOLID etc.)
}

void compat_draw_layout (
	CompatPixmap *pixmap, CompatGC *gc,
	int x, int y,
	PangoLayout *layout
) {
	if (!pixmap || !pixmap->cr || !layout || !gc) return;

	cairo_set_source_rgb(pixmap->cr, gc->r, gc->g, gc->b);
	cairo_move_to(pixmap->cr, x, y);
	pango_cairo_show_layout(pixmap->cr, layout);
}

void compat_draw_drawable (
	GtkWidget *target_widget,
	CompatPixmap *source,
	int src_x, int src_y,
	int dest_x, int dest_y,
	int width, int height
) {
	if (!target_widget || !source || !source->surface)
		return;

	cairo_t *cr = gdk_cairo_create(gtk_widget_get_window(target_widget));

	cairo_set_source_surface(cr, source->surface, dest_x - src_x, dest_y - src_y);
	cairo_rectangle(cr, dest_x, dest_y, width, height);
	cairo_clip(cr);
	cairo_paint(cr);

	cairo_destroy(cr);
}

void compat_pixmap_free (CompatPixmap *pixmap) {
	cairo_destroy(pixmap->cr);
	cairo_surface_destroy(pixmap->surface);
	g_free(pixmap);
}

CompatPixmap* compat_pixmap_new (
	GtkWidget *widget,
	int width, int height
) {
	CompatPixmap *pixmap = g_malloc(sizeof(CompatPixmap));

	pixmap->surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width, height);
	pixmap->cr = cairo_create(pixmap->surface);
	pixmap->width = width;
	pixmap->height = height;

	return pixmap;
}

void compat_draw_rectangle (
	CompatPixmap *pixmap,
	CompatGC *gc,
	gboolean filled,
	int x, int y, int width, int height
) {
	cairo_set_source_rgb(pixmap->cr, gc->r, gc->g, gc->b);

	if (filled) {
		cairo_rectangle(pixmap->cr, x, y, width, height);
		cairo_fill(pixmap->cr);
	} else {
		cairo_rectangle(pixmap->cr, x, y, width, height);
		cairo_stroke(pixmap->cr);
	}
}

void compat_combo_box_remove_text (GtkComboBox *combobox, gint index) {
	GtkTreeModel *model = gtk_combo_box_get_model(combobox);

	if (model) {
		GtkTreeIter iter;
		if (gtk_tree_model_iter_nth_child(model, &iter, NULL, index)) {
			gtk_list_store_remove(GTK_LIST_STORE(model), &iter);
		}
	}
}

void compat_combo_box_prepend_text (GtkComboBox *combobox, const gchar *text) {
	if (GTK_IS_COMBO_BOX_TEXT(combobox)) {
		gtk_combo_box_text_prepend_text(GTK_COMBO_BOX_TEXT(combobox), text);
	} else if (GTK_IS_COMBO_BOX(combobox)) {
		GtkListStore *store = GTK_LIST_STORE(gtk_combo_box_get_model(combobox));
		GtkTreeIter iter;

		gtk_list_store_prepend(store, &iter);
		gtk_list_store_set(store, &iter, 0, text, -1);
	}
}

gchar* compat_combo_box_get_active_text (GtkComboBox *widget) {
	if (GTK_IS_COMBO_BOX_TEXT(widget)) {

		return g_strdup(gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(widget)));
	} else if (GTK_IS_COMBO_BOX(widget)) {
		GtkTreeIter iter;
		GtkTreeModel *model = gtk_combo_box_get_model(widget);

		if (gtk_combo_box_get_active_iter(widget, &iter)) {
			gchar *text = NULL;
			gtk_tree_model_get(model, &iter, 0, &text, -1);
			gchar *result = g_strdup(text);
			g_free(text);
			return result;
		}
	}
	return NULL;
}


