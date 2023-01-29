#include "test.h"
#include <stdio.h>


void test_impl()
{
  struct JsonValue *value;

  value = json_parse("[]");
  assert_true_with_description(value != NULL, "empty array not detected");
  assert_true_with_description(value->type == JSON_TYPE_ARRAY, "array type not set");
  assert_size_equal(json_array_size(value->value->array), 0);
  json_release_value(value);

  value = json_parse("[   ]");
  assert_true_with_description(value != NULL, "empty array not detected");
  assert_true_with_description(value->type == JSON_TYPE_ARRAY, "array type not set");
  assert_size_equal(json_array_size(value->value->array), 0);
  json_release_value(value);

  value = json_parse("[1]");
  assert_true_with_description(value != NULL, "number array not detected");
  assert_true_with_description(value->type == JSON_TYPE_ARRAY, "array type not set");
  assert_size_equal(json_array_size(value->value->array), 1);
  json_release_value(value);
} /* test_impl */


int main()
{
  test_run(test_impl);
}

