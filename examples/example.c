#include "json.h"
#include <stdio.h>


int main()
{
  // parsing the json string
  struct JsonValue *value = json_parse("{\"number\":1.6, \"null_key\" : null,\n"
                                       "\"bool_true\": true,\"bool_false\":false\n"
                                       "\"string\": \"my string\nsecond line\" \n"
                                       " \"arr\": [1, 2.7, 3]  \n"
                                       "\"obj\": {\"subkey\": 88}}, \"subobj\": {\"sub\":{\"subkey\": 77}}}");

  struct JsonValue *sub_value;

  // objects are converted to hashtables, so need to use the hashtable api to access/modify
  sub_value = hashtable_get(value->value->object, "string");

  // The actual value is based on the type
  printf("string value: %s\n", sub_value->value->string);

  // arrays are converted to vectors, so need to use the vector api to access/modify
  sub_value = hashtable_get(value->value->object, "arr");
  sub_value = vector_get(sub_value->value->array, 0);
  printf("array[0] value: %Lf\n", sub_value->value->number);

  // once done, release the parsed object
  json_release_value(value);

  return(0);
}
