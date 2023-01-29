#include "hashtable.h"
#include "json.h"
#include "stringbuffer.h"
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
static bool _json_skip_whitespaces(char *, size_t /* length */, size_t * /* offset */);

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

  size_t length = strlen(text);
  if (!length)
  {
    return(NULL);
  }

  size_t           offset = 0;
  struct JsonValue *value = _json_parse(text, length, &offset);

  if (offset < length)
  {
    // we have some leftovers that we can't parse
    json_release_value(value);
    return(NULL);
  }

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
    _json_release_json_object(value->value->object);
    break;

  case JSON_TYPE_ARRAY:
    _json_release_json_array(value->value->array);
    break;

  case JSON_TYPE_STRING:
    _json_free(value->value->string);

  default:
    // no need to release
    break;
  }

  _json_free(value->value);
  _json_free(value);
}


size_t json_array_size(struct JsonArray *array)
{
  if (array == NULL)
  {
    return(0);
  }

  return(vector_size(array->vector));
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

  // skip whitespaces
  if (!_json_skip_whitespaces(text, length, offset))
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

  if (text[*offset] == '-' || isdigit(text[*offset]))
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
} /* _json_parse */

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
  if (text == NULL || (*offset + 2 > length) || text[*offset] != '[')
  {
    return(NULL);
  }

  *offset = *offset + 1;

  struct JsonArray *array = malloc(sizeof(struct JsonArray));
  array->vector = vector_new_with_options(100 /* initial size */, true /* allow resize */);
  struct JsonValue *value = _json_create_null_value();
  value->type         = JSON_TYPE_ARRAY;
  value->value->array = array;

  bool found_seperator = true;
  for ( ; *offset < length; *offset = *offset + 1)
  {
    // skip whitespaces
    if (!_json_skip_whitespaces(text, length, offset))
    {
      json_release_value(value);
      return(NULL);
    }

    size_t index = *offset;

    if (text[index] == ']')
    {
      *offset = *offset + 1;
      break;
    }
    else if (text[index] == ',')
    {
      if (found_seperator)
      {
        json_release_value(value);
        return(NULL);
      }
      else
      {
        found_seperator = true;
      }
    }
    else if (found_seperator)
    {
      found_seperator = false;

      struct JsonValue *array_item = _json_parse(text, length, offset);
      if (array_item == NULL)
      {
        json_release_value(value);
        return(NULL);
      }

      vector_push(array->vector, array_item);
    }
    else
    {
      json_release_value(value);
      return(NULL);
    }
  }

  return(value);
} /* _json_parse_array */

static struct JsonValue *_json_parse_string(char *text, size_t length, size_t *offset)
{
  if (text == NULL || (*offset + 2 > length) || text[*offset] != '"')
  {
    return(NULL);
  }

  size_t              delta_offset = 0;
  bool                in_escape    = false;
  bool                found_end    = false;
  struct StringBuffer *buffer      = stringbuffer_new_with_options(100 /* initial size */, true /* allow resize */);
  for (size_t index = *offset + 1; index < length; index++)
  {
    char character = text[index];

    if (in_escape)
    {
      in_escape    = false;
      delta_offset = delta_offset + 1;

      if (character == 'b')
      {
        stringbuffer_append(buffer, '\b');
      }
      else if (character == 'f')
      {
        stringbuffer_append(buffer, '\f');
      }
      else if (character == 'n')
      {
        stringbuffer_append(buffer, '\n');
      }
      else if (character == 'r')
      {
        stringbuffer_append(buffer, '\r');
      }
      else if (character == 't')
      {
        stringbuffer_append(buffer, '\t');
      }
      else if (character == '"')
      {
        stringbuffer_append(buffer, '"');
      }
      else if (character == '\\')
      {
        stringbuffer_append(buffer, '\\');
      }
      else
      {
        // invalid/unsupported escape
        stringbuffer_release(buffer);
        return(NULL);
      }
    }
    else if (character == '\\')
    {
      in_escape = true;
    }
    else if (character == '"')
    {
      found_end = true;
      break;
    }
    else
    {
      stringbuffer_append(buffer, character);
    }
  }

  if (!found_end)
  {
    stringbuffer_release(buffer);
    return(NULL);
  }

  // new offset is now previous offset + 2 (start/end ") + content length + escape characters cound
  *offset = *offset + delta_offset + stringbuffer_get_content_size(buffer) + 2;
  struct JsonValue *value = _json_create_null_value();
  value->type          = JSON_TYPE_STRING;
  value->value->string = stringbuffer_to_string(buffer);
  stringbuffer_release(buffer);

  return(value);
} /* _json_parse_string */

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
    if (index == start && character == '-')
    {
      end = end + 1;
    }
    else if (character == '.' && !found_decimal_point)
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

  *offset = end;
  struct JsonValue *value = _json_create_null_value();
  value->type = JSON_TYPE_NUMBER;
  char             *subtext = stringfn_substring(text, (int)start, end - start);
  value->value->number = strtold(subtext, NULL);
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
    value->type           = JSON_TYPE_BOOLEAN;
    value->value->boolean = true;
    *offset               = *offset + 4;

    return(value);
  }
  else if ((*offset + 5 <= length) && stringfn_starts_with(subtext, "false"))
  {
    struct JsonValue *value = _json_create_null_value();
    value->type           = JSON_TYPE_BOOLEAN;
    value->value->boolean = false;

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


static bool _json_skip_whitespaces(char *text, size_t length, size_t *offset)
{
  if (text == NULL || *offset >= length)
  {
    return(false);
  }

  // skip whitespaces
  for ( ; *offset < length; *offset = *offset + 1)
  {
    if (!isspace(text[*offset]))
    {
      break;
    }
  }

  return(*offset < length);
}

