#include "test.h"


void test_impl()
{
  struct JsonValue *value;
  struct JsonValue *sub_value;

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

  value = json_parse("{\"key\":1}");
  assert_true_with_description(value != NULL, "number object not detected");
  assert_true_with_description(value->type == JSON_TYPE_OBJECT, "object type not set");
  assert_size_equal(hashtable_size(value->value->object), 1);
  sub_value = hashtable_get(value->value->object, "key");
  assert_true_with_description(sub_value != NULL, "key not found");
  assert_true(sub_value->type == JSON_TYPE_NUMBER);
  assert_num_equal(sub_value->value->number, 1L);
  json_release_value(value);

  value = json_parse("{\"number\":1.6, \"null_key\" : null,\n"
                     "\"bool_true\": true,\"bool_false\":false,\n"
                     "\"string\": \"my string\nsecond line\" ,\n"
                     " \"arr\": [1, 2.7, 3]  \n"
                     ",\"obj\": {\"subkey\": 88}, \"subobj\": {\"sub\":{\"subkey\": 77}}}");
  assert_true_with_description(value != NULL, "mixed object not detected");
  assert_true_with_description(value->type == JSON_TYPE_OBJECT, "object type not set");
  assert_size_equal(hashtable_size(value->value->object), 8);
  sub_value = hashtable_get(value->value->object, "number");
  assert_true_with_description(sub_value != NULL, "number not found");
  assert_true(sub_value->type == JSON_TYPE_NUMBER);
  assert_num_equal(sub_value->value->number, 1.6L);
  sub_value = hashtable_get(value->value->object, "null_key");
  assert_true_with_description(sub_value != NULL, "null_key not found");
  assert_true(sub_value->type == JSON_TYPE_NULL);
  sub_value = hashtable_get(value->value->object, "bool_true");
  assert_true_with_description(sub_value != NULL, "bool_true not found");
  assert_true(sub_value->type == JSON_TYPE_BOOLEAN);
  assert_true(sub_value->value->boolean);
  sub_value = hashtable_get(value->value->object, "bool_false");
  assert_true_with_description(sub_value != NULL, "bool_false not found");
  assert_true(sub_value->type == JSON_TYPE_BOOLEAN);
  assert_true(!sub_value->value->boolean);
  sub_value = hashtable_get(value->value->object, "string");
  assert_true(sub_value->type == JSON_TYPE_STRING);
  assert_string_equal(sub_value->value->string, "my string\nsecond line");
  sub_value = hashtable_get(value->value->object, "arr");
  assert_true(sub_value->type == JSON_TYPE_ARRAY);
  sub_value = vector_get(sub_value->value->array, 1);
  assert_true(sub_value->type == JSON_TYPE_NUMBER);
  assert_num_equal(sub_value->value->number, 2.7L);
  sub_value = hashtable_get(value->value->object, "obj");
  assert_true(sub_value->type == JSON_TYPE_OBJECT);
  sub_value = hashtable_get(sub_value->value->object, "subkey");
  assert_true(sub_value->type == JSON_TYPE_NUMBER);
  assert_num_equal(sub_value->value->number, 88L);
  sub_value = hashtable_get(value->value->object, "subobj");
  assert_true(sub_value->type == JSON_TYPE_OBJECT);
  sub_value = hashtable_get(sub_value->value->object, "sub");
  assert_true(sub_value->type == JSON_TYPE_OBJECT);
  sub_value = hashtable_get(sub_value->value->object, "subkey");
  assert_true(sub_value->type == JSON_TYPE_NUMBER);
  assert_num_equal(sub_value->value->number, 77L);
  json_release_value(value);
} /* test_impl */


int main()
{
  test_run(test_impl);
}

