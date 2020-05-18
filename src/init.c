

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <glib.h>
#include <glib/gstdio.h>
#include <glib/gprintf.h>
#include <gtk/gtk.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <gdk/gdk.h>
#include <math.h>
#include <time.h>
#include <errno.h>

#include "globals.h"
#include "map_management.h"
#include "converter.h"
#include "gps_functions.h"
#include "support.h"
#include "callbacks.h"
#include "wp.h"


GSList *settings_get_repolist();
void repoconfig__set_current_list_pointer();
void repoconfig__create_dropdown();


void
pre_init()
{
#if ! GLIB_CHECK_VERSION(2,35,0)
	/* Deprecated since 2.35.0. */
	g_type_init();
#endif

	trackpoint_list = g_queue_new();

	global_home_dir = getenv("HOME");

	global_settings	= g_settings_new(SCHEMA);
	global_curr_reponame	= g_settings_get_string(global_settings, "repo-name");


	if(global_curr_reponame == NULL)
	{
		printf("repo_name setting not set\n");
		global_curr_reponame = g_strdup("OSM");
	}
	settings_get_repolist();
	repoconfig__set_current_list_pointer();

	global_x = g_settings_get_int(
				global_settings,
				"global-x");
	global_y = g_settings_get_int(
				global_settings,
				"global-y");
	global_zoom = g_settings_get_int(
				global_settings,
				"global-zoom");
	global_detail_zoom = g_settings_get_int (
				global_settings,
				"global-detail-zoom");


	if(global_zoom <= 2)
	{
		global_x = 890;
		global_y = 515;
		global_zoom = 3;
	}


	global_server = g_settings_get_string (global_settings,
						 "gpsd-host");
	if (global_server == NULL)
	{
		printf("GPSD address setting not set\n");
		global_server	= g_strdup("127.0.0.1");
	}

	global_port = g_settings_get_string (global_settings,
					       "gpsd-port");
	if (global_port == NULL)
	{
		printf("GPSD port setting not set\n");
		global_port	= g_strdup("2947");
	}

	global_no_redownload = g_settings_get_boolean(
				global_settings,
				"no-redownload");

	global_auto_download = g_settings_get_boolean(global_settings,
	                                             "auto-download");
}

