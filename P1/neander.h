#ifndef NEANDER_H
#define NEANDER_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define MEMORYSIZE 516
#define LINESIZE 16
#define HEADERSIZE 4
#define RESULTOFFSET 0x202

void print_memory(uint8_t *bytes, size_t size);

#endif // NEANDER_H

void print_memory(uint8_t *bytes, size_t size) {
    size_t offset = 0;
    while (offset < size) {
        printf("%08zx: ", offset);
        for (size_t i = 0; i < LINESIZE; i++) {
            if (offset + i < size)
                printf("%02x ", bytes[offset + i]);
            else
                printf("   ");
        }
        printf("\n");
        offset += LINESIZE;
    }
}
