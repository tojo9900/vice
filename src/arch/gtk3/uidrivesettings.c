/** \file   src/arch/gtk3/uidrivesettings.c
 * \brief   Drive settings dialog
 *
 * Controls the following resource(s):
 *  DriveTrueEmulation
 *
 *  (for more, see used widgets)
 *
 *
 * Written by
 *  Bas Wassink <b.wassink@ziggo.nl>
 *
 * This file is part of VICE, the Versatile Commodore Emulator.
 * See README for copyright notice.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 *  02111-1307  USA.
 *
 */

#include "vice.h"

#include <stdio.h>
#include <string.h>
#include <gtk/gtk.h>

#include "debug_gtk3.h"
#include "not_implemented.h"
#include "drive.h"
#include "drive-check.h"
#include "machine.h"
#include "resources.h"
#include "widgethelpers.h"
#include "driveunitwidget.h"
#include "drivetypewidget.h"
#include "driveextendpolicywidget.h"
#include "driveexpansionwidget.h"
#include "driveidlemethodwidget.h"
#include "driveparallelcablewidget.h"
#include "driveoptionswidget.h"

#include "uidrivesettings.h"


/** \brief  Unit currently having its settings changed
 */
static int unit_number = 8;


/** \brief  Reference to the drive type widget
 *
 * Used in unit_changed_callback() to update the widget
 */
static GtkWidget *drive_type_widget = NULL;


/** \brief  Widget controlling 40-track handling
 *
 * This is horrible, this really should be static
 */
GtkWidget *drive_extend_widget = NULL;


GtkWidget *drive_expansion_widget = NULL;

GtkWidget *drive_idle_method_widget = NULL;

GtkWidget *drive_parallel_cable_widget = NULL;

GtkWidget *drive_options_widget = NULL;


/** \brief  Extra callback when the unit number has changed
 *
 * This will update the various widgets with the proper settings for the
 * selected unit
 *
 * \param[in]   unit    drive unit number (8-11)
 */
static void unit_changed_callback(int unit)
{
    gchar res_name[64];
    int type;

    g_snprintf(res_name, 64, "Drive%dType", unit);
    resources_get_int(res_name, &type);

    debug_gtk3("got unit %d\n", unit);

    if (drive_type_widget != NULL) {
        update_drive_type_widget(drive_type_widget, unit);
    }

    if (drive_extend_widget != NULL) {
        update_drive_extend_policy_widget(drive_extend_widget, unit);
        gtk_widget_set_sensitive(drive_extend_widget,
                drive_check_extend_policy(type));
    }

    if (drive_expansion_widget != NULL) {
        update_drive_expansion_widget(drive_expansion_widget, unit);
    }

    if (drive_idle_method_widget != NULL) {
        update_drive_idle_method_widget(drive_idle_method_widget, unit);
    }

    if (drive_parallel_cable_widget != NULL) {
        update_drive_parallel_cable_widget(drive_parallel_cable_widget, unit);
    }

    if (drive_options_widget != NULL) {
        update_drive_options_widget(drive_options_widget, unit);
    }
}


/** \brief  Handler for the "destroy" event of the widget
 *
 * Right now sets widget references to `NULL` to avoid other event handlers
 * trying to update those widgets. I should look into setting up the event
 * handlers *after* all widgets are created and their values set, which should
 * result in event handlers not being triggered while setting up the UI
 *
 * \param[in]   widget      widget triggering the event (unused)
 * \param[in]   user_data   data for event (unused)
 */
static void on_destroy(GtkWidget *widget, gpointer user_data)
{
    drive_type_widget = NULL;
    drive_extend_widget = NULL;
    drive_expansion_widget = NULL;
    drive_idle_method_widget = NULL;
    drive_parallel_cable_widget = NULL;
    drive_options_widget = NULL;
}


/** \brief  Handler for the "toggled" event of the TDE check button
 *
 * \param[in]   widget      widget triggering the event
 * \param[in]   user_data   data for event (unused)
 */
