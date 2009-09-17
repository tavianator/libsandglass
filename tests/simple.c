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

#include <sandglass.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>

int
main()
{
  sandglass_t sandglass;
  sandglass_attributes_t min = { SANDGLASS_MONOTONIC, SANDGLASS_SYSTEM },
                         max = { SANDGLASS_MONOTONIC, SANDGLASS_CPUTIME };
  struct timespec tosleep = { .tv_sec = 0, .tv_nsec = 100000000 };

  if (sandglass_create(&sandglass, &min, &max) != 0) {
    perror("sandglass_create()");
    return EXIT_FAILURE;
  }

  if (sandglass_begin(&sandglass) != 0) {
    perror("sandglass_begin()");
    return EXIT_FAILURE;
  }
  while (nanosleep(&tosleep, &tosleep) != 0);
  if (sandglass_elapse(&sandglass) != 0) {
    perror("sandglass_elapse()");
    return EXIT_FAILURE;
  }

  printf("0.1 seconds timed by sandglass as %ld grains; %g s\n",
         sandglass.grains,
         sandglass.grains/sandglass.resolution);

  return EXIT_SUCCESS;
}
