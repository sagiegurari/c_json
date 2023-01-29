#ifndef __TEST_H__
#define __TEST_H__

#include "json.h"
#include <stdbool.h>
#include <stddef.h>

void test_run(void (*fn)());
void test_fail();
void assert_true(bool);
void assert_true_with_description(bool, char *);

void assert_size_equal(size_t, size_t);
void assert_num_equal(long double, long double);
void assert_string_equal(char *, char *);

#endif

