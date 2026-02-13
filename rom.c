#include "rom.h"

#include <stdio.h>

int rom_load(uint8_t *dest, size_t max_size, const char *filepath) {
    FILE *f = fopen(filepath, "rb");
    if (!f) {
        fprintf(stderr, "rom_load: cannot open '%s'\n", filepath);
        return -1;
    }

    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    rewind(f);

    if (size <= 0 || (size_t) size > max_size) {
        fprintf(stderr, "rom_load: invalid ROM size %ld (max %zu)\n", size, max_size);
        fclose(f);
        return -1;
    }

    size_t read = fread(dest, 1, (size_t) size, f);
    fclose(f);

    if ((long) read != size) {
        fprintf(stderr, "rom_load: read %zu of %ld bytes\n", read, size);
        return -1;
    }

    printf("Loaded ROM: %s (%ld bytes)\n", filepath, size);

    for (long i = 0; i < size; i++) {
        if (i % 16 == 0)
            printf("  %04lx: ", i);
        printf("%02x ", dest[i]);
        if (i % 16 == 15 || i == size - 1)
            printf("\n");
    }

    return 0;
}
