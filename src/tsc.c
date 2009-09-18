/*************************************************************************
 * Copyright (C) 2008 Tavian Barnes <tavianator@gmail.com>               *
 *                                                                       *
 * This file is part of The Sandglass Library.                           *
 *                                                                       *
 * The Sandglass Library is free software; you can redistribute it       *
 * and/or modify it under the terms of the GNU Lesser General Public     *
 * License as published by the Free Software Foundation; either version  *
 * 3 of the License, or (at your option) any later version.              *
 *                                                                       *
 * The Sandglass Library is distributed in the hope that it will be      *
 * useful, but WITHOUT ANY WARRANTY; without even the implied warranty   *
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU  *
 * Lesser General Public License for more details.                       *
 *                                                                       *
 * You should have received a copy of the GNU Lesser General Public      *
 * License along with this program.  If not, see                         *
 * <http://www.gnu.org/licenses/>.                                       *
 *************************************************************************/

#include "sandglass_impl.h"
#include "sandglass.h"
#include <time.h>
#include <unistd.h>

/* Gets the number of clock ticks per second */
double
sandglass_tsc_resolution()
{
  static long tsc = 0, grains1, grains2;

  int monotonic;
  struct timespec ts;

  if (tsc == 0) {
    monotonic = sysconf(_SC_MONOTONIC_CLOCK) > 0;
    if (monotonic) {
      if (clock_gettime(CLOCK_MONOTONIC, &ts) != 0)
        return 0.0/0.0;
    } else {
      if (clock_gettime(CLOCK_REALTIME, &ts) != 0)
        return 0.0/0.0;
    }
    tsc = sandglass_get_tsc();
    grains1 = sandglass_timespec_grains(&ts);
    grains2 = grains1;

    while (((grains2 >= grains1) ? grains2 - grains1
                                 : 1000000000L + (grains2 - grains1))
           < 10000000L)
    {
      if (monotonic) {
        if (clock_gettime(CLOCK_MONOTONIC, &ts) != 0)
          return 0.0/0.0;
      } else {
        if (clock_gettime(CLOCK_REALTIME, &ts) != 0)
          return 0.0/0.0;
      }
      grains2 = ts.tv_nsec;
    }

    tsc = sandglass_get_tsc() - tsc;
  }

  return tsc*1.0e9/(grains2 - grains1);
}
