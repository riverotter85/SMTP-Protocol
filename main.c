#include <unistd.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFF_LEN 256
#define SMTP_PORT 25

void exit_error(const char *msg)
{
    perror(msg);
    exit(1);
}

void verify_num_args(int num_args)
{
    if (num_args < 4)
    {
        exit_error("Insufficient number of arguments.");
    }
}

int create_socket()
{
    int sfd = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    if (!sfd)
    {
        exit_error("Failed to create socket.");
    }

    return sfd;
}

struct hostent *get_host_info(const char* host_name)
{
    struct hostent *hi = gethostbyname(host_name);
    if (!hi)
    {
        exit_error("Failed to obtain host info.");
    }

    return hi;
}

void connect_socket(int sfd, struct sockaddr_in *addr, size_t size)
{
    if (connect(sfd, (struct sockaddr *) addr, size) < 0)
    {
        exit_error("Socket failed to connect to host.");
    }
}

void setup_host_address(struct sockaddr_in *addr, const char *host_name)
{
    bzero(addr, sizeof(struct sockaddr_in));
    addr->sin_family = AF_INET;
    addr->sin_port = htons(SMTP_PORT);
    
    struct hostent *host_info = get_host_info(host_name);
    addr->sin_addr = *(struct in_addr *) host_info->h_addr;
}

void read_msg(int sfd, char *buff, size_t len)
{
    if (!read(sfd, buff, len))
    {
        exit_error("Error reading from socket.");
    }
}

void write_msg(int sfd, const char *buff, size_t len)
{
    if (!write(sfd, buff, len))
    {
        exit_error("Error writing to socket.");
    }
}

void prepare_msg(int sfd, char* buff)
{
    write_msg(sfd, buff, strlen(buff));
    printf("C: %s", buff);

    bzero(buff, BUFF_LEN);
    read_msg(sfd, buff, BUFF_LEN-1);
    printf("S: %s", buff);
}

void prepare_initial_read(int sfd)
{
    char buff[BUFF_LEN];

    bzero(&buff, BUFF_LEN);
    read_msg(sfd, buff, BUFF_LEN-1);
    printf("S: %s", buff);
}

void prepare_HELO(int sfd, char *client_id)
{
    char buff[BUFF_LEN];

    bzero(&buff, BUFF_LEN);
    strcpy(buff, "HELO ");
    strcat(buff, client_id);
    strcat(buff, "\n");
    prepare_msg(sfd, buff);
}

void prepare_MAIL_FROM(int sfd, char *client_id)
{
    char buff[BUFF_LEN];

    bzero(&buff, BUFF_LEN);
    strcpy(buff, "MAIL FROM: <");
    strcat(buff, client_id);
    strcat(buff, ">\n");
    prepare_msg(sfd, buff);
}

void prepare_RCPT_TO(int sfd, char *server_id)
{
    char buff[BUFF_LEN];

    bzero(&buff, BUFF_LEN);
    strcpy(buff, "RCPT TO: <");
    strcat(buff, server_id);
    strcat(buff, ">\n");
    prepare_msg(sfd, buff);
}

void prepare_data_request(int sfd)
{
    char buff[BUFF_LEN];

    bzero(&buff, BUFF_LEN);
    strcpy(buff, "DATA\n");
    prepare_msg(sfd, buff);
}

int equal_to_END_MESSAGE(const char *expr)
{
    return strcmp(expr, ".\n") == 0;
}

void prepare_data_body(int sfd)
{
    char buff[BUFF_LEN];

    bzero(&buff, BUFF_LEN);

    while (!equal_to_END_MESSAGE(buff))
    {
        bzero(buff, BUFF_LEN);
        printf("C: ");
        fgets(buff, BUFF_LEN, stdin);
        write_msg(sfd, buff, strlen(buff));
    }

    bzero(buff, BUFF_LEN);
    read_msg(sfd, buff, BUFF_LEN-1);
    printf("S: %s", buff);
}

void prepare_DATA(int sfd)
{
    prepare_data_request(sfd);
    prepare_data_body(sfd);
}

void prepare_QUIT(int sfd)
{
    char buff[BUFF_LEN];

    bzero(&buff, BUFF_LEN);
    strcpy(buff, "QUIT\n");
    prepare_msg(sfd, buff);
}

int main(int argc, char *argv[])
{
    int socket_fd;
    struct sockaddr_in host_addr;

    verify_num_args(argc);

    // Create socket and setup connection
    socket_fd = create_socket();
    setup_host_address(&host_addr, argv[1]);
    connect_socket(socket_fd, &host_addr, sizeof(host_addr));

    // Run SMTP commands with args given
    prepare_initial_read(socket_fd);
    prepare_HELO(socket_fd, argv[2]);
    prepare_MAIL_FROM(socket_fd, argv[2]);
    prepare_RCPT_TO(socket_fd, argv[3]);
    prepare_DATA(socket_fd);
    prepare_QUIT(socket_fd);

    // Close socket
    close(socket_fd);
 
    exit(0);
}
