#include <stdio.h>
#include <uuid/uuid.h>
#include "es_client_internal.h"

static int net_order[16] = {3, 2, 1, 0, 5, 4, 7, 6, 8, 9, 10, 11, 12, 13, 14, 15};

void get_uuid_from_wtf (uuid_t dst, const char *wtf_data) {
    uint8_t *ret = dst;
    for(int i=0;i<16;i++) {
        ret[i] = wtf_data[net_order[i]];
    }
}

void write_uuid_to_wtf (uuid_t uuid, unsigned char *wtf_data) {
    for(int i=0;i<16;i++) {
        wtf_data[net_order[i]] = uuid[i];
    }
}

#ifdef TESTS

void to_lower (char *str) {
    for (int i=0; str[i]; i++)
        str[i] = tolower((unsigned char)str[i]);
}

void test_read_wtf_uuid () {
    char uuid_str[37];
    char data[16] = {0x46, 0x6c,0xbc, 0x3e, 0x72,0xe2, 0x26, 0x42, 0xbc,0xb5,0xaa,0x93,0xc4,0x11,0xed,0x0d };
    char *expected = "3ebc6c46-e272-4226-bcb5-aa93c411ed0d";
    uuid_t uuid;
    get_uuid_from_wtf (uuid, data);
    uuid_unparse (uuid, uuid_str);
    to_lower (uuid_str);
    CU_ASSERT_STRING_EQUAL(uuid_str, expected);
    char data1[16] = {0xf8, 0xa6, 0xbb, 0x92, 0xe4, 0x07, 0x4d, 0x4a, 0xb1, 0xdf, 0x19, 0x04, 0xa9, 0x3c, 0x65, 0xa9};
    expected = "92bba6f8-07e4-4a4d-b1df-1904a93c65a9";
    get_uuid_from_wtf (uuid, data1);
    uuid_unparse (uuid, uuid_str);
    to_lower (uuid_str);
    CU_ASSERT_STRING_EQUAL (uuid_str, expected);
}

void test_write_wtf_uuid() {
    uuid_t uuid;
    char temp[37];
    unsigned char output[16];
    uuid_parse("3ebc6c46-e272-4226-bcb5-aa93c411ed0d", uuid);
    uuid_unparse(uuid, temp);

    char expected[16] = {0x46, 0x6c,0xbc, 0x3e, 0x72,0xe2, 0x26, 0x42, 0xbc,0xb5,0xaa,0x93,0xc4,0x11,0xed,0x0d };
    write_uuid_to_wtf (uuid, output);

    uuid_unparse(output, temp);
    for(int i=0;i<16;i++) {
        CU_ASSERT(output[i] == (unsigned char) expected[i]);
    }
}

int register_wtf_uuid_tests() {
   CU_pSuite pSuite = NULL;
    pSuite = CU_add_suite("wtf uuid tests", NULL, NULL);
    if (NULL == pSuite) {
       CU_cleanup_registry();
       return CU_get_error();
    }

    if ((NULL == CU_add_test(pSuite, "test write wtf uuid", test_write_wtf_uuid)) ||
        (NULL == CU_add_test(pSuite, "test read wtf uuid", test_read_wtf_uuid))||
        0)
    {
       CU_cleanup_registry();
       return CU_get_error();
    }
    return 0;
}

#endif
