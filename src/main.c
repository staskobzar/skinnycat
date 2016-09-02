#include <stdio.h>
#include <apr_general.h>
#include <apr_network_io.h>

#define REMOTE_HOST   "192.168.122.1"
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
  /*
  struct keep_alive {
    uint32_t length;
    uint32_t version;
    uint32_t type;
  };
  struct keep_alive buf = {
    4, 0, 0
  };*/
  char buf[] = {0x44,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x53,0x45,0x50,0x30,0x30,0x30,0x44,0x36,0x35,0x33,0x33,0x41,0x38,0x42,0x37,0x00,0x00,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x0a,0xe6,0x08,0x65,0x08,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0b,0x00,0x60,0x85,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
  len = sizeof(buf);
  printf ("--> KeepAlive [%zu]\n",len);
  rv = apr_socket_send(s, buf, &len);
  ISOK(rv, "Failed send StationRegister.");

  char port[] = {0x08,0x00,0x00,0x00,0x16,0x00,0x00,0x00,0x02,0x00,0x00,0x00,0xac,0x0d,0x00,0x00};
    //{0x04,0x00,0x00,0x00,0x11,0x00,0x00,0x00,0x9b,0x00,0x00,0x00};
  len = sizeof(port);
  printf ("--> StationPort [%zu]\n",len);
  rv = apr_socket_send(s, port, &len);
  ISOK(rv, "Failed send StationRegister.");

  while(1){
    char mbuf[2056];
    char *p;
    p  = mbuf;
    apr_size_t len = sizeof(mbuf);
    apr_status_t rv = apr_socket_recv(s, p, &len);
    if (rv == APR_EOF || len == 0) {
      printf ("<-- Received respons [%zu] %.*s.\n", len, (int)len, p);
      break;
    }
  }


  apr_socket_close(s);
  apr_pool_destroy(mp);
  apr_terminate();

  return 0;
}
