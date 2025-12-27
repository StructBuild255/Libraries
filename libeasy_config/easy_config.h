#ifndef EASY_CONFIG_H
#define EASY_CONFIG_H

#include <stdbool.h>

typedef struct {
    /**
     * @brief Loads a configuration file into memory.
     * Supports format: KEY=VALUE
     * Lines starting with # are comments.
     * @param filename Path to the config file (e.g., "server.conf")
     * @return true if loaded successfully, false otherwise.
     */
    bool (*Load)(const char* filename);

    /**
     * @brief Get a string value.
     * @param key The key to look for (case-sensitive).
     * @param default_val Value to return if key is not found.
     * @return The value from the file, or default_val.
     */
    const char* (*GetString)(const char* key, const char* default_val);

    /**
     * @brief Get an integer value.
     * @param key The key to look for.
     * @param default_val Value to return if key is not found.
     * @return The integer value.
     */
    int (*GetInt)(const char* key, int default_val);

    /**
     * @brief Get a boolean value.
     * Handles "true", "yes", "1" as true.
     */
    bool (*GetBool)(const char* key, bool default_val);

    /**
     * @brief Frees the memory used by the loaded config.
     */
    void (*Cleanup)(void);

} EasyConfig_t;

extern const EasyConfig_t Config;

#endif