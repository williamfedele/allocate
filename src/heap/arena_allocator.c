#include "arena_allocator.h"
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>

arena_t *arena_init(size_t size) {
  // The arena will be placed at the beginning of the mapped memory
  size_t total_size = sizeof(arena_t) + size;

  // void *mmap(void addr[.length], size_t length, int prot, int flags, int fd,
  // off_t offset); addr NULL: Don't care where the mapping starts prot
  // PROT_READ, PROT_WRITE: Pages can be read and written flags
  // MAP_PRIVATE: Changes are private to the process flags
  // MAP_ANONYMOUS: Mapping not backed by a file. Contents zero initialized
  // File descriptor fd: -1 with MAP_ANONYMOUS
  // offset 0: No offset with MAP_ANONYMOUS
  void *region = mmap(NULL, total_size, PROT_READ | PROT_WRITE,
                      MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

  // mmap() returns either a pointer to the mapped area or MAP_FAILED
  if (region == MAP_FAILED) {
    return NULL;
  }

  // Cast raw memory as arena
  arena_t *arena = (arena_t *)region;
  // Cast as char* for 1 byte offset increments
  arena->memory = (char *)region + sizeof(arena_t);
  arena->size = size;
  arena->offset = 0;

  return arena;
}

void *arena_alloc(arena_t *arena, size_t size, size_t alignment) {

  // Alignment must be a power of 2
  if (alignment == 0 || (alignment & (alignment - 1)) != 0) {
    return NULL;
  }

  // Align the next addr with the alignment boundary provided
  size_t aligned_used = (arena->offset + alignment - 1) & ~(alignment - 1);

  // Make sure we don't overflow the arena
  if (aligned_used + size > arena->size) {
    return NULL;
  }

  // memory+used provides the next available space
  void *next_free_addr = arena->memory + aligned_used;
  arena->offset = aligned_used + size;

  memset(next_free_addr, 0, size);

  return next_free_addr;
}

void arena_free(arena_t *arena) {
  // Reset the arena by simply setting setting used to 0
  // Further allocations with overwrite previously allocated memory
  arena->offset = 0;
}

int arena_destroy(arena_t *arena) {
  // munmap returns 0 upon success, -1 otherwise
  return munmap(arena->memory, arena->size);
}
