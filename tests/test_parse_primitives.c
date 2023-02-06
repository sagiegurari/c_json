#include "test.h"


void test_impl()
{
  struct JsonValue *value;

  value = json_parse("null");
  assert_true_with_description(value != NULL, "null not detected");
  assert_true_with_description(value->type == JSON_TYPE_NULL, "null type not set");
  json_release(value);

  value = json_parse("true");
  assert_true_with_description(value != NULL, "true not detected");
  assert_true_with_description(value->type == JSON_TYPE_BOOLEAN, "boolean type for true not set");
  assert_true_with_description(value->value->boolean, "boolean value not true");
  json_release(value);

  value = json_parse("false");
  assert_true_with_description(value != NULL, "false not detected");
  assert_true_with_description(value->type == JSON_TYPE_BOOLEAN, "boolean type for false not set");
  assert_true_with_description(!value->value->boolean, "boolean value not false");
  json_release(value);

  value = json_parse("0");
  assert_true_with_description(value != NULL, "number 0 not detected");
  assert_true_with_description(value->type == JSON_TYPE_NUMBER, "number type for 0 not set");
  assert_num_equal(value->value->number, 0);
  json_release(value);

  value = json_parse("12345");
  assert_true_with_description(value != NULL, "number 12345 not detected");
  assert_true_with_description(value->type == JSON_TYPE_NUMBER, "number type for 12345 not set");
  assert_num_equal(value->value->number, 12345);
  json_release(value);

  value = json_parse("123.45");
  assert_true_with_description(value != NULL, "number 123.45 not detected");
  assert_true_with_description(value->type == JSON_TYPE_NUMBER, "number type for 123.45 not set");
  assert_num_equal(value->value->number, 123.45L);
  json_release(value);

  value = json_parse("-12345");
  assert_true_with_description(value != NULL, "number -12345 not detected");
  assert_true_with_description(value->type == JSON_TYPE_NUMBER, "number type for -12345 not set");
  assert_num_equal(value->value->number, -12345);
  json_release(value);

  value = json_parse("\"first line\\n\\n"
                     "second line\\tafter tab\"");
  assert_true_with_description(value != NULL, "string not detected");
  assert_true_with_description(value->type == JSON_TYPE_STRING, "string type not set");
  assert_string_equal(value->value->string, "first line\n\n"
                      "second line\tafter tab");
  json_release(value);

  value = json_parse("null2");
  assert_true_with_description(value == NULL, "bad null value");

  value = json_parse("true2");
  assert_true_with_description(value == NULL, "bad true value");

  value = json_parse("false2");
  assert_true_with_description(value == NULL, "bad false value");

  value = json_parse("12345a");
  assert_true_with_description(value == NULL, "bad number value");

  value = json_parse("123.4.5");
  assert_true_with_description(value == NULL, "bad flat value");

  value = json_parse("--12345");
  assert_true_with_description(value == NULL, "bad negative number value");

  value = json_parse("\"abc");
  assert_true_with_description(value == NULL, "bad string value");

  value = json_parse("\"abc\\q\"");
  assert_true_with_description(value == NULL, "bad string value");
} /* test_impl */


int main()
{
  test_run(test_impl);
}

