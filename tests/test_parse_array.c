#include "test.h"
#include <stdio.h>


void test_impl()
{
  struct JsonValue *value;
  struct JsonValue *sub_value;

  value = json_parse("[]");
  assert_true_with_description(value != NULL, "empty array not detected");
  assert_true_with_description(value->type == JSON_TYPE_ARRAY, "array type not set");
  assert_size_equal(vector_size(value->value->array), 0);
  json_release_value(value);

  value = json_parse("[   ]");
  assert_true_with_description(value != NULL, "empty array not detected");
  assert_true_with_description(value->type == JSON_TYPE_ARRAY, "array type not set");
  assert_size_equal(vector_size(value->value->array), 0);
  json_release_value(value);

  value = json_parse("[1]");
  assert_true_with_description(value != NULL, "number array not detected");
  assert_true_with_description(value->type == JSON_TYPE_ARRAY, "array type not set");
  assert_size_equal(vector_size(value->value->array), 1);
  sub_value = vector_get(value->value->array, 0);
  assert_true(sub_value->type == JSON_TYPE_NUMBER);
  assert_num_equal(sub_value->value->number, 1L);
  json_release_value(value);

  value = json_parse("[1,2]");
  assert_true_with_description(value != NULL, "multi number array not detected");
  assert_true_with_description(value->type == JSON_TYPE_ARRAY, "array type not set");
  assert_size_equal(vector_size(value->value->array), 2);
  sub_value = vector_get(value->value->array, 0);
  assert_true(sub_value->type == JSON_TYPE_NUMBER);
  assert_num_equal(sub_value->value->number, 1L);
  sub_value = vector_get(value->value->array, 1);
  assert_true(sub_value->type == JSON_TYPE_NUMBER);
  assert_num_equal(sub_value->value->number, 2L);
  json_release_value(value);

  value = json_parse("[  1, \"test\", true, false, null, 2.2, {} ]");
  assert_true_with_description(value != NULL, "mixed array not detected");
  assert_true_with_description(value->type == JSON_TYPE_ARRAY, "array type not set");
  assert_size_equal(vector_size(value->value->array), 7);
  sub_value = vector_get(value->value->array, 0);
  assert_true(sub_value->type == JSON_TYPE_NUMBER);
  assert_num_equal(sub_value->value->number, 1L);
  sub_value = vector_get(value->value->array, 1);
  assert_true(sub_value->type == JSON_TYPE_STRING);
  assert_string_equal(sub_value->value->string, "test");
  sub_value = vector_get(value->value->array, 2);
  assert_true(sub_value->type == JSON_TYPE_BOOLEAN);
  assert_true(sub_value->value->boolean);
  sub_value = vector_get(value->value->array, 3);
  assert_true(sub_value->type == JSON_TYPE_BOOLEAN);
  assert_true(!sub_value->value->boolean);
  sub_value = vector_get(value->value->array, 4);
  assert_true(sub_value->type == JSON_TYPE_NULL);
  sub_value = vector_get(value->value->array, 5);
  assert_true(sub_value->type == JSON_TYPE_NUMBER);
  assert_num_equal(sub_value->value->number, 2.2L);
  sub_value = vector_get(value->value->array, 6);
  assert_true(sub_value->type == JSON_TYPE_OBJECT);
  assert_size_equal(hashtable_size(sub_value->value->object), 0);
  json_release_value(value);
} /* test_impl */


int main()
{
  test_run(test_impl);
}

