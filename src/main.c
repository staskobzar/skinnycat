#include <stdio.h>
#include <apr_general.h>
#include <apr_network_io.h>

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
  apr_size_t len;
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


  /**
   * KeepAlive
   */
  struct skinny_hdr_s {
    uint32_t length;
    uint32_t version;
    uint32_t msg_id;
  };
  typedef struct skinny_hdr_s SkinnyHeader;

  struct message_register{
    char device_name[16];
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
    unsigned char padding[16];
  };
  typedef struct message_register RegisterMsg;

  struct register_packet {
    SkinnyHeader header;
    RegisterMsg data;
  };
  typedef struct register_packet RegisterPacket;

  struct register_ack {
    uint32_t keepalive_interval_1;
    unsigned char date_tmpl[6];
    unsigned char alignement[2];
    uint32_t keepalive_interval_2;
    unsigned char proto_ver;
    unsigned char unknown;
    unsigned char features[2];
  };
  typedef struct register_ack RegisterAck;

  RegisterPacket reg = {
    {68, 0, 1},
    {"SEP000D6533A8B7", 0, 1, 0xc0a80176, 8, 0, 0, 0x0b, 0, {0x60, 0x85}, 0,
      {
        0x00,0x00,0x00,0x00,  0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x00,  0x00,0x00,0x00,0x00
      }}
  };
  printf("reg.header.length=%zu\n", reg.header.length);
  printf("reg.data.device_name=%.*s\n", 16, reg.data.device_name);
  printf("reg.data.proto_ver=%d\n", reg.data.proto_ver);

  char buf[] = {  0x44,0x00,0x00,0x00, // 4: Data length: 68
                  0x00,0x00,0x00,0x00, // 4: Header version: Basic (0x00000000)
                  0x01,0x00,0x00,0x00, // 4: Message ID: Register (1)
                  // 16: DeviceName: SEP000D6533A8B7
                  0x53,0x45,0x50,0x30,  0x30,0x30,0x44,0x36,
                  0x35,0x33,0x33,0x41,  0x38,0x42,0x37,0x00,
                  0x00,0x00,0x00,0x00, // 4: reserved_for_future_use: 0
                  0x01,0x00,0x00,0x00, // 4: instance: 1
                  0xc0,0xa8,0x01,0x76, // 4: stationIpAddr: 192.168.1.118
                  0x08,0x00,0x00,0x00, // 4: Device Type: StationTelecasterBus (0x00000008)
                  0x00,0x00,0x00,0x00, // 4: Maximum Number of Concurrent RTP Streams: 0
                  0x00,0x00,0x00,0x00, // 4: Active RTP Streams: 0
                  0x0b,                // 1: Protocol Version: 11
                  0x00,                // 1: unknown: 0
                  0x60,0x85,           // 2: PhoneFeatures
                  0x00,0x00,0x00,0x00, // 4: Maximum Number of Concurrent Conferences: 0
                  // 4: padding
                  0x00,0x00,0x00,0x00,  0x00,0x00,0x00,0x00,
                  0x00,0x00,0x00,0x00,  0x00,0x00,0x00,0x00
  };
  len = sizeof(buf);
  printf ("--> StationRegister [%zu] register_packet len = %zu\n",len, sizeof(struct register_packet));
  rv = apr_socket_send(s, buf, &len);
  ISOK(rv, "Failed send StationRegister.");

  char port[] = { 0x08,0x00,0x00,0x00, 0x16,0x00,0x00,0x00,
                  0x02,0x00,0x00,0x00, 0xac,0x0d,0x00,0x00};
    //{0x04,0x00,0x00,0x00,0x11,0x00,0x00,0x00,0x9b,0x00,0x00,0x00};
  len = sizeof(port);
  printf ("--> StationPort [%zu]\n",len);
  rv = apr_socket_send(s, port, &len);
  ISOK(rv, "Failed send StationRegister.");

  if(memcmp((char*)&reg, &buf, len) == 0)
    printf("char array and structure are identical!\n");


  while(1){
    char mbuf[2056];
    char *p;
    RegisterAck *ack;
    p  = mbuf;
    apr_size_t len = sizeof(mbuf);
    apr_status_t rv = apr_socket_recv(s, p, &len);
    printf ("<-- [keep] Received respons [len: %zu, rv: %d, APR_EOF = %d] %.*s.\n", len, rv, APR_EOF, (int)len, p);
    SkinnyHeader *header = (SkinnyHeader*)p;
    printf("length = %zu, msg_id = %zu\n", header->length, header->msg_id);
    if (header->msg_id == 129) {
      p += 12;
      ack = (RegisterAck *)p;
      printf("<-- ACK date template = %.*s\n", 6, ack->date_tmpl);
    }
    if (header->msg_id == 155){
      printf("<-- Capibilities Request \n");
      char cap_res[] = {0x88,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x10,0x00,0x00,0x00,0x08,0x00,0x00,0x00,0x19,0x00,0x00,0x00,0x78,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x00,0x00,0x00,0x28,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x02,0x00,0x00,0x00,0x28,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0f,0x00,0x00,0x00,0x3c,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x10,0x00,0x00,0x00,0x3c,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0b,0x00,0x00,0x00,0x3c,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0c,0x00,0x00,0x00,0x3c,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x01,0x00,0x00,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
      len = sizeof(cap_res);
      printf("--> Capibilities Response \n");
      rv = apr_socket_send(s, cap_res, &len);
      break;
    }
    if (rv == APR_EOF || len == 0) {
      printf ("<-- [keep] Received respons [len: %zu, rv: %d, APR_EOF = %d] %.*s.\n", len, rv, APR_EOF, (int)len, p);
      //printf ("<-- Received respons [%zu] %.*s.\n", len, (int)len, p);
      break;
    }
  }


  apr_socket_close(s);
  apr_pool_destroy(mp);
  apr_terminate();

  return 0;
}
