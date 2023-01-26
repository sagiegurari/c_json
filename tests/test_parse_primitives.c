#include "test.h"
#include <stdio.h>


void test_impl()
{
  struct JsonValue *value;

  value = json_parse("null");
  assert_true_with_description(value != NULL, "null not detected");
  assert_true_with_description(value->type == JSON_TYPE_NULL, "null type not set");
  json_release_value(value);

  value = json_parse("true");
  assert_true_with_description(value != NULL, "true not detected");
  assert_true_with_description(value->type == JSON_TYPE_BOOLEAN, "boolean type for true not set");
  assert_true_with_description(value->value->value_boolean, "boolean value not true");
  json_release_value(value);

  value = json_parse("false");
  assert_true_with_description(value != NULL, "false not detected");
  assert_true_with_description(value->type == JSON_TYPE_BOOLEAN, "boolean type for false not set");
  assert_true_with_description(!value->value->value_boolean, "boolean value not false");
  json_release_value(value);

  value = json_parse("0");
  assert_true_with_description(value != NULL, "number 0 not detected");
  assert_true_with_description(value->type == JSON_TYPE_NUMBER, "number type for 0 not set");
  assert_num_equal(value->value->value_number, 0);
  json_release_value(value);

  value = json_parse("12345");
  assert_true_with_description(value != NULL, "number 12345 not detected");
  assert_true_with_description(value->type == JSON_TYPE_NUMBER, "number type for 12345 not set");
  assert_num_equal(value->value->value_number, 12345);
  json_release_value(value);

  value = json_parse("123.45");
  assert_true_with_description(value != NULL, "number 123.45 not detected");
  assert_true_with_description(value->type == JSON_TYPE_NUMBER, "number type for 123.45 not set");
  assert_num_equal(value->value->value_number, 123.45);
  json_release_value(value);
}


int main()
{
  test_run(test_impl);
}

