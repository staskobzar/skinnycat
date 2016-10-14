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
#include "skinnycat.h"
#include "skinny_msg.h"

struct skinny_msg_list messages[] = {
  { MID_KEEPALIVE,        unpack_keepalive },
  { MID_REGISTER,         unpack_register },
  { MID_CAPABILITIES_RES, unpack_capabilities_res },
  { MID_BUTTON_TMPL_REQ,  unpack_button_tmpl_req },
  { MID_REGISTER_ACK,     unpack_register_ack },
  { MID_SETLAMP,          unpack_setlamp },
  { MID_CAPABILITIES_REQ, unpack_capabilities_req },
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
  LOG_DBG("Unpack message KEEPALIVE");
  return MID_KEEPALIVE;
}

skinny_msg_id unpack_register (const char *packet, struct skinny_message *msg)
{
  LOG_DBG("Unpack message REGISTER");
  struct skinny_header *hdr =  (struct skinny_header *) packet;
  msg->header = hdr;
  packet += SKINNY_HEADER_LEN;
  struct message_register *data = (struct message_register *) packet;
  msg->data = (union skinny_message_data*) data;
  return hdr->msg_id;
}

skinny_msg_id unpack_capabilities_res (const char *packet, struct skinny_message *msg)
{
  LOG_DBG("Unpack message CAPABILITIES RESPONSE");
  struct skinny_header *hdr =  (struct skinny_header *) packet;
  msg->header = hdr;
  packet += SKINNY_HEADER_LEN;
  struct message_capabilities_res *data = (struct message_capabilities_res *) packet;
  msg->data = (union skinny_message_data*) data;
  return hdr->msg_id;
}

skinny_msg_id unpack_button_tmpl_req (const char *packet, struct skinny_message *msg)
{
  (void)msg;
  LOG_DBG("Unpack message BUTTON TEMPLATE REQUEST");
  return MID_BUTTON_TMPL_REQ;
}

skinny_msg_id unpack_register_ack (const char *packet, struct skinny_message *msg)
{
  LOG_DBG("Unpack message REGISTER_ACK");
  struct skinny_header *hdr =  (struct skinny_header *) packet;
  msg->header = hdr;
  packet += SKINNY_HEADER_LEN;
  struct message_register_ack *data = (struct message_register_ack *) packet;
  msg->data = (union skinny_message_data*) data;
  return hdr->msg_id;
}

skinny_msg_id unpack_setlamp (const char *packet, struct skinny_message *msg)
{
  LOG_DBG("Unpack message SET LAMP");
  struct skinny_header *hdr =  (struct skinny_header *) packet;
  msg->header = hdr;
  packet += SKINNY_HEADER_LEN;
  struct message_setlamp *data = (struct message_setlamp *) packet;
  msg->data = (union skinny_message_data*) data;
  return hdr->msg_id;
}

skinny_msg_id unpack_capabilities_req (const char *packet, struct skinny_message *msg)
{
  (void)msg;
  LOG_DBG("Unpack message CAPABILITIES REQUEST");
  return MID_CAPABILITIES_REQ;
}

skinny_msg_id unpack_register_reject (const char *packet, struct skinny_message *msg)
{
  LOG_DBG("Unpack message REGISTER_REJECT");
  struct skinny_header *hdr =  (struct skinny_header *) packet;
  msg->header = hdr;
  packet += SKINNY_HEADER_LEN;
  struct message_register_reject *data = (struct message_register_reject *) packet;
  msg->data = (union skinny_message_data*) data;
  return hdr->msg_id;
}

skinny_msg_id unpack_reset (const char *packet, struct skinny_message *msg)
{
  LOG_DBG("Unpack message RESET");
  struct skinny_header *hdr =  (struct skinny_header *) packet;
  msg->header = hdr;
  packet += SKINNY_HEADER_LEN;
  struct message_reset *data = (struct message_reset *) packet;
  msg->data = (union skinny_message_data*) data;
  return hdr->msg_id;
}

skinny_msg_id unpack_keepalive_ack (const char *packet, struct skinny_message *msg)
{
  (void)msg;
  LOG_DBG("Unpack message KEEPALIVE_ACK");
  return MID_KEEPALIVE_ACK;
}

