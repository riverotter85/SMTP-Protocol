#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

int main()
{
    int socket_fd;
    struct sockaddr_in host_addr;
    struct hostent *host_info;

    socklen_t host_addr_size;

    char buffer[256];

    // Create socket; returns 0 for success, -1 for failure
    socket_fd = create_socket();

    setup_host_address(&host_addr, "Mail.csc.tntech.edu");

    // Connect socket to host
    if (connect(socket_fd, (struct sockaddr *) &host_addr, sizeof(host_addr)) < 0)
    {
        exit_error("Socket failed to connect to host.");
    }

    read_msg(socket_fd, buffer, 255);

    //printf("Enter a message to send: ");
    //bzero(&buffer, 256); // Keep an eye on this e_e
    //fgets(buffer, 255, stdin);
    //write_msg(socket_fd, buffer, strlen(buffer));
    //printf("WRITE: %s", buffer);

    while (strcmp(buffer, "exit\n") != 0)
    {
        bzero(&buffer, 256);
        read_msg(socket_fd, buffer, 255);
        printf("READ: %s", buffer);

        printf("Enter a message to send: ");
        bzero(&buffer, 256); // Keep an eye on this e_e
        fgets(buffer, 255, stdin);
        write_msg(socket_fd, buffer, strlen(buffer));
        printf("WRITE: %s", buffer);
    }

    return 0;

    //// Bind address to socket
    //if (!bind(socket_fd, (struct sockaddr *) &my_addr, sizeof(my_addr)))
    //{
    //    perror("Failed to bind socket to address.");
    //    return 1;
    //}

    //// Listen for request from server
    //if (!listen(socket_fd, LISTEN_BACKLOG))
    //{
    //    perror("Socket failed to listen to host.");
    //    return 1;
    //}

    //// Accept
    //host_addr_size = sizeof(struct sockaddr_in);
    //if (!accept(socket_fd, (struct sockaddr *) &host_addr, &host_addr_size))
    //{
    //    perror("Failed to accept response from host.");
    //    return 1;
    //}

    // Write reply to socket

    // Close socket
}
