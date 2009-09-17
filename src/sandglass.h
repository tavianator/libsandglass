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

/*
 * libsandglass - a library for extremely high-resolution timing, benchmarking,
 * and profiling.
 */

#ifndef SANDGLASS_H_INCLUDED
#define SANDGLASS_H_INCLUDED

#ifdef __cplusplus
/* We've been included from a C++ file; mark everything here as extern "C" */
extern "C" {
#endif

/* A type to represent a clock's time measurement attributes */
enum sandglass_incrementation_t
{
  /*
   * A clock which only increments during the current process's execution.  Less
   * precise than a monotonic clock, but more accurate and useful for profiling
   * longer operations.
   */
  SANDGLASS_INTROSPECTIVE,

  /*
   * A clock which increments in real-time, regardless of whether the current
   * process was executing during that time or not.  Allows for greater timing
   * resolution, but will be inaccurate if the process is context-switched
   * while being timed.
   */
  SANDGLASS_MONOTONIC
};
typedef enum sandglass_incrementation_t sandglass_incrementation_t;

/* A type to represent a clock's timing resolution */
enum sandglass_resolution_t
{
  /*
   * Rely on the kernel to provide time information; less precise, more
   * portable.  Uses times().
   */
  SANDGLASS_SYSTEM,

  /*
   * Get timing information directly from the processor; more precise, less
   * portable.  Uses the CLOCK_THREAD_CPUTIME_ID clock for
   * SANDGLASS_INTROSPECTIVE mode, and the raw TSC for SANDGLASS_MONOTONIC.
   */
  SANDGLASS_CPUTIME,

  /*
   * Get timing information accurate to within one CPU clock cycle - requires
   * some looping and other trickery to account for TSCs which are not accurate
   * to within one clock cycle.  Not valid with SANDGLASS_INTROSPECTIVE.
   */
  SANDGLASS_REALTICKS
};
typedef enum sandglass_resolution_t sandglass_resolution_t;

/* Attributes of a clock */
struct sandglass_attributes_t
{
  sandglass_incrementation_t incrementation;
  sandglass_resolution_t     resolution;
};
typedef struct sandglass_attributes_t sandglass_attributes_t

struct sandglass_t
{
  /* The attributes of the clock */
  sandglass_attributes_t attributes;

  /* Units of time which have passed */
  long grains;

  /* grains/resolution should give elapsed time in seconds */
  long resolution;

  /*
   * Internal fields
   */

  /* For SANDGLASS_TICKS looping support */
  unsigned int i, loops;
};
typedef struct sandglass_t sandglass_t;

/*
 * Creates a timer with at least the precision of `min', and at most the
 * precision of `max'.  Precisions are compared first by incrementation type:
 * all monotonic timers are considered more precise than introspective timers.
 * Then, higher resolution timers take precidence.
 *
 * If `min' is NULL, it defaults to { SANDGLASS_INTROSPECTIVE,
 * SANDGLASS_SYSTEM }.
 *
 * If `max' is NULL, it defaults to at least `min', but not less than
 * { SANDGLASS_INTROSPECTIVE, SANDGLASS_CPUTIME }.
 */
int sandglass_create(sandglass_t *sandglass,
                     const sandglass_attributes_t *min,
                     const sandglass_attributes_t *max);

int sandglass_begin(sandglass_t *sandglass);
int sandglass_elapse(sandglass_t *sandglass);

/* Use this to prevent a loop from being unrolled */
#define SANDGLASS_NO_UNROLL() __asm__ __volatile__ ("")

/*
 * Macro to facilitate correct benchmarking of blocks of code.  May be called
 * like so:
 *   sandglass_bench(&sandglass, f(x))
 * or like so:
 *   sandglass_bench(&sandglass, {
 *     f(x);
 *     g(x);
 *   });
 */
#define sandglass_bench(sandglass, routine)                                    \
  do {                                                                         \
    routine;                                                                   \
    sandglass_begin(sandglass);                                                \
    for ((sandglass)->i = 0; (sandglass)->i < (sandglass)->loops; ++i) {       \
      SANDGLASS_NO_UNROLL();                                                   \
      routine;                                                                 \
    }                                                                          \
    sandglass_elapse(sandglass);                                               \
  } while (0)

/*
 * Low-level API
 */

/* Read the time stamp counter */
long sandglass_get_tsc();

#ifdef __cplusplus
}
#endif

#endif /* SANDGLASS_H_INCLUDED */
