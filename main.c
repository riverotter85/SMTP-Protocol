#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFF_LEN 256

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
    addr->sin_port = htons(25); // SMTP port
    
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

void prepare_msg(int sfd, const char* msg)
{
    char buff[BUFF_LEN];

    bzero(&buff, BUFF_LEN);
    read_msg(sfd, buff, BUFF_LEN-1);
    printf("S: %s", buff);

    bzero(&buff, BUFF_LEN);
    write_msg(sfd, msg, strlen(msg));
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

    //while (strcmp(buffer, "exit\n") != 0)
    //{
    //    bzero(&buffer, 256);
    //    read_msg(socket_fd, buffer, 255);
    //    printf("READ: %s", buffer);
    //
    //    printf("Enter a message to send: ");
    //    bzero(&buffer, 256); // Keep an eye on this e_e
    //    fgets(buffer, 255, stdin);
    //    write_msg(socket_fd, buffer, strlen(buffer));
    //    printf("WRITE: %s", buffer);
    //}

    // Close socket
    close(socket_fd);
    
    exit(0);
}
