#include "easy_config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_ENTRIES 100
#define MAX_LINE_LEN 256

// --- Internal Storage ---
typedef struct {
    char* key;
    char* value;
} ConfigEntry;

static ConfigEntry entries[MAX_ENTRIES];
static int entry_count = 0;

// --- Helper: Trim whitespace ---
static char* trim(char* str) {
    char* end;
    while(isspace((unsigned char)*str)) str++; // Trim leading
    if(*str == 0) return str;
    end = str + strlen(str) - 1;
    while(end > str && isspace((unsigned char)*end)) end--; // Trim trailing
    *(end+1) = 0;
    return str;
}

// --- Helper: Cleanup ---
static void Config_Cleanup() {
    for (int i = 0; i < entry_count; i++) {
        free(entries[i].key);
        free(entries[i].value);
    }
    entry_count = 0;
}

// --- Implementation ---

static bool Config_Load(const char* filename) {
    Config_Cleanup(); // Clear old config if reloading
    
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("EasyConfig: Could not open file");
        return false;
    }

    char line[MAX_LINE_LEN];
    while (fgets(line, sizeof(line), file)) {
        // Skip comments and empty lines
        char* start = trim(line);
        if (*start == '#' || *start == '\0') continue;

        // Find '='
        char* delimiter = strchr(start, '=');
        if (!delimiter) continue; // Invalid line

        // Split and trim
        *delimiter = '\0'; // Cut string at =
        char* key = trim(start);
        char* val = trim(delimiter + 1);

        if (entry_count < MAX_ENTRIES) {
            entries[entry_count].key = strdup(key);
            entries[entry_count].value = strdup(val);
            entry_count++;
        }
    }

    fclose(file);
    printf("EasyConfig: Loaded %d entries from %s\n", entry_count, filename);
    return true;
}

static const char* Config_GetString(const char* key, const char* default_val) {
    for (int i = 0; i < entry_count; i++) {
        if (strcmp(entries[i].key, key) == 0) {
            return entries[i].value;
        }
    }
    return default_val;
}

static int Config_GetInt(const char* key, int default_val) {
    const char* val = Config_GetString(key, NULL);
    if (!val) return default_val;
    return atoi(val);
}

static bool Config_GetBool(const char* key, bool default_val) {
    const char* val = Config_GetString(key, NULL);
    if (!val) return default_val;
    
    if (strcasecmp(val, "true") == 0 || 
        strcasecmp(val, "yes") == 0 || 
        strcmp(val, "1") == 0) {
        return true;
    }
    return false;
}

// --- Interface Mapping ---
const EasyConfig_t Config = {
    .Load = Config_Load,
    .GetString = Config_GetString,
    .GetInt = Config_GetInt,
    .GetBool = Config_GetBool,
    .Cleanup = Config_Cleanup
};