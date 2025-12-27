#ifndef EASY_SERIAL_H
#define EASY_SERIAL_H

#include <stdint.h>
#include <stdbool.h>

/* * This struct defines the "Object" style syntax.
 * You will access everything via the global 'RS232' instance.
 */
typedef struct {
    /**
     * @brief Initialize the serial port.
     * @param port_name The file path (e.g., "/dev/ttyUSB0" or "/dev/ttyS0")
     * @param baud_rate The speed (e.g., 9600, 115200)
     * @return true if successful, false if failed (check console for error)
     */
    bool (*Init)(const char* port_name, int baud_rate);

    /**
     * @brief Send a null-terminated string.
     * @param message The text to send.
     */
    void (*Send)(const char* message);

    /**
     * @brief Send raw binary bytes (Crucial for LoRa packet headers).
     * @param data Pointer to the byte array.
     * @param length Number of bytes to send.
     */
    void (*SendBytes)(const uint8_t* data, int length);

    /**
     * @brief Read data from the buffer.
     * @param buffer Array to store received data.
     * @param max_len Maximum bytes to read.
     * @return Number of bytes actually read.
     */
    int (*Receive)(uint8_t* buffer, int max_len);

    /**
     * @brief Close the connection and release resources.
     */
    void (*Close)(void);

} SerialDriver_t;

// The global instance you asked for
extern const SerialDriver_t RS232;

#endif