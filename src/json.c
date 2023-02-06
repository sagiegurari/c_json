#include "json.h"
#include "stringbuffer.h"
#include "stringfn.h"
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

// private functions
static void _json_free(void *);
static void _json_release_json_array(struct Vector *);
static struct JsonValue *_json_create_null_value();
static struct JsonValue *_json_parse(char *, size_t /* length */, size_t * /* offset */);
static struct JsonValue *_json_parse_object(char *, size_t /* length */, size_t * /* offset */);
static struct JsonValue *_json_parse_array(char *, size_t /* length */, size_t * /* offset */);
static struct JsonValue *_json_parse_string(char *, size_t /* length */, size_t * /* offset */);
static struct JsonValue *_json_parse_number(char *, size_t /* length */, size_t * /* offset */);
static struct JsonValue *_json_parse_boolean(char *, size_t /* length */, size_t * /* offset */);
static struct JsonValue *_json_parse_null(char *, size_t /* length */, size_t * /* offset */);
static bool _json_skip_whitespaces(char *, size_t /* length */, size_t * /* offset */);
static bool _json_parse_object_key(struct HashTable *, char *, size_t /* length */, size_t * /* offset */);
static void _json_release_hashtable_key_value(char *, void *);
static bool _json_stringify(struct JsonValue *, struct StringBuffer *, bool /* multi line */, size_t /* indentation */, size_t /* current indentation */, bool /* skip indent */);
static bool _json_stringify_object(struct HashTable *, struct StringBuffer *, bool /* multi line */, size_t /* indentation */, size_t /* current indentation */, bool /* skip indent */);
static bool _json_stringify_array(struct Vector *, struct StringBuffer *, bool /* multi line */, size_t /* indentation */, size_t /* current indentation */, bool /* skip indent */);
static bool _json_stringify_string(char *, struct StringBuffer *);
static void _json_add_indentation(struct StringBuffer *, size_t /* indentation */);

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


char *json_stringify(struct JsonValue *value)
{
  return(json_stringify_with_options(value, false /* multi line */, 0 /* indentation */));
}


char *json_stringify_with_options(struct JsonValue *value, bool multi_line, size_t indentation)
{
  if (value == NULL)
  {
    return(NULL);
  }

  struct StringBuffer *buffer = stringbuffer_new();

  bool                done = _json_stringify(value, buffer, multi_line, indentation, 0, false);
  if (!done)
  {
    stringbuffer_release(buffer);
    return(NULL);
  }

  char *json_string = stringbuffer_to_string(buffer);
  stringbuffer_release(buffer);

  return(json_string);
}


