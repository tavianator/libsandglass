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
#include <unistd.h>
#include <time.h>
#include <errno.h>

int
sandglass_init_introspective(sandglass_t *sandglass, sandglass_resolution_t res)
{
  switch (res) {
  case SANDGLASS_CPUTIME:
    if (sysconf(_SC_THREAD_CPUTIME) > 0 || sysconf(_SC_CPUTIME) > 0) {
      sandglass->freq  = 1e9;
      sandglass->loops = 1;
    } else {
      errno = ENOTSUP;
      return -1;
    }
    break;

  case SANDGLASS_SYSTEM:
    sandglass->freq  = CLOCKS_PER_SEC;
    sandglass->loops = 1;
    break;

  default:
    errno = EINVAL;
    return -1;
  }

  sandglass->incrementation = SANDGLASS_INTROSPECTIVE;
  sandglass->resolution     = res;
  return 0;
}

int
sandglass_init_monotonic(sandglass_t *sandglass, sandglass_resolution_t res)
{
  switch (res) {
  case SANDGLASS_CPUTIME:
#ifdef SANDGLASS_TSC
    sandglass->freq  = sandglass_tsc_freq();
    sandglass->loops = sandglass_tsc_loops();
    break;
#else
    errno = ENOTSUP;
    return -1;
#endif

  case SANDGLASS_SYSTEM:
    sandglass->freq  = 1e9;
    sandglass->loops = 1;
    break;

  default:
    errno = EINVAL;
    return -1;
  }

  sandglass->incrementation = SANDGLASS_MONOTONIC;
  sandglass->resolution     = res;
  return 0;
}

/* Store a timer value in sandglass->grains */
static int sandglass_real_gettime(sandglass_t *sandglass);

/* Start timing */
int
sandglass_begin(sandglass_t *sandglass)
{
  return sandglass_real_gettime(sandglass);
}

/* Finish timing */
int
sandglass_elapse(sandglass_t *sandglass)
{
  long oldgrains = sandglass->grains;

  if (sandglass_real_gettime(sandglass))
    return -1;

  sandglass->grains -= oldgrains;
  if (sandglass->grains < 0)
    /* Magical correction for timespec-based grains */
    sandglass->grains += sandglass->adjustment;

  return 0;
}

/* Store a timer value in sandglass->grains */
static int
sandglass_real_gettime(sandglass_t *sandglass)
{
  struct timespec ts;
  clock_t clock_ticks;

  switch (sandglass->incrementation) {
    case SANDGLASS_MONOTONIC:
      switch (sandglass->resolution) {
        case SANDGLASS_CPUTIME:
#ifdef SANDGLASS_TSC
          sandglass->grains = sandglass_get_tsc();
          break;
#else
          errno = ENOTSUP;
          return -1;
#endif

        case SANDGLASS_SYSTEM:
          if (sysconf(_SC_MONOTONIC_CLOCK) > 0) {
            if (clock_gettime(CLOCK_MONOTONIC, &ts) != 0)
              return -1;
          } else {
            if (clock_gettime(CLOCK_REALTIME, &ts) != 0)
              return -1;
          }
          sandglass->grains     = ts.tv_nsec;
          sandglass->adjustment = 1000000000L;
          break;

        default:
          errno = EINVAL;
          return -1;
      }
      break;

    case SANDGLASS_INTROSPECTIVE:
      switch (sandglass->resolution) {
        case SANDGLASS_CPUTIME:
          if (sysconf(_SC_THREAD_CPUTIME) > 0) {
            if (clock_gettime(CLOCK_THREAD_CPUTIME_ID, &ts) != 0)
              return -1;
          } else if (sysconf(_SC_CPUTIME) > 0) {
            if (clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ts) != 0)
              return -1;
          } else {
            errno = EINVAL;
            return -1;
          }
          sandglass->grains     = ts.tv_nsec;
          sandglass->adjustment = 1000000000L;
          break;

        case SANDGLASS_SYSTEM:
          if ((clock_ticks = clock()) == -1)
            return -1;
          sandglass->grains = clock_ticks;
          break;

        default:
          errno = EINVAL;
          return -1;
      }
      break;

    default:
      errno = EINVAL;
      return -1;
  }

  return 0;
}
