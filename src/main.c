



#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <stdlib.h>
#include <locale.h>
#include <string.h>
#include <gtk/gtk.h>

#include <glade/glade.h>

#include "interface.h"
#include "support.h"
#include "init.h"
#include "callbacks.h"
#include "globals.h"
#include "wp.h"
#include "poi.h"
#include "converter.h"
#include "map_management.h"

int
main (int argc, char *argv[])
{
	GError *error = NULL;

	gboolean fullscreen = FALSE;
	gboolean show_version = FALSE;
	char * waypoint_lat = NULL;
	char * waypoint_lon = NULL;

	GOptionEntry cmd_options[] =
	{
		{"version", 0, 0, G_OPTION_ARG_NONE, &show_version,
		 N_("Print the program version and exit"), NULL},
		{"fullscreen", 0, 0, G_OPTION_ARG_NONE, &fullscreen,
		 N_("Start in fullscreen mode"), NULL},
		{"gui", 0, 0, G_OPTION_ARG_FILENAME, &gladefile,
		 N_("Load the GUI from this GladeXML file"), "GLADEFILE"},
		{"lat", 0, 0, G_OPTION_ARG_STRING, &waypoint_lat,
		 N_("Select waypoint's latitude"), "LAT"},
		{"lon", 0, 0, G_OPTION_ARG_STRING, &waypoint_lon,
		 N_("Select waypoint's longitude"), "LON"},
		{NULL}
	};

	GOptionContext *option_context =
		g_option_context_new ("\n\nA friendly GPS & mapping "
				      "application.");

	g_option_context_add_group (option_context,
				    gtk_get_option_group (TRUE));

	g_option_context_add_main_entries (option_context,
					   cmd_options,
#ifdef ENABLE_NLS
					   GETTEXT_PACKAGE
#else
					   NULL
#endif
					   );

#ifdef ENABLE_NLS
	bindtextdomain (GETTEXT_PACKAGE, PACKAGE_LOCALE_DIR);
	bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
	textdomain (GETTEXT_PACKAGE);
#endif

	gtk_set_locale ();

	if (!g_option_context_parse (option_context, &argc, &argv, &error))
	{
		g_print (_("option parsing failed: %s\n"), error->message);
		return 1;
	}

	if (show_version)
	{
		g_print ("%s %s\n", _(PACKAGE_NAME), _(PACKAGE_VERSION));
		exit (0);
	}

	/* g_thread_init is deprecated since version 2.32 and is no longer
	 * necessary. https://developer.gnome.org/glib/2.34/glib-Deprecated-Thread-APIs.html#g-thread-init
	 * For more on GLIB_CHECK_VERSION, see
	 * https://developer.gnome.org/glib/stable/glib-Version-Information.html#GLIB-CHECK-VERSION:CAPS */
#if ! GLIB_CHECK_VERSION(2,32,0)
	if (!g_thread_supported ())
		g_thread_init (NULL);
#endif
	gdk_threads_init ();
	gdk_threads_enter ();
	gtk_init (&argc, &argv);

	setlocale (LC_NUMERIC, "C");

	add_pixmap_directory (PACKAGE_DATA_DIR "/" PACKAGE "/pixmaps");


	glade_init ();

	gladexml = glade_xml_new (gladefile, NULL, GETTEXT_PACKAGE);

	if (!gladexml)
	{
		/* Developers may run into this if they're naively
		   trying to run from the build-tree without having
		   specifically configured the build to allow that or
		   having passed a "--gui=..." option to the program,
		   so we need to actually catch this error and output
		   an informative message.

		   This is, however, not an error that end users should face;
		   if they do, then someone upstream from them messed up--
		   either `make install' worked only half-way, or a packager
		   left something critical out of the package. In either case,
		   we want to fail in a way that indicates to the user
		   that something is really wrong and they should report
		   a bug.
		*/

		g_error (_("%s could not load its user interface; "
			   "%s does not not appear to be properly installed."),
			 PACKAGE, PACKAGE);
	}

	glade_xml_signal_autoconnect (gladexml);

	pre_init();
	window1 = glade_xml_get_widget (gladexml, "window1");
	char *window_title =
		g_strdup_printf (gtk_window_get_title (GTK_WINDOW (window1)),
		                 _(PACKAGE_NAME));
	gtk_window_set_title (GTK_WINDOW (window1), window_title);
	g_free (window_title); window_title = NULL;


	int screen_height, screen_width;
	GtkWidget *toolbar;

	screen_height = gdk_screen_get_height(gdk_screen_get_default());
	screen_width  = gdk_screen_get_width (gdk_screen_get_default());

	if (screen_height > screen_width)
	{
		toolbar = lookup_widget(window1, "toolbar1");
		global_landscape = FALSE;
	}
	else {
		toolbar = lookup_widget(window1, "toolbar4");
		global_landscape = TRUE;
	}
	gtk_widget_show(toolbar);


	if(screen_height < 640 && screen_height > screen_width)
	{
		gtk_window_resize(GTK_WINDOW(window1),
				  (screen_width < 480) ? screen_width : 480,
				  screen_height-30);
	}
	else if(screen_height < 640 && screen_width <= 1024 && screen_height < screen_width)
	{
		gtk_window_resize(GTK_WINDOW(window1),
				  (screen_width < 480) ? screen_width : 480,
				  screen_height-60);
	}

	GtkWidget *hbox;
	hbox = lookup_widget(window1, "hbox49a");
	global_infopane_widgets = gtk_container_get_children(GTK_CONTAINER(hbox));

	GtkWidget *version_label;
	char *package_string;
	version_label = lookup_widget(window1, "label117");
	package_string = g_strdup_printf
		(gtk_label_get_label (GTK_LABEL (version_label)),
		_(PACKAGE_NAME), _(PACKAGE_VERSION));
	gtk_label_set_label (GTK_LABEL (version_label), package_string);
	g_free (package_string); package_string = NULL;

	gtk_widget_show (window1);

	if (fullscreen)
	{
		GtkToggleToolButton *toggle;

		toggle = GTK_TOGGLE_TOOL_BUTTON (lookup_widget (window1,
		                                                "button1"));
		gtk_toggle_tool_button_set_active(toggle, TRUE);

		toggle = GTK_TOGGLE_TOOL_BUTTON (lookup_widget (window1,
		                                                "button53"));
		gtk_toggle_tool_button_set_active(toggle, TRUE);
	}

	window2 = glade_xml_get_widget (gladexml, "window2");
	window3 = glade_xml_get_widget (gladexml, "window3");
	menu1 = glade_xml_get_widget (gladexml, "menu1");
	route_menu = glade_xml_get_widget (gladexml, "route_menu");

#ifdef ENABLE_HRM
	gtk_widget_show (glade_xml_get_widget (gladexml, "frame15"));
#else
	/* It looks like we can't hide widgets attached to a grid,
	   so the next best thing is to just `null them out':
	 */
	gtk_label_set_label (glade_xml_get_widget (gladexml, "label205"), "");
	gtk_label_set_label (glade_xml_get_widget (gladexml, "label206"), "");
	gtk_label_set_label (glade_xml_get_widget (gladexml, "label207"), "");
#endif

	init();

	if (waypoint_lat && waypoint_lon)
	{
		double lat = deg2rad(parse_degrees(waypoint_lat));
		double lon = deg2rad(parse_degrees(waypoint_lon));

		set_current_wp(lat, lon);
	}

	gtk_main ();

	gdk_threads_leave ();

	return 0;
}
