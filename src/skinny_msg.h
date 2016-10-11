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
 * @file skinny_msg.h
 * @brief Skinny protocol messages functions
 *
 * @author Stas Kobzar <stas@modulis.ca>
 */
#ifndef __SKINNY_MSG_H
#define __SKINNY_MSG_H

#include <stdint.h>
#include <apr_general.h>
#include "skinnycat_opts.h"

#define DEVICE_NAME_LEN     16
#define SKINNY_HEADER_LEN   12

/**
 * Reset types
 */
#define SKINNY_DEV_RESET         1
#define SKINNY_DEV_RESTART       2
#define SKINNY_DEV_RELOAD_CONFIG 3
#define SKINNY_MAX_PACK_LEN      2056

/**
 * Skinny messages IDs
 */
typedef enum {
  MID_INVALID             = -1,
  MID_KEEPALIVE           = 0x0000,
  MID_REGISTER            = 0x0001,
  MID_IPPORT              = 0x0002,
  MID_REGISTER_ACK        = 0x0081,
  MID_SETLAMP             = 0x0086,
  MID_REGISTER_REJECT     = 0x009d,
  MID_RESET               = 0x009f,
  MID_KEEPALIVE_ACK       = 0x0100,
} skinny_msg_id;

enum skinny_lamp_mode{
  SKINNY_LAMP_OFF   = 1,
  SKINNY_LAMP_ON    = 2,
  SKINNY_LAMP_WINK  = 3,
  SKINNY_LAMP_FLASH = 4,
  SKINNY_LAMP_BLINK = 5,
};

enum skinny_button_definition {
  SKINNY_BUTTON_UNKNOWN             = 0x00,
  SKINNY_BUTTON_LAST_NUMBER_REDIAL  = 0x01,
  SKINNY_BUTTON_SPEED_DIAL          = 0x02,
  SKINNY_BUTTON_HOLD                = 0x03,
  SKINNY_BUTTON_TRANSFER            = 0x04,
  SKINNY_BUTTON_FORWARDALL          = 0x05,
  SKINNY_BUTTON_LINE                = 0x09,
  SKINNY_BUTTON_VOICEMAIL           = 0x0F,
  SKINNY_BUTTON_PRIVACY             = 0x13,
  SKINNY_BUTTON_SERVICE_URL         = 0x14,
  SKINNY_BUTTON_UNDEFINED           = 0xFF,
};

struct skinny_header {
  uint32_t length;
  uint32_t version;
  uint32_t msg_id;
};

struct message_register {
  char device_name[DEVICE_NAME_LEN];
  uint32_t reserved;
  uint32_t instance;
  uint32_t ip_addr;
  uint32_t dev_type;
  uint32_t max_concurrent_streams;
  uint32_t active_rtp_streams;
  unsigned char proto_ver;
  unsigned char unknown;
  unsigned char phone_features[2];
  uint32_t max_conferences;
};

struct message_ipport {
  uint32_t port;
};

struct message_register_ack {
  uint32_t keepalive_primary;
  unsigned char date_template[6];
  unsigned char padding[2];
  uint32_t keepalive_secondary;
  unsigned char max_proto_ver;
  unsigned char unknown;
  unsigned char phone_features[2];
};

struct message_setlamp {
  uint32_t stimulus;
  uint32_t instance;
  uint32_t lampMode;
};

struct message_register_reject {
  unsigned char error_text[32];
};

struct message_reset {
  uint32_t reset_type;
};

union skinny_message_data {
  struct message_register reg;
  struct message_register_ack reg_ack;
  struct message_setlamp setlamp;
  struct message_register_reject reg_reject;
  struct message_reset reset;
};

struct skinny_message {
  struct skinny_header *header;
  union skinny_message_data *data;
};

/**
 * Skinny messages list element.
 * Used to extract and create Skinny messages.
 */
struct skinny_msg_list {
  /** Skinny message id */
  skinny_msg_id id;
  /** Skinny message extract function pointer. Converts received packet to structure. */
  skinny_msg_id (*unpack)(const char *packet, struct skinny_message *msg);
};

/**
 * Unpack skinny message.
 * @param packet
 * @param message   Skinny message struct
 */
skinny_msg_id unpack_message (const char *packet, struct skinny_message *msg);

/**
 * Extract KEEPALIVE message from the skinny packet.
 * @param packet    Raw packet
 * @param message   Skinny message structure
 * @return Packet identifier
 */
skinny_msg_id unpack_keepalive (const char *packet, struct skinny_message *msg);

/**
 * Extract REGISTER message from the skinny packet.
 * @param packet    Raw packet
 * @param message   Skinny message structure
 * @return Packet identifier
 */
skinny_msg_id unpack_register (const char *packet, struct skinny_message *msg);

/**
 * Extract REGISTER ACK message from the skinny packet.
 * @param packet    Raw packet
 * @param message   Skinny message structure
 * @return Packet identifier
 */
skinny_msg_id unpack_register_ack (const char *packet, struct skinny_message *msg);

/**
 * Extract SET LAMP message from the skinny packet.
 * @param packet    Raw packet
 * @param message   Skinny message structure
 * @return Packet identifier
 */
skinny_msg_id unpack_setlamp (const char *packet, struct skinny_message *msg);

/**
 * Extract REGISTER REJECT message from the skinny packet.
 * @param packet    Raw packet
 * @param message   Skinny message structure
 * @return Packet identifier
 */
skinny_msg_id unpack_register_reject (const char *packet, struct skinny_message *msg);

/**
 * Extract RESET message from the skinny packet.
 * @param packet    Raw packet
 * @param message   Skinny message structure
 * @return Packet identifier
 */
skinny_msg_id unpack_reset (const char *packet, struct skinny_message *msg);

/**
 * Extract KEEPALIVE ACK message from the skinny packet.
 * @param packet    Raw packet
 * @param message   Skinny message structure
 * @return Packet identifier
 */
skinny_msg_id unpack_keepalive_ack (const char *packet, struct skinny_message *msg);

/**
 * Create Skinny register message as char buffer ready to send with socket.
 * @param mp      Memory pool
 * @param buf     Register packet buffer
 * @param opts    Application options
 * @param ip_addr IP address
 * @return Packet size
 */
apr_size_t create_msg_register (apr_pool_t *mp, char **buf, skinnycat_opts *opts, unsigned long ip_addr);

#endif
