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
