#include "test.h"


void test_impl()
{
  struct JsonValue *value;
  char             *string;

  value  = json_parse("null");
  string = json_stringify(value);
  assert_string_equal(string, "null");
  free(string);
  string = json_stringify_with_options(value, true, 2);
  assert_string_equal(string, "null");
  free(string);
  json_release_value(value);

  value  = json_parse("true");
  string = json_stringify(value);
  assert_string_equal(string, "true");
  free(string);
  string = json_stringify_with_options(value, true, 2);
  assert_string_equal(string, "true");
  free(string);
  json_release_value(value);

  value  = json_parse("false");
  string = json_stringify(value);
  assert_string_equal(string, "false");
  free(string);
  string = json_stringify_with_options(value, true, 2);
  assert_string_equal(string, "false");
  free(string);
  json_release_value(value);

  value  = json_parse("0");
  string = json_stringify(value);
  assert_string_equal(string, "0.000000");
  free(string);
  string = json_stringify_with_options(value, true, 2);
  assert_string_equal(string, "0.000000");
  free(string);
  json_release_value(value);

  value  = json_parse("12345");
  string = json_stringify(value);
  assert_string_equal(string, "12345.000000");
  free(string);
  string = json_stringify_with_options(value, true, 2);
  assert_string_equal(string, "12345.000000");
  free(string);
  json_release_value(value);

  value  = json_parse("123.45");
  string = json_stringify(value);
  assert_string_equal(string, "123.450000");
  free(string);
  string = json_stringify_with_options(value, true, 2);
  assert_string_equal(string, "123.450000");
  free(string);
  json_release_value(value);

  value  = json_parse("-12345");
  string = json_stringify(value);
  assert_string_equal(string, "-12345.000000");
  free(string);
  string = json_stringify_with_options(value, true, 2);
  assert_string_equal(string, "-12345.000000");
  free(string);
  json_release_value(value);

  value = json_parse("\"first\\\\line\\n\\n"
                     "second line\\tafter tab\"");
  string = json_stringify(value);
  assert_string_equal(string, "\"first\\\\line\\n\\n"
                      "second line\\tafter tab\"");
  free(string);
  string = json_stringify_with_options(value, true, 2);
  assert_string_equal(string, "\"first\\\\line\\n\\n"
                      "second line\\tafter tab\"");
  free(string);
  json_release_value(value);
} /* test_impl */


int main()
{
  test_run(test_impl);
}

