#include "test.h"


void test_impl()
{
  struct JsonValue *value;
  char             *string;

  value  = json_parse("{}");
  string = json_stringify(value);
  assert_string_equal(string, "{}");
  free(string);
  string = json_stringify_with_options(value, true, 2);
  assert_string_equal(string, "{\n"
                      "}");
  free(string);
  json_release_value(value);

  value  = json_parse("{\"key1\":1,\"key2\":\"test\",\"key3\":false,\"key4\":true,\"key5\":null,\"key6\":12.5,\"key7\":[true,false,[true, {\"sub\":1,\"a\":[true]}, 1]]]");
  string = json_stringify(value);
  assert_string_equal(string, "{\"key1\":1.000000,\"key2\":\"test\",\"key3\":false,\"key4\":true,\"key5\":null,\"key6\":12.500000,\"key7\":[true,false,[true,{\"a\":[true],\"sub\":1.000000},1.000000]]}");
  free(string);
  string = json_stringify_with_options(value, true, 2);
  assert_string_equal(string, "{\n"
                      "  \"key1\": 1.000000,\n"
                      "  \"key2\": \"test\",\n"
                      "  \"key3\": false,\n"
                      "  \"key4\": true,\n"
                      "  \"key5\": null,\n"
                      "  \"key6\": 12.500000,\n"
                      "  \"key7\": [\n"
                      "    true,\n"
                      "    false,\n"
                      "    [\n"
                      "      true,\n"
                      "      {\n"
                      "        \"a\": [\n"
                      "          true\n"
                      "        ],\n"
                      "        \"sub\": 1.000000\n"
                      "      },\n"
                      "      1.000000\n"
                      "    ]\n"
                      "  ]\n"
                      "}");
  free(string);
  json_release_value(value);
} /* test_impl */


int main()
{
  test_run(test_impl);
}

