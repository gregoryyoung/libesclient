#include <sys/socket.h>
#include <sys/types.h>
#include <sys/poll.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <uuid/uuid.h>
#include "es_tcp_messages.h"
#include "wtf_uuid.h"

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

struct Header *
create_header (const char command, const char flags, const uuid_t correlation_id, const char *username, const char *password) {
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

void
destroy_header (struct Header **header) {
    assert (header);
    if(*header) {
        struct Header *self = *header;
        if(self->username) free(self->username);
        if(self->password) free(self->password);
        free(self);
        *header = NULL;
    }
}

struct ParserState *
create_parser_state (int buffer_size) {
    struct ParserState *ret = malloc ( sizeof ( struct ParserState));
    ret ->buffer_start = malloc (buffer_size);
    ret->buffer_end = ret->buffer_start + buffer_size;
    ret->parser_read = ret->buffer_start;
    ret->buffer_write = ret->buffer_start;
    return ret;
}

void
destroy_parser_state (struct ParserState **state) {
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


int
read_header (struct Buffer *buffer, struct Header **header) {
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

int32_t
write_header (struct Header *header, char *buffer) {
    buffer[COMMANDOFFSET] = header->command;
    buffer[FLAGSOFFSET] = header->flags;
    write_uuid_to_wtf (*(header->correlation_id), buffer + CORRELATIONOFFSET);
    return AUTHOFFSET;
}

char *
get_string_for_header(struct Header *header) {
    char *ret = malloc(1024);
    char uuid_str[37];
    char *cmd_str = get_string_for_tcp_message(header->command);
    uuid_unparse(*header->correlation_id, uuid_str);
    sprintf (ret, "Command: %s, flags %d, correlation_id: %s\n", cmd_str, header->flags, uuid_str);
    return ret;
}

struct Buffer *
read_next (struct ParserState *state) {
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

int32_t
add_data(struct ParserState *state, struct Buffer *data) {
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

void
compress_space(struct ParserState *state) {
    assert (state);
    if (state->parser_read == state->buffer_start) return;
    int length = state->buffer_write - state->parser_read;
    if (state->buffer_write > state->parser_read)
        memcpy(state->buffer_start, state->parser_read, length);
    state->buffer_write = state->buffer_start + length;
    state->parser_read = state->buffer_start;
    return;

}



static void
test_add_data (void) {
    char data[16] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p'};
    struct ParserState *state = create_parser_state(32);
    assert (state->buffer_end);
    assert (state->buffer_start);
    assert (state->parser_read == state->buffer_start);
    assert (state->buffer_start == state->buffer_write);
    //simple case
    struct Buffer b;
    b.length = 3;
    b.location = (char *) &data;
    char *old_buffer_start = state->buffer_start;
    char *old_buffer_end = state->buffer_end;
    assert (add_data (state, &b) == 0);
    assert (state->buffer_write - state->buffer_start == 3);
    assert (state->parser_read == state->buffer_start);
    assert (state->buffer_start == old_buffer_start);
    assert (state->buffer_end == old_buffer_end);
    b.length = 15;
    assert (add_data (state,&b) == 0);
    //overflow
    assert (add_data (state,&b) == -1);
    destroy_parser_state (&state);
}

static void
test_compress (void) {
    struct ParserState *state = create_parser_state(4096);
    char data[24] = {0x0A,0,0,0,0,1,2,3,4,5,6,7,8,9,0,1,2,3,4,5,6,7,8,9};
    struct Buffer b;
    char *original_start = state->buffer_start;
    b.length = 14;
    b.location = (char *) &data;
    //whole message
    add_data (state, &b);
    compress_space (state);
    assert (state->parser_read == state->buffer_start);
    assert (state->buffer_write - state->buffer_start == 14);
    b.length = 6;
    b.location[0] = 0x0B;
    add_data (state, &b);
    struct Buffer * read = read_next (state);
    assert (read);
    assert (state->buffer_write - state->buffer_start == 20);
    assert (state->parser_read == state->buffer_start + 14);
    compress_space (state);
    assert (state->parser_read == state->buffer_start);
    assert (state->buffer_write - state->buffer_start == 6);
    assert (original_start == state->buffer_start);
    assert (state->buffer_start[0] == 0x0B);
    assert (state->buffer_start[1] == 0);
    assert (state->buffer_start[2] == 0);
    assert (state->buffer_start[3] == 0);
    destroy_parser_state (&state);
}

static void
test_read (void) {
    struct ParserState *state = create_parser_state(4096);
    char data[24] = {0x0A,0,0,0,0,1,2,3,4,5,6,7,8,9,0,1,2,3,4,5,6,7,8,9};
    struct Buffer b;
    //too short
    b.length = 6;
    b.location = (char *) &data;
    char *old_parser_read = state->parser_read;
    assert (add_data (state, &b) == 0);
    assert (read_next (state) == NULL);
    assert (old_parser_read == state->parser_read);
    destroy_parser_state (&state);

    //perfect fit
    state = create_parser_state (4096);
    b.length = 14;
    assert (add_data (state, &b) == 0);
    struct  Buffer *ret = read_next (state);
    assert (ret);
    assert (ret->location == state->buffer_start + 4);
    assert (ret->length == 10);
    assert (old_parser_read + 14 == state->parser_read);
    destroy_parser_state (&state);
    free (ret);

    //more than enough
    state = create_parser_state (4096);
    b.length = 20;
    assert (add_data (state, &b) == 0);
    ret = read_next (state);
    assert (ret);
    assert (ret->location == state->buffer_start + 4);
    assert (ret->length == 10);
    assert (old_parser_read + 14 == state->parser_read);
    destroy_parser_state (&state);
    free (ret);
}

int
main(int argc, char *argv[])
{
    int sockfd = 0, n = 0;
    char recv_buf[1024];
    char send_buf[1024];
    struct sockaddr_in serv_addr;
    struct ParserState *state = create_parser_state(4096);

    test_add_data();
    test_read();
    test_compress();
    test_read_wtf_uuid();
    test_write_wtf_uuid();
    if(argc != 2)
    {
        printf("\n Usage: %s <ip of server> \n",argv[0]);
        return 1;
    }

    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        perror("\n Error : Could not create socket \n");
    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(1113);

    if(inet_pton(AF_INET, argv[1], &serv_addr.sin_addr)<=0)
        perror("\n inet_pton error occured\n");

    if(connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
       perror("\n Error : Connect Failed");

    int32_t flags = fcntl(sockfd, F_GETFL, 0);
    fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);
    struct pollfd pfds[2];

    pfds[0].fd = sockfd;
    pfds[0].events = POLLIN;

    pfds[1].fd = sockfd;
    pfds[1].events = POLLERR | POLLHUP;
    while(3) {
        poll(pfds, 2, -1);
        if (pfds[0].revents & POLLIN) {
            char buf[1024];
            int32_t i = read(sockfd, buf, 1024);
            printf("read %d bytes\n", i);
            struct Buffer data;
            data.location = buf;
            data.length = i;
            add_data (state, &data);
            struct Buffer *msg = read_next (state);
            if (!i) {
                printf("socket dropped");
                return 0;
             }
            while(msg != NULL) {
                printf("read message bytes = %d\n", msg->length);
                struct Header *header;
                if(read_header (msg, &header)) {
                    char *info = get_string_for_header(header);
                    printf("header is %s\n", info);
                    free(info);
                    if(header->command == MESSAGE_HEARTBEATREQUEST) {
                        struct Header * resp = create_header (MESSAGE_HEARTBEATRESPONSE, 0, *header->correlation_id, NULL, NULL);
                        char *r = get_string_for_header (resp);
                        printf ("response is %s\n", r);
                        int32_t len = write_header (resp, send_buf + 4);
                        int *resplen = (int*) send_buf;
                        *resplen = len;
                        send (sockfd, send_buf, len + 4, 0);
                    }
                }
                msg = read_next (state);
            }
            compress_space (state);

        } else {
            printf("mpt POLLIN");
        }

        if (pfds[1].revents & POLLERR) {
            printf("socket error\n");
        }
    }
/*    while ( (n = read(sockfd, recvBuff, sizeof(recvBuff)-1)) > 0)
    {
        recvBuff[n] = 0;
        printf("received %d bytes\n", n);
        if(fputs(recvBuff, stdout) == EOF)
        {
            printf("\n Error : Fputs error\n");
        }
    }

    if(n < 0)
    {
        printf("\n Read error \n");
    }
*/

    return 0;
}
