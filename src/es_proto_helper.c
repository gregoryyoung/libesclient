#include "es_proto.h"
#include "es_client_internal.h"


struct Buffer {
	int length;
	unsigned char *location;
};

struct NewEvent {
	uuid_t event_id;
	char *event_type;
	int32_t data_content_type;
	int32_t metadata_content_type;
	struct Buffer data;
	struct Buffer metadata;
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

struct WriteEvents {
	char *event_stream_id;
	int32_t expected_version;
	int32_t num_events;
	bool require_master;
	struct NewEvent *events;
};

struct ReadStreamEvents {
	char *event_stream_id;
	uint32_t from_event_number;
	uint32_t max_count;
	bool resolve_link_tos;
	bool require_master;
};

struct ReadEvent {
	char *event_stream_id;
	uint32_t event_number;
	bool resolve_link_tos;
	bool require_master;
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
	msg.has_hard_delete = true;
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
	ret->hard_delete = false;
	if(msg->has_hard_delete) {
		ret->hard_delete = msg->hard_delete;
	}
	event_store__client__messages__delete_stream__free_unpacked (msg, NULL);
	return ret;
}


void destroy_subscribe_to_stream(struct SubscribeToStream **item) {
	assert(item);
	struct SubscribeToStream *self = *item;
	if (self->event_stream_id) free(self->event_stream_id);
	free (self);
	*item = NULL;
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

void destroy_write_events(struct WriteEvents **item) {
	assert(item);
	struct WriteEvents *self = *item;
	if (self->event_stream_id) free(self->event_stream_id);
	free (self);
	*item = NULL;
}


int es_pack_write_events(struct WriteEvents *write, struct Buffer buffer) {
	EventStore__Client__Messages__WriteEvents msg = EVENT_STORE__CLIENT__MESSAGES__WRITE_EVENTS__INIT;
	unsigned len;

	assert (write);
	assert (write->num_events >= 0);
	msg.event_stream_id = write->event_stream_id;
	msg.expected_version = write->expected_version;
	msg.require_master = write->require_master;
	msg.events = malloc (sizeof (struct EventStore__Client__Messages__NewEvent *) * write->num_events);
	for(int i=0; i<write->num_events; i++) {
		struct NewEvent *cur = &write->events[i];
		assert (cur->data.location);
		assert (cur->data.length > 0);
		EventStore__Client__Messages__NewEvent ev = EVENT_STORE__CLIENT__MESSAGES__NEW_EVENT__INIT;
		ev.data_content_type = cur->data_content_type;
		ev.metadata_content_type = cur->metadata_content_type;
		ev.data.data = cur->data.location;
		ev.data.len = cur->data.length;
		if(cur->metadata.length > 0) {
			ev.metadata.data = cur->metadata.location;
			ev.metadata.len = cur->metadata.length;
		}
		msg.events[i] = &ev;
	}
	len = event_store__client__messages__write_events__get_packed_size (&msg);
	if (len > buffer.length) {
		free (msg.events);
		return 0;
	}
	event_store__client__messages__write_events__pack (&msg, buffer.location);
	free (msg.events);
	return 0;
}


void destroy_read_stream_events(struct ReadStreamEvents **item) {
	assert(item);
	struct ReadStreamEvents *self = *item;
	if (self->event_stream_id) free(self->event_stream_id);
	free (self);
	*item = NULL;
}


int es_pack_read_stream_events(struct ReadStreamEvents *read, struct Buffer buffer) {
	EventStore__Client__Messages__ReadStreamEvents msg = EVENT_STORE__CLIENT__MESSAGES__READ_STREAM_EVENTS__INIT;
	unsigned len;

	assert (read);
	assert (read->event_stream_id);
	msg.event_stream_id = read->event_stream_id;
	msg.from_event_number = read->from_event_number;
	msg.max_count = read->max_count;
	msg.resolve_link_tos = read->resolve_link_tos;
	msg.require_master = read->require_master;
	len = event_store__client__messages__read_stream_events__get_packed_size (&msg);
	if (len > buffer.length)
		return 0;
	event_store__client__messages__read_stream_events__pack (&msg,buffer.location);
	return len;
}

struct ReadStreamEvents *es_unpack_read_stream_events(struct Buffer buffer) {
	EventStore__Client__Messages__ReadStreamEvents *msg;
	msg = event_store__client__messages__read_stream_events__unpack(NULL, buffer.length, buffer.location);
	if(msg == NULL) return NULL;
	struct ReadStreamEvents *ret = malloc (sizeof (struct ReadStreamEvents));
	ret->event_stream_id = strdup (msg->event_stream_id);
	ret->from_event_number = msg->from_event_number;
	ret->require_master = msg->require_master;
	ret->resolve_link_tos = msg->resolve_link_tos;
	ret->max_count = msg->max_count;
	event_store__client__messages__read_stream_events__free_unpacked (msg, NULL);
	return ret;
}


void destroy_read_event(struct ReadEvent **item) {
	assert(item);
	struct ReadEvent *self = *item;
	if (self->event_stream_id) free(self->event_stream_id);
	free (self);
	*item = NULL;
}


int es_pack_read_event(struct ReadEvent *read, struct Buffer buffer) {
	EventStore__Client__Messages__ReadEvent msg = EVENT_STORE__CLIENT__MESSAGES__READ_EVENT__INIT;
	unsigned len;

	assert (read);
	assert (read->event_stream_id);
	msg.event_stream_id = read->event_stream_id;
	msg.event_number = read->event_number;
	msg.resolve_link_tos = read->resolve_link_tos;
	msg.require_master = read->require_master;
	len = event_store__client__messages__read_event__get_packed_size (&msg);
	if (len > buffer.length)
		return 0;
	event_store__client__messages__read_event__pack (&msg,buffer.location);
	return len;
}

struct ReadEvent *es_unpack_read_event(struct Buffer buffer) {
	EventStore__Client__Messages__ReadEvent *msg;
	msg = event_store__client__messages__read_event__unpack(NULL, buffer.length, buffer.location);
	if(msg == NULL) return NULL;
	struct ReadEvent *ret = malloc (sizeof (struct ReadEvent));
	ret->event_stream_id = strdup (msg->event_stream_id);
	ret->event_number = msg->event_number;
	ret->require_master = msg->require_master;
	ret->resolve_link_tos = msg->resolve_link_tos;
	event_store__client__messages__read_event__free_unpacked (msg, NULL);
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
	int32_t len = es_pack_delete_stream (&d, buffer);
	buffer.length = len;
	struct DeleteStream *msg = es_unpack_delete_stream (buffer);
	CU_ASSERT_PTR_NOT_NULL_FATAL (msg);
	CU_ASSERT_STRING_EQUAL ("testing", msg->event_stream_id);
	CU_ASSERT_EQUAL (msg->expected_version, -1);
	CU_ASSERT (msg->require_master);
	CU_ASSERT (msg->hard_delete);
	destroy_delete_stream (&msg);
	free (buffer.location);
}

void test_subscribe_to_stream (void) {
	struct SubscribeToStream d;
	d.event_stream_id = "testing";
	d.resolve_link_tos = true;
	struct Buffer buffer = get_test_buffer(1024);
	int32_t len = es_pack_subscribe_to_stream (&d, buffer);
	buffer.length = len;
	struct SubscribeToStream *msg = es_unpack_subscribe_to_stream (buffer);
	CU_ASSERT_PTR_NOT_NULL_FATAL (msg);
	CU_ASSERT_STRING_EQUAL ("testing", msg->event_stream_id);
	CU_ASSERT (msg->resolve_link_tos);
	destroy_subscribe_to_stream (&msg);
	free (buffer.location);
}

void test_read_stream_events (void) {
	struct ReadStreamEvents r;
	r.event_stream_id = "testing";
	r.from_event_number = 19;
	r.max_count = 10;
	r.require_master = true;
	r.resolve_link_tos = true;
	struct Buffer buffer = get_test_buffer (1024);
	int32_t len = es_pack_read_stream_events (&r, buffer);
	buffer.length = len;
	struct ReadStreamEvents *msg = es_unpack_read_stream_events (buffer);
	CU_ASSERT_PTR_NOT_NULL_FATAL (msg);
	CU_ASSERT_STRING_EQUAL ("testing", msg->event_stream_id);
	CU_ASSERT_EQUAL (19, msg->from_event_number);
	CU_ASSERT_EQUAL (10, msg->max_count);
	CU_ASSERT (msg->resolve_link_tos);
	CU_ASSERT (msg->require_master);
	destroy_read_stream_events (&msg);
	free (buffer.location);
}

void test_read_event (void) {
	struct ReadEvent r;
	r.event_stream_id = "testing";
	r.event_number = 19;
	r.require_master = true;
	struct Buffer buffer = get_test_buffer (1024);
	int32_t len = es_pack_read_event (&r, buffer);
	buffer.length = len;
	struct ReadEvent *msg = es_unpack_read_event (buffer);
	CU_ASSERT_PTR_NOT_NULL_FATAL (msg);
	CU_ASSERT_STRING_EQUAL ("testing", msg->event_stream_id);
	CU_ASSERT_EQUAL (19, msg->event_number);
	CU_ASSERT (msg->resolve_link_tos);
	CU_ASSERT (msg->require_master);
	destroy_read_event (&msg);
	free (buffer.location);
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
        (NULL == CU_add_test(pSuite, "test proto ReadStreamEvents", test_read_stream_events))||
        (NULL == CU_add_test(pSuite, "test proto ReadEvent", test_read_event))||
        0)
    {
       CU_cleanup_registry();
       return CU_get_error();
    }
    return 0;
}
