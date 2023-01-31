#include "test.h"
#include <stdio.h>


void test_impl()
{
  struct JsonValue *value;

  value = json_parse("{}");
  assert_true_with_description(value != NULL, "empty object not detected");
  assert_true_with_description(value->type == JSON_TYPE_OBJECT, "object type not set");
  assert_size_equal(hashtable_size(value->value->object), 0);
  json_release_value(value);

  value = json_parse("{   }");
  assert_true_with_description(value != NULL, "empty object not detected");
  assert_true_with_description(value->type == JSON_TYPE_OBJECT, "object type not set");
  assert_size_equal(hashtable_size(value->value->object), 0);
  json_release_value(value);
} /* test_impl */


int main()
{
  test_run(test_impl);
}

