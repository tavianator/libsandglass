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
sandglass_tsc_freq()
{
  static long tsc = 0;
  static struct timespec ts = { .tv_sec = 0, .tv_nsec = 10000000L };
  struct timespec curr, until;

  if (tsc == 0) {
    sandglass_get_currtime(&curr);
    until = curr;
    sandglass_timespec_add(&until, &ts);
    tsc = sandglass_get_tsc();

    /* Spin */
    do {
      sandglass_get_currtime(&curr);
    } while (sandglass_timespec_cmp(&curr, &until) < 0);

    tsc = sandglass_get_tsc() - tsc;

    /* Adjust ts to the time actually waited */
    sandglass_timespec_sub(&curr, &until);
    sandglass_timespec_add(&ts, &curr);
  }

  return tsc*1.0e9/ts.tv_nsec;
}
