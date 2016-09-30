/**
 * skinnycat -- Skinny Cisco protocol command line utility
 * Copyright (C) 2016, Stas Kobzar <stas@modulis.ca>
 *
 * This file is part of skinnycat.
 *
 * skinnycat is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * skinnycat is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with skinnycat.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * @file main.c
 * @brief skinnycat utility main
 *
 * @author Stas Kobzar <stas@modulis.ca>
 */
#include <stdio.h>
#include <apr_general.h>
#include <apr_network_io.h>

#include "skinnycat_opts.h"
#include "skinny_proto.h"

int main(int argc, const char *argv[])
{
  apr_status_t rv = APR_SUCCESS;
  apr_pool_t *mp;
  skinnycat_opts opts;

  apr_initialize();
  apr_pool_create(&mp, NULL);

  rv = parse_opts (&mp, &opts, argc, argv);
  if (rv != APR_EOF) { return rv; }

  apr_pool_destroy(mp);
  apr_terminate();

  return 0;
}

