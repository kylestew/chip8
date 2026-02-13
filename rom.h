#ifndef ROM_H
#define ROM_H

#include <stdint.h>
#include <stddef.h>

// Load ROM from filepath into dest buffer (up to max_size bytes).
// Returns 0 on success, -1 on error (prints reason to stderr).
int rom_load(uint8_t *dest, size_t max_size, const char *filepath);

#endif
