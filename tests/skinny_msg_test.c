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

/* RAW SET LAMP */
const char raw_set_lamp[] = {0x10,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x86,0x00,
  0x00,0x00,0x09,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x02,0x00,0x00,0x00};
/* END: Raw packets for testing. */

/*
 * Setup and teardown for unpack message tests
 */
static int setup_unpack_msg(void **state) {
  struct skinny_message *msg = (struct skinny_message *) malloc (sizeof(struct skinny_message));
  if (msg == NULL) return -1;
  *state = msg;

  return 0;
}

static int teardown_unpack_msg(void **state) {
  free (*state);
  return 0;
}

/*
 * Setup and teardown for create message tests
 */
static int setup_create_msg(void **state) {
  apr_pool_t *mp;

  apr_initialize();
  apr_pool_create(&mp, NULL);

  *state = mp;

  return 0;
}

static int teardown_create_msg(void **state) {
  apr_pool_destroy(*state);
  apr_terminate();
  return 0;
}


/* ===================== */
/* unpack messages tests */
/* ===================== */
static void test_unpack_invalid (void **state)
{
  (void)*state;
  struct skinny_message msg;
  assert_int_equal(unpack_message(invalid_packet, &msg), MID_INVALID);
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
  struct skinny_message *msg = *state;
  skinny_msg_id mid = unpack_message (raw_reg_packet, msg);
  assert_int_equal (mid, MID_REGISTER);
  assert_int_equal (msg->header->length, 68);
  assert_int_equal (msg->data->reg.dev_type, 8);
  assert_string_equal (msg->data->reg.device_name, "SEP000D6533A8B7");
}

static void test_unpack_reg_ack (void **state)
{
  struct skinny_message *msg = *state;
  skinny_msg_id mid = unpack_message (raw_reg_ack_packet, msg);
  assert_int_equal (mid, MID_REGISTER_ACK);
  assert_int_equal (msg->header->length, 24);
  assert_int_equal (msg->data->reg_ack.keepalive_primary, 24);
  assert_string_equal (msg->data->reg_ack.date_template, "D-M-YY");
}

static void test_unpack_reg_reject (void **state)
{
  struct skinny_message *msg = *state;
  skinny_msg_id mid = unpack_message (raw_reg_reject_packet, msg);
  assert_int_equal (mid, MID_REGISTER_REJECT);
  assert_int_equal (msg->header->length, 37);
  assert_string_equal (msg->data->reg_reject.error_text, "No Authority: ");
}

static void test_unpack_reset (void **state)
{
  struct skinny_message *msg = *state;
  skinny_msg_id mid = unpack_message (raw_reset_packet, msg);
  assert_int_equal (mid, MID_RESET);
  assert_int_equal (msg->header->length, 8);
  assert_int_equal (msg->data->reset.reset_type, SKINNY_DEV_RESTART);
}

static void test_unpack_set_lamp (void **state)
{
  struct skinny_message *msg = *state;
  skinny_msg_id mid = unpack_message (raw_set_lamp, msg);
  assert_int_equal (mid, MID_SETLAMP);
  assert_int_equal (msg->header->length, 16);
  assert_int_equal (msg->data->setlamp.stimulus, SKINNY_BUTTON_LINE);
  assert_int_equal (msg->data->setlamp.lampMode, SKINNY_LAMP_ON);
}

/* ===================== */
/* create messages tests */
/* ===================== */
static void test_create_meg_register (void **state)
{
  apr_pool_t *mp = *state;
  struct skinny_message *reg;
  skinny_msg_id mid;
  char *buf;
  unsigned long ip_addr = 1979820224;
  skinnycat_opts opts = { .mac = "1234BEEF4DAD" };

  reg = (struct skinny_message*) apr_palloc (mp, sizeof(struct skinny_message));

  create_msg_register (mp, &buf, &opts, ip_addr);
  mid = unpack_message (buf, reg);
  assert_int_equal (mid, MID_REGISTER);
  assert_int_equal (reg->header->msg_id, MID_REGISTER);
  assert_string_equal (reg->data->reg.device_name, "SEP1234BEEF4DAD");
  assert_int_equal (reg->data->reg.ip_addr, ip_addr);

}

int main(int argc, const char *argv[])
{
  printf("Testing Skinny message\n");
  const struct CMUnitTest tests[] = {
    cmocka_unit_test (test_unpack_invalid),
    cmocka_unit_test (test_unpack_keepalive),
    cmocka_unit_test (test_unpack_keepalive_ack),
    cmocka_unit_test_setup_teardown (test_unpack_reg,         setup_unpack_msg, teardown_unpack_msg),
    cmocka_unit_test_setup_teardown (test_unpack_reg_ack,     setup_unpack_msg, teardown_unpack_msg),
    cmocka_unit_test_setup_teardown (test_unpack_set_lamp,    setup_unpack_msg, teardown_unpack_msg),
    cmocka_unit_test_setup_teardown (test_unpack_reg_reject,  setup_unpack_msg, teardown_unpack_msg),
    cmocka_unit_test_setup_teardown (test_unpack_reset,       setup_unpack_msg, teardown_unpack_msg),
    cmocka_unit_test_setup_teardown (test_create_meg_register,setup_create_msg, teardown_create_msg),
  };
  return cmocka_run_group_tests_name("SKINNY message", tests, NULL, NULL);
}
