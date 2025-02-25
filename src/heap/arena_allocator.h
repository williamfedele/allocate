#ifndef ARENA_ALLOCATOR_H
#define ARENA_ALLOCATOR_H

#include <stddef.h>

typedef struct arena {
  void *memory; // Start of mem region
  size_t size;  // Total arena size
  size_t used;  // Bytes currently allocated
} arena_t;

#endif
