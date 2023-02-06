#include "test.h"


void test_impl()
{
  struct JsonValue *value;
  char             *string;

  value  = json_parse("[]");
  string = json_stringify(value);
  assert_string_equal(string, "[]");
  free(string);
  string = json_stringify_with_options(value, true, 2);
  assert_string_equal(string, "[\n"
                      "]");
  free(string);
  json_release(value);

  value  = json_parse("[1,2,\"test\",false,true,null,12.5,[true,false,[{\"key\":\"value\"},true]]]");
  string = json_stringify(value);
  assert_string_equal(string, "[1.000000,2.000000,\"test\",false,true,null,12.500000,[true,false,[{\"key\":\"value\"},true]]]");
  free(string);
  string = json_stringify_with_options(value, true, 2);
  assert_string_equal(string, "[\n"
                      "  1.000000,\n"
                      "  2.000000,\n"
                      "  \"test\",\n"
                      "  false,\n"
                      "  true,\n"
                      "  null,\n"
                      "  12.500000,\n"
                      "  [\n"
                      "    true,\n"
                      "    false,\n"
                      "    [\n"
                      "      {\n"
                      "        \"key\": \"value\"\n"
                      "      },\n"
                      "      true\n"
                      "    ]\n"
                      "  ]\n"
                      "]");
  free(string);
  json_release(value);
} /* test_impl */


int main()
{
  test_run(test_impl);
}