static void on_tde_toggled(GtkWidget *widget, gpointer user_data)
{
    resources_set_int("DriveTrueEmulation",
            gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)));
}

static void on_drive_sound_toggled(GtkWidget *widget, gpointer user_data)
{
}

static GtkWidget *create_tde_check_button(void)
{
    GtkWidget *check;
    int state;

    check = gtk_check_button_new_with_label("Enable true drive emulation");
    g_object_set(check, "margin-left", 8, NULL);
    resources_get_int("DriveTrueEmulation", &state);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check), state);

    g_signal_connect(check, "toggled", G_CALLBACK(on_tde_toggled), NULL);
    return check;
}


static GtkWidget *create_drive_sound_check_button(void)
{
    GtkWidget *check = gtk_check_button_new_with_label(
            "Enable drive sound emulation");
    g_object_set(check, "margin-left", 8, NULL);
    g_signal_connect(check, "toggled", G_CALLBACK(on_drive_sound_toggled), NULL);
    return check;
}





/** \brief  Create drive settings widget for the settings dialog
 *
 * \paramp[in]  parent  parent widget
 *
 * \return  GtkGrid
 */
GtkWidget *uidrivesettings_create_central_widget(GtkWidget *parent)
{
    GtkWidget *layout;
    GtkWidget *unit_widget;

    layout = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(layout), 8);
    gtk_grid_set_row_spacing(GTK_GRID(layout), 8);
    g_object_set(layout, "margin", 8, NULL);


    gtk_grid_attach(GTK_GRID(layout), create_tde_check_button(), 0, 0, 3, 1);
    gtk_grid_attach(GTK_GRID(layout), create_drive_sound_check_button(),
            0, 1, 3, 1);

    unit_widget = create_drive_unit_widget(8, &unit_number, unit_changed_callback);
    g_object_set(unit_widget, "margin-left", 8, NULL);
    /* row 2, column 0-2 */
    gtk_grid_attach(GTK_GRID(layout), unit_widget, 0, 2, 3, 1);

    /* row 3 & 4, column 0 */
    drive_type_widget = create_drive_type_widget(unit_number, unit_changed_callback);
    update_drive_type_widget(drive_type_widget, unit_number);
    gtk_grid_attach(GTK_GRID(layout), drive_type_widget, 0, 3, 1, 3);

    /* row 3, colum 1 */
    drive_extend_widget = create_drive_extend_policy_widget(unit_number);
    update_drive_extend_policy_widget(drive_extend_widget, unit_number);
    gtk_grid_attach(GTK_GRID(layout), drive_extend_widget, 1, 3, 1, 1);

    /* row 4, column 1 */
    drive_expansion_widget = create_drive_expansion_widget(unit_number);
    update_drive_expansion_widget(drive_expansion_widget, unit_number);
    gtk_grid_attach(GTK_GRID(layout), drive_expansion_widget, 1, 4, 1, 1);

    /* row 3, column 2 */
    drive_idle_method_widget = create_drive_idle_method_widget(unit_number);
    update_drive_idle_method_widget(drive_idle_method_widget, unit_number);
    gtk_grid_attach(GTK_GRID(layout), drive_idle_method_widget, 2, 3, 1, 1);

    /* row 4, column 2 */
    drive_parallel_cable_widget = create_drive_parallel_cable_widget(unit_number);
    update_drive_parallel_cable_widget(drive_parallel_cable_widget, unit_number);
    gtk_grid_attach(GTK_GRID(layout), drive_parallel_cable_widget, 2, 4, 1, 1);

    /* row 5, column 1 & 2 */
    drive_options_widget = create_drive_options_widget(unit_number);
    update_drive_options_widget(drive_options_widget, unit_number);
    gtk_grid_attach(GTK_GRID(layout), drive_options_widget, 1, 5, 2, 1);


    g_signal_connect(layout, "destroy", G_CALLBACK(on_destroy), NULL);

    gtk_widget_show_all(layout);
    return layout;
}
