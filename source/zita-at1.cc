// ----------------------------------------------------------------------
//
//  Copyright (C) 2010-2011 Fons Adriaensen <fons@linuxaudio.org>
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//
// ----------------------------------------------------------------------


#include <stdlib.h>
#include <stdio.h>
#include <clthreads.h>
#include <sys/mman.h>
#include <signal.h>
#include "global.h"
#include "styles.h"
#include "jclient.h"
#include "mainwin.h"

#include "oscserver.h"


#define NOPTS 9
#define CP (char *)



XrmOptionDescRec options [NOPTS] =
{
    {CP"-h",    CP".help",      XrmoptionNoArg,   CP"true" },
    {CP"-g",    CP".geometry",  XrmoptionSepArg,  0        },
    {CP"-s",    CP".server",    XrmoptionSepArg,  0        },
    {CP"-t",    CP".tuning",    XrmoptionSepArg,  0        },
    {CP"-b",    CP".bias",      XrmoptionSepArg,  0        },
    {CP"-f",    CP".filter",    XrmoptionSepArg,  0        },
    {CP"-c",    CP".correction",XrmoptionSepArg,  0        },
    {CP"-o",    CP".offset",    XrmoptionSepArg,  0        },
    {CP"-p",    CP".port",    XrmoptionSepArg,    0        }
};



static Jclient  *jclient = 0;
static Mainwin  *mainwin = 0;
// static OSCServer      *oscserver = 0;
OSCServer * oscserver;

static void help (void)
{
    fprintf (stderr, "\n%s-%s\n\n", PROGNAME, VERSION);
    fprintf (stderr, "  (C) 2010-2014 Fons Adriaensen  <fons@linuxaudio.org>\n\n");
    fprintf (stderr, "Options:\n");
    fprintf (stderr, "  -h              Display this text\n");
    fprintf (stderr, "  -name <name>    Jack client name\n");
    fprintf (stderr, "  -s <server>     Jack server name\n");
    fprintf (stderr, "  -p port         OSC port\n");
    fprintf (stderr, "  -g <geometry>   Window position\n");
    fprintf (stderr, "  -t tuning       Tuning knob value [400 : 480]\n");
    fprintf (stderr, "  -b bias         Bias knob value [0 : 1]\n");
    fprintf (stderr, "  -f filter       Filter knob value [0.5 : 0.02]\n");
    fprintf (stderr, "  -c correction   Correction knob value [0 : 1]\n");
    fprintf (stderr, "  -o offset       Offset knob value [-24 : 24]\n");
    exit (1);
}


static void sigint_handler (int)
{
    signal (SIGINT, SIG_IGN);
    mainwin->stop ();
}


int main (int ac, char *av [])
{
    X_resman       xresman;
    X_display     *display;
    X_handler     *handler;
    X_rootwin     *rootwin;
    int           ev, xp, yp, xs, ys;

    xresman.init (&ac, av, CP PROGNAME, options, NOPTS);
    if (xresman.getb (".help", 0)) help ();

    display = new X_display (xresman.get (".display", 0));
    if (display->dpy () == 0)
    {
	fprintf (stderr, "Can't open display.\n");
        delete display;
	return 1;
    }

    xp = yp = 100;
    xs = Mainwin::XSIZE + 4;
    ys = Mainwin::YSIZE + 30;
    xresman.geometry (".geometry", display->xsize (), display->ysize (), 1, xp, yp, xs, ys);


    int port = (xresman.get(".port", 0) == 0) ? 9900   : atof(xresman.get(".port", 0));
    oscserver = new OSCServer(port);
    oscserver->start();

    styles_init (display, &xresman);
    jclient = new Jclient (xresman.rname (), xresman.get (".server", 0));
    rootwin = new X_rootwin (display);
    mainwin = new Mainwin (rootwin, &xresman, xp, yp, jclient, oscserver);
    rootwin->handle_event ();
    handler = new X_handler (display, mainwin, EV_X11);
    handler->next_event ();
    XFlush (display->dpy ());
    ITC_ctrl::connect (jclient, EV_EXIT, mainwin, EV_EXIT);

    if (mlockall (MCL_CURRENT | MCL_FUTURE)) fprintf (stderr, "Warning: memory lock failed.\n");
    signal (SIGINT, sigint_handler);

    do
    {
	ev = mainwin->process ();
	if (ev == EV_X11)
	{
	    rootwin->handle_event ();
	    handler->next_event ();
	}
	if (ev == Esync::EV_TIME)
	{
            rootwin->handle_event ();
	}
    }
    while (ev != EV_EXIT);

    styles_fini (display);
    delete jclient;
    delete handler;
    delete rootwin;
    delete display;
    oscserver->stop();

    return 0;
}
