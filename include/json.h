#ifndef JSON_H
#define JSON_H

#include "hashtable.h"
#include "vector.h"
#include <stdbool.h>
#include <stddef.h>

enum JsonType
{
  JSON_TYPE_OBJECT = 1,
  JSON_TYPE_ARRAY,
  JSON_TYPE_STRING,
  JSON_TYPE_NUMBER,
  JSON_TYPE_BOOLEAN,
  JSON_TYPE_NULL
};

struct JsonValue
{
  enum JsonType        type;
  union JsonValueUnion *value;
};

union JsonValueUnion
{
  struct HashTable *object;
  struct Vector    *array;
  char             *string;
  long double      number;
  bool             boolean;
};

/**
 * Parses the give string and returns the json value union.
 * In case of any error, this function will return null.
 * The json value must be fully released once done.
 */
struct JsonValue *json_parse(char * /* text */);

/**
 * Releases the json value and all internal memory used.
 * All internal strings will also be released, therefore no const strings
 * or freed strings must reside in the structure.
 */
void json_release_value(struct JsonValue *);

#endif

