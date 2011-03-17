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

#include "sandglass-impl.h"
#include "sandglass.h"
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

void
sandglass_get_currtime(struct timespec *ts)
{
  static int monotonic = 0;
  if (!monotonic) {
    monotonic = sysconf(_SC_MONOTONIC_CLOCK);
  }

  if (monotonic > 0) {
    if (clock_gettime(CLOCK_MONOTONIC, ts) != 0) {
      fprintf(stderr,
              "libsandglass: couldn't get value of clock CLOCK_MONOTONIC\n");
      exit(EXIT_FAILURE);
    }
  } else {
    if (clock_gettime(CLOCK_REALTIME, ts) != 0) {
      fprintf(stderr,
              "libsandglass: couldn't get value of clock CLOCK_REALTIME\n");
      exit(EXIT_FAILURE);
    }
  }
}

void
sandglass_timespec_add(struct timespec *ts, const struct timespec *d)
{
  if (d->tv_nsec >= 1000000000L - ts->tv_nsec) {
    ts->tv_nsec += d->tv_nsec - 1000000000L;
    ++ts->tv_sec;
  } else {
    ts->tv_nsec += d->tv_nsec;
  }
  ts->tv_sec += d->tv_sec;
}

void
sandglass_timespec_sub(struct timespec *ts, const struct timespec *d)
{
  if (d->tv_nsec > ts->tv_nsec) {
    ts->tv_nsec -= d->tv_nsec - 1000000000L;
    --ts->tv_sec;
  } else {
    ts->tv_nsec -= d->tv_nsec;
  }
  ts->tv_sec -= d->tv_sec;
}

int
sandglass_timespec_cmp(const struct timespec *a, const struct timespec *b)
{
  if (a->tv_sec > b->tv_sec) {
    return 1;
  } else if (a->tv_sec == b->tv_sec) {
    if (a->tv_nsec > b->tv_nsec)
      return 1;
    else if (a->tv_nsec == b->tv_nsec)
      return 0;
    else
      return -1;
  } else {
    return -1;
  }
}

/* Spins for the time interval specified by ts */
void
sandglass_spin(const struct timespec *ts)
{
  struct timespec curr, until;
  sandglass_get_currtime(&curr);
  until = curr;
  sandglass_timespec_add(&until, ts);

  /* Spin */
  do {
    sandglass_get_currtime(&curr);
  } while (sandglass_timespec_cmp(&curr, &until) < 0);
}
