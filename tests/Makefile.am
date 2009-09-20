###########################################################################
## Copyright (C) 2009 Tavian Barnes <tavianator@gmail.com>               ##
##                                                                       ##
## This file is part of The Dimension Build Suite.                       ##
##                                                                       ##
## The Dimension Build Suite is free software; you can redistribute it   ##
## and/or modify it under the terms of the GNU General Public License as ##
## published by the Free Software Foundation; either version 3 of the    ##
## License, or (at your option) any later version.                       ##
##                                                                       ##
## The Dimension Build Suite is distributed in the hope that it will be  ##
## useful, but WITHOUT ANY WARRANTY; without even the implied warranty   ##
## of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU  ##
## General Public License for more details.                              ##
##                                                                       ##
## You should have received a copy of the GNU General Public License     ##
## along with this program.  If not, see <http://www.gnu.org/licenses/>. ##
###########################################################################

check_PROGRAMS = introspective-system-test                                     \
                 introspective-cputime-test                                    \
                 monotonic-system-test                                         \
                 monotonic-cputime-test                                        \
                 monotonic-realticks-test
TESTS          = $(check_PROGRAMS)

INCLUDES = -I../src

introspective_system_test_SOURCES = introspective-system.c
introspective_system_test_LDADD   = ../src/libsandglass.la

introspective_cputime_test_SOURCES = introspective-cputime.c
introspective_cputime_test_LDADD   = ../src/libsandglass.la

monotonic_system_test_SOURCES = monotonic-system.c
monotonic_system_test_LDADD   = ../src/libsandglass.la

monotonic_cputime_test_SOURCES = monotonic-cputime.c
monotonic_cputime_test_LDADD   = ../src/libsandglass.la

monotonic_realticks_test_SOURCES = monotonic-realticks.c
monotonic_realticks_test_LDADD   = ../src/libsandglass.la
