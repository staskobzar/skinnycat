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

/* RAW REGISTER */
const char raw_ipport[] = { 0x08,0x00,0x00,0x00,0x16,0x00,0x00,0x00,0x02,0x00,0x00,
  0x00,0xac,0x0d,0x00,0x00};

/* RAW CAPABILITIES RES */
const char raw_cap_res[] = {  0x88, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x10,0x00,0x00,0x00,0x08,0x00,0x00,0x00, 0x19,0x00,0x00,0x00,0x78,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 0x00,0x04,0x00,0x00,0x00,0x28,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x02,0x00,0x00,0x00,0x28,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x0f,0x00,0x00,0x00,
  0x3c,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x10,0x00,0x00,
  0x00,0x3c,0x00,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x0b,0x00,
  0x00,0x00,0x3c,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00,0x0c,
  0x00,0x00,0x00,0x3c,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x01,0x01,0x00,0x00,0x04,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00 };

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

/* RAW CAPABILITIES REQ */
const char raw_capabilities_req[] = {0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x9b,0x00,
  0x00,0x00};

/* RAW TIME DATE REQUEST */
const char raw_timedate_req[] = {0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0d,0x00,0x00,0x00};

/* RAW BUTTON TEMPLATE REQUEST */
const char raw_btn_tmpl_req[] = {0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0e,0x00,0x00,0x00};

/* RAW BUTTON TEMPLATE */
const char raw_button_template[] = {0x80,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00, 0x97,0x00,0x00,0x00, 0x00,0x00,0x00,0x00, 0x02,0x00,0x00,0x00,
  0x02,0x00,0x00,0x00, 0x01,0x09,0x01,0x02, 0x00,0xff,0x00,0xff, 0x00,0xff,0x00,0xff,
  0x00,0xff,0x00,0xff, 0x00,0xff,0x00,0xff, 0x00,0xff,0x00,0xff, 0x00,0xff,0x00,0xff,
  0x00,0xff,0x00,0xff, 0x00,0xff,0x00,0xff, 0x00,0xff,0x00,0xff, 0x00,0xff,0x00,0xff,
  0x00,0xff,0x00,0xff, 0x00,0xff,0x00,0xff, 0x00,0xff,0x00,0xff, 0x00,0xff,0x00,0xff,
  0x00,0xff,0x00,0xff, 0x00,0xff,0x00,0xff, 0x00,0xff,0x00,0xff, 0x00,0xff,0x00,0xff,
  0x00,0xff,0x00,0xff, 0x00,0xff,0x00,0xff, 0x00,0xff,0x00,0xff, 0x00,0xff,0x00,0xff,
  0x00,0xff,0x00,0xff, 0x00,0xff,0x00,0xff, 0x00,0xff,0x00,0xff, 0x00,0xff,0x00,0xff,
  0x00,0xff,0x00,0xff};

/* RAW DATETIME DEFINE */
const char raw_define_datetime[] = { 0x28,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,
  0x94,0x00,0x00,0x00, 0xe0,0x07,0x00,0x00, 0x09,0x00,0x00,0x00, 0x04,0x00,0x00,0x00,
  0x01,0x00,0x00,0x00, 0x0e,0x00,0x00,0x00, 0x34,0x00,0x00,0x00, 0x22,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00, 0xf2,0x78,0xc8,0x57};

/* RAW CLEAR PROMPT */
const char raw_clear_prompt[] = { 0x0c,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,
  0x13,0x01,0x00,0x00, 0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00 };

/* RAW CLEAR PROMPT */
const char raw_dyn_prompt[] = { 0x14,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,
  0x45,0x01,0x00,0x00, 0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,
  0x80,0x13,0x00,0x00 };

/* RAW DISPLAY NOTIFY */
const char raw_display_notify[] = { 0x14,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,
  0x43,0x01,0x00,0x00, 0x05,0x00,0x00,0x00, 0x41,0x73,0x74,0x65, 0x72,0x69,0x73,0x6b,
  0x20,0x80,0x18,0x00 };

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

static void test_unpack_cap_res (void **state)
{
  struct skinny_message *msg = *state;
  skinny_msg_id mid = unpack_message (raw_cap_res, msg);
  assert_int_equal (mid, MID_CAPABILITIES_RES);
  assert_int_equal (msg->header->length, 136);
  assert_int_equal (msg->data->cap_res.cap_count, 8);
}

