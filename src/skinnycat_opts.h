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
 * @file skinnycat_opts.h
 * @brief Command line arguments/options parser and setting
 *        utility internal parameters.
 *
 * @author Stas Kobzar <stas@modulis.ca>
 */
#ifndef __SKINNYCAT_OPTS_H
#define __SKINNYCAT_OPTS_H

#include <apr_getopt.h>

#define LEN_MAC     16
#define LEN_HOST    40
#define LEN_METHOD  32

/**
 * Structure to store configuration parameters
 */
struct skinnycat_opts_s {
  /** MAC address of the phone. If not present in cli arguments will be generated. */
  char mac[LEN_MAC];
  /** Destination hostname or IP address. */
  char host[LEN_HOST];
  /** Destination port. Default is 2000. */
  unsigned int port;
  /** Method to use (register, call etc.) */
  char method[LEN_METHOD];
};
/**
 * @see skinnycat_opts_s
 */
typedef struct skinnycat_opts_s skinnycat_opts;

/**
 * Init configuration parameters structure
 * @param opts  Options structure
 * @return      APR status
 */
apr_status_t init_conf_options (skinnycat_opts *opts);

/**
 * Parses the options, initialize and set options structure.
 * @param mp    APR memory pool
 * @param opts  Options structure
 * @param argc  Number of argumets
 * @param argv  Array of arguments
 * @return      APR status
 */
apr_status_t parse_opts (apr_pool_t **mp, skinnycat_opts *opts, int argc, const char *argv[]);

#endif
