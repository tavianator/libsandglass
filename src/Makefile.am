###########################################################################
## Copyright (C) 2008 Tavian Barnes <tavianator@gmail.com>               ##
##                                                                       ##
## This file is part of The FPFD Library Build Suite.                    ##
##                                                                       ##
## The FPFD Library Build Suite is free software; you can redistribute   ##
## it and/or modify it under the terms of the GNU General Public License ##
## as published by the Free Software Foundation; either version 3 of the ##
## License, or (at your option) any later version.                       ##
##                                                                       ##
## The FPFD Library Build Suite is distributed in the hope that it will  ##
## be useful, but WITHOUT ANY WARRANTY; without even the implied         ##
## warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See ##
## the GNU General Public License for more details.                      ##
##                                                                       ##
## You should have received a copy of the GNU General Public License     ##
## along with this program.  If not, see <http://www.gnu.org/licenses/>. ##
###########################################################################

lib_LTLIBRARIES = libsandglass.la

nobase_include_HEADERS = sandglass.h

libsandglass_la_SOURCES    = sandglass.h                                       \
                             sandglass_impl.h                                  \
                             sandglass.c                                       \
                             tsc.c

if X86
  libsandglass_la_SOURCES += x86/tsc-x86.s
endif

if X86_64
  libsandglass_la_SOURCES += x86_64/tsc-x86_64.s
endif

libsandglass_la_LIBADD     = -lrt
