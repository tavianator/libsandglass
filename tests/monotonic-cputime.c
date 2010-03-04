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

#include "../src/sandglass_impl.h"
#include "../src/sandglass.h"
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>

int
main()
{
  sandglass_t sandglass;
  struct timespec tosleep = { .tv_sec = 0, .tv_nsec = 111111111L };

  if (sandglass_init_monotonic(&sandglass, SANDGLASS_CPUTIME) != 0) {
    perror("sandglass_init_monotonic()");
    return EXIT_FAILURE;
  }

  sandglass_bench(&sandglass, sandglass_spin(&tosleep));

  printf("%.15g\n", sandglass.grains/sandglass.freq);

  return EXIT_SUCCESS;
}
