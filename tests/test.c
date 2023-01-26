#include "test.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void test_run(void (*fn)(void))
{
  printf("Test ... ");
  fn();
  printf("Done\n");
}


void test_fail()
{
  printf(" Error\n");
  exit(1);
}


void assert_true(bool value)
{
  assert_true_with_description(value, NULL);
}


void assert_true_with_description(bool value, char *description)
{
  if (!value)
  {
    if (description != NULL)
    {
      printf("Assert Failed, %s", description);
    }

    test_fail();
  }
}


void assert_num_equal(long double value1, long double value2)
{
  if (value1 != value2)
  {
#ifdef linux
    printf("Assert Failed, value: %Lf not equals to value: %Lf", value1, value2);
#endif
    test_fail();
  }
}


void assert_string_equal(char *value1, char *value2)
{
  if (strcmp(value1, value2) != 0)
  {
    printf("Assert Failed, value: %s not equals to value: %s", value1, value2);
    test_fail();
  }
}

