#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
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
    bzero(addr, sizeof(struct sockaddr_in)); // Clear my_addr struct
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

int main()
{
    int socket_fd;
    struct sockaddr_in host_addr;

    char buffer[BUFF_LEN];

    // Create socket and setup connection to "Mail.csc.tntech.edu"
    socket_fd = create_socket();
    setup_host_address(&host_addr, "Mail.csc.tntech.edu");
    connect_socket(socket_fd, &host_addr, sizeof(host_addr));

    // Initial read to server
    bzero(&buffer, BUFF_LEN);
    read_msg(socket_fd, buffer, BUFF_LEN-1);
    printf("S: %s", buffer);

    // HELO message
    bzero(&buffer, BUFF_LEN);
    strcpy(buffer, "HELO ");
    strcat(buffer, "ltdavis42\n"); // NOTE: Command MUST have \n character; Replace with argument/input e.e
    prepare_msg(socket_fd, buffer);

    // MAIL FROM: message
    bzero(&buffer, BUFF_LEN);
    strcpy(buffer, "MAIL FROM: <");
    strcat(buffer, "ltdavis42");
    strcat(buffer, ">\n");
    prepare_msg(socket_fd, buffer);

    // RCPT TO: message
    bzero(&buffer, BUFF_LEN);
    strcpy(buffer, "RCPT TO: <");
    strcat(buffer, "ltdavis42");
    strcat(buffer, ">\n");
    prepare_msg(socket_fd, buffer);

    // DATA message
    bzero(&buffer, BUFF_LEN);
    strcpy(buffer, "DATA\n");
    prepare_msg(socket_fd, buffer);

    // Data body
    while (strcmp(buffer, ".\n"))
    {
        bzero(&buffer, BUFF_LEN);
        printf("C: ");
        fgets(buffer, BUFF_LEN, stdin);
        write_msg(socket_fd, buffer, strlen(buffer));
    }
    bzero(&buffer, BUFF_LEN);
    read_msg(socket_fd, buffer, BUFF_LEN-1);
    printf("S: %s", buffer);

    // QUIT message
    bzero(&buffer, BUFF_LEN);
    strcpy(buffer, "QUIT\n");
    prepare_msg(socket_fd, buffer);

    // Close socket
    close(socket_fd);
 
    exit(0);
}
