#ifndef WTF_UUID
#define WTF_UUID

uuid_t *
get_uuid_from_wtf (char *wtf_data) ;

void
write_uuid_to_wtf (uuid_t uuid, char *wtf_data) ;

#endif