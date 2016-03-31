#include <assert.h>
#include <netinet/in.h>
#include "es_client_internal.h"

struct ParserState {
    char *parser_read;
    char *buffer_end;
    char *buffer_start;
    char *buffer_write;
};

struct Header {
    char command;
    char flags;
    uuid_t *correlation_id;
    char *username;
    char *password;
};

struct Buffer {
    int length;
    char *location;
};

struct Header * create_header (const char command, const char flags, const uuid_t correlation_id, const char *username, const char *password) {
    struct Header *ret = malloc (sizeof (struct Header));
    ret->correlation_id = malloc (sizeof (uuid_t));
    ret->command = command;
    ret->flags = flags;
    uuid_copy(*(ret->correlation_id), correlation_id);
    ret->username = NULL;
    ret->password = NULL;
    if(username != NULL) ret->username = strdup(username);
    if(password != NULL) ret->password = strdup(password);
    return ret;
}

void destroy_header (struct Header **header) {
    assert (header);
    if(*header) {
        struct Header *self = *header;
        if(self->username) free(self->username);
        if(self->password) free(self->password);
        free(self);
        *header = NULL;
    }
}

struct ParserState * create_parser_state (int buffer_size) {
    struct ParserState *ret = malloc ( sizeof ( struct ParserState));
    ret ->buffer_start = malloc (buffer_size);
    ret->buffer_end = ret->buffer_start + buffer_size;
    ret->parser_read = ret->buffer_start;
    ret->buffer_write = ret->buffer_start;
    return ret;
}

void destroy_parser_state (struct ParserState **state) {
    assert (state);
    if (*state) {
        struct ParserState *self = *state;
        free(self->buffer_start);
        free(self);
        *state = NULL;
    }
}

#define COMMANDOFFSET 0
#define FLAGSOFFSET COMMANDOFFSET + 1
#define CORRELATIONOFFSET FLAGSOFFSET + 1
#define AUTHOFFSET CORRELATIONOFFSET + 16
#define MANDATORYSIZE AUTHOFFSET


int read_header (struct Buffer *buffer, struct Header **header) {
    //buffer has length prefix removed
    if (buffer->length < MANDATORYSIZE) return 0;
    char command = buffer->location[COMMANDOFFSET];
    char flags = buffer->location[FLAGSOFFSET];
    uuid_t *correlation_id = get_uuid_from_wtf(buffer->location + CORRELATIONOFFSET);
    char *username = NULL;
    char *password = NULL;
    struct Header *local = create_header(command, flags, *correlation_id, username, password);
    free (correlation_id);
    *header = local;

    return 1;
}

int32_t write_header (struct Header *header, unsigned char *buffer) {
    buffer[COMMANDOFFSET] = header->command;
    buffer[FLAGSOFFSET] = header->flags;
    write_uuid_to_wtf (*(header->correlation_id), buffer + CORRELATIONOFFSET);
    return AUTHOFFSET;
}

char * get_string_for_header(struct Header *header) {
    char *ret = malloc(1024);
    char uuid_str[37];
    char *cmd_str = get_string_for_tcp_message(header->command);
    uuid_unparse(*header->correlation_id, uuid_str);
    sprintf (ret, "Command: %s, flags %d, correlation_id: %s\n", cmd_str, header->flags, uuid_str);
    return ret;
}

struct Buffer * read_next (struct ParserState *state) {
    assert (state);
    int32_t length = 0;
    if(state->buffer_write - state->parser_read < 4) return NULL;
    char *i = state->parser_read;
    //TODO in one operation?
    length = (i[0] << 24) | (i[1] << 16) | (i[2] << 8) | i[3];
    length = ntohl(length);
    if(state->buffer_write - state->parser_read < length + 4) return NULL;
    struct Buffer *ret = malloc(sizeof(struct Buffer));
    ret->length = length;
    ret->location = state->parser_read + 4;
    state->parser_read = state->parser_read + length + 4;
    return ret;
}

int32_t add_data(struct ParserState *state, struct Buffer *data) {
    assert (state);
    assert (data);
    if (data->length > 0) {
        if (state->buffer_write + data->length > state->buffer_end) {
            return -1;
        }
        memcpy(state->buffer_write, data->location, data->length);
    }
    state->buffer_write += data->length;
    return 0;
}

void compress_space(struct ParserState *state) {
    assert (state);
    if (state->parser_read == state->buffer_start) return;
    int length = state->buffer_write - state->parser_read;
    if (state->buffer_write > state->parser_read)
        memcpy(state->buffer_start, state->parser_read, length);
    state->buffer_write = state->buffer_start + length;
    state->parser_read = state->buffer_start;
    return;

}

char data[24] = {0x0A,0,0,0,0,1,2,3,4,5,6,7,8,9,0,1,2,3,4,5,6,7,8,9};

static void test_parser_create() {
    struct ParserState *state = create_parser_state(32);
    CU_ASSERT_PTR_NOT_NULL (state->buffer_end);
    CU_ASSERT_PTR_NOT_NULL (state->buffer_start);
    CU_ASSERT (state->parser_read == state->buffer_start);
    CU_ASSERT (state->buffer_start == state->buffer_write);
    destroy_parser_state (&state);
}

static void test_normal_add_data (void) {
    struct ParserState *state = create_parser_state(32);
    struct Buffer b;
    b.length = 3;
    b.location = (char *) &data;
    char *old_buffer_start = state->buffer_start;
    char *old_buffer_end = state->buffer_end;
    CU_ASSERT (add_data (state, &b) == 0);
    CU_ASSERT (state->buffer_write - state->buffer_start == 3);
    CU_ASSERT (state->parser_read == state->buffer_start);
    CU_ASSERT (state->buffer_start == old_buffer_start);
    CU_ASSERT (state->buffer_end == old_buffer_end);
    destroy_parser_state (&state);
}

static void test_overflow_add_data (void) {
    struct ParserState *state = create_parser_state(32);
    struct Buffer b;
    b.length = 3;
    b.location = (char *) &data;
    CU_ASSERT (add_data (state, &b) == 0);
    b.length = 15;
    CU_ASSERT (add_data (state,&b) == 0);
    //overflow
    CU_ASSERT_EQUAL (add_data (state,&b), -1);
    destroy_parser_state (&state);
}

static void test_compress (void) {
    char ldata[24] = {0x0A,0,0,0,0,1,2,3,4,5,6,7,8,9,0,1,2,3,4,5,6,7,8,9};
    struct ParserState *state = create_parser_state(4096);
    struct Buffer b;
    char *original_start = state->buffer_start;
    b.length = 14;
    b.location = (char *) &ldata;
    //whole message
    add_data (state, &b);
    compress_space (state);
    CU_ASSERT (state->parser_read == state->buffer_start);
    CU_ASSERT (state->buffer_write - state->buffer_start == 14);
    b.length = 6;
    b.location[0] = 0x0B;
    add_data (state, &b);
    struct Buffer * read = read_next (state);
    CU_ASSERT_PTR_NOT_NULL (read);
    assert (state->buffer_write - state->buffer_start == 20);
    CU_ASSERT_EQUAL (state->parser_read, state->buffer_start + 14);
    compress_space (state);
    CU_ASSERT_EQUAL (state->parser_read, state->buffer_start);
    CU_ASSERT_EQUAL (state->buffer_write - state->buffer_start, 6);
    CU_ASSERT_EQUAL (original_start, state->buffer_start);
    CU_ASSERT_EQUAL (state->buffer_start[0], 0x0B);
    CU_ASSERT_EQUAL (state->buffer_start[1], 0);
    CU_ASSERT_EQUAL (state->buffer_start[2], 0);
    CU_ASSERT_EQUAL (state->buffer_start[3], 0);
    destroy_parser_state (&state);
    free (read);
}

static void test_too_short_read (void) {
    struct ParserState *state = create_parser_state(4096);
    struct Buffer b;
    b.length = 6;
    b.location = (char *) &data;
    char *old_parser_read = state->parser_read;
    CU_ASSERT_EQUAL (add_data (state, &b), 0);
    CU_ASSERT_EQUAL (read_next (state), NULL);
    CU_ASSERT (old_parser_read == state->parser_read);
    destroy_parser_state (&state);
}

static void test_perfect_fit_read (void) {
    struct ParserState *state = create_parser_state (4096);
    struct Buffer b;
    b.location = (char *) &data;
    b.length = 14;
    CU_ASSERT_EQUAL (add_data (state, &b), 0);
    char *old_parser_read = state->parser_read;
    struct  Buffer *ret = read_next (state);
    CU_ASSERT_PTR_NOT_NULL (ret);
    CU_ASSERT (ret->location == state->buffer_start + 4);
    CU_ASSERT_EQUAL (ret->length,10);
    CU_ASSERT (old_parser_read + 14 == state->parser_read);
    destroy_parser_state (&state);
    free (ret);
}

static void test_too_much_data_read (void) {
    struct ParserState *state = create_parser_state (4096);
    struct Buffer b;
    b.length = 20;
    b.location = (char *) &data;
    CU_ASSERT_EQUAL (add_data (state, &b), 0);
    char *old_parser_read = state->parser_read;
    struct  Buffer *ret = read_next (state);
    CU_ASSERT_PTR_NOT_NULL (ret);
    CU_ASSERT (ret->location == state->buffer_start + 4);
    CU_ASSERT_EQUAL (ret->length,10);
    CU_ASSERT (old_parser_read + 14 == state->parser_read);
    destroy_parser_state (&state);
    free (ret);
}

int register_es_msg_tests() {
    CU_pSuite pSuite = NULL;
    pSuite = CU_add_suite("parser tests", NULL, NULL);
    if (NULL == pSuite) {
       CU_cleanup_registry();
       return CU_get_error();
    }

    if ((NULL == CU_add_test(pSuite, "test of compess", test_compress)) ||
        (NULL == CU_add_test(pSuite, "test parser create", test_parser_create))||
        (NULL == CU_add_test(pSuite, "test normal add data", test_normal_add_data))||
        (NULL == CU_add_test(pSuite, "test overflow add data", test_overflow_add_data))||
        (NULL == CU_add_test(pSuite, "test read too short", test_too_short_read))||
        (NULL == CU_add_test(pSuite, "test read perfect fit", test_perfect_fit_read))||
        (NULL == CU_add_test(pSuite, "test read too much data", test_too_much_data_read))||
        0)
    {
       CU_cleanup_registry();
       return CU_get_error();
    }
    return 0;
}
