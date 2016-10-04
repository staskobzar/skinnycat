#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdio.h>

#include "../src/skinny_msg.h"
#include <stdlib.h>
#include <arpa/inet.h>

/*
 * Raw packets for testing.
 */
/* RAW ARP BROADCAST packet to test invalid packet */
const char invalid_packet[] = { 0x00,0x01,0x08,0x00,0x06,0x04,0x00,0x01,0x00,0x16,
  0xe3,0x19,0x27,0x15,0xc0,0xa8,0x01,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0xc0,0xa8,
  0x01,0x02};

/* RAW KEEPALIVE */
const char raw_keepalive_packet[] = {0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00};

/* RAW KEEPALIVE ACK */
const char raw_keepalive_ack_packet[] = { 0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x01,0x00,0x00};

/* RAW REGISTER */
const char raw_reg_packet[] = {  0x44,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x01,0x00,0x00,0x00,0x53,0x45,0x50,0x30, 0x30,0x30,0x44,0x36,0x35,0x33,0x33,0x41,
  0x38,0x42,0x37,0x00,0x00,0x00,0x00,0x00, 0x01,0x00,0x00,0x00,0x0a,0xe6,0x08,0x65,
  0x08,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x0b,0x00,0x60,0x85,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00};

/* RAW REGISTER ACK */
const char raw_reg_ack_packet[] = {   0x18,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x81,
  0x00,0x00,0x00,0x18,0x00,0x00,0x00,0x44, 0x2d,0x4d,0x2d,0x59,0x59,0x00,0x00,0x18,
  0x00,0x00,0x00,0x0b,0x20,0xf1,0xff};

/* RAW REGISTER REJECT */
const char raw_reg_reject_packet[] = {0x25,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x9d,
  0x00,0x00,0x00,0x4e,0x6f,0x20,0x41,0x75, 0x74,0x68,0x6f,0x72,0x69,0x74,0x79,0x3a,
  0x20,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00};

/* RAW REGISTER REJECT */
const char raw_reset_packet[] = {0x08,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x9f,0x00,
  0x00,0x00,0x02,0x00,0x00,0x00};

/* END: Raw packets for testing. */

static void test_unpack_invalid (void **state)
{
  (void)*state;
  struct skinny_message msg;
  assert_int_equal(unpack_message(invalid_packet, &msg), -1);
}

static void test_unpack_keepalive (void **state)
{
  (void)*state;
  struct skinny_message msg;
  assert_int_equal (unpack_message(raw_keepalive_packet, &msg), MID_KEEPALIVE);
}

static void test_unpack_keepalive_ack (void **state)
{
  (void)*state;
  struct skinny_message msg;
  assert_int_equal (unpack_message(raw_keepalive_ack_packet, &msg), MID_KEEPALIVE_ACK);
}

static void test_unpack_reg (void **state)
{
  (void)*state;
  struct skinny_message *msg = (struct skinny_message *) malloc (sizeof(struct skinny_message));
  skinny_msg_id mid = unpack_message (raw_reg_packet, msg);
  assert_int_equal (mid, MID_REGISTER);
  assert_int_equal (msg->header->length, 68);
  assert_int_equal (msg->data->reg.dev_type, 8);
  assert_string_equal (msg->data->reg.device_name, "SEP000D6533A8B7");

  free(msg);
}

static void test_unpack_reg_ack (void **state)
{
  (void)*state;
  struct skinny_message *msg = (struct skinny_message *) malloc (sizeof(struct skinny_message));
  skinny_msg_id mid = unpack_message (raw_reg_ack_packet, msg);
  assert_int_equal (mid, MID_REGISTER_ACK);
  assert_int_equal (msg->header->length, 24);
  assert_int_equal (msg->data->reg_ack.keepalive_primary, 24);
  assert_string_equal (msg->data->reg_ack.date_template, "D-M-YY");

  free(msg);
}

static void test_unpack_reg_reject (void **state)
{
  (void)*state;
  struct skinny_message *msg = (struct skinny_message *) malloc (sizeof(struct skinny_message));
  skinny_msg_id mid = unpack_message (raw_reg_reject_packet, msg);
  assert_int_equal (mid, MID_REGISTER_REJECT);
  assert_int_equal (msg->header->length, 37);
  assert_string_equal (msg->data->reg_reject.error_text, "No Authority: ");

  free(msg);
}

static void test_unpack_reset (void **state)
{
  (void)*state;
  struct skinny_message *msg = (struct skinny_message *) malloc (sizeof(struct skinny_message));
  skinny_msg_id mid = unpack_message (raw_reset_packet, msg);
  assert_int_equal (mid, MID_RESET);
  assert_int_equal (msg->header->length, 8);
  assert_int_equal (msg->data->reset.reset_type, SKINNY_DEV_RESTART);

  free(msg);
}

int main(int argc, const char *argv[])
{
  printf("Testing Skinny message\n");
  const struct CMUnitTest tests[] = {
    cmocka_unit_test (test_unpack_invalid),
    cmocka_unit_test (test_unpack_keepalive),
    cmocka_unit_test (test_unpack_keepalive_ack),
    cmocka_unit_test (test_unpack_reg),
    cmocka_unit_test (test_unpack_reg_ack),
    cmocka_unit_test (test_unpack_reg_reject),
    cmocka_unit_test (test_unpack_reset),
  };
  return cmocka_run_group_tests(tests, NULL, NULL);
}
