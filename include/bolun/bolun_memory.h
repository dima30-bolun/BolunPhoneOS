#ifndef BOLUN_MEMORY_H
#define BOLUN_MEMORY_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct BolunMemoryBlock {
    size_t size;
    uint8_t in_use;
    struct BolunMemoryBlock *next;
} BolunMemoryBlock;

typedef struct BolunMemoryAllocator {
    void *heap_start;
    size_t heap_size;
    size_t used_size;
    BolunMemoryBlock *first_block;
} BolunMemoryAllocator;

typedef struct BolunMemoryStats {
    size_t total_size;
    size_t used_size;
    size_t free_size;
    size_t allocation_count;
    size_t fragmentation_percent;
} BolunMemoryStats;

int bolun_memory_init(BolunMemoryAllocator *allocator, void *heap_start, size_t heap_size);
void *bolun_memory_alloc(BolunMemoryAllocator *allocator, size_t size);
int bolun_memory_free(BolunMemoryAllocator *allocator, void *ptr);
int bolun_memory_get_stats(const BolunMemoryAllocator *allocator, BolunMemoryStats *stats);
size_t bolun_memory_compact(BolunMemoryAllocator *allocator);

#ifdef __cplusplus
}
#endif

#endif // BOLUN_MEMORY_H
