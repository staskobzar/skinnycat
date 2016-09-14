#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdio.h>

#include "../src/skinny_msg.h"

static void test_int_ok (void **state)
{
  (void)*state;
  assert_int_equal (5, 5);
}

static void test_int_fail (void **state)
{
  (void)*state;
  assert_int_equal (5, 55);
}

int main(int argc, const char *argv[])
{
  printf("Testing Skinny message\n");
  const struct CMUnitTest tests[] = {
    cmocka_unit_test (test_int_ok),
    cmocka_unit_test (test_int_fail),
  };
  return cmocka_run_group_tests(tests, NULL, NULL);
  return 0;
}
