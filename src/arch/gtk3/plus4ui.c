/** \file   src/arch/gtk3/plus4ui.c
 * \brief   Native GTK3 PLUS4 UI
 *
 * Written by
 *  Marco van den Heuvel <blackystardust68@yahoo.com>
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

#include "not_implemented.h"
#include "machine.h"
#include "plus4model.h"
#include "widgethelpers.h"
#include "machinemodelwidget.h"
#include "videomodelwidget.h"
#include "sampler.h"
#include "uisamplersettings.h"

#include "plus4ui.h"


/** \brief  List of Plus/4 models
 *
 * Used in the machine-model widget
 */
static const char *plus4_model_list[] = {
    "C16 PAL",
    "C16 NTSC",
    "Plus4 PAL",
    "Plus4 NTSC",
    "V364 NTSC",
    "232 NTSC",
    NULL
};


/** \brief  List of TED models
 *
 * Used in the TED model widget
 */
static ui_radiogroup_entry_t plus4_ted_models[] = {
    { "PAL", MACHINE_SYNC_PAL },
    { "NTSC", MACHINE_SYNC_NTSC },
    { NULL, -1 }
};


int plus4ui_init_early(void)
{
    INCOMPLETE_IMPLEMENTATION();
    return 0;
}


/** \brief  Initialize the UI
 *
 * \return  0 on success, -1 on failure
 */
int plus4ui_init(void)
{
    /* Some of the work here is done by video.c now, and would need to
     * be shifted over */

    machine_model_widget_getter(plus4model_get);
    machine_model_widget_setter(plus4model_set);
    machine_model_widget_set_models(plus4_model_list);

    video_model_widget_set_title("TED model");
    video_model_widget_set_resource("MachineVideoStandard");
    video_model_widget_set_models(plus4_ted_models);

    uisamplersettings_set_devices_getter(sampler_get_devices);

    INCOMPLETE_IMPLEMENTATION();
    return 0;
}


/** \brief  Shut down the UI
 */
void plus4ui_shutdown(void)
{
    INCOMPLETE_IMPLEMENTATION();
}
