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

struct SubscribeToStream {
	char *event_stream_id;
	bool resolve_link_tos;
};

void destroy_delete_stream(struct DeleteStream **item) {
	assert(item);
	struct DeleteStream *self = *item;
	if (self->event_stream_id) free(self->event_stream_id);
	free (self);
	*item = NULL;
}

int es_pack_delete_stream(struct DeleteStream *delete, struct Buffer buffer) {
	EventStore__Client__Messages__DeleteStream msg = EVENT_STORE__CLIENT__MESSAGES__DELETE_STREAM__INIT;
	unsigned len;

	assert (delete);
	msg.event_stream_id = delete->event_stream_id;
	msg.expected_version = delete->expected_version;
	msg.require_master = delete->require_master;
	msg.hard_delete = delete->hard_delete;
	len = event_store__client__messages__delete_stream__get_packed_size (&msg);
	if (len > buffer.length)
		return 0;
	event_store__client__messages__delete_stream__pack (&msg,buffer.location);
	return len;
}

struct DeleteStream *es_unpack_delete_stream(struct Buffer buffer) {
	EventStore__Client__Messages__DeleteStream *msg;
	msg = event_store__client__messages__delete_stream__unpack(NULL, buffer.length, buffer.location);
	if(msg == NULL) return NULL;
	struct DeleteStream *ret = malloc (sizeof (struct DeleteStream));
	ret->event_stream_id = strdup (msg->event_stream_id);
	ret->expected_version = msg->expected_version;
	ret->require_master = msg->require_master;
	ret->hard_delete = true;
	if(msg->has_hard_delete) {
		ret->hard_delete = msg->hard_delete;
	}
	event_store__client__messages__delete_stream__free_unpacked (msg, NULL);
	return ret;
}

int es_pack_subscribe_to_stream(struct SubscribeToStream *subscribe, struct Buffer buffer) {
	EventStore__Client__Messages__SubscribeToStream msg = EVENT_STORE__CLIENT__MESSAGES__SUBSCRIBE_TO_STREAM__INIT;
	unsigned len;

	assert (subscribe);
	msg.event_stream_id = subscribe->event_stream_id;
	msg.resolve_link_tos = subscribe->resolve_link_tos;
	len = event_store__client__messages__subscribe_to_stream__get_packed_size (&msg);
	if (len > buffer.length)
		return 0;
	event_store__client__messages__subscribe_to_stream__pack (&msg, buffer.location);
	return len;
}

void destroy_subscribe_to_stream(struct SubscribeToStream **item) {
	assert(item);
	struct SubscribeToStream *self = *item;
	if (self->event_stream_id) free(self->event_stream_id);
	free (self);
	*item = NULL;
}

struct SubscribeToStream *es_unpack_subscribe_to_stream(struct Buffer buffer) {
	EventStore__Client__Messages__SubscribeToStream *msg;
	msg = event_store__client__messages__subscribe_to_stream__unpack(NULL, buffer.length, buffer.location);
	if(msg == NULL) return NULL;
	struct SubscribeToStream *ret = malloc (sizeof (struct SubscribeToStream));
	ret->event_stream_id = strdup (msg->event_stream_id);
	ret->resolve_link_tos = msg->resolve_link_tos;
	event_store__client__messages__subscribe_to_stream__free_unpacked (msg, NULL);
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
	int len = es_pack_delete_stream (&d, buffer);
	buffer.length = len;
	struct DeleteStream *msg = es_unpack_delete_stream (buffer);
	CU_ASSERT_PTR_NOT_NULL_FATAL (msg);
	CU_ASSERT_STRING_EQUAL ("testing", msg->event_stream_id);
	CU_ASSERT_EQUAL (d.expected_version, -1);
	CU_ASSERT (d.require_master);
	CU_ASSERT (d.hard_delete);
	destroy_delete_stream (&msg);
	free(buffer.location);
}


void test_subscribe_to_stream (void) {
	struct SubscribeToStream d;
	d.event_stream_id = "testing";
	d.resolve_link_tos = true;
	struct Buffer buffer = get_test_buffer(1024);
	int len = es_pack_subscribe_to_stream (&d, buffer);
	buffer.length = len;
	struct SubscribeToStream *msg = es_unpack_subscribe_to_stream (buffer);
	CU_ASSERT_PTR_NOT_NULL_FATAL (msg);
	CU_ASSERT_STRING_EQUAL ("testing", msg->event_stream_id);
	CU_ASSERT (d.resolve_link_tos);
	destroy_subscribe_to_stream (&msg);
	free(buffer.location);
}

int register_es_proto_helper_tests() {
   CU_pSuite pSuite = NULL;
    pSuite = CU_add_suite("proto serialization tests", NULL, NULL);
    if (NULL == pSuite) {
       CU_cleanup_registry();
       return CU_get_error();
    }

    if ((NULL == CU_add_test(pSuite, "test proto DeleteStream", test_delete_stream)) ||
        (NULL == CU_add_test(pSuite, "test proto SubscribeToStream", test_subscribe_to_stream))||
        0)
    {
       CU_cleanup_registry();
       return CU_get_error();
    }
    return 0;
}
