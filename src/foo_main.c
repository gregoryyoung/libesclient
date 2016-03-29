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
#include "es_client_internal.h"


int
not_main(int argc, char *argv[])
{
    /*
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
*/
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
