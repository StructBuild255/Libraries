#include "easy_socket.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h> // For sockaddr_in, inet_addr
#include <sys/socket.h>

// --- Helper Implementation ---

static int Socket_StartServer(int port) {
    int server_fd;
    struct sockaddr_in address;
    int opt = 1;

    // 1. Create Socket File Descriptor (IPv4, TCP)
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("EasySocket: Socket creation failed");
        return -1;
    }

    // 2. Set Socket Options (Prevents "Address already in use" error on restart)
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("EasySocket: setsockopt failed");
        close(server_fd);
        return -1;
    }

    // 3. Define the Address/Port structure
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY; // Listen on 0.0.0.0 (All interfaces)
    address.sin_port = htons(port);       // Convert integer to Network Byte Order

    // 4. Bind the socket to the port
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("EasySocket: Bind failed");
        close(server_fd);
        return -1;
    }

    // 5. Start Listening (Backlog of 3 connections)
    if (listen(server_fd, 3) < 0) {
        perror("EasySocket: Listen failed");
        close(server_fd);
        return -1;
    }

    printf("EasySocket: Server listening on port %d...\n", port);
    return server_fd;
}

static int Socket_Accept(int server_fd) {
    int new_socket;
    struct sockaddr_in client_addr;
    socklen_t addrlen = sizeof(client_addr);

    // Accept the connection
    if ((new_socket = accept(server_fd, (struct sockaddr *)&client_addr, &addrlen)) < 0) {
        perror("EasySocket: Accept failed");
        return -1;
    }

    // Optional: Print who connected
    char *client_ip = inet_ntoa(client_addr.sin_addr);
    printf("EasySocket: Connection accepted from %s\n", client_ip);

    return new_socket;
}

static int Socket_Connect(const char* ip, int port) {
    int sock = 0;
    struct sockaddr_in serv_addr;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("EasySocket: Socket creation error");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, ip, &serv_addr.sin_addr) <= 0) {
        printf("EasySocket: Invalid address/ Address not supported \n");
        close(sock);
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("EasySocket: Connection Failed");
        close(sock);
        return -1;
    }

    return sock;
}

static bool Socket_Send(int fd, const char* message) {
    if (send(fd, message, strlen(message), 0) < 0) {
        perror("EasySocket: Send failed");
        return false;
    }
    return true;
}

static int Socket_Receive(int fd, char* buffer, int max_len) {
    int bytes_read = read(fd, buffer, max_len);
    if (bytes_read < 0) {
        perror("EasySocket: Read error");
    }
    return bytes_read;
}

static void Socket_Close(int fd) {
    close(fd);
    printf("EasySocket: Connection closed.\n");
}

// Map the functions
const EasySocket_t Socket = {
    .StartServer = Socket_StartServer,
    .Accept = Socket_Accept,
    .Connect = Socket_Connect,
    .Send = Socket_Send,
    .Receive = Socket_Receive,
    .Close = Socket_Close
};