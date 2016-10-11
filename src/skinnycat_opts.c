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
 * @file skinnycat_opts.c
 * @brief Command line arguments/options parser and setting
 *        utility internal parameters implementation.
 *
 * @author Stas Kobzar <stas@modulis.ca>
 */
#include <stdio.h>
#include <stdlib.h>
#include "skinnycat_opts.h"

/**
 * Structure of skinnycat parameters.
 */
static const apr_getopt_option_t optset[] = {
  { "help",   'h',  FALSE,  "Prints out a simple usage help message." },
  { "mac",    'm',  TRUE,   "MAC address to use as SKINNY MAC." },
  { "host",   'H',  TRUE,   "Destination host." },
  { "port",   'p',  TRUE,   "Destination port." },
  { "method", 'M',  TRUE,   "Action method." },
  { "debug",  'd',  FALSE,  "Enable debugging." },
  { "verbose",'v',  FALSE,  "Enable verbose output." },
  { NULL, 0, 0, NULL },
};

/**
 * Print usage message.
 */
static void usage (const char *program, const apr_getopt_option_t *opts);
static void gen_cisco_mac (char *mac);

/*
 * Parsing command line arguments.
 */
apr_status_t parse_opts (apr_pool_t **mp,
                         skinnycat_opts *options,
                         int argc,
                         const char *argv[])
{
  apr_status_t rv;
  apr_getopt_t *opt;
  int optch;
  const char *optarg;
  char *endptr;
  int port;

  // no arguments, print help
  if (argc < 2) {
    usage (argv[0], optset);
    return APR_SUCCESS;
  }

  init_conf_options(options);
  apr_getopt_init(&opt, *mp, argc, argv);
  while ((rv = apr_getopt_long(opt, optset, &optch, &optarg)) == APR_SUCCESS) {
    switch (optch) {
      case 'h': // print usage
        usage (argv[0], optset);
        return rv;
        break;
      case 'm': // mac address
        apr_cpystrn(options->mac, optarg, LEN_MAC);
        break;
      case 'H': // destination host
        apr_cpystrn(options->host, optarg, LEN_HOST);
        break;
      case 'p': // destination port
        port = strtol(optarg, &endptr, 10);
        if (port < 1 || port > 65535) {
          printf("%s: invalid port %d\n", argv[0], port);
          return APR_BADARG;
        }
        options->port = port;
        break;
      case 'M': // action method
        apr_cpystrn(options->method_str, optarg, LEN_METHOD);
        options->action = action_id_for_method(optarg);
        if (options->action == -1) {
          printf("%s: invalid method %s\n", argv[0], optarg);
          return APR_BADARG;
        }
        break;
      case 'd':
        options->debug = true;
        printf("Debugging output enabled.\n");
        break;
      case 'v':
        options->verb = true;
        printf("Verbosity enabled.\n");
        break;

      default:
        break;
    }
  }
  if (rv != APR_EOF) {
    printf("Run \"%s --help\" for options list.\n", argv[0]);
  }
  return rv;
}

/*
 * Init configuration parameters structure
 */
apr_status_t init_conf_options (skinnycat_opts *opts) {
  apr_status_t rv = APR_SUCCESS;
  // generate random Cisco mac
  gen_cisco_mac (opts->mac);
  // default port
  opts->port = 2000;
  // timeout
  opts->sock_timeout = DEFAULT_TIMEOUT;
  opts->debug = false;
  opts->verb  = false;
  return rv;
}

/*
 * Usage message
 */
static void usage (const char *program,
                   const apr_getopt_option_t *opts) {
  printf("Usage: %s [OPTIONS]\n", program);
  printf("       Cisco Skinny protocol command line utility.\n");
  printf("\n");
  printf("OPTIONS:\n");
  while (opts->optch != 0) {
    printf("  -%c, --%s ", opts->optch, opts->name);
    if (opts->has_arg) printf("VALUE");
    printf("\n");
    printf("        %s\n", opts->description);
    printf("\n");
    opts++;
  }
}

/*
 * Generate random CISCO MAC address
 */
static void gen_cisco_mac (char *mac) {
  apr_cpystrn(mac, "1C17D354D46F", LEN_MAC);
}

/*
 * Get action id from method name.
 */
action_id action_id_for_method(const char *method)
{
  action_id aid = -1;
    if (apr_strnatcasecmp(method, "REGISTER") == 0) {
      aid = AID_REGISTER;
    }
  return aid;
}

/*
 * Log print
 */
void log_print (char *file, int line, int level, int fd, char *fmt, ...)
{
  va_list args;
  switch (level) {
    case LOG_LVL_DEBUG:
      dprintf(fd, "%s:%d [DEBUG] ", file, line);
      break;
    case LOG_LVL_VERB:
      dprintf(fd, " ");
      break;
    case LOG_LVL_ERROR:
      dprintf(fd, "%s:%d ERROR: ", file, line);
      break;
  }
  va_start(args, fmt);
  vdprintf(fd, fmt, args);
  va_end(args);
  dprintf(fd, "\n");
}

