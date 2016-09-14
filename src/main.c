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

#include "skinny_proto.h"

#define REMOTE_HOST   "192.168.1.138"
#define REMOTE_PORT   2000
#define SOCK_TIMEOUT  (APR_USEC_PER_SEC * 30)
#define NL "\n"

#define ISOK(rv, msg) do {  \
  if (rv != APR_SUCCESS) {  \
    printf (msg NL);        \
    return rv;              \
  }                         \
} while(0);

int main(int argc, const char *argv[])
{
  apr_status_t rv = APR_SUCCESS;
  //apr_size_t len;
  apr_pool_t *mp;
  apr_socket_t *s;
  apr_sockaddr_t *sa;

  printf ("Skinny Cat.\n");

  apr_initialize();
  apr_pool_create(&mp, NULL);

  rv = apr_sockaddr_info_get(&sa, REMOTE_HOST, APR_INET, REMOTE_PORT, 0, mp);
  ISOK(rv, "Can not get socket address info.");

  rv = apr_socket_create(&s, sa->family, SOCK_STREAM, APR_PROTO_TCP, mp);
  ISOK(rv, "Failed create socket.");

  apr_socket_opt_set(s, APR_SO_NONBLOCK, 1);
  apr_socket_timeout_set(s, SOCK_TIMEOUT);

  rv = apr_socket_connect(s, sa);
  ISOK(rv, "Failed connect socket.");

  //**************************************
  char packets[3][62] = {
    // register
    {0x34,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x53,0x45,0x50,0x30,0x30,0x30,0x44,0x36,0x35,0x33,0x33,0x41,0x38,0x42,0x37,0x00,0x00,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0xc5,0xa8,0x01,0x8a,0x08,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0b,0x00,0x60,0x85,0x00,0x00,0x00,0x00},
    // reject
    {0x25,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x9d,0x00,0x00,0x00,0x4e,0x6f,0x20,0x41,0x75,0x74,0x68,0x6f,0x72,0x69,0x74,0x79,0x3a,0x20,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
    // ack
    {0x18,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x81,0x00,0x00,0x00,0x18,0x00,0x00,0x00,0x44,0x2d,0x4d,0x2d,0x59,0x59,0x00,0x00,0x18,0x00,0x00,0x00,0x0b,0x20,0xf1,0xff}
  };
  struct skinny_header *hdr;
  int i;
  for (i = 0; i < 3; i++) {
    hdr = (struct skinny_header*) &packets[i];
    switch (hdr->msg_id) {
      case MSG_ID_REGISTER:
        printf("===> Register message.\n");
        struct message_register *reg = (struct message_register *)&packets[i];
        printf("Device: %s\n", reg->device_name);
        break;
      case MSG_ID_REGISTER_ACK:
        printf("===> Register ACK.\n");
        struct message_register_ack *ack = (struct message_register_ack *)&packets[i];
        printf("date template: %s\n", ack->date_template);
        break;
      case MSG_ID_REGISTER_REJECT:
        printf("===> Register Rejected.\n");
        struct message_register_reject *msg = (struct message_register_reject *)&packets[i];
        printf("Error message: %s\n", msg->error_text);
        break;
      default:
        printf("Unknown packet: %u [length: %u]\n", hdr->msg_id, hdr->length);
    }
  }

  //**************************************

  apr_socket_close(s);
  apr_pool_destroy(mp);
  apr_terminate();

  return 0;
}
