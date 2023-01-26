#include "hashtable.h"
#include "json.h"
#include "stringfn.h"
#include "vector.h"
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

// private functions
static void _json_free(void *);
static void _json_release_json_object(struct JsonObject *);
static void _json_release_json_array(struct JsonArray *);
static struct JsonValue *_json_create_null_value();
static struct JsonValue *_json_parse(char *, size_t /* length */, size_t * /* offset */);
static struct JsonValue *_json_parse_object(char *, size_t /* length */, size_t * /* offset */);
static struct JsonValue *_json_parse_array(char *, size_t /* length */, size_t * /* offset */);
static struct JsonValue *_json_parse_string(char *, size_t /* length */, size_t * /* offset */);
static struct JsonValue *_json_parse_number(char *, size_t /* length */, size_t * /* offset */);
static struct JsonValue *_json_parse_boolean(char *, size_t /* length */, size_t * /* offset */);
static struct JsonValue *_json_parse_null(char *, size_t /* length */, size_t * /* offset */);

struct JsonObject
{
  struct HashTable *table;
};

struct JsonArray
{
  struct Vector *vector;
};


struct JsonValue *json_parse(char *text)
{
  if (text == NULL)
  {
    return(NULL);
  }

  char   *trimmed_text = stringfn_trim(text);
  size_t length        = strlen(trimmed_text);
  if (!length)
  {
    _json_free(trimmed_text);
    return(NULL);
  }

  size_t           offset = 0;
  struct JsonValue *value = _json_parse(trimmed_text, length, &offset);

  _json_free(trimmed_text);

  return(value);
}


void json_release_value(struct JsonValue *value)
{
  if (value == NULL)
  {
    return;
  }

  switch (value->type)
  {
  case JSON_TYPE_OBJECT:
    _json_release_json_object(value->value->value_object);
    break;

  case JSON_TYPE_ARRAY:
    _json_release_json_array(value->value->value_array);
    break;

  case JSON_TYPE_STRING:
    _json_free(value->value->value_string);

  default:
    // no need to release
    break;
  }

  _json_free(value->value);
  _json_free(value);
}


static void _json_free(void *value)
{
  if (value == NULL)
  {
    return;
  }

  free(value);
}


static void _json_release_json_object(struct JsonObject *object)
{
  if (object == NULL)
  {
    return;
  }

  hashtable_release(object->table);
  _json_free(object);
}


static void _json_release_json_array(struct JsonArray *array)
{
  if (array == NULL)
  {
    return;
  }

  size_t count = vector_size(array->vector);
  for (size_t index = 0; index < count; index++)
  {
    struct JsonValue *value = vector_get(array->vector, index);
    json_release_value(value);
  }
  vector_release(array->vector);
  _json_free(array);
}

static struct JsonValue *_json_create_null_value()
{
  struct JsonValue *value = malloc(sizeof(struct JsonValue));

  value->type  = JSON_TYPE_NULL;
  value->value = malloc(sizeof(union JsonValueUnion));

  return(value);
}

static struct JsonValue *_json_parse(char *text, size_t length, size_t *offset)
{
  if (text == NULL || !length || *offset >= length)
  {
    return(NULL);
  }

  // detect next value type
  if (text[*offset] == '{')
  {
    return(_json_parse_object(text, length, offset));
  }

  if (text[*offset] == '[')
  {
    return(_json_parse_array(text, length, offset));
  }

  if (isdigit(text[*offset]))
  {
    return(_json_parse_number(text, length, offset));
  }

  if (text[*offset] == '"')
  {
    return(_json_parse_string(text, length, offset));
  }

  if (text[*offset] == 't' || text[*offset] == 'f')
  {
    return(_json_parse_boolean(text, length, offset));
  }

  if (text[*offset] == 'n')
  {
    return(_json_parse_null(text, length, offset));
  }

  return(NULL);
}

static struct JsonValue *_json_parse_object(char *text, size_t length, size_t *offset)
{
  if (text == NULL || (*offset + 2 > length))
  {
    return(NULL);
  }

  // TODO IMPL THIS
  return(NULL);
}

static struct JsonValue *_json_parse_array(char *text, size_t length, size_t *offset)
{
  if (text == NULL || (*offset + 2 > length))
  {
    return(NULL);
  }

  // TODO IMPL THIS
  return(NULL);
}

static struct JsonValue *_json_parse_string(char *text, size_t length, size_t *offset)
{
  if (text == NULL || (*offset + 2 > length))
  {
    return(NULL);
  }

  // TODO IMPL THIS
  return(NULL);
}

static struct JsonValue *_json_parse_number(char *text, size_t length, size_t *offset)
{
  if (text == NULL || (*offset + 1 > length))
  {
    return(NULL);
  }

  size_t start               = *offset;
  size_t end                 = start;
  bool   found_decimal_point = false;
  for (size_t index = start; index < length; index++)
  {
    char character = text[index];
    if (character == '.' && !found_decimal_point)
    {
      found_decimal_point = true;
      end                 = end + 1;
    }
    else if (isdigit(character))
    {
      end = end + 1;
    }
    else
    {
      break;
    }
  }

  if (start == end)
  {
    return(NULL);
  }

  *offset = end + 1;
  struct JsonValue *value = _json_create_null_value();
  value->type = JSON_TYPE_NUMBER;
  char             *subtext = stringfn_substring(text, (int)start, end - start);
  value->value->value_number = strtold(subtext, NULL);
  _json_free(subtext);

  return(value);
} /* _json_parse_number */

static struct JsonValue *_json_parse_boolean(char *text, size_t length, size_t *offset)
{
  if (text == NULL)
  {
    return(NULL);
  }

  char *subtext = text + *offset;

  if ((*offset + 4 <= length) && stringfn_starts_with(subtext, "true"))
  {
    struct JsonValue *value = _json_create_null_value();
    value->type                 = JSON_TYPE_BOOLEAN;
    value->value->value_boolean = true;

    *offset = *offset + 4;

    return(value);
  }
  else if ((*offset + 5 <= length) && stringfn_starts_with(subtext, "false"))
  {
    struct JsonValue *value = _json_create_null_value();
    value->type                 = JSON_TYPE_BOOLEAN;
    value->value->value_boolean = false;

    *offset = *offset + 5;

    return(value);
  }

  return(NULL);
}

static struct JsonValue *_json_parse_null(char *text, size_t length, size_t *offset)
{
  if (text == NULL || (*offset + 4 > length))
  {
    return(NULL);
  }

  char *subtext = text + *offset;

  if (stringfn_starts_with(subtext, "null"))
  {
    *offset = *offset + 4;
    return(_json_create_null_value());
  }

  return(NULL);
}