static void test_unpack_ipport (void **state)
{
  struct skinny_message *msg = *state;
  skinny_msg_id mid = unpack_message (raw_ipport, msg);
  assert_int_equal (mid, MID_IPPORT);
  assert_int_equal (msg->header->length, 8);
  assert_int_equal (msg->data->ipport.port, 3500);
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

static void test_unpack_capabilities_req (void **state)
{
  (void)*state;
  struct skinny_message msg;
  assert_int_equal(unpack_message(raw_capabilities_req, &msg), MID_CAPABILITIES_REQ);
}

static void test_unpack_datetime_req (void **state)
{
  (void)*state;
  struct skinny_message msg;
  assert_int_equal(unpack_message(raw_timedate_req, &msg), MID_DATETIME_REQ);
}

static void test_unpack_btn_tmpl_req (void **state)
{
  (void)*state;
  struct skinny_message msg;
  assert_int_equal(unpack_message(raw_btn_tmpl_req, &msg), MID_BUTTON_TMPL_REQ);
}

static void test_unpack_def_datetime (void **state)
{
  struct skinny_message *msg = *state;
  assert_int_equal(unpack_message(raw_define_datetime, msg), MID_DEFINE_DATETIME);
  assert_int_equal(msg->data->dtime.year, 2016);
  assert_int_equal(msg->data->dtime.hour, 14);
  assert_int_equal(msg->data->dtime.sec, 34);
  assert_int_equal(msg->data->dtime.systime, 1472755954);
}

static void test_unpack_btn_tmpl (void **state)
{
  struct skinny_message *msg = *state;
  assert_int_equal(unpack_message(raw_button_template, msg), MID_BUTTON_TMPL);
  assert_int_equal(msg->data->btn_tmpl.btn_count, 2);
  assert_int_equal(msg->data->btn_tmpl.btn[0].definition, SKINNY_BUTTON_LINE);
  assert_int_equal(msg->data->btn_tmpl.btn[1].definition, SKINNY_BUTTON_SPEED_DIAL);
}

static void test_unpack_display_notify (void **state)
{
  struct skinny_message *msg = *state;
  assert_int_equal(unpack_message(raw_display_notify, msg), MID_DISPLAY_NOTIFY);
  assert_int_equal(msg->data->notify.timeout, 5);
}

static void test_unpack_clear_prompt (void **state)
{
  (void)*state;
  struct skinny_message msg;
  assert_int_equal(unpack_message(raw_clear_prompt, &msg), MID_CLEAR_PROMPT);
}

static void test_unpack_display_dyn_prompt (void **state)
{
  (void)*state;
  struct skinny_message msg;
  assert_int_equal(unpack_message(raw_dyn_prompt, &msg), MID_DISPLAY_DYN_PROMPT);
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

static void test_create_meg_cap_res (void **state)
{
  apr_pool_t *mp = *state;
  struct skinny_message *cap;
  skinny_msg_id mid;
  char *buf;

  cap = (struct skinny_message*) apr_palloc (mp, sizeof(struct skinny_message));

  create_msg_cap_res (mp, &buf);
  mid = unpack_message (buf, cap);
  assert_int_equal (mid, MID_CAPABILITIES_RES);
  assert_int_equal (cap->header->msg_id, MID_CAPABILITIES_RES);
  assert_int_equal (cap->data->cap_res.cap_count, 3);
}

/* ===================== */
/* utility methods       */
/* ===================== */
static void test_lamp_mode_to_str (void **state)
{
  (void)*state;
  assert_string_equal (lamp_mode_to_str(SKINNY_LAMP_ON), "ON");
  assert_string_equal (lamp_mode_to_str(SKINNY_LAMP_FLASH), "FLASH");
  assert_string_equal (lamp_mode_to_str(SKINNY_LAMP_BLINK), "BLINK");
}

static void test_btn_def_to_str (void **state)
{
  (void)*state;
  assert_string_equal (btn_def_to_str(SKINNY_BUTTON_LINE), "LINE");
  assert_string_equal (btn_def_to_str(SKINNY_BUTTON_SPEED_DIAL), "SPEED DIAL");
  assert_string_equal (btn_def_to_str(SKINNY_BUTTON_VOICEMAIL), "VOICEMAIL");
}

/* ===================== */
/* MAIN proc             */
/* ===================== */
int main(int argc, const char *argv[])
{
  printf("Testing Skinny message\n");
  const struct CMUnitTest tests[] = {
    cmocka_unit_test (test_unpack_invalid),
    cmocka_unit_test (test_unpack_keepalive),
    cmocka_unit_test (test_unpack_keepalive_ack),
    cmocka_unit_test_setup_teardown (test_unpack_reg,         setup_unpack_msg, teardown_unpack_msg),
    cmocka_unit_test_setup_teardown (test_unpack_cap_res,     setup_unpack_msg, teardown_unpack_msg),
    cmocka_unit_test_setup_teardown (test_unpack_reg_ack,     setup_unpack_msg, teardown_unpack_msg),
    cmocka_unit_test_setup_teardown (test_unpack_set_lamp,    setup_unpack_msg, teardown_unpack_msg),
    cmocka_unit_test_setup_teardown (test_unpack_reg_reject,  setup_unpack_msg, teardown_unpack_msg),
    cmocka_unit_test_setup_teardown (test_unpack_reset,       setup_unpack_msg, teardown_unpack_msg),
    cmocka_unit_test_setup_teardown (test_create_meg_register,setup_create_msg, teardown_create_msg),
    cmocka_unit_test (test_lamp_mode_to_str),
    cmocka_unit_test (test_btn_def_to_str),
    cmocka_unit_test (test_unpack_capabilities_req),
    cmocka_unit_test_setup_teardown (test_create_meg_cap_res, setup_create_msg, teardown_create_msg),
    cmocka_unit_test (test_unpack_btn_tmpl_req),
    cmocka_unit_test_setup_teardown (test_unpack_btn_tmpl,    setup_unpack_msg, teardown_unpack_msg),
    cmocka_unit_test_setup_teardown (test_unpack_ipport,      setup_unpack_msg, teardown_unpack_msg),
    cmocka_unit_test (test_unpack_datetime_req),
    cmocka_unit_test_setup_teardown (test_unpack_def_datetime, setup_unpack_msg, teardown_unpack_msg),
    cmocka_unit_test (test_unpack_clear_prompt),
    cmocka_unit_test_setup_teardown (test_unpack_display_dyn_prompt, setup_unpack_msg, teardown_unpack_msg),
    cmocka_unit_test_setup_teardown (test_unpack_display_notify, setup_unpack_msg, teardown_unpack_msg),
  };
  return cmocka_run_group_tests_name("SKINNY message", tests, NULL, NULL);
}
