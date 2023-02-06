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
 * Converts the provided json value to string without any special formatting.
 * In case of any error or invalid value, NULL will be returned.
 */
char *json_stringify(struct JsonValue *);

/**
 * Converts the provided json value to string with formatting based on the provided options.
 * In case of any error or invalid value, NULL will be returned.
 */
char *json_stringify_with_options(struct JsonValue *, bool /* multi line */, size_t /* indentation */);

/**
 * Parses the provided json string and stringifies it back with
 * the provided formatting options.
 */
char *json_prettify(char *, bool /* multi line */, size_t /* indentation */);

/**
 * Releases the json value and all internal memory used.
 * All internal strings will also be released, therefore no const strings
 * or freed strings must reside in the structure.
 */
void json_release(struct JsonValue *);

#endif

