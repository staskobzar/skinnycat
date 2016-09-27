#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdio.h>

#include "../src/skinny_msg.h"

static void test_int_ok (void **state)
{
  (void)*state;
  assert_int_equal (foo(), 1);
}

static void test_int_sq (void **state)
{
  (void)*state;
  assert_int_equal (bar(5), 5 * 5);
}

int main(int argc, const char *argv[])
{
  printf("Testing Skinny message\n");
  const struct CMUnitTest tests[] = {
    cmocka_unit_test (test_int_ok),
    cmocka_unit_test (test_int_sq),
  };
  return cmocka_run_group_tests(tests, NULL, NULL);
  return 0;
}
