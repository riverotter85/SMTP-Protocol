#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//const char* get_ip_from_domain(const char* domain_name) {
//    const char* domain_ip;
//    struct in_addr
//}

//int connect_to_server(const char* server_name) {
//    int socket_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
//    struct sockaddr_in addr;
//
//    memset(&addr, 0, sizeof(addr));
//    addr.sin_family = AF_INET;
//    addr.sin_port = htons(25); // SMTP port
//
//    if ()
//}

int main()
{
    int socket_fd;
    struct sockaddr_in host_addr;
    struct hostent *host_info;

    socklen_t host_addr_size;

    char buffer[256];

    // Create socket; returns 0 for success, -1 for failure
    socket_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    if (!socket_fd)
    {
        perror("Failed to create socket.");
        return 1;
    }

    memset(&host_addr, 0, sizeof(struct sockaddr_in)); // Clear my_addr struct
    host_addr.sin_family = AF_INET;
    host_addr.sin_port = htons(25); // SMTP port
    host_info = gethostbyname("Mail.csc.tntech.edu");
    if (!host_info)
    {
        perror("Failed to obtain host info.");
        return 1;
    }
    host_addr.sin_addr = *(struct in_addr *) host_info->h_addr; // NOTE: May need to change!

    // Connect socket to host
    if (connect(socket_fd, (struct sockaddr *) &host_addr, sizeof(host_addr)) < 0)
    {
        perror("Socket failed to connect to host.");
        return 1;
    }

    // NOTE: Possibly remove this
    if (!read(socket_fd, buffer, 255))
    {
        perror("Error reading from socket.");
        return 1;
    }

    printf("Enter a message to send: ");
    bzero(&buffer, 256); // Keep an eye on this e_e
    fgets(buffer, 255, stdin);
    if (!write(socket_fd, buffer, strlen(buffer))) //!!!
    {
        perror("Error writing to socket.");
        return 1;
    }
    printf("WRITE: %s\n", buffer);

    bzero(&buffer, 256);
    if (!read(socket_fd, buffer, 255))
    {
        perror("Error reading from socket.");
        return 1;
    }
    printf("READ: %s\n", buffer);

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
