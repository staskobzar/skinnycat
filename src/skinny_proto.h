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
 * @file skinny_proto.h
 * @brief Basic implementation of Skinny protocol.
 *
 * @author Stas Kobzar <stas@modulis.ca>
 */
#ifndef __SKINNY_PROTO_H
#define __SKINNY_PROTO_H

#include "skinny_msg.h"

/**
 * Main skinny cycle
 * @param mp    APR memory pool pointer
 * @param opts  Application configuration options
 * @param sock  Socket
 * @return APR status
 */
apr_status_t skinny_uac_run(apr_pool_t *mp, skinnycat_opts *opts, apr_socket_t *sock);

/**
 * Registration call flow
 * @param mp    APR memory pool pointer
 * @param opts  Application configuration options
 * @param sock  Socket
 * @return APR status
 */
apr_status_t callflaw_register(apr_pool_t *mp, skinnycat_opts *opts, apr_socket_t *sock);

/**
 * Keepalive call flow
 * @param mp    APR memory pool pointer
 * @param opts  Application configuration options
 * @param sock  Socket
 * @return APR status
 */
apr_status_t callflaw_keepalive(apr_pool_t *mp, skinnycat_opts *opts, apr_socket_t *sock);

/**
 * Get local IP from socket.
 * @param sock  APR socket
 * @return IP as long integer
 */
unsigned long sock_local_ip(apr_socket_t *sock);

#endif