apr_size_t create_msg_register (apr_pool_t *mp,
                                char **buf,
                                skinnycat_opts *opts,
                                unsigned long ip_addr)
{
  apr_size_t size = SKINNY_HEADER_LEN + sizeof(struct message_register);
  struct message {
    struct skinny_header hdr;
    struct message_register data;
  } message;
  struct message *msg = apr_palloc(mp, sizeof(message));
  LOG_DBG("Create REGISTER packet.");
  msg->hdr.msg_id = MID_REGISTER;
  msg->hdr.length = sizeof(struct message_register) + 4;
  msg->hdr.version = 0;
  msg->data.reserved = 0;
  msg->data.instance = 1;
  msg->data.ip_addr = ip_addr;
  msg->data.dev_type = 8; // 7940
  msg->data.max_concurrent_streams = 0;
  msg->data.active_rtp_streams = 0;
  msg->data.proto_ver = 0x0b; // 11
  msg->data.unknown = 0;
  msg->data.phone_features[0] = 0x60;
  msg->data.phone_features[1] = 0x85;
  apr_cpystrn(msg->data.device_name, apr_pstrcat(mp, "SEP", opts->mac, NULL), DEVICE_NAME_LEN);
  *buf = (char *)msg;

  return size;
}

apr_size_t create_msg_cap_res (apr_pool_t *mp, char **buf)
{
  apr_size_t size = SKINNY_HEADER_LEN + sizeof(struct message_capabilities_res);
  struct message {
    struct skinny_header hdr;
    uint32_t cap_count;
    struct station_capabilities caps[18];
  } message;

  struct station_capabilities data[] = {
    {0x0019, 0x0078, {0,0,0,0}},
    {0x0004, 0x0028, {0,0,0,0}},
    {0x0002, 0x0028, {0,0,0,0}},
  };

  struct message *msg = apr_palloc(mp, sizeof(message));
  LOG_DBG("Create CAPABILITIES RES packet.");
  msg->hdr.msg_id = MID_CAPABILITIES_RES;
  msg->hdr.length = sizeof(struct message_capabilities_res) + 4;
  msg->hdr.version = 0;
  msg->cap_count = 3;
  msg->caps[0] = data[0];
  msg->caps[1] = data[1];
  msg->caps[2] = data[2];

  *buf = (char*)msg;
  return size;
}

apr_size_t create_msg_btn_tmpl_req (apr_pool_t *mp, char **buf)
{
  apr_size_t size = SKINNY_HEADER_LEN;
  struct skinny_header *tmpl = apr_palloc(mp, sizeof(struct skinny_header));
  tmpl->msg_id = MID_BUTTON_TMPL_REQ;
  tmpl->length = 4;
  tmpl->version = 0;

  *buf = (char*)tmpl;
  return size;
}

const char* lamp_mode_to_str (enum skinny_lamp_mode mode)
{
  switch (mode) {
    case SKINNY_LAMP_OFF:
      return "OFF";
    case SKINNY_LAMP_ON:
      return "ON";
    case SKINNY_LAMP_WINK:
      return "WINK";
    case SKINNY_LAMP_FLASH:
      return "FLASH";
    case SKINNY_LAMP_BLINK:
      return "BLINK";
    default:
      return "UNKNOWN";
  }
}

const char* btn_def_to_str (enum skinny_button_definition btn)
{
  switch (btn) {
    case SKINNY_BUTTON_LAST_NUMBER_REDIAL:
      return "LAST NUMBER REDIAL";
    case SKINNY_BUTTON_SPEED_DIAL:
      return "SPEED DIAL";
    case SKINNY_BUTTON_HOLD:
      return "HOLD";
    case SKINNY_BUTTON_TRANSFER:
      return "TRANSFER";
    case SKINNY_BUTTON_FORWARDALL:
      return "FORWARD ALL";
    case SKINNY_BUTTON_LINE:
      return "LINE";
    case SKINNY_BUTTON_VOICEMAIL:
      return "VOICEMAIL";
    case SKINNY_BUTTON_PRIVACY:
      return "PRIVACY";
    case SKINNY_BUTTON_SERVICE_URL:
      return "SERVICE URL";
    case SKINNY_BUTTON_UNDEFINED:
      return "UNDEFINED";
    default:
      return "UNKNOWN";
  }
}
