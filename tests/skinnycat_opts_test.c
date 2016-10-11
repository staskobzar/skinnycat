#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdio.h>

#include "../src/skinnycat_opts.h"

static void test_action_id_for_method_register (void **state)
{
  (void)*state;
  assert_int_equal (action_id_for_method("REGISTER"), AID_REGISTER);
  assert_int_equal (action_id_for_method("register"), AID_REGISTER);
}

int main(int argc, const char *argv[])
{
  printf("Testing CLI options\n");
  const struct CMUnitTest tests[] = {
    cmocka_unit_test (test_action_id_for_method_register),
  };
  return cmocka_run_group_tests(tests, NULL, NULL);
}
