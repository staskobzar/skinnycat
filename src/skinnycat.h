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
 * @file skinnycat.h
 * @brief Command line arguments/options parser and setting
 *        utility internal parameters.
 *
 * @author Stas Kobzar <stas@modulis.ca>
 */
#ifndef __SKINNYCAT_OPTS_H
#define __SKINNYCAT_OPTS_H

#include <apr_getopt.h>
#include <apr_network_io.h>
#include <apr_strings.h>

#define LEN_MAC     16
#define LEN_HOST    40
#define LEN_METHOD  32
#define DEFAULT_TIMEOUT (APR_USEC_PER_SEC * 10) // default socket timeout is 10 sec

typedef unsigned char bool;
#define false 0
#define true  (!false)

enum {
  LOG_LVL_DEBUG = 1,
  LOG_LVL_VERB  = 2,
  LOG_LVL_ERROR = 4
};

extern int LOGLVL;

#define LOG_DBG(...)  log_print(__FILE__, __LINE__, LOG_LVL_DEBUG, 1, __VA_ARGS__)
#define LOG_VERB(...) log_print(__FILE__, __LINE__, LOG_LVL_VERB,  1, __VA_ARGS__)
#define LOG_ERR(...)  log_print(__FILE__, __LINE__, LOG_LVL_ERROR, 2, __VA_ARGS__)

/*
 * Actions IDs
 */
enum action_id_e {
  AID_REGISTER = 1,
};
typedef enum action_id_e action_id;

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
  char method_str[LEN_METHOD];
  /** Action id */
  action_id action;
  /* Socket timeout */
  unsigned int sock_timeout;
  /* Enable debugging */
  bool debug;
  /* Enable verbosity */
  bool verb;
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

/**
 * Extract action ID from method name.
 * @param method  Method name as string
 * @return enum action
 */
action_id action_id_for_method(const char *method);

/**
 * Log debug/verbose/error output function.
 * @param file  File name
 * @param line  Line number
 * @param level Output level
 * @param fd    File descriptor
 * @param fmt   Formatted string
 * @param ...   Formatted string params list
 */
void log_print (char *file, int line, int level, int fd, char *fmt, ...);

#endif
