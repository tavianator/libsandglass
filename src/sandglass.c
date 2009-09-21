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

static int sandglass_real_create(sandglass_t *sandglass,
                                 const sandglass_attributes_t *attr);

/* Create a timer */
int
sandglass_create(sandglass_t *sandglass,
                 const sandglass_attributes_t *min,
                 const sandglass_attributes_t *max)
{
  sandglass_attributes_t realmin, realmax;

  /* Get our real min and max values */

  if (min)
    realmin = *min;
  else {
    /* min defaults to { SANDGLASS_INTROSPECTIVE, SANDGLASS_SYSTEM } */
    realmin.incrementation = SANDGLASS_INTROSPECTIVE;
    realmin.resolution     = SANDGLASS_SYSTEM;
  }

  if (max)
    realmax = *max;
  else {
    /* max defaults to the greater of min and { SANDGLASS_INTROSPECTIVE,
       SANDGLASS_CPUTIME } */
    if (realmin.incrementation > SANDGLASS_INTROSPECTIVE
        || (realmin.incrementation == SANDGLASS_INTROSPECTIVE
            && realmin.resolution > SANDGLASS_CPUTIME))
      realmax = realmin;
    else {
      realmax.incrementation = SANDGLASS_INTROSPECTIVE;
      realmax.resolution     = SANDGLASS_CPUTIME;
    }
  }

  /* Ensure max >= min */
  if (realmax.incrementation < realmin.incrementation
      || (realmax.incrementation == realmin.incrementation
          && realmax.resolution < realmin.resolution))
  {
    errno = EINVAL;
    return -1;
  }

  /* Now search for available timers, starting from max */

  while (sandglass_real_create(sandglass, &realmax) != 0) {
    /* Once we reach the minimum attributes, bail out */
    if (realmax.incrementation == realmin.incrementation
        && realmax.resolution == realmin.resolution)
    {
      errno = ENOTSUP;
      return -1;
    }

    /* Try the next lowest allowable settings */
    if (realmax.resolution)
      --realmax.resolution;
    else {
      if (realmax.incrementation) {
        --realmax.incrementation;
        realmax.resolution = SANDGLASS_CPUTIME;
      } else {
        errno = ENOTSUP;
        return -1;
      }
    }
  }

  return 0;
}

static int
sandglass_real_create(sandglass_t *sandglass,
                      const sandglass_attributes_t *attr)
{
  switch (attr->incrementation) {
    case SANDGLASS_MONOTONIC:
      switch (attr->resolution) {
        case SANDGLASS_REALTICKS:
#ifdef SANDGLASS_TSC
          sandglass->resolution = sandglass_tsc_resolution();
          sandglass->loops      = sandglass_tsc_loops();
          break;
#else
          return -1;
#endif

        case SANDGLASS_CPUTIME:
#ifdef SANDGLASS_TSC
          sandglass->resolution = sandglass_tsc_resolution();
          sandglass->loops      = 1;
          break;
#else
          return -1;
#endif

        case SANDGLASS_SYSTEM:
          sandglass->resolution = 1e9;
          sandglass->loops      = 1;
          break;

        default:
          return -1;
      }
      break;

    case SANDGLASS_INTROSPECTIVE:
      switch (attr->resolution) {
        case SANDGLASS_REALTICKS:
          /* No such thing as an introspective raw TSC */
          return -1;

        case SANDGLASS_CPUTIME:
          if (sysconf(_SC_THREAD_CPUTIME) > 0 || sysconf(_SC_CPUTIME) > 0) {
            sandglass->resolution = 1e9;
            sandglass->loops      = 1;
          } else
            return -1;
          break;

        case SANDGLASS_SYSTEM:
          sandglass->resolution = CLOCKS_PER_SEC;
          sandglass->loops      = 1;
          break;

        default:
          return -1;
      }
      break;

    default:
      return -1;
  }

  sandglass->attributes = *attr;
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
  sandglass_t baseline;

  if (sandglass_real_gettime(sandglass) != 0)
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

  switch (sandglass->attributes.incrementation) {
    case SANDGLASS_MONOTONIC:
      switch (sandglass->attributes.resolution) {
        case SANDGLASS_REALTICKS:
        case SANDGLASS_CPUTIME:
#ifdef SANDGLASS_TSC
          sandglass->grains = sandglass_get_tsc();
          break;
#else
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
          return -1;
      }
      break;

    case SANDGLASS_INTROSPECTIVE:
      switch (sandglass->attributes.resolution) {
        case SANDGLASS_REALTICKS:
          /* No such thing as an introspective raw TSC */
          return -1;

        case SANDGLASS_CPUTIME:
          if (sysconf(_SC_THREAD_CPUTIME) > 0) {
            if (clock_gettime(CLOCK_THREAD_CPUTIME_ID, &ts) != 0)
              return -1;
          } else if (sysconf(_SC_CPUTIME) > 0) {
            if (clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ts) != 0)
              return -1;
          } else
            return -1;
          sandglass->grains     = ts.tv_nsec;
          sandglass->adjustment = 1000000000L;
          break;

        case SANDGLASS_SYSTEM:
          if ((clock_ticks = clock()) == -1)
            return -1;
          sandglass->grains = clock();
          break;

        default:
          return -1;
      }
      break;

    default:
      return -1;
  }

  return 0;
}
