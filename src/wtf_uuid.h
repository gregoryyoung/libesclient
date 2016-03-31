#ifndef WTF_UUID
#define WTF_UUID
#include <uuid/uuid.h>


void get_uuid_from_wtf (uuid_t dst, const char *wtf_data);

void write_uuid_to_wtf (uuid_t uuid, unsigned char *wtf_data);

int register_wtf_uuid_tests();

#endif