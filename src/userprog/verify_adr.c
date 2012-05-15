#include <stdlib.h>
#include "userprog/pagedir.h"

/*
struct test_case_t
{
  void* start;
  unsigned length;
};


#define TEST_CASE_COUNT 6
const struct test_case_t test_case[TEST_CASE_COUNT] =
{
  {(void*)199, 102},
  {(void*)101, 98},
  {(void*)250, 190},
  {(void*)250, 200},
  {(void*)250, 210}
};

int main()
{
  int i;
  bool result;
  
  for (i = 0; i < TEST_CASE_COUNT; ++i)
  {

    start_evaluate_algorithm(test_case[i].start, test_case[i].length);
    result = verify_fix_length(test_case[i].start, test_case[i].length);
    evaluate(result);
    end_evaluate_algorithm();	
   
    start_evaluate_algorithm(test_case[i].start, test_case[i].length);
    result = verify_variable_length(test_case[i].start);
    evaluate(result);    
    end_evaluate_algorithm();
  }

  return 0;
}

*/
