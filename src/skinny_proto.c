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
 * @file skinny_proto.c
 * @brief Basic implementation of Skinny protocol.
 *
 * @author Stas Kobzar <stas@modulis.ca>
 */
#include "skinny_proto.h"
#include <apr_strings.h>

char *generate_davice_name()
{
  return "SEP000D6533A8B7";
}

void device_name_set(char *dst, char *name)
{
  apr_cpystrn (dst, name ? name : generate_davice_name(), DEVICE_NAME_LEN);
}

apr_size_t init_register_message (apr_pool_t *mp, struct message_register **msg)
{
  struct message_register *reg = apr_palloc(mp, sizeof(struct message_register));
  device_name_set(reg->device_name, NULL);
  reg->reserved = 0;
  reg->instance = 1;
  reg->dev_type = 8; // 7940
  reg->max_concurrent_streams = 0;
  reg->active_rtp_streams = 0;
  reg->proto_ver = 0x0b; // 11
  reg->unknown = 0;
  reg->phone_features[0] = 0x60;
  reg->phone_features[1] = 0x85;

  *msg = reg;
  return sizeof(struct message_register);
}
