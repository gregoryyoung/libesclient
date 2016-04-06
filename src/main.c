#include <stdlib.h>
#include <string.h>
#include <CUnit/Basic.h>
#include "es_client_internal.h"

int main()
{

#ifdef TESTS
   //initialize the CUnit test registry
   if (CUE_SUCCESS != CU_initialize_registry())
      return CU_get_error();

   int err;
   if((err = register_wtf_uuid_tests())) return err;
   if((err = register_es_msg_tests())) return err;
   if((err = register_es_proto_helper_tests())) return err;
   //if(err) return err;

   CU_basic_set_mode(CU_BRM_VERBOSE);
   CU_basic_run_tests();
   CU_cleanup_registry();
   return CU_get_error();
#endif
   return 0;
}