#include "es_proto.h"
#include "es_client_internal.h"


typedef enum {
	  Success = 0,
	  PrepareTimeout = 1,
	  CommitTimeout = 2,
	  ForwardTimeout = 3,
	  WrongExpectedVersion = 4,
	  StreamDeleted = 5,
	  InvalidTransaction = 6,
	  AccessDenied = 7
} OperationResult;

typedef enum {
	DropReason_Unsubscribed = 0,
	DropReason_AccessDenied = 1,
	DropReason_NotFound=2,
	DropReason_PersistentSubscriptionDeleted=3,
	DropReason_SubscriberMaxCountReached=4
} SubscriptionDropReason;

typedef enum {
	ScavengeResult_Success = 0,
	ScavengeResult_InProgress = 1,
	ScavengeResult_Failed = 2
} ScavengeResult;


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

struct EventRecord {
	char *event_stream_id;
	int32_t event_number;
	uuid_t event_id;
	char *event_type;
	int32_t data_content_type;
	int32_t metadata_content_type;
	struct Buffer data;
	struct Buffer metadata;
	int64_t created;
	int64_t created_epoch;
};

struct ResolvedEvent {
	struct EventRecord event;
	struct EventRecord link;
	int64_t commit_position;
	int64_t prepare_position;
};

struct DeleteStream {
	char *event_stream_id;
	int32_t expected_version;
	bool require_master;
	bool hard_delete;
};

struct DeleteStreamCompleted {
	OperationResult result;
	char *message ;
	int64_t prepare_position;
	int64_t commit_position;
};

struct SubscribeToStream {
	char *event_stream_id;
	bool resolve_link_tos;
};


struct SubscriptionConfirmation {
	int64_t last_commit_position;
	int32_t last_event_number;
};

struct SubscriptionDropped {
	SubscriptionDropReason reason;
};

struct StreamEventAppeared {
	struct ResolvedEvent event;
};

struct WriteEvents {
	char *event_stream_id;
	int32_t expected_version;
	int32_t num_events;
	bool require_master;
	struct NewEvent **events;
};

struct WriteEventsCompleted {
	OperationResult result;
	char *message;
	uint32_t first_event_number;
	uint32_t last_event_number;
	int64_t prepare_position;
	int64_t commit_position;
};

struct ReadStreamEvents {
	char *event_stream_id;
	uint32_t from_event_number;
	uint32_t max_count;
	bool resolve_link_tos;
	bool require_master;
};

