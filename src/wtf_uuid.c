#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <uuid/uuid.h>

static int net_order[16] = {3, 2, 1, 0, 5, 4, 7, 6, 8, 9, 10, 11, 12, 13, 14, 15};

uuid_t *
get_uuid_from_wtf (char *wtf_data) {
    char *ret = malloc (sizeof (uuid_t));
    for(int i=0;i<16;i++) {
        ret[i] = wtf_data[net_order[i]];
    }
    return (uuid_t *)ret;
}

void
write_uuid_to_wtf (uuid_t uuid, char *wtf_data) {
    for(int i=0;i<16;i++) {
        wtf_data[net_order[i]] = uuid[i];
    }
}
