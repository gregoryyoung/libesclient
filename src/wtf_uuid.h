#ifndef WTF_UUID
#define WTF_UUID

uuid_t *
get_uuid_from_wtf (char *wtf_data) ;

void
write_uuid_to_wtf (uuid_t uuid, char *wtf_data) ;


//TODO move to cunit
void
test_read_wtf_uuid();

void
test_write_wtf_uuid();

#endif