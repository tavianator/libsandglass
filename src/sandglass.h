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

/* A type to represent a clock's timing resolution */
typedef enum sandglass_resolution_t
{
  /*
   * Rely on the kernel to provide time information; less precise, more
   * portable
   */
  SANDGLASS_SYSTEM,

  /*
   * Get timing information directly from the processor; more precise, less
   * portable
   */
  SANDGLASS_CPUTIME
} sandglass_resolution_t;

/* An internal type to represent a clock's time measurement attributes */
typedef enum sandglass_incrementation_t
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
} sandglass_incrementation_t;

/* An high resolution timer */
typedef struct sandglass_t
{
  /* The attributes of the clock */
  sandglass_incrementation_t incrementation;
  sandglass_resolution_t     resolution;

  /* Units of time which have passed */
  long grains;

  /* grains/freq should give elapsed time in seconds */
  double freq;

  /*
   * Internal fields
   */

  /* Adjustment to be added for negative (i.e. overflowed) grains counts */
  long adjustment;

  /* For sandglass_bench_fine() looping support */
  int i, loops;

  /* A field used by sandglass_bench() to store the overhead of
     sandglass_begin()/_elapse(), and of looping */
  long baseline;
} sandglass_t;

/* Create a timer */
int sandglass_init_introspective(sandglass_t *sandglass,
                                 sandglass_resolution_t res);
int sandglass_init_monotonic(sandglass_t *sandglass,
                             sandglass_resolution_t res);

int sandglass_begin(sandglass_t *sandglass);
int sandglass_elapse(sandglass_t *sandglass);

/* Use this to prevent a loop from being unrolled */
#define SANDGLASS_NO_UNROLL() __asm__ __volatile__ ("")

/*
 * Macros to facilitate correct benchmarking of blocks of code.  May be called
 * like so:
 *   sandglass_bench*(&sandglass, f(x))
 * or like so:
 *   sandglass_bench*(&sandglass, {
 *     f(x);
 *     g(x);
 *   });
 */

/* Provides single clock cycle resolution in some cases */
#define sandglass_bench_fine(sandglass, routine)                               \
  do {                                                                         \
    /* Warm up the cache for these functions */                                \
    sandglass_begin(sandglass);                                                \
    sandglass_elapse(sandglass);                                               \
    sandglass_begin(sandglass);                                                \
    sandglass_elapse(sandglass);                                               \
                                                                               \
    /* Time an empty loop for our baseline */                                  \
    sandglass_begin(sandglass);                                                \
    for ((sandglass)->i = 0;                                                   \
         (sandglass)->i < (sandglass)->loops;                                  \
         ++(sandglass)->i) {                                                   \
      SANDGLASS_NO_UNROLL();                                                   \
    }                                                                          \
    sandglass_elapse(sandglass);                                               \
    (sandglass)->baseline = (sandglass)->grains;                               \
                                                                               \
    /* Warm up the cache for our routine */                                    \
    routine;                                                                   \
    routine;                                                                   \
                                                                               \
    /* Time our routine in a loop */                                           \
    sandglass_begin(sandglass);                                                \
    for ((sandglass)->i = 0;                                                   \
         (sandglass)->i < (sandglass)->loops;                                  \
         ++(sandglass)->i) {                                                   \
      SANDGLASS_NO_UNROLL();                                                   \
      routine;                                                                 \
      SANDGLASS_NO_UNROLL();                                                   \
    }                                                                          \
    sandglass_elapse(sandglass);                                               \
                                                                               \
    /* Subtract the baseline and divide by the loop count */                   \
    (sandglass)->grains -= (sandglass)->baseline;                              \
    (sandglass)->grains /= (sandglass)->loops;                                 \
  } while (0)

/* General high resolution timer */
#define sandglass_bench(sandglass, routine)                                    \
  do {                                                                         \
    /* Warm up the cache for these functions */                                \
    sandglass_begin(sandglass);                                                \
    sandglass_elapse(sandglass);                                               \
    sandglass_begin(sandglass);                                                \
    sandglass_elapse(sandglass);                                               \
                                                                               \
    /* Time an empty routine for our baseline */                               \
    sandglass_begin(sandglass);                                                \
    sandglass_elapse(sandglass);                                               \
    (sandglass)->baseline = (sandglass)->grains;                               \
                                                                               \
    /* Warm up the cache for our routine */                                    \
    routine;                                                                   \
    routine;                                                                   \
                                                                               \
    /* Time the routine */                                                     \
    sandglass_begin(sandglass);                                                \
    routine;                                                                   \
    sandglass_elapse(sandglass);                                               \
                                                                               \
    /* Subtract the baseline */                                                \
    (sandglass)->grains -= (sandglass)->baseline;                              \
  } while (0)

/* Only executes routine once - useful if routine has side-effects */
#define sandglass_bench_noprecache(sandglass, routine)                         \
  do {                                                                         \
    /* Warm up the cache for these functions */                                \
    sandglass_begin(sandglass);                                                \
    sandglass_elapse(sandglass);                                               \
    sandglass_begin(sandglass);                                                \
    sandglass_elapse(sandglass);                                               \
                                                                               \
    /* Time an empty loop for our baseline */                                  \
    sandglass_begin(sandglass);                                                \
    sandglass_elapse(sandglass);                                               \
    (sandglass)->baseline = (sandglass)->grains;                               \
                                                                               \
    /* Time the routine */                                                     \
    sandglass_begin(sandglass);                                                \
    routine;                                                                   \
    sandglass_elapse(sandglass);                                               \
                                                                               \
    /* Subtract the baseline */                                                \
    (sandglass)->grains -= (sandglass)->baseline;                              \
  } while (0)

#ifdef __cplusplus
}
#endif

#endif /* SANDGLASS_H_INCLUDED */
