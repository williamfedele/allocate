#include "arena_allocator.h"
#include <stddef.h>
#include <sys/mman.h>

arena_t *init(size_t size) {
  // The arena will be placed at the beginning of the mapped memory
  size_t total_size = sizeof(arena_t) + size;

  // mmap ref: https://man7.org/linux/man-pages/man2/mmap.2.html
  // Definition:
  // void *mmap(void addr[.length], size_t length, int prot, int flags, int fd,
  // off_t offset); addr NULL: Don't care where the mapping starts prot
  // PROT_READ, PROT_WRITE: Pages can be read and written flags MAP_PRIVATE:
  // Changes are private to the process flags MAP_ANONYMOUS: Mapping not backed
  // by a file File descriptor fd: -1 with MAP_ANONYMOUS offset 0: No offset
  // with MAP_ANONYMOUS
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
  arena->used = 0;

  return arena;
}

void *alloc(arena_t *arena, size_t size) {
  // TODO: allocate size using arena memory+used as a ref point
  // Make sure we don't overflow the arena
  if (arena->used + size > arena->size) {
    return NULL;
  }

  void *next_free_addr = arena->memory + arena->used;
  arena->used += size;
  return next_free_addr;
}

void reset(arena_t *arena) {
  // Reset the arena by simply setting setting used to 0
  // Further allocations with overwrite previously allocated memory
  arena->used = 0;
}

int destroy(arena_t *arena) {
  // TODO: return all memory to the OS using munmap
  // munmap returns 0 upon success, -1 otherwise
  return munmap(arena->memory, arena->size);
}
