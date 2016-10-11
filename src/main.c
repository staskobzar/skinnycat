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
#include "skinny_proto.h"

int main(int argc, const char *argv[])
{
  apr_status_t rv = APR_SUCCESS;
  apr_pool_t *mp;
  skinnycat_opts opts;

  apr_socket_t *sock;
  apr_sockaddr_t *sockaddr;

  apr_initialize();
  apr_pool_create(&mp, NULL);

  rv = parse_opts (&mp, &opts, argc, argv);
  if (rv != APR_EOF) { goto terminate; }

  LOG_DBG("Debugging test %d", 55);
  LOG_VERB("Verbose test %d", 55);
  LOG_ERR("Error test %d", 55);

  /* Init socket */
  rv = apr_sockaddr_info_get(&sockaddr, opts.host, APR_INET, opts.port, 0, mp);
  if (rv != APR_SUCCESS) {
    printf("Can not get socket address info %s:%d.\n", opts.host, opts.port);
    goto terminate;
  }
  /* Create socket */
  rv = apr_socket_create(&sock, sockaddr->family, SOCK_STREAM, APR_PROTO_TCP, mp);
  if (rv != APR_SUCCESS) {
    printf("Failed create socket.\n");
    goto terminate;
  }
  apr_socket_opt_set(sock, APR_SO_NONBLOCK, 1);
  apr_socket_timeout_set(sock, opts.sock_timeout);
  rv = apr_socket_connect(sock, sockaddr);

  rv = skinny_uac_run(mp, &opts, sock);

terminate:
  if (sock) apr_socket_close(sock);
  apr_pool_destroy(mp);
  apr_terminate();

  return rv;
}

