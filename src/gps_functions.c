

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <glib.h>
#include <glib/gprintf.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <math.h>

#include "gps_functions.h"
#include "globals.h"
#include "support.h"
#include "tile_management.h"
#include "map_management.h"
#include "converter.h"
#include "wp.h"
#include "tracks.h"
#include "hrm_functions.h"


#define BUFSIZE 512
char * distance2scale(float distance, float *factor);


gboolean
cb_gps_timer()
{
	return TRUE;
}

gboolean
reset_gpsd_io()
{
	return FALSE;
}


void
osd_speed(gboolean force_redraw)
{

}

void
set_label_newmsg()
{

}

void
set_label_nogps()
{

}

void
set_label()
{

}

static gboolean
cb_gpsd_io_error(GIOChannel *src, GIOCondition condition, gpointer data)
{
	return FALSE;
}

static gboolean
cb_gpsd_data(GIOChannel *src, GIOCondition condition, gpointer data)
{
	return TRUE;
}

void
get_gps()
{

}

void
map_scale_indicator()
{

}

char *
distance2scale(float distance, float *factor)
{
	float unit_conv=1;
	char *buf = NULL;
	char unit[5];

	switch (global_speed_unit)
	{
		case METRICAL:
			unit_conv = 1.0;
			strncpy(unit, "km", 3);
			break;
		case IMPERIAL :
			unit_conv = 1.0/1.609344;
			strncpy(unit, "m", 2);
			break;
		case NAUTICAL :
			unit_conv = 1.0 / 1.852;
			strncpy(unit, "NM", 3);
			break;
	}

	distance *= unit_conv;
	if (distance >= 5000) {
		buf = g_strconcat("5000", unit, NULL);
		*factor = 5000/distance;
	}
	else if (distance < 5000 && distance >= 2000) {
		buf = g_strconcat("2000", unit, NULL);
		*factor = 2000/distance;
	}
	else if (distance < 2000 && distance >= 1000) {
		buf = g_strconcat("1000", unit, NULL);
		*factor = 1000/distance;
	}
	else if (distance < 1000 && distance >= 500) {
		buf = g_strconcat("500", unit, NULL);
		*factor = 500/distance;
	}
	else if (distance < 500 && distance >= 200) {
		buf = g_strconcat("200", unit, NULL);
		*factor = 200/distance;
	}
	else if (distance < 200 && distance >= 100) {
		buf = g_strconcat("100", unit, NULL);
		*factor = 100/distance;
	}
	else if (distance < 100 && distance >= 50) {
		buf = g_strconcat("50", unit, NULL);
		*factor = 50/distance;
	}
	else if (distance < 50 && distance >= 20) {
		buf = g_strconcat("20", unit, NULL);
		*factor = 20/distance;
	}
	else if (distance < 20 && distance >= 10) {
		buf = g_strconcat("10", unit, NULL);
		*factor = 10/distance;
	}
	else if (distance < 10 && distance >= 5) {
		buf = g_strconcat("5", unit, NULL);
		*factor = 5/distance;
	}
	else if (distance < 5 && distance >= 2) {
		buf = g_strconcat("2", unit, NULL);
		*factor = 2/distance;
	}
	else if (distance < 2 && distance >= 1) {
		buf = g_strconcat("1", unit, NULL);
		*factor = 1/distance;
	}
	else if(global_speed_unit == METRICAL)
	{
		if (distance < 1 && distance >= 0.5) {
			buf = g_strdup("500m");
			*factor = 0.5/distance;
		}
		else if (distance < 0.5 && distance >= 0.2) {
			buf = g_strdup("200m");
			*factor = 0.2/distance;
		}
		else if (distance < 0.2 && distance >= 0.1) {
			buf = g_strdup("100m");
			*factor = 0.1/distance;
		}
		else if (distance < 0.1 && distance >= 0.05) {
			buf = g_strdup("50m");
			*factor = 0.05/distance;
		}
		else if (distance < 0.05 && distance >= 0.02) {
			buf = g_strdup("20m");
			*factor = 0.02/distance;
		}
		else {
			buf = g_strdup("10m");
			*factor = 0.01/distance;
		}
	}
	else if(global_speed_unit == IMPERIAL)
	{
		distance *= 5280;
		if (distance >= 5000) {
			buf = g_strdup("5000ft");
			*factor = 5000/distance;
		}
		else if (distance < 5000 && distance >= 2000) {
			buf = g_strdup("2000ft");
			*factor = 2000/distance;
		}
		else if (distance < 2000 && distance >= 1000) {
			buf = g_strdup("1000ft");
			*factor = 1000/distance;
		}
		else if (distance < 1000 && distance >= 500) {
			buf = g_strdup("500ft");
			*factor = 500/distance;
		}
		else if (distance < 500 && distance >= 200) {
			buf = g_strdup("200ft");
			*factor = 200/distance;
		}
		else {
			buf = g_strdup("100ft");
			*factor = 100/distance;
		}
	}
	else if(global_speed_unit == NAUTICAL)
	{
		if (distance < 1 && distance >= 0.5) {
			buf = g_strdup("0.5NM");
			*factor = 0.5/distance;
		}
		else if (distance < 0.5 && distance >= 0.2) {
			buf = g_strdup("0.2NM");
			*factor = 0.2/distance;
		}
		else if (distance < 0.2 && distance >= 0.1) {
			buf = g_strdup("0.1NM");
			*factor = 0.1/distance;
		}
		else if (distance < 0.1 && distance >= 0.05) {
			buf = g_strdup("0.05NM");
			*factor = 0.05/distance;
		}
		else if (distance < 0.05 && distance >= 0.02) {
			buf = g_strdup("0.02NM");
			*factor = 0.02/distance;
		}
		else {
			buf = g_strdup("0.01NM");
			*factor = 0.01/distance;
		}
	}

	if(!buf)
		buf=g_strdup("bingo");

	return buf;
}
