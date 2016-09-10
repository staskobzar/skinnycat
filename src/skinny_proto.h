#ifndef __SKINNY_PROTO_H
#define __SKINNY_PROTO_H

#include <stdint.h>
#include <apr_general.h>

#define DEVICE_NAME_LEN     16
#define SKINNY_HEADER_LEN   12

struct skinny_header {
  uint32_t length;
  uint32_t version;
  uint32_t msg_id;
};

#define MSG_ID_REGISTER 0x0001
struct message_register {
  struct skinny_header header;
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

#define MSG_ID_REGISTER_ACK 0x0081
struct message_register_ack {
  struct skinny_header header;
  uint32_t keepalive_primary;
  unsigned char date_template[6];
  unsigned char padding[2];
  uint32_t keepalive_secondary;
  unsigned char max_proto_ver;
  unsigned char unknown;
  unsigned char phone_features[2];
};

#define MSG_ID_REGISTER_REJECT 0x009d
struct message_register_reject {
  struct skinny_header header;
  unsigned char error_text[32];
};

apr_size_t init_register_message (apr_pool_t *mp, struct message_register **msg);
void device_name_set (char *dst, char *name);
char *generate_device_name(void);

#endif

