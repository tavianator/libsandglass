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
 * Internal libsandglass API
 */

#ifndef SANDGLASS_IMPL_H_INCLUDED
#define SANDGLASS_IMPL_H_INCLUDED

#include "sandglass.h"
#include <time.h>

#if SANDGLASS_TSC
/* Read the time stamp counter */
long sandglass_get_tsc();
/* Get the frequency of the TSC */
double sandglass_tsc_freq();
/* Get the necessary number of loops for sandglass_bench_fine() */
unsigned int sandglass_tsc_loops();
#endif

void sandglass_get_currtime(struct timespec *ts);
void sandglass_timespec_add(struct timespec *ts, const struct timespec *d);
void sandglass_timespec_sub(struct timespec *ts, const struct timespec *d);
int  sandglass_timespec_cmp(const struct timespec *a, const struct timespec *b);
void sandglass_spin(const struct timespec *ts);

#endif /* SANDGLASS_IMPL_H_INCLUDED */
