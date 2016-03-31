#include <string.h>
#include "es_proto.h"
#include "es_client_internal.h"


struct Buffer {
	int length;
	unsigned char *location;
};

struct DeleteStream {
	char *event_stream_id;
	int32_t expected_version;
	bool require_master;
	bool hard_delete;
};

int es_pack_delete_stream(struct DeleteStream delete, struct Buffer buffer) {
	EventStore__Client__Messages__DeleteStream msg = EVENT_STORE__CLIENT__MESSAGES__DELETE_STREAM__INIT;
	unsigned len;

	msg.event_stream_id = delete.event_stream_id;
	msg.expected_version = delete.expected_version;
	msg.require_master = delete.require_master;
	msg.hard_delete = delete.hard_delete;
	len = event_store__client__messages__delete_stream__get_packed_size(&msg);
	if (len > buffer.length)
		return 0;
	event_store__client__messages__delete_stream__pack(&msg,buffer.location);
	return len;
}

struct DeleteStream *es_unpack_delete_stream(struct Buffer buffer) {
	EventStore__Client__Messages__DeleteStream *msg;
	msg = event_store__client__messages__delete_stream__unpack(NULL, buffer.length, buffer.location);
	if(msg == NULL) return NULL;
	struct DeleteStream *ret = malloc(sizeof(struct DeleteStream));
	ret->event_stream_id = strdup(msg->event_stream_id);
	ret->expected_version = msg->expected_version;
	ret->require_master = msg->require_master;
	ret->hard_delete = true;
	if(msg->has_hard_delete) {
		ret->hard_delete = msg->hard_delete;
	}
	event_store__client__messages__delete_stream__free_unpacked (msg, NULL);
	return ret;
}

struct Buffer get_test_buffer(int size) {
	struct Buffer buffer;
	buffer.location = malloc(size);
	buffer.length = size;
	return buffer;
}

void test_delete_stream (void) {
	struct DeleteStream d;
	d.event_stream_id = "testing";
	d.expected_version = -1;
	d.require_master = true;
	d.hard_delete = true;
	struct Buffer buffer = get_test_buffer(1024);
	int len = es_pack_delete_stream (d, buffer);
	buffer.length = len;
	struct DeleteStream *msg;
	msg = es_unpack_delete_stream (buffer);
	CU_ASSERT_PTR_NOT_NULL_FATAL (msg);
	CU_ASSERT_STRING_EQUAL ("testing", msg->event_stream_id);
	CU_ASSERT_EQUAL (d.expected_version, -1);
	CU_ASSERT (d.require_master);
	CU_ASSERT (d.hard_delete);
	free(buffer.location);
}

int register_es_proto_helper_tests() {
   CU_pSuite pSuite = NULL;
    pSuite = CU_add_suite("proto serialiation tests", NULL, NULL);
    if (NULL == pSuite) {
       CU_cleanup_registry();
       return CU_get_error();
    }

    if ((NULL == CU_add_test(pSuite, "test proto DeleteStream", test_delete_stream)) ||
        //(NULL == CU_add_test(pSuite, "test read wtf uuid", test_read_wtf_uuid))||
        0)
    {
       CU_cleanup_registry();
       return CU_get_error();
    }
    return 0;
}
