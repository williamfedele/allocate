#ifndef ARENA_ALLOCATOR_H
#define ARENA_ALLOCATOR_H

#include <stddef.h>

typedef struct arena {
  void *memory; // Start of mem region
  size_t size;  // Total arena size
  size_t used;  // Bytes currently allocated
} arena_t;

// Initialize the arena with memory reserved from the OS
arena_t* init(size_t size);

// Allocate memory from the arena
void* alloc(arena_t* arena, size_t size);

// Resets the arena allocator and frees all allocated memory
void reset(arena_t* arena);

// Destroy the arena and give the memory back to the OS
void destroy(arena_t* arena);

#endif