void
init()
{
	GError	*err = NULL;
	const gchar *nick, *pass, *me_msg, *hrm_bt_addr;
	GtkWidget *widget;
	gchar buffer[128];
	gboolean gconf_fftimer_running;
	char *str = NULL;


	foxtrotgps_dir = g_strconcat(global_home_dir, "/." PACKAGE, NULL);
	g_mkdir(foxtrotgps_dir, 0700);

	repoconfig__create_dropdown();




	global_settings	= g_settings_new(SCHEMA);
	nick			= g_settings_get_string(global_settings, "nick");
	pass			= g_settings_get_string(global_settings, "pass");
	me_msg			= g_settings_get_string(global_settings, "me-msg");
	hrm_bt_addr		= g_settings_get_string(global_settings, "hrm-bt-addr");

	global_speed_unit	= g_settings_get_int(global_settings, "speed-unit");
	global_alt_unit		= g_settings_get_int(global_settings, "alt-unit");
	global_latlon_unit	= g_settings_get_int(global_settings, "latlon-unit");

	widget = lookup_widget(window1, "button3");
	gtk_toggle_tool_button_set_active(GTK_TOGGLE_TOOL_BUTTON(widget),
	                                  global_autocenter);

	widget = lookup_widget(window1, "button56");
	gtk_toggle_tool_button_set_active(GTK_TOGGLE_TOOL_BUTTON(widget),
	                                  global_autocenter);

	switch (global_speed_unit)
	{
		case 1:
			widget = lookup_widget(window1, "radiobutton15");
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widget), TRUE);
			break;
		case 2:
			widget = lookup_widget(window1, "radiobutton16");
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widget), TRUE);
			break;
	}

	switch (global_alt_unit)
	{
		case 1:
			widget = lookup_widget(window1, "radiobutton18");
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widget), TRUE);
			break;
	}

	switch (global_latlon_unit)
	{
		case 1:
			widget = lookup_widget(window1, "radiobutton20");
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widget), TRUE);
			break;
		case 2:
			widget = lookup_widget(window1, "radiobutton21");
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widget), TRUE);
			break;
	}

	widget  = lookup_widget(window1, "entry7");
	if(nick)
		gtk_entry_set_text( GTK_ENTRY(widget), nick );
	widget  = lookup_widget(window1, "entry8");
	if(pass)
		gtk_entry_set_text( GTK_ENTRY(widget), pass );
	widget  = lookup_widget(window1, "entry29");
	if(me_msg)
		gtk_entry_set_text( GTK_ENTRY(widget), me_msg );
	widget  = lookup_widget(window1, "entry33");
	if(hrm_bt_addr)
		gtk_entry_set_text( GTK_ENTRY(widget), hrm_bt_addr );


	widget = lookup_widget(window1, "vscale1");
	gtk_range_set_value(GTK_RANGE(widget), (double) global_zoom);

	global_track_dir	= g_settings_get_string(global_settings, "track-dir");
	if(!global_track_dir)
		global_track_dir = g_strdup_printf("%s/Maps/",global_home_dir);

	global_myposition.lat = 0;
	global_myposition.lon = 0;

	widget = lookup_widget(window1, "checkbutton2");
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widget), global_auto_download);

	widget = glade_xml_get_widget (gladexml, "tile_redownload_toggle");
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widget),
	                             !global_no_redownload);

	hrm_on = g_settings_get_boolean(global_settings, "hrm-on");
	widget = lookup_widget(window1, "checkbutton18");
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widget), hrm_on);

	gconf_fftimer_running = FALSE;

	global_ffupdate_interval_minutes = g_settings_get_double(global_settings, "ffupdate-interval-minutes");

	if(!global_ffupdate_interval_minutes)
		global_ffupdate_interval_minutes = 5;

	global_ffupdate_interval = global_ffupdate_interval_minutes * 60000;

	if (global_ffupdate_interval_minutes<5)
		g_sprintf(buffer, "%.1f", global_ffupdate_interval_minutes);
	else
		g_sprintf(buffer, "%.0f", global_ffupdate_interval_minutes);

	widget = lookup_widget(window1, "entry16");
	// gtk_entry_set_text( GTK_ENTRY(widget), buffer );

	if(gconf_fftimer_running)
	{
		widget = lookup_widget(menu1, "item19");
		gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(widget), TRUE);
	}



	str = g_settings_get_string(global_settings, "gpsd-host");
	widget = lookup_widget(window1, "entry3");
	if(str)
		gtk_entry_set_text(GTK_ENTRY(widget), g_strdup(str));
	g_free(str);

	str = g_settings_get_string(global_settings, "gpsd-port");
	widget = lookup_widget(window1, "entry4");
	if(str)
		gtk_entry_set_text(GTK_ENTRY(widget), g_strdup(str));
	g_free(str);

	if (g_settings_get_boolean(global_settings, "tracklog-on")) {
		widget = lookup_widget(window1, "checkbutton17");
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widget), TRUE);
	}

	get_gps ();

	gtk_window_set_icon_from_file(GTK_WINDOW(window1), PACKAGE_PIXMAPS_DIR "/" PACKAGE ".png" ,&err);
	if (err)
	{
		fprintf (stderr, _("Failed to load pixbuf file: %s\n"),
		         err->message);
		g_error_free (err);
	}


	global_myposition.lat = g_settings_get_double(global_settings, "myposition-lat");
	global_myposition.lon = g_settings_get_double(global_settings, "myposition-lon");
	paint_myposition();
}

