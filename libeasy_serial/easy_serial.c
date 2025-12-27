#include "easy_serial.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>   // File Control
#include <errno.h>   // Error handling
#include <termios.h> // POSIX Terminal Control
#include <unistd.h>  // UNIX Standard functions

// Internal file descriptor for the open port
static int serial_fd = -1;

// --- Helper: Convert integer baud to termios constant ---
static int get_baud_constant(int baud) {
    switch (baud) {
        case 9600:   return B9600;
        case 19200:  return B19200;
        case 38400:  return B38400;
        case 57600:  return B57600;
        case 115200: return B115200;
        default:     return -1;
    }
}

// --- Implementation Functions ---

static bool Serial_Init(const char* port_name, int baud_rate) {
    if (serial_fd != -1) {
        close(serial_fd); // Close if already open
    }

    // Open port: Read/Write, No controlling terminal, No Delay
    serial_fd = open(port_name, O_RDWR | O_NOCTTY | O_NDELAY);
    
    if (serial_fd == -1) {
        perror("RS232 Error: Unable to open port");
        return false;
    }

    struct termios options;
    tcgetattr(serial_fd, &options); // Get current config

    // Set Baud Rate
    int baud_flag = get_baud_constant(baud_rate);
    if (baud_flag == -1) {
        fprintf(stderr, "RS232 Error: Unsupported baud rate %d\n", baud_rate);
        close(serial_fd);
        return false;
    }
    cfsetispeed(&options, baud_flag);
    cfsetospeed(&options, baud_flag);

    // --- RAW MODE CONFIGURATION (Critical for LoRa/Binary) ---
    
    // c_cflag: Control Options
    options.c_cflag |= (CLOCAL | CREAD);  // Enable receiver, ignore modem lines
    options.c_cflag &= ~CSIZE;            // Mask character size bits
    options.c_cflag |= CS8;               // 8 data bits
    options.c_cflag &= ~PARENB;           // No Parity
    options.c_cflag &= ~CSTOPB;           // 1 Stop bit
    options.c_cflag &= ~CRTSCTS;          // No Hardware Flow Control (RTS/CTS)

    // c_lflag: Local Options
    // Disable Canonical mode (line-by-line), Echo, Signals
    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);

    // c_oflag: Output Options
    options.c_oflag &= ~OPOST;            // Raw output (no processing)

    // c_iflag: Input Options
    // Disable software flow control (XON/XOFF)
    options.c_iflag &= ~(IXON | IXOFF | IXANY);

    // Timeout settings (Non-blocking read behavior)
    options.c_cc[VMIN]  = 0;              // Read doesn't block
    options.c_cc[VTIME] = 1;              // 0.1 seconds read timeout

    // Apply settings
    if (tcsetattr(serial_fd, TCSANOW, &options) != 0) {
        perror("RS232 Error: Failed to set attributes");
        close(serial_fd);
        return false;
    }

    // Flush old data
    tcflush(serial_fd, TCIOFLUSH);
    
    // Restore blocking behavior (optional, but good for stability)
    fcntl(serial_fd, F_SETFL, 0);

    printf("RS232: Port %s opened at %d baud.\n", port_name, baud_rate);
    return true;
}

static void Serial_Send(const char* message) {
    if (serial_fd == -1) return;
    int len = strlen(message);
    int w = write(serial_fd, message, len);
    if (w < 0) perror("RS232 Write Error");
}

static void Serial_SendBytes(const uint8_t* data, int length) {
    if (serial_fd == -1) return;
    int w = write(serial_fd, data, length);
    if (w < 0) perror("RS232 Write Error");
}

static int Serial_Receive(uint8_t* buffer, int max_len) {
    if (serial_fd == -1) return -1;
    
    // Attempt to read bytes
    int n = read(serial_fd, buffer, max_len);
    if (n < 0) {
        // If "Resource temporarily unavailable", it's just empty, not a crash
        if (errno == EAGAIN) return 0; 
        perror("RS232 Read Error");
        return -1;
    }
    return n;
}

static void Serial_Close(void) {
    if (serial_fd != -1) {
        close(serial_fd);
        serial_fd = -1;
        printf("RS232: Port closed.\n");
    }
}

// Map the functions to the struct instance
const SerialDriver_t RS232 = {
    .Init = Serial_Init,
    .Send = Serial_Send,
    .SendBytes = Serial_SendBytes, // Used for Hex/LoRa packets
    .Receive = Serial_Receive,
    .Close = Serial_Close
};