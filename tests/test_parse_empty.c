#include "test.h"


void test_impl()
{
  struct JsonValue *value;

  value = json_parse("");
  assert_true(value == NULL);

  value = json_parse("    ");
  assert_true(value == NULL);
}


int main()
{
  test_run(test_impl);
}

