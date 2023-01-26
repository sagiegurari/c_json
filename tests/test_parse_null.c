#include "test.h"


void test_impl()
{
  struct JsonValue *value = json_parse(NULL);

  assert_true(value == NULL);
}


int main()
{
  test_run(test_impl);
}

