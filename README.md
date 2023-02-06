# json

[![CI](https://github.com/sagiegurari/c_json/workflows/CI/badge.svg?branch=master)](https://github.com/sagiegurari/c_json/actions)
[![Release](https://img.shields.io/github/v/release/sagiegurari/c_json)](https://github.com/sagiegurari/c_json/releases)
[![license](https://img.shields.io/github/license/sagiegurari/c_json)](https://github.com/sagiegurari/c_json/blob/master/LICENSE)

> JSON parser/writer for C.

* [Overview](#overview)
* [Usage](#usage)
* [Contributing](.github/CONTRIBUTING.md)
* [Release History](CHANGELOG.md)
* [License](#license)

<a name="overview"></a>
## Overview
This library provides both parsing JSON text/files and creating JSON text/files.

<a name="usage"></a>
## Usage

```c
#include "json.h"
#include <stdio.h>
#include <stdlib.h>


int main()
{
  // parsing the json string
  struct JsonValue *value = json_parse("{\"number\":1.6, \"null_key\" : null,\n"
                                       "\"bool_true\": true,\"bool_false\":false\n"
                                       ",\"string\": \"my string\nsecond line\" ,\n"
                                       " \"arr\": [1, 2.7, 3],  \n"
                                       "\"obj\": {\"subkey\": 88}, \"subobj\": {\"sub\":{\"subkey\": 77}}}");

  struct JsonValue *sub_value;


  // objects are converted to hashtables, so need to use the hashtable api to access/modify
  sub_value = hashtable_get(value->value->object, "string");

  // The actual value is based on the type
  printf("string value: %s\n", sub_value->value->string);

  // arrays are converted to vectors, so need to use the vector api to access/modify
  sub_value = hashtable_get(value->value->object, "arr");
  sub_value = vector_get(sub_value->value->array, 0);
  printf("array[0] value: %Lf\n", sub_value->value->number);

  // you can convert the parsed object back to string
  // The below is the same as json_stringify_with_options(value, false, 0);
  char *json_string = json_stringify(value);
  printf("JSON string:\n%s\n", json_string);
  free(json_string);

  // once done, release the parsed object
  json_release(value);

  // you can also prettify json strings which will
  // parse them and stringify them back with the
  // provided formatting options
  json_string = json_prettify("{\"key1\":1,\"key2\":\"test\",\"key3\":false,\"key4\":true,\"key5\":null,\"key6\":12.5,\"key7\":[true,false,[true, {\"sub\":1,\"a\":[true]}, {\"sub\":1,\"b\":[true,[true]]}, 1]]]", true, 2);
  printf("JSON string:\n%s\n", json_string);

  return(0);
} /* main */
```

## Contributing
See [contributing guide](.github/CONTRIBUTING.md)

<a name="history"></a>
## Release History

See [Changelog](CHANGELOG.md)

<a name="license"></a>
## License
Developed by Sagie Gur-Ari and licensed under the Apache 2 open source license.
