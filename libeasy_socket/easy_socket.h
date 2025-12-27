#ifndef EASY_SOCKET_H
#define EASY_SOCKET_H

#include <stdbool.h>
#include <stdint.h>

typedef struct {
    /**
     * @brief Start a TCP Server.
     * Handles socket creation, SO_REUSEADDR, binding, and listening.
     * @param port The port to listen on (e.g., 8080)
     * @return The server_fd (file descriptor) or -1 on failure.
     */
    int (*StartServer)(int port);

    /**
     * @brief Accept a new client connection.
     * Blocks until a client connects.
     * @param server_fd The file descriptor returned by StartServer.
     * @return The client_fd (file descriptor) or -1 on failure.
     */
    int (*Accept)(int server_fd);

    /**
     * @brief Connect to a remote server (Client mode).
     * @param ip The IP address (e.g., "192.168.1.50")
     * @param port The port to connect to.
     * @return The connection_fd or -1 on failure.
     */
    int (*Connect)(const char* ip, int port);

    /**
     * @brief Send a string message.
     */
    bool (*Send)(int fd, const char* message);

    /**
     * @brief Receive data into a buffer.
     * @return Number of bytes read, 0 if closed, -1 if error.
     */
    int (*Receive)(int fd, char* buffer, int max_len);

    /**
     * @brief Close a socket.
     */
    void (*Close)(int fd);

} EasySocket_t;

extern const EasySocket_t Socket;

#endif