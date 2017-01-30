# --------------------------------------------------------------------------
#
#  Copyright (C) 2010-2014 Fons Adriaensen <fons@linuxaudio.org>
#
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
#
# --------------------------------------------------------------------------


PREFIX = /usr/local
SUFFIX := $(shell uname -m | sed -e 's/^unknown/$//' -e 's/^i.86/$//' -e 's/^x86_64/$/64/')
LIBDIR = lib$(SUFFIX)
BINDIR = $(PREFIX)/bin
SHARED = $(PREFIX)/share/zita-at1
VERSION = 0.4.0
CPPFLAGS += -MMD -MP -DVERSION=\"$(VERSION)\" -DSHARED=\"$(SHARED)\"
CXXFLAGS += -O2 -ffast-math -Wall -MMD -MP
CXXFLAGS += -march=native


all:	zita-at1

ZITA-AT1_O = zita-at1.o styles.o jclient.o mainwin.o png2img.o guiclass.o \
             button.o rotary.o tmeter.o retuner.o
zita-at1:	CPPFLAGS += -I/usr/X11R6/include `freetype-config --cflags`
zita-at1:	LDLIBS += -lcairo -lclxclient -lclthreads -lzita-resampler -lfftw3f -ljack -lpthread -lpng -lXft -lX11 -lrt
zita-at1:	LDFLAGS += -L/usr/X11R6/lib
zita-at1:	LDFLAGS += -pthread
zita-at1:	$(ZITA-AT1_O) 
	g++ $(LDFLAGS) -o $@ $(ZITA-AT1_O) $(LDLIBS)
$(ZITA-AT1_O):
-include $(ZITA-AT1_O:%.o=%.d)



install:	all
	install -d $(DESTDIR)$(BINDIR)
	install -d $(DESTDIR)$(SHARED)
	install -m 755 zita-at1 $(DESTDIR)$(BINDIR)
	rm -rf $(DESTDIR)$(SHARED)/*
	install -m 644 ../share/* $(DESTDIR)$(SHARED)


uninstall:
	rm -f  $(DESTDIR)$(BINDIR)/zita-at1
	rm -rf $(DESTDIR)$(SHARED)


clean:
	/bin/rm -f *~ *.o *.a *.d *.so
	/bin/rm -f zita-at1