char *json_prettify(char *value, bool multi_line, size_t indentation)
{
  struct JsonValue *parsed = json_parse(value);

  if (parsed == NULL)
  {
    return(NULL);
  }

  char *formatted = json_stringify_with_options(parsed, multi_line, indentation);

  json_release_value(parsed);

  return(formatted);
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
    hashtable_release(value->value->object);
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


static void _json_free(void *value)
{
  if (value == NULL)
  {
    return;
  }

  free(value);
}


static void _json_release_json_array(struct Vector *array)
{
  if (array == NULL)
  {
    return;
  }

  size_t count = vector_size(array);
  for (size_t index = 0; index < count; index++)
  {
    struct JsonValue *value = vector_get(array, index);
    json_release_value(value);
  }
  vector_release(array);
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
  if (text == NULL || (*offset + 2 > length) || text[*offset] != '{')
  {
    return(NULL);
  }

  *offset = *offset + 1;

  struct HashTable *object = hashtable_new();
  struct JsonValue *value  = _json_create_null_value();
  value->type          = JSON_TYPE_OBJECT;
  value->value->object = object;

  bool found_separator      = false;
  bool can_start_next_entry = true;
  for ( ; *offset < length; *offset = *offset + 1)
  {
    // skip whitespaces
    if (!_json_skip_whitespaces(text, length, offset))
    {
      json_release_value(value);
      return(NULL);
    }

    size_t index = *offset;

    if (found_separator)
    {
      if (text[index] == '"')
      {
        if (!_json_parse_object_key(object, text, length, offset))
        {
          json_release_value(value);
          return(NULL);
        }

        // we are in next char now, but for loop will advance it one more, so go back 1
        *offset = *offset - 1;

        found_separator      = false;
        can_start_next_entry = false;
      }
      else
      {
        json_release_value(value);
        return(NULL);
      }
    }
    else if (!found_separator && text[index] == '}')
    {
      *offset = *offset + 1;
      break;
    }
    else if (can_start_next_entry && text[index] == '"')
    {
      if (!_json_parse_object_key(object, text, length, offset))
      {
        json_release_value(value);
        return(NULL);
      }

      // we are in next char now, but for loop will advance it one more, so go back 1
      *offset = *offset - 1;

      can_start_next_entry = false;
      found_separator      = false;
    }
    else if (!found_separator && text[index] == ',')
    {
      found_separator      = true;
      can_start_next_entry = true;
    }
  }

  return(value);
} /* _json_parse_object */

static struct JsonValue *_json_parse_array(char *text, size_t length, size_t *offset)
{
  if (text == NULL || (*offset + 2 > length) || text[*offset] != '[')
  {
    return(NULL);
  }

  *offset = *offset + 1;

  struct Vector    *array = vector_new_with_options(100 /* initial size */, true /* allow resize */);
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

      vector_push(array, array_item);

      // we are in next char now, but for loop will advance it one more, so go back 1
      *offset = *offset - 1;
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
  bool   found_decimal_point = false;
  for ( ; *offset < length; *offset = *offset + 1)
  {
    size_t index = *offset;

    char   character = text[index];
    if (isdigit(character) || (index == start && character == '-'))
    {
      // continue
    }
    else if (character == '.' && !found_decimal_point)
    {
      found_decimal_point = true;
    }
    else
    {
      break;
    }
  }

  if (start == *offset)
  {
    return(NULL);
  }

  struct JsonValue *value = _json_create_null_value();
  value->type = JSON_TYPE_NUMBER;
  char             *subtext = stringfn_substring(text, (int)start, *offset - start);
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


static bool _json_parse_object_key(struct HashTable *object, char *text, size_t length, size_t *offset)
{
  if (object == NULL || text == NULL || (*offset + 4 > length) || text[*offset] != '"')
  {
    return(false);
  }

  // parse key
  struct JsonValue *json_value = _json_parse_string(text, length, offset);
  if (json_value == NULL)
  {
    return(false);
  }
  if (json_value->type != JSON_TYPE_STRING)
  {
    json_release_value(json_value);
    return(false);
  }

  char *key = json_value->value->string;
  _json_free(json_value->value);
  _json_free(json_value);

  // skip whitespaces
  if (!_json_skip_whitespaces(text, length, offset) || text[*offset] != ':')
  {
    _json_free(key);
    return(false);
  }
  *offset = *offset + 1;

  // parse value
  json_value = _json_parse(text, length, offset);
  if (json_value == NULL)
  {
    _json_free(key);
    return(false);
  }

  bool added = hashtable_insert(object, key, json_value, _json_release_hashtable_key_value);

  if (!added)
  {
    _json_free(key);
    json_release_value(json_value);
    return(false);
  }

  return(true);
} /* _json_parse_object_key */


static void _json_release_hashtable_key_value(char *key, void *value)
{
  _json_free(key);
  struct JsonValue *json_value = (struct JsonValue *)value;
  json_release_value(json_value);
}


static bool _json_stringify(struct JsonValue *value, struct StringBuffer *buffer, bool multi_line, size_t indentation, size_t current_indentation, bool skip_indent)
{
  if (value == NULL || buffer == NULL)
  {
    return(false);
  }

  if (multi_line && !skip_indent)
  {
    if (!stringbuffer_is_empty(buffer))
    {
      stringbuffer_append(buffer, '\n');
    }

    if (value->type != JSON_TYPE_OBJECT && value->type != JSON_TYPE_ARRAY)
    {
      _json_add_indentation(buffer, current_indentation);
    }
  }

  switch (value->type)
  {
  case JSON_TYPE_OBJECT:
    if (!_json_stringify_object(value->value->object, buffer, multi_line, indentation, current_indentation, skip_indent))
    {
      return(false);
    }
    break;

  case JSON_TYPE_ARRAY:
    if (!_json_stringify_array(value->value->array, buffer, multi_line, indentation, current_indentation, skip_indent))
    {
      return(false);
    }
    break;

  case JSON_TYPE_STRING:
    _json_stringify_string(value->value->string, buffer);
    break;

  case JSON_TYPE_NUMBER:
    stringbuffer_append_long_double(buffer, value->value->number);
    break;

  case JSON_TYPE_BOOLEAN:
    stringbuffer_append_bool(buffer, value->value->boolean);
    break;

  case JSON_TYPE_NULL:
    stringbuffer_append_string(buffer, "null");
    break;
  }

  return(true);
} /* _json_stringify */


static bool _json_stringify_object(struct HashTable *object, struct StringBuffer *buffer, bool multi_line, size_t indentation, size_t current_indentation, bool skip_indent)
{
  if (object == NULL || buffer == NULL)
  {
    return(false);
  }

  if (multi_line && !skip_indent)
  {
    _json_add_indentation(buffer, current_indentation);
  }
  stringbuffer_append(buffer, '{');

  size_t size = hashtable_size(object);
  if (size)
  {
    struct HashTableEntries entries = hashtable_entries(object);

    bool                    added            = false;
    size_t                  next_indentation = indentation + current_indentation;
    for (size_t index = 0; index < size; index++)
    {
      char             *key   = entries.keys[index];
      struct JsonValue *value = (struct JsonValue *)entries.values[index];

      if (key != NULL && value != NULL)
      {
        if (!added)
        {
          added = true;
        }
        else
        {
          stringbuffer_append(buffer, ',');
        }

        if (multi_line)
        {
          stringbuffer_append(buffer, '\n');
          _json_add_indentation(buffer, next_indentation);
        }

        if (!_json_stringify_string(key, buffer))
        {
          return(false);
        }
        stringbuffer_append(buffer, ':');
        if (multi_line)
        {
          stringbuffer_append(buffer, ' ');
        }

        if (!_json_stringify(value, buffer, multi_line, indentation, next_indentation, true))
        {
          return(false);
        }
      }
    }

    _json_free(entries.keys);
    _json_free(entries.values);
  }

  if (multi_line)
  {
    stringbuffer_append(buffer, '\n');
    _json_add_indentation(buffer, current_indentation);
  }
  stringbuffer_append(buffer, '}');

  return(true);
} /* _json_stringify_object */


static bool _json_stringify_array(struct Vector *array, struct StringBuffer *buffer, bool multi_line, size_t indentation, size_t current_indentation, bool skip_indent)
{
  if (array == NULL || buffer == NULL)
  {
    return(false);
  }

  if (multi_line && !skip_indent)
  {
    _json_add_indentation(buffer, current_indentation);
  }
  stringbuffer_append(buffer, '[');

  size_t size             = vector_size(array);
  bool   added            = false;
  size_t next_indentation = indentation + current_indentation;
  for (size_t index = 0; index < size; index++)
  {
    struct JsonValue *value = vector_get(array, index);

    if (value != NULL)
    {
      if (!added)
      {
        added = true;
      }
      else
      {
        stringbuffer_append(buffer, ',');
      }

      if (!_json_stringify(value, buffer, multi_line, indentation, next_indentation, false))
      {
        return(false);
      }
    }
  }

  if (multi_line)
  {
    stringbuffer_append(buffer, '\n');
    _json_add_indentation(buffer, current_indentation);
  }
  stringbuffer_append(buffer, ']');

  return(true);
} /* _json_stringify_array */


static bool _json_stringify_string(char *text, struct StringBuffer *buffer)
{
  if (text == NULL || buffer == NULL)
  {
    return(false);
  }

  size_t length = strlen(text);

  stringbuffer_append(buffer, '"');
  for (size_t index = 0; index < length; index++)
  {
    char character = text[index];

    if (character == '\b')
    {
      stringbuffer_append_string(buffer, "\\b");
    }
    else if (character == '\f')
    {
      stringbuffer_append_string(buffer, "\\f");
    }
    else if (character == '\n')
    {
      stringbuffer_append_string(buffer, "\\n");
    }
    else if (character == '\r')
    {
      stringbuffer_append_string(buffer, "\\r");
    }
    else if (character == '\t')
    {
      stringbuffer_append_string(buffer, "\\t");
    }
    else if (character == '"')
    {
      stringbuffer_append_string(buffer, "\\\"");
    }
    else if (character == '\\')
    {
      stringbuffer_append_string(buffer, "\\\\");
    }
    else
    {
      stringbuffer_append(buffer, character);
    }
  }
  stringbuffer_append(buffer, '"');

  return(true);
} /* _json_stringify_string */


static void _json_add_indentation(struct StringBuffer *buffer, size_t indentation)
{
  if (buffer == NULL)
  {
    return;
  }

  for (size_t index = 0; index < indentation; index++)
  {
    stringbuffer_append(buffer, ' ');
  }
}
