/*
 * vsyncarch.c - End-of-frame handling for Unix
 *
 * Written by
 *  Dag Lem <resid@nimrod.no>
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

/* This is altered and trimmed down to fit into the GTK3-native world,
 * but it's still heavily reliant on UNIX internals. */

#include "vice.h"

#include "kbdbuf.h"
#include "ui.h"
#include "vsyncapi.h"
#include "videoarch.h"

#ifdef HAS_JOYSTICK
#include "joy.h"
#endif

#ifdef HAVE_NANOSLEEP
#include <time.h>
#else
#include <unistd.h>
#endif
#include <sys/time.h>

#ifdef MACOSX_SUPPORT
#include <mach/mach.h>
#include <mach/mach_time.h>
#endif

/* hook to ui event dispatcher */
static void_hook_t ui_dispatch_hook;
static int pause_pending = 0;

/* ------------------------------------------------------------------------- */
#ifdef HAVE_NANOSLEEP
#define TICKSPERSECOND  1000000000L  /* Nanoseconds resolution. */
#define TICKSPERMSEC    1000000L
#define TICKSPERUSEC    1000L
#define TICKSPERNSEC    1L
#else
#define TICKSPERSECOND  1000000L     /* Microseconds resolution. */
#define TICKSPERMSEC    1000L
#define TICKSPERUSEC    1L
#endif

/* FIXME: this function should be a constant */
/* Number of timer units per second. */
unsigned long vsyncarch_frequency(void)
{
    return TICKSPERSECOND;
}

/* Get time in timer units. */
unsigned long vsyncarch_gettime(void)
{
/* FIXME: configure checks for clock_gettime need to be added */
#ifdef HAVE_NANOSLEEP
 #ifdef MACOSX_SUPPORT
    static uint64_t factor = 0;
    uint64_t time = mach_absolute_time();
    if (!factor) {
        mach_timebase_info_data_t info;
        kern_return_t ret = mach_timebase_info(&info);
        factor = info.numer / info.denom;
    }
    return time * factor;
 #else
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    return (TICKSPERSECOND * now.tv_sec) + (TICKSPERNSEC * now.tv_nsec);
 #endif
#else
    /* this is really really bad, we should never use the wallclock
       see: https://blog.habets.se/2010/09/gettimeofday-should-never-be-used-to-measure-time.html */
    struct timeval now;
    gettimeofday(&now, NULL);
    return (TICKSPERSECOND * now.tv_sec) + (TICKSPERUSEC * now.tv_usec);
#endif
}

void vsyncarch_init(void)
{
    vsync_set_event_dispatcher(ui_dispatch_events);
}

/* Display speed (percentage) and frame rate (frames per second). */
/* TODO: Why does this exist? ui_* is more generic and more widely exposed! */
void vsyncarch_display_speed(double speed, double frame_rate, int warp_enabled)
{
    ui_display_speed((float)speed, (float)frame_rate, warp_enabled);
}

/* Sleep a number of timer units. */
void vsyncarch_sleep(unsigned long delay)
{
#ifdef HAVE_NANOSLEEP
    struct timespec ts;
#endif
    unsigned long thistime, targettime, timewait;
#if 0
    /* HACK: to prevent any multitasking stuff getting in the way, we return
             immediately on delays up to 0.1ms */
    if (delay < (TICKSPERMSEC / 10)) {
        return;
    }
#endif
    targettime = vsyncarch_gettime() + delay;

    /* repeatedly sleep until the requested delay is over. we do this so we get
       a somewhat accurate delay even if the sleep function itself uses the
       wall clock, which under certain circumstance may wait less than the
       requested time */
    while ((thistime = vsyncarch_gettime()) < targettime) {
        /* we use increasingly smaller delays, and for the last 100 steps just
           poll the current time */
        timewait = (targettime - thistime) / 10;
        if (timewait > 100) {
            /* FIXME: this should use a sleep function with monotonous clock
                      source, eg clock_nanosleep */
#ifdef HAVE_NANOSLEEP
            ts.tv_sec = timewait / TICKSPERSECOND;
            ts.tv_nsec = (timewait % TICKSPERSECOND);
            /* wait until whole interval has elapsed */
            while (nanosleep(&ts, &ts));
#else
            usleep(timewait);
#endif
        }
    }
}

void vsyncarch_presync(void)
{
    /* TODO: Lightpen */
    kbdbuf_flush();
#ifdef HAS_JOYSTICK
    joystick();
#endif

}

void_hook_t vsync_set_event_dispatcher(void_hook_t hook)
{
    void_hook_t t = ui_dispatch_hook;

    ui_dispatch_hook = hook;
    return t;
}

/* FIXME: ui_pause_emulation is not implemented in the OSX port */
void vsyncarch_postsync(void)
{
    (*ui_dispatch_hook)();

    /* this function is called once a frame, so this
       handles single frame advance */
    if (pause_pending) {
        ui_pause_emulation(1);
        pause_pending = 0;
    }
}

void vsyncarch_advance_frame(void)
{
    ui_pause_emulation(0);
    pause_pending = 1;
}