struct ReadAllEvents {
	uint64_t commit_position;
	uint64_t prepare_position;
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

struct DeletePersistentSubscription {
	char *subscription_group_name;
	char *event_stream_id;
};

struct TransactionCommit {
	uint64_t transaction_id;
	bool require_master;
};


struct ScavengeDatabase {
};

struct ScavengeDatabaseCompleted {
	ScavengeResult result;
	char *error;
	int32_t total_time_ms;
	int64_t total_space_saved;
};

void destroy_delete_stream (struct DeleteStream **item) {
	assert(item);
	struct DeleteStream *self = *item;
	if (self->event_stream_id) free(self->event_stream_id);
	free (self);
	*item = NULL;
}

int es_pack_delete_stream (struct DeleteStream *delete, struct Buffer buffer) {
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

struct DeleteStream *es_unpack_delete_stream (struct Buffer buffer) {
	EventStore__Client__Messages__DeleteStream *msg;
	msg = event_store__client__messages__delete_stream__unpack (NULL, buffer.length, buffer.location);
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

void destroy_delete_stream_completed (struct DeleteStreamCompleted **item) {
	assert(item);
	struct DeleteStreamCompleted *self = *item;
	if (self->message) free (self->message);
	free (self);
	*item = NULL;
}

int es_pack_delete_stream_completed (struct DeleteStreamCompleted *delete, struct Buffer buffer) {
	EventStore__Client__Messages__DeleteStreamCompleted msg = EVENT_STORE__CLIENT__MESSAGES__DELETE_STREAM_COMPLETED__INIT;
	unsigned len;

	assert (delete);
	msg.result= delete->result;
	if (delete->message)
		msg.message = delete->message;
	msg.has_prepare_position = true;
	msg.has_commit_position = true;
	msg.prepare_position = delete->prepare_position;
	msg.commit_position = delete->commit_position;
	len = event_store__client__messages__delete_stream_completed__get_packed_size (&msg);
	if (len > buffer.length)
		return 0;
	event_store__client__messages__delete_stream_completed__pack (&msg,buffer.location);
	return len;
}

struct DeleteStreamCompleted *es_unpack_delete_stream_completed (struct Buffer buffer) {
	EventStore__Client__Messages__DeleteStreamCompleted *msg;
	msg = event_store__client__messages__delete_stream_completed__unpack (NULL, buffer.length, buffer.location);
	if(msg == NULL) return NULL;
	struct DeleteStreamCompleted *ret = malloc (sizeof (struct DeleteStreamCompleted));
	ret->message = strdup (msg->message);
	ret->prepare_position = msg->prepare_position;
	ret->commit_position = msg->commit_position;
	ret->result = msg->result;
	event_store__client__messages__delete_stream_completed__free_unpacked (msg, NULL);
	return ret;
}

void destroy_subscribe_to_stream (struct SubscribeToStream **item) {
	assert(item);
	struct SubscribeToStream *self = *item;
	if (self->event_stream_id) free(self->event_stream_id);
	free (self);
	*item = NULL;
}

int es_pack_subscribe_to_stream (struct SubscribeToStream *subscribe, struct Buffer buffer) {
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

struct SubscribeToStream *es_unpack_subscribe_to_stream (struct Buffer buffer) {
	EventStore__Client__Messages__SubscribeToStream *msg;
	msg = event_store__client__messages__subscribe_to_stream__unpack (NULL, buffer.length, buffer.location);
	if(msg == NULL) return NULL;
	struct SubscribeToStream *ret = malloc (sizeof (struct SubscribeToStream));
	ret->event_stream_id = strdup (msg->event_stream_id);
	ret->resolve_link_tos = msg->resolve_link_tos;
	event_store__client__messages__subscribe_to_stream__free_unpacked (msg, NULL);
	return ret;
}

void destroy_subscription_confirmation (struct SubscriptionConfirmation **item) {
	assert(item);
	struct SubscriptionConfirmation *self = *item;
	free (self);
	*item = NULL;
}

int es_pack_subscription_confirmation (struct SubscriptionConfirmation *subscribe, struct Buffer buffer) {
	EventStore__Client__Messages__SubscriptionConfirmation msg = EVENT_STORE__CLIENT__MESSAGES__SUBSCRIPTION_CONFIRMATION__INIT;
	unsigned len;

	assert (subscribe);
	msg.last_commit_position = subscribe->last_commit_position;
	msg.has_last_event_number = true;
	msg.last_event_number = subscribe->last_event_number;
	len = event_store__client__messages__subscription_confirmation__get_packed_size (&msg);
	if (len > buffer.length)
		return 0;
	event_store__client__messages__subscription_confirmation__pack (&msg, buffer.location);
	return len;
}

struct SubscriptionConfirmation *es_unpack_subscription_confirmation (struct Buffer buffer) {
	EventStore__Client__Messages__SubscriptionConfirmation *msg;
	msg = event_store__client__messages__subscription_confirmation__unpack (NULL, buffer.length, buffer.location);
	if(msg == NULL) return NULL;
	struct SubscriptionConfirmation *ret = malloc (sizeof (struct SubscriptionConfirmation));
	ret->last_commit_position = msg->last_commit_position;
	ret->last_event_number = msg->last_event_number;
	event_store__client__messages__subscription_confirmation__free_unpacked (msg, NULL);
	return ret;
}

void destroy_subscription_dropped (struct SubscriptionDropped **item) {
	assert(item);
	struct SubscriptionDropped *self = *item;
	free (self);
	*item = NULL;
}

int es_pack_subscription_dropped (struct SubscriptionDropped *drop, struct Buffer buffer) {
	EventStore__Client__Messages__SubscriptionDropped msg = EVENT_STORE__CLIENT__MESSAGES__SUBSCRIPTION_DROPPED__INIT;
	unsigned len;

	assert (drop);
	msg.has_reason = true;
	msg.reason = drop->reason;
	len = event_store__client__messages__subscription_dropped__get_packed_size (&msg);
	if (len > buffer.length)
		return 0;
	event_store__client__messages__subscription_dropped__pack (&msg, buffer.location);
	return len;
}

struct SubscriptionDropped *es_unpack_subscription_dropped (struct Buffer buffer) {
	EventStore__Client__Messages__SubscriptionDropped *msg;
	msg = event_store__client__messages__subscription_dropped__unpack(NULL, buffer.length, buffer.location);
	if(msg == NULL) return NULL;
	struct SubscriptionDropped *ret = malloc (sizeof (struct SubscriptionDropped));
	ret->reason = msg->reason;
	event_store__client__messages__subscription_dropped__free_unpacked (msg, NULL);
	return ret;
}


void destroy_scavenge_database_completed (struct ScavengeDatabaseCompleted **item) {
	assert(item);
	struct ScavengeDatabaseCompleted *self = *item;
	free (self);
	*item = NULL;
}

int es_pack_scavenge_database_completed (struct ScavengeDatabaseCompleted *drop, struct Buffer buffer) {
	EventStore__Client__Messages__ScavengeDatabaseCompleted msg = EVENT_STORE__CLIENT__MESSAGES__SCAVENGE_DATABASE_COMPLETED__INIT;
	unsigned len;

	assert (drop);
	msg.result = drop->result;
	msg.error = drop->error;
	msg.total_space_saved = drop->total_space_saved;
	msg.total_time_ms = drop->total_time_ms;
	len = event_store__client__messages__scavenge_database_completed__get_packed_size (&msg);
	if (len > buffer.length)
		return 0;
	event_store__client__messages__scavenge_database_completed__pack (&msg, buffer.location);
	return len;
}

struct ScavengeDatabaseCompleted *es_unpack_scavenge_database_completed (struct Buffer buffer) {
	EventStore__Client__Messages__ScavengeDatabaseCompleted *msg;
	msg = event_store__client__messages__scavenge_database_completed__unpack (NULL, buffer.length, buffer.location);
	if (msg == NULL) return NULL;
	struct ScavengeDatabaseCompleted *ret = malloc (sizeof (struct ScavengeDatabaseCompleted));
	ret->result = msg->result;
	ret->error = strdup (msg->error);
	ret->total_time_ms = msg->total_time_ms;
	ret->total_space_saved = msg->total_space_saved;
	event_store__client__messages__scavenge_database_completed__free_unpacked (msg, NULL);
	return ret;
}

void destroy_scavenge_database (struct ScavengeDatabase **item) {
	assert (item);
	struct ScavengeDatabase *self = *item;
	free (self);
	*item = NULL;
}

int es_pack_scavenge_database (struct ScavengeDatabase *drop, struct Buffer buffer) {
	EventStore__Client__Messages__ScavengeDatabase msg = EVENT_STORE__CLIENT__MESSAGES__SCAVENGE_DATABASE__INIT;
	unsigned len;

	assert (drop);
	len = event_store__client__messages__scavenge_database__get_packed_size (&msg);
	if (len > buffer.length)
		return 0;
	event_store__client__messages__scavenge_database__pack (&msg, buffer.location);
	return len;
}

struct ScavengeDatabase *es_unpack_scavenge_database (struct Buffer buffer) {
	EventStore__Client__Messages__ScavengeDatabase *msg;
	msg = event_store__client__messages__scavenge_database__unpack (NULL, buffer.length, buffer.location);
	if(msg == NULL) return NULL;
	struct ScavengeDatabase *ret = malloc (sizeof (struct ScavengeDatabase));
	event_store__client__messages__scavenge_database__free_unpacked (msg, NULL);
	return ret;
}


void destroy_write_events(struct WriteEvents **item) {
	assert (item);
	struct WriteEvents *self = *item;
	if (self->event_stream_id) free(self->event_stream_id);
	if (self->num_events > 0) {
		for (int i=0;i<self->num_events;i++) {
			if(self->events[i]->event_type)
				free (self->events[i]->event_type);
			free (self->events[i]);
		}
	}
	if (self->events) free(self->events);
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
	fflush (stdout);

	msg.events = malloc (sizeof (struct EventStore__Client__Messages__NewEvent *) * write->num_events);
	msg.n_events = write->num_events;
	for(int i=0; i<write->num_events; i++) {
		struct NewEvent *cur = write->events[i];
		assert (cur->data.location);
		assert (cur->data.length > 0);
		EventStore__Client__Messages__NewEvent ev = EVENT_STORE__CLIENT__MESSAGES__NEW_EVENT__INIT;
		ev.data_content_type = cur->data_content_type;
		ev.event_type = cur->event_type;
		ev.metadata_content_type = cur->metadata_content_type;
		ev.data.data = cur->data.location;
		ev.data.len = cur->data.length;
		ev.metadata.len = cur->metadata.length;
		if(cur->metadata.length > 0) {
			ev.has_metadata = true;
			ev.metadata.data = cur->metadata.location;
		}
		EventStore__Client__Messages__NewEvent *t = malloc (sizeof (EventStore__Client__Messages__NewEvent));
		memcpy (t, &ev, sizeof (EventStore__Client__Messages__NewEvent));
		msg.events[i] = t;
	}
	len = event_store__client__messages__write_events__get_packed_size (&msg);
	if (len > buffer.length) {
		return 0;
	}
	event_store__client__messages__write_events__pack (&msg, buffer.location);
	for(int i=0;i<msg.n_events; i++) {
		free (msg.events[i]);
	}
	free (msg.events);
	return len;
}

struct WriteEvents *es_unpack_write_events(struct Buffer buffer) {
	EventStore__Client__Messages__WriteEvents *msg;
	msg = event_store__client__messages__write_events__unpack (NULL, buffer.length, buffer.location);
	if(msg == NULL) return NULL;
	struct WriteEvents *ret = malloc (sizeof (struct WriteEvents));
	ret->event_stream_id = strdup (msg->event_stream_id);
	ret->expected_version = msg->expected_version;
	ret->require_master = msg->require_master;
	ret->events = malloc (sizeof (struct NewEvent*) * msg->n_events);
	ret->num_events = msg->n_events;
	fflush (stdout);
	for(int i=0;i<msg->n_events;i++) {
		fflush (stdout);
		EventStore__Client__Messages__NewEvent *ev = msg->events[i];
		struct NewEvent *cur = malloc (sizeof(struct NewEvent));
		cur->event_type = strdup (ev->event_type);
		cur->data_content_type = ev->data_content_type;
		cur->metadata_content_type = ev->metadata_content_type;
		cur->data.location = ev->data.data;
		cur->data.length = ev->data.len;
		if(ev->has_metadata) {
			cur->metadata.location = ev->metadata.data;
			cur->metadata.length = ev->metadata.len;
		} else {
			cur->metadata.length = 0;
			cur->metadata.location = NULL;
		}
		ret->events[i] = cur;
	}
	event_store__client__messages__write_events__free_unpacked (msg, NULL);
	return ret;
}


void destroy_write_events_completed(struct WriteEventsCompleted **item) {
	assert(item);
	struct WriteEventsCompleted *self = *item;
	if (self->message) free(self->message);
	free (self);
	*item = NULL;
}

int es_pack_write_events_completed(struct WriteEventsCompleted *write, struct Buffer buffer) {
	EventStore__Client__Messages__WriteEventsCompleted msg = EVENT_STORE__CLIENT__MESSAGES__WRITE_EVENTS_COMPLETED__INIT;
	unsigned len;

	assert (write);
	msg.result = write->result;
	msg.message = write->message;
	msg.first_event_number = write->first_event_number;
	msg.last_event_number = write->last_event_number;
	msg.has_prepare_position = true;
	msg.prepare_position = write->prepare_position;
	msg.has_commit_position = true;
	msg.commit_position = write->commit_position;
	len = event_store__client__messages__write_events_completed__get_packed_size (&msg);
	if (len > buffer.length)
		return 0;
	event_store__client__messages__write_events_completed__pack (&msg,buffer.location);
	return len;
}

struct WriteEventsCompleted *es_unpack_write_events_completed (struct Buffer buffer) {
	EventStore__Client__Messages__WriteEventsCompleted *msg;
	msg = event_store__client__messages__write_events_completed__unpack (NULL, buffer.length, buffer.location);
	if(msg == NULL) return NULL;
	struct WriteEventsCompleted *ret = malloc (sizeof (struct WriteEventsCompleted));
	ret->message = strdup (msg->message);
	ret->first_event_number = msg->first_event_number;
	ret->last_event_number = msg->last_event_number;
	ret->prepare_position = msg->prepare_position;
	ret->commit_position = msg->commit_position;
	ret->result = msg->result;
	event_store__client__messages__write_events_completed__free_unpacked (msg, NULL);
	return ret;
}


void destroy_read_stream_events (struct ReadStreamEvents **item) {
	assert(item);
	struct ReadStreamEvents *self = *item;
	if (self->event_stream_id) free(self->event_stream_id);
	free (self);
	*item = NULL;
}


int es_pack_read_stream_events (struct ReadStreamEvents *read, struct Buffer buffer) {
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

struct ReadStreamEvents *es_unpack_read_stream_events (struct Buffer buffer) {
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


void destroy_read_event (struct ReadEvent **item) {
	assert(item);
	struct ReadEvent *self = *item;
	if (self->event_stream_id) free(self->event_stream_id);
	free (self);
	*item = NULL;
}


int es_pack_read_event (struct ReadEvent *read, struct Buffer buffer) {
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

struct ReadEvent *es_unpack_read_event (struct Buffer buffer) {
	EventStore__Client__Messages__ReadEvent *msg;
	msg = event_store__client__messages__read_event__unpack (NULL, buffer.length, buffer.location);
	if(msg == NULL) return NULL;
	struct ReadEvent *ret = malloc (sizeof (struct ReadEvent));
	ret->event_stream_id = strdup (msg->event_stream_id);
	ret->event_number = msg->event_number;
	ret->require_master = msg->require_master;
	ret->resolve_link_tos = msg->resolve_link_tos;
	event_store__client__messages__read_event__free_unpacked (msg, NULL);
	return ret;
}


void destroy_read_all_events (struct ReadAllEvents **item) {
	assert(item);
	struct ReadAllEvents *self = *item;
	free (self);
	*item = NULL;
}


int es_pack_read_all_events (struct ReadAllEvents *read, struct Buffer buffer) {
	EventStore__Client__Messages__ReadAllEvents msg = EVENT_STORE__CLIENT__MESSAGES__READ_ALL_EVENTS__INIT;
	unsigned len;

	assert (read);
	msg.prepare_position = read->prepare_position;
	msg.commit_position = read->commit_position;
	msg.max_count = read->max_count;
	msg.resolve_link_tos = read->resolve_link_tos;
	msg.require_master = read->require_master;
	len = event_store__client__messages__read_all_events__get_packed_size (&msg);
	if (len > buffer.length)
		return 0;
	event_store__client__messages__read_all_events__pack (&msg,buffer.location);
	return len;
}

struct ReadAllEvents *es_unpack_read_all_events (struct Buffer buffer) {
	EventStore__Client__Messages__ReadAllEvents *msg;
	msg = event_store__client__messages__read_all_events__unpack (NULL, buffer.length, buffer.location);
	if(msg == NULL) return NULL;
	struct ReadAllEvents *ret = malloc (sizeof (struct ReadAllEvents));
	ret->commit_position = msg->commit_position;
	ret->prepare_position = msg->prepare_position;
	ret->require_master = msg->require_master;
	ret->resolve_link_tos = msg->resolve_link_tos;
	ret->max_count = msg->max_count;
	event_store__client__messages__read_all_events__free_unpacked (msg, NULL);
	return ret;
}

void destroy_delete_persistent_subscription (struct DeletePersistentSubscription **item) {
	assert(item);
	struct DeletePersistentSubscription *self = *item;
	if (self->event_stream_id) free (self->event_stream_id);
	if (self->subscription_group_name) free (self->subscription_group_name);
	free (self);
	*item = NULL;
}


int es_pack_delete_persistent_subscription (struct DeletePersistentSubscription *read, struct Buffer buffer) {
	EventStore__Client__Messages__DeletePersistentSubscription msg = EVENT_STORE__CLIENT__MESSAGES__DELETE_PERSISTENT_SUBSCRIPTION__INIT;
	unsigned len;

	assert (read);
	msg.event_stream_id = read->event_stream_id;
	msg.subscription_group_name = read->subscription_group_name;
	len = event_store__client__messages__delete_persistent_subscription__get_packed_size (&msg);
	if (len > buffer.length)
		return 0;
	event_store__client__messages__delete_persistent_subscription__pack (&msg,buffer.location);
	return len;
}

struct DeletePersistentSubscription *es_unpack_delete_persistent_subscription (struct Buffer buffer) {
	EventStore__Client__Messages__DeletePersistentSubscription *msg;
	msg = event_store__client__messages__delete_persistent_subscription__unpack(NULL, buffer.length, buffer.location);
	if(msg == NULL) return NULL;
	struct DeletePersistentSubscription *ret = malloc (sizeof (struct DeletePersistentSubscription));
	ret->event_stream_id = strdup (msg->event_stream_id);
	ret->subscription_group_name = strdup (msg->subscription_group_name);
	event_store__client__messages__delete_persistent_subscription__free_unpacked (msg, NULL);
	return ret;
}

void destroy_transaction_commit (struct TransactionCommit **item) {
	assert(item);
	struct TransactionCommit *self = *item;
	free (self);
	*item = NULL;
}


int es_pack_transaction_commit (struct TransactionCommit *tran, struct Buffer buffer) {
	EventStore__Client__Messages__TransactionCommit msg = EVENT_STORE__CLIENT__MESSAGES__TRANSACTION_COMMIT__INIT;
	unsigned len;

	assert (tran);
	msg.transaction_id = tran->transaction_id;
	msg.require_master = tran->require_master;
	len = event_store__client__messages__transaction_commit__get_packed_size (&msg);
	if (len > buffer.length)
		return 0;
	event_store__client__messages__transaction_commit__pack (&msg,buffer.location);
	return len;
}

struct TransactionCommit *es_unpack_transaction_commit (struct Buffer buffer) {
	EventStore__Client__Messages__TransactionCommit *msg;
	msg = event_store__client__messages__transaction_commit__unpack (NULL, buffer.length, buffer.location);
	if(msg == NULL) return NULL;
	struct TransactionCommit *ret = malloc (sizeof (struct TransactionCommit));
	ret->transaction_id = msg->transaction_id;
	ret->require_master = msg->require_master;
	event_store__client__messages__transaction_commit__free_unpacked (msg, NULL);
	return ret;
}

#ifdef TESTS
struct Buffer get_test_buffer (int size) {
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

void test_delete_stream_completed (void) {
	struct DeleteStreamCompleted d;
	d.message = "testing";
	d.result = 1;
	d.prepare_position = 17;
	d.commit_position = 19;
	struct Buffer buffer = get_test_buffer(1024);
	int32_t len = es_pack_delete_stream_completed (&d, buffer);
	buffer.length = len;
	struct DeleteStreamCompleted *msg = es_unpack_delete_stream_completed (buffer);
	CU_ASSERT_PTR_NOT_NULL_FATAL (msg);
	CU_ASSERT_STRING_EQUAL ("testing", msg->message);
	CU_ASSERT_EQUAL (msg->prepare_position, 17);
	CU_ASSERT_EQUAL (msg->commit_position, 19);
	CU_ASSERT_EQUAL (msg->result, 1);
	destroy_delete_stream_completed (&msg);
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

void test_subscription_confirmation (void) {
	struct SubscriptionConfirmation d;
	d.last_commit_position = 1919;
	d.last_commit_position = 1919;
	d.last_event_number = 6;
	struct Buffer buffer = get_test_buffer(1024);
	int32_t len = es_pack_subscription_confirmation (&d, buffer);
	buffer.length = len;
	struct SubscriptionConfirmation *msg = es_unpack_subscription_confirmation (buffer);
	CU_ASSERT_PTR_NOT_NULL_FATAL (msg);
	CU_ASSERT_EQUAL (1919, msg->last_commit_position);
	CU_ASSERT_EQUAL (6, msg->last_event_number);
	destroy_subscription_confirmation (&msg);
	free (buffer.location);
}

void test_subscription_dropped (void) {
	struct SubscriptionDropped d;
	d.reason = DropReason_NotFound;
	struct Buffer buffer = get_test_buffer(1024);
	int32_t len = es_pack_subscription_dropped (&d, buffer);
	buffer.length = len;
	struct SubscriptionDropped *msg = es_unpack_subscription_dropped (buffer);
	CU_ASSERT_PTR_NOT_NULL_FATAL (msg);
	CU_ASSERT_EQUAL (DropReason_NotFound, msg->reason);
	destroy_subscription_dropped (&msg);
	free (buffer.location);
}

void test_scavenge_database (void) {
	struct ScavengeDatabase d;
	struct Buffer buffer = get_test_buffer (1024);
	int32_t len = es_pack_scavenge_database (&d, buffer);
	buffer.length = len;
	struct ScavengeDatabase *msg = es_unpack_scavenge_database (buffer);
	CU_ASSERT_PTR_NOT_NULL_FATAL (msg);
	destroy_scavenge_database (&msg);
	free (buffer.location);
}

void test_scavenge_database_completed (void) {
	struct ScavengeDatabaseCompleted d;
	struct Buffer buffer = get_test_buffer (1024);
	d.total_space_saved = 1234;
	d.total_time_ms = 17;
	d.error = "test error";
	d.result = ScavengeResult_InProgress;
	int32_t len = es_pack_scavenge_database_completed (&d, buffer);
	buffer.length = len;
	struct ScavengeDatabaseCompleted *msg = es_unpack_scavenge_database_completed (buffer);
	CU_ASSERT_PTR_NOT_NULL_FATAL (msg);
	CU_ASSERT_EQUAL (1234, msg->total_space_saved);
	CU_ASSERT_EQUAL (17, msg->total_time_ms);
	CU_ASSERT_EQUAL (ScavengeResult_InProgress, msg->result);
	CU_ASSERT_STRING_EQUAL ("test error", msg->error);
	destroy_scavenge_database_completed (&msg);
	free (buffer.location);
}

void test_write_events_completed (void) {
	struct WriteEventsCompleted d;
	d.message = "testing";
	d.first_event_number = 1;
	d.last_event_number = 3;
	d.prepare_position = 17;
	d.commit_position = 19;
	d.result = 1;
	struct Buffer buffer = get_test_buffer(1024);
	int32_t len = es_pack_write_events_completed (&d, buffer);
	buffer.length = len;
	struct WriteEventsCompleted *msg = es_unpack_write_events_completed (buffer);
	CU_ASSERT_PTR_NOT_NULL_FATAL (msg);
	CU_ASSERT_STRING_EQUAL ("testing", msg->message);
	CU_ASSERT_EQUAL (msg->first_event_number, 1);
	CU_ASSERT_EQUAL (msg->last_event_number, 3);
	CU_ASSERT_EQUAL (msg->prepare_position, 17);
	CU_ASSERT_EQUAL (msg->commit_position, 19);
	CU_ASSERT_EQUAL (msg->result, 1);
	destroy_write_events_completed (&msg);
	free (buffer.location);
}

void test_write_events (void) {
	struct WriteEvents r;
	unsigned char data[16] = {0x46, 0x6c,0xbc, 0x3e, 0x72,0xe2, 0x26, 0x42, 0xbc,0xb5,0xaa,0x93,0xc4,0x11,0xed,0x0d };
	struct Buffer buffer = get_test_buffer(1024);
	r.event_stream_id = "test";
	r.expected_version = 19;
	r.require_master = true;
	r.num_events = 2;
	r.events = malloc (sizeof (struct NewEvent*) * 2);
	struct NewEvent item;
	uuid_generate(item.event_id);
	item.event_type = "ev1";
	item.data_content_type = 1;
	item.metadata_content_type = 2;
	item.data.location = (unsigned char *) &data;
	item.data.length = 5;
	item.metadata.location = NULL;
	item.metadata.length = 0;
	r.events[0] = &item;
	struct NewEvent item2;
	uuid_generate(item2.event_id);
	item2.event_type = "ev2";
	item2.data_content_type = 2;
	item2.metadata_content_type = 3;
	item2.data.location = (unsigned char *) &data;
	item2.data.length = 17;
	item2.metadata.location = (unsigned char *) &data;
	item2.metadata.length = 5;
	r.events[1] = &item2;
	int len = es_pack_write_events (&r, buffer);
	free (r.events);
	buffer.length = len;
	struct WriteEvents *msg = es_unpack_write_events(buffer);
	CU_ASSERT_PTR_NOT_NULL_FATAL (msg);
	CU_ASSERT_STRING_EQUAL ("test", msg->event_stream_id);
	CU_ASSERT_EQUAL (19, msg->expected_version);
	CU_ASSERT (msg->require_master);
	CU_ASSERT_EQUAL (2, msg->num_events);
	CU_ASSERT_PTR_NOT_NULL_FATAL (msg->events);

	struct NewEvent *foo = msg->events[0];
	CU_ASSERT_STRING_EQUAL ("ev1", foo->event_type);
	CU_ASSERT_EQUAL (1, foo->data_content_type);
	CU_ASSERT_EQUAL (2, foo->metadata_content_type);
	CU_ASSERT_EQUAL (5, foo->data.length);
	CU_ASSERT_EQUAL (0, foo->metadata.length);
	foo = msg->events[1];
	CU_ASSERT_STRING_EQUAL ("ev2", foo->event_type);
	CU_ASSERT_EQUAL (2, foo->data_content_type);
	CU_ASSERT_EQUAL (3, foo->metadata_content_type);
	CU_ASSERT_EQUAL (17, foo->data.length);
	CU_ASSERT_EQUAL (5, foo->metadata.length);
	destroy_write_events (&msg);
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

void test_read_all_events (void) {
	struct ReadAllEvents r;
	r.prepare_position = 1001;
	r.commit_position = 1002;
	r.max_count = 10;
	r.require_master = true;
	r.resolve_link_tos = true;
	struct Buffer buffer = get_test_buffer (1024);
	int32_t len = es_pack_read_all_events (&r, buffer);
	buffer.length = len;
	struct ReadAllEvents *msg = es_unpack_read_all_events (buffer);
	CU_ASSERT_PTR_NOT_NULL_FATAL (msg);
	CU_ASSERT_EQUAL (1001, msg->prepare_position);
	CU_ASSERT_EQUAL (1002, msg->commit_position);
	CU_ASSERT_EQUAL (10, msg->max_count);
	CU_ASSERT (msg->resolve_link_tos);
	CU_ASSERT (msg->require_master);
	destroy_read_all_events (&msg);
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

void test_delete_persistent_subscription (void) {
	struct DeletePersistentSubscription d;
	d.event_stream_id = "testing";
	d.subscription_group_name = "group";
	struct Buffer buffer = get_test_buffer(1024);
	int32_t len = es_pack_delete_persistent_subscription (&d, buffer);
	buffer.length = len;
	struct DeletePersistentSubscription *msg = es_unpack_delete_persistent_subscription (buffer);
	CU_ASSERT_PTR_NOT_NULL_FATAL (msg);
	CU_ASSERT_STRING_EQUAL ("testing", msg->event_stream_id);
	CU_ASSERT_STRING_EQUAL ("group", msg->subscription_group_name);
	destroy_delete_persistent_subscription (&msg);
	free (buffer.location);
}

void test_transaction_commit (void) {
	struct TransactionCommit d;
	d.transaction_id = 123456;
	d.require_master = true;
	struct Buffer buffer = get_test_buffer(1024);
	int32_t len = es_pack_transaction_commit (&d, buffer);
	buffer.length = len;
	struct TransactionCommit *msg = es_unpack_transaction_commit (buffer);
	CU_ASSERT_PTR_NOT_NULL_FATAL (msg);
	CU_ASSERT_EQUAL (123456, msg->transaction_id);
	CU_ASSERT (msg->require_master);
	destroy_transaction_commit (&msg);
	free (buffer.location);
}

int register_es_proto_helper_tests() {
   CU_pSuite pSuite = NULL;
    pSuite = CU_add_suite ("proto serialization tests", NULL, NULL);
    if (NULL == pSuite) {
       CU_cleanup_registry ();
       return CU_get_error ();
    }

    if ((NULL == CU_add_test(pSuite, "test proto DeleteStream", test_delete_stream)) ||
        (NULL == CU_add_test(pSuite, "test proto SubscribeToStream", test_subscribe_to_stream))||
        (NULL == CU_add_test(pSuite, "test proto ReadStreamEvents", test_read_stream_events))||
        (NULL == CU_add_test(pSuite, "test proto ReadAllEvents", test_read_all_events))||
        (NULL == CU_add_test(pSuite, "test proto ReadEvent", test_read_event))||
        (NULL == CU_add_test(pSuite, "test proto DeletePersistentSubscription", test_delete_persistent_subscription))||
        (NULL == CU_add_test(pSuite, "test proto TransactionCommit", test_transaction_commit))||
        (NULL == CU_add_test(pSuite, "test proto WriteEvents", test_write_events))||
        (NULL == CU_add_test(pSuite, "test proto DeleteStreamCompleted", test_delete_stream_completed))||
        (NULL == CU_add_test(pSuite, "test proto WriteEventsCompleted", test_write_events_completed))||
        (NULL == CU_add_test(pSuite, "test proto SubscriptionConfirmation", test_subscription_confirmation))||
        (NULL == CU_add_test(pSuite, "test proto ScavengeDatabase", test_scavenge_database))||
        (NULL == CU_add_test(pSuite, "test proto ScavengeDatabaseCompleted", test_scavenge_database_completed))||
        0)
    {
       CU_cleanup_registry ();
       return CU_get_error ();
    }
    return 0;
}
#endif