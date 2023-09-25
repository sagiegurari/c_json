#include "test.h"


void test_impl()
{
  char *string;

  string = json_prettify("{}", true, 2);
  assert_string_equal(string, "{\n"
                      "}");
  free(string);

  string = json_prettify("{\"key1\":1,\"key2\":\"test\",\"key3\":false,\"key4\":true,\"key5\":null,\"key6\":12.5,\"key7\":[true,false,[true, {\"sub\":1,\"a\":[true]}, {\"sub\":1,\"b\":[true,[true]]}, 1]]]", true, 2);
  assert_string_equal(string, "{\n"
                      "  \"key1\": 1,\n"
                      "  \"key2\": \"test\",\n"
                      "  \"key3\": false,\n"
                      "  \"key4\": true,\n"
                      "  \"key5\": null,\n"
                      "  \"key6\": 12.5,\n"
                      "  \"key7\": [\n"
                      "    true,\n"
                      "    false,\n"
                      "    [\n"
                      "      true,\n"
                      "      {\n"
                      "        \"a\": [\n"
                      "          true\n"
                      "        ],\n"
                      "        \"sub\": 1\n"
                      "      },\n"
                      "      {\n"
                      "        \"b\": [\n"
                      "          true,\n"
                      "          [\n"
                      "            true\n"
                      "          ]\n"
                      "        ],\n"
                      "        \"sub\": 1\n"
                      "      },\n"
                      "      1\n"
                      "    ]\n"
                      "  ]\n"
                      "}");
  free(string);
} /* test_impl */


int main()
{
  test_run(test_impl);
}

