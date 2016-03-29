#ifndef WTF_UUID
#define WTF_UUID
#include <uuid/uuid.h>


uuid_t * get_uuid_from_wtf (char *wtf_data) ;

void write_uuid_to_wtf (uuid_t uuid, char *wtf_data) ;

int register_wtf_uuid_tests();

#endif