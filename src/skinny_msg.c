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
 * @file skinny_msg.c
 * @brief Skinny protocol messages functions
 *
 * @author Stas Kobzar <stas@modulis.ca>
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "skinny_msg.h"

struct skinny_msg_list messages[] = {
  { MID_KEEPALIVE,        unpack_keepalive },
  { MID_REGISTER,         unpack_register },
  { MID_REGISTER_ACK,     unpack_register_ack },
  { MID_REGISTER_REJECT,  unpack_register_reject },
  { MID_RESET,            unpack_reset },
  { MID_KEEPALIVE_ACK,    unpack_keepalive_ack },
  /* sentinel */
  { MID_INVALID,          NULL }
};

/*
 * Identify Skinny message
 */
skinny_msg_id unpack_message (const char *packet, struct skinny_message *msg) {
  struct skinny_header *hdr = (struct skinny_header *) packet;
  struct skinny_msg_list *p = messages;
  for (; p->id >= 0; p++)
    if (p->id == hdr->msg_id) {
      return p->unpack(packet, msg);
    }
  return p->id;
}

skinny_msg_id unpack_keepalive (const char *packet, struct skinny_message *msg)
{
  (void)msg;
  return MID_KEEPALIVE;
}

skinny_msg_id unpack_register (const char *packet, struct skinny_message *msg)
{
  struct skinny_header *hdr =  (struct skinny_header *) packet;
  msg->header = hdr;
  packet += SKINNY_HEADER_LEN;
  struct message_register *data = (struct message_register *) packet;
  msg->data = data;
  return hdr->msg_id;
}

skinny_msg_id unpack_register_ack (const char *packet, struct skinny_message *msg)
{
  struct skinny_header *hdr =  (struct skinny_header *) packet;
  msg->header = hdr;
  packet += SKINNY_HEADER_LEN;
  struct message_register_ack *data = (struct message_register_ack *) packet;
  msg->data = data;
  return hdr->msg_id;
}

skinny_msg_id unpack_register_reject (const char *packet, struct skinny_message *msg)
{
  struct skinny_header *hdr =  (struct skinny_header *) packet;
  msg->header = hdr;
  packet += SKINNY_HEADER_LEN;
  struct message_register_reject *data = (struct message_register_reject *) packet;
  msg->data = data;
  return hdr->msg_id;
}

skinny_msg_id unpack_reset (const char *packet, struct skinny_message *msg)
{
  struct skinny_header *hdr =  (struct skinny_header *) packet;
  msg->header = hdr;
  packet += SKINNY_HEADER_LEN;
  struct message_reset *data = (struct message_reset *) packet;
  msg->data = data;
  return hdr->msg_id;
}

skinny_msg_id unpack_keepalive_ack (const char *packet, struct skinny_message *msg)
{
  (void)msg;
  return MID_KEEPALIVE_ACK;
}