GSList *
settings_get_repolist()
{
	gchar  **repo_list;


	repo_list = g_settings_get_strv(	global_settings,
						"repos");

	if (repo_list == NULL || repo_list[0] == NULL)
	{
		repo_t *repo1 = g_new0(repo_t, 1);
		repo_t *repo2 = g_new0(repo_t, 1);
		repo_t *repo3 = g_new0(repo_t, 1);
		repo_t *repo4 = g_new0(repo_t, 1);
		repo_t *repo5 = g_new0(repo_t, 1);


		printf("REPOLIST == NULL\n");

		repo1->name = g_strdup("OSM");
		repo1->uri  = g_strdup("https://tile.openstreetmap.org/%d/%d/%d.png");
		repo1->dir  = g_strdup_printf("%s/Maps/OSM",global_home_dir);
		repo1->inverted_zoom = 0;
		global_repo_list = g_slist_append(global_repo_list, repo1);

		repo2->name = g_strdup("Maps-for-free.com");
		repo2->uri  = g_strdup("maps-for-free");
		repo2->dir  = g_strdup_printf("%s/Maps/maps4free",global_home_dir);
		repo2->inverted_zoom = 0;
		global_repo_list = g_slist_append(global_repo_list, repo2);

		repo3->name = g_strdup("OpenCycleMap");
		repo3->uri  = g_strdup("http://tile.opencyclemap.org/cycle/%d/%d/%d.png");
		repo3->dir  = g_strdup_printf("%s/Maps/opencyclemap",global_home_dir);
		repo3->inverted_zoom = 0;
		global_repo_list = g_slist_append(global_repo_list, repo3);

		repo4->name = g_strdup("Google Maps (testing only)");
		repo4->uri  = g_strdup("https://mt0.google.com/vt/lyrs=m&hl=en&x=%d&y=%d&z=%d");
		repo4->dir  = g_strdup_printf("%s/Maps/googlemaps",global_home_dir);
		repo4->inverted_zoom = 1;
		global_repo_list = g_slist_append(global_repo_list, repo4);

		repo5->name = g_strdup("Google Sat (testing only)");
		repo5->uri  = g_strdup("https://mt0.google.com/vt/lyrs=s&hl=en&x=%d&y=%d&z=%d");
		repo5->dir  = g_strdup_printf("%s/Maps/googlesat",global_home_dir);
		repo5->inverted_zoom = 1;
		global_repo_list = g_slist_append(global_repo_list, repo5);


	}


	for(gint i = 0; repo_list[i] != NULL; i++)
	{
		gchar **array;
		gchar *str = repo_list[i];
		repo_t *repo = g_new0(repo_t, 1);

		array = g_strsplit(str,"|",4);

		repo->name = g_strdup(array[0]);
		repo->uri  = g_strdup(array[1]);
		repo->dir  = g_strdup(array[2]);
		repo->inverted_zoom = (atoi(g_strdup(array[3])) == 1) ? TRUE : FALSE;

		global_repo_list = g_slist_append(global_repo_list, repo);
	}


	g_strfreev (repo_list);

	return global_repo_list;

}

void
settings_set_repolist()
{

	GSList	  *list;
	GPtrArray *setting_list = NULL;
	gboolean success = FALSE;



	setting_list = g_ptr_array_new ();

	for(list = global_repo_list; list != NULL; list = list->next)
	{
		repo_t *repo;
		gchar setting_str[1024];

		repo = list->data;


		g_sprintf(	setting_str,
				"%s|%s|%s|%i",
				repo->name, repo->uri, repo->dir, repo->inverted_zoom);

	        g_ptr_array_add(setting_list, (gpointer) g_strdup(setting_str));
	}

	g_ptr_array_add(setting_list, NULL);

	success = g_settings_set_strv(	global_settings,
						"repos",
						(const gchar * const *) setting_list->pdata);

	g_strfreev((gchar **) setting_list->pdata);
	g_ptr_array_free (setting_list, FALSE);
}



void
repoconfig__set_current_list_pointer()
{


	GSList		*list;
	const gchar	*reponame;
	int unused;

	for(list = global_repo_list; list != NULL; list = list->next)
	{
		repo_t *repo;
		repo	= list->data;

		reponame = g_strdup(repo->name);

		if(	g_strrstr(reponame,global_curr_reponame) != NULL &&
			strlen(reponame) == strlen(global_curr_reponame)
		)
			global_curr_repo = list;
	}
	if(!global_curr_repo)
	{
		printf("\n#\n#\n#  ERROR: repository %s is broken \n#\n#\n", global_curr_reponame);

		printf("Resetting repo_name and exiting now.\n\n");
		printf("If problem persists after restart, \n");
		printf("execute in a terminal: gsettings reset " SCHEMA " repos\n");
		unused = system("gsettings reset " SCHEMA " repo_name");
		exit(EXIT_FAILURE);
	}
}


void
repoconfig__create_dropdown()
{
	GtkWidget	*combobox;
	GSList		*list;
	int 		i = 0;
	int		j = 0;
	const gchar	*reponame;

	combobox = lookup_widget(window1, "combobox1");

	for(list = global_repo_list; list != NULL; list = list->next)
	{
		repo_t	*repo;

		repo = list->data;
		reponame = g_strdup(repo->name);
		gtk_combo_box_append_text (GTK_COMBO_BOX(combobox), g_strdup(repo->name));

		if(	g_strrstr(reponame,global_curr_reponame) != NULL &&
			strlen(reponame) == strlen(global_curr_reponame)
		)
		{
			j = i;
			global_curr_repo = list;
		}
		i++;
	}
	global_repo_cnt = i;
	gtk_combo_box_set_active(GTK_COMBO_BOX(combobox), j);
}
