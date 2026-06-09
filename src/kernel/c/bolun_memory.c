#include "bolun/bolun_memory.h"
#include "bolun/bolun_result.h"

#include <string.h>

#define BOLUN_MEMORY_BLOCK_SIZE sizeof(BolunMemoryBlock)
#define BOLUN_MEMORY_MIN_ALLOC 32

static BolunMemoryBlock *find_free_block(BolunMemoryAllocator *allocator, size_t size) {
    BolunMemoryBlock *current = allocator->first_block;
    
    while (current != NULL) {
        if (!current->in_use && current->size >= size) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

static void split_block(BolunMemoryBlock *block, size_t size) {
    if (block->size > size + BOLUN_MEMORY_BLOCK_SIZE + BOLUN_MEMORY_MIN_ALLOC) {
        BolunMemoryBlock *new_block = (BolunMemoryBlock *)((uint8_t *)block + BOLUN_MEMORY_BLOCK_SIZE + size);
        new_block->size = block->size - size - BOLUN_MEMORY_BLOCK_SIZE;
        new_block->in_use = 0;
        new_block->next = block->next;
        block->size = size;
        block->next = new_block;
    }
}

int bolun_memory_init(BolunMemoryAllocator *allocator, void *heap_start, size_t heap_size) {
    if (allocator == NULL || heap_start == NULL || heap_size < BOLUN_MEMORY_BLOCK_SIZE) {
        return BOLUN_ERROR_INVALID_ARGUMENT;
    }

    allocator->heap_start = heap_start;
    allocator->heap_size = heap_size;
    allocator->used_size = BOLUN_MEMORY_BLOCK_SIZE;

    allocator->first_block = (BolunMemoryBlock *)heap_start;
    allocator->first_block->size = heap_size - BOLUN_MEMORY_BLOCK_SIZE;
    allocator->first_block->in_use = 0;
    allocator->first_block->next = NULL;

    return BOLUN_OK;
}

void *bolun_memory_alloc(BolunMemoryAllocator *allocator, size_t size) {
    if (allocator == NULL || size == 0) {
        return NULL;
    }

    size = (size + sizeof(void *) - 1) & ~(sizeof(void *) - 1);

    BolunMemoryBlock *block = find_free_block(allocator, size);
    if (block == NULL) {
        return NULL;
    }

    block->in_use = 1;
    split_block(block, size);
    
    allocator->used_size += size + BOLUN_MEMORY_BLOCK_SIZE;
    if (allocator->used_size > allocator->heap_size) {
        allocator->used_size = allocator->heap_size;
    }

    return (uint8_t *)block + BOLUN_MEMORY_BLOCK_SIZE;
}

int bolun_memory_free(BolunMemoryAllocator *allocator, void *ptr) {
    if (allocator == NULL || ptr == NULL) {
        return BOLUN_ERROR_INVALID_ARGUMENT;
    }

    BolunMemoryBlock *block = (BolunMemoryBlock *)((uint8_t *)ptr - BOLUN_MEMORY_BLOCK_SIZE);

    if (!block->in_use) {
        return BOLUN_ERROR_INVALID_ARGUMENT;
    }

    block->in_use = 0;

    if (block->next != NULL && !block->next->in_use) {
        block->size += BOLUN_MEMORY_BLOCK_SIZE + block->next->size;
        block->next = block->next->next;
    }

    if (block != allocator->first_block) {
        BolunMemoryBlock *prev = allocator->first_block;
        while (prev != NULL && prev->next != block) {
            prev = prev->next;
        }
        if (prev != NULL && !prev->in_use) {
            prev->size += BOLUN_MEMORY_BLOCK_SIZE + block->size;
            prev->next = block->next;
        }
    }

    if (allocator->used_size >= block->size + BOLUN_MEMORY_BLOCK_SIZE) {
        allocator->used_size -= (block->size + BOLUN_MEMORY_BLOCK_SIZE);
    }

    return BOLUN_OK;
}

int bolun_memory_get_stats(const BolunMemoryAllocator *allocator, BolunMemoryStats *stats) {
    if (allocator == NULL || stats == NULL) {
        return BOLUN_ERROR_INVALID_ARGUMENT;
    }

    memset(stats, 0, sizeof(*stats));
    stats->total_size = allocator->heap_size;
    stats->used_size = allocator->used_size;
    stats->free_size = allocator->heap_size - allocator->used_size;

    BolunMemoryBlock *current = allocator->first_block;
    size_t fragment_count = 0;

    while (current != NULL) {
        if (!current->in_use) {
            fragment_count++;
        }
        stats->allocation_count++;
        current = current->next;
    }

    if (stats->used_size > 0) {
        stats->fragmentation_percent = (fragment_count * 100) / stats->allocation_count;
    }

    return BOLUN_OK;
}

size_t bolun_memory_compact(BolunMemoryAllocator *allocator) {
    if (allocator == NULL) {
        return 0;
    }

    size_t freed = 0;
    BolunMemoryBlock *current = allocator->first_block;

    while (current != NULL && current->next != NULL) {
        if (!current->in_use && !current->next->in_use) {
            current->size += BOLUN_MEMORY_BLOCK_SIZE + current->next->size;
            freed += BOLUN_MEMORY_BLOCK_SIZE;
            current->next = current->next->next;
        } else {
            current = current->next;
        }
    }

    if (freed > 0) {
        allocator->used_size = allocator->used_size >= freed ? allocator->used_size - freed : 0;
    }

    return freed;
}
