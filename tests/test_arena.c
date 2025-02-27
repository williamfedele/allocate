#include <stdio.h>

#include "../src/heap/arena_allocator.h"

void test_arena_init() {

  arena_t *arena = arena_init(2 * sizeof(int));

  if (arena == NULL) {
    printf("test_arena_init: Failed to initialize arena\n");
    return;
  }

  if (arena->size != 2 * sizeof(int)) {
    printf("test_arena_init: Arena did not allocate enough memory\n");
    return;
  }

  if (arena->used != 0) {
    printf("test_arena_init: New arena should not have used memory\n");
    return;
  }

  printf("test_arena_init: Passed\n");
}

void test_allocate_and_overflow() {

  arena_t *arena = arena_init(2 * sizeof(int));

  void *alloc1 = arena_alloc(arena, sizeof(int));

  if (arena->used != sizeof(int)) {
    printf("test_allocate_and_overflow: Arena failed the first allocation\n");
    return;
  }

  void *alloc2 = arena_alloc(arena, sizeof(int));

  if (arena->used != 2 * sizeof(int)) {
    printf("test_allocate_and_overflow: Arena failed the second allocation\n");
    return;
  }

  // Overflow
  void *alloc3 = arena_alloc(arena, 1);

  if (alloc3 != NULL) {
    printf("test_allocate_and_overflow: Arena allocation overflow\n");
    return;
  }

  printf("test_allocate_and_overflow: Passed\n");
}

void test_reset() {

  arena_t *arena = arena_init(sizeof(int));

  void *ptr = arena_alloc(arena, sizeof(int));

  if (arena->used != sizeof(int)) {
    printf("test_reset: Arena allocation failed\n");
    return;
  }

  arena_free(arena);

  if (arena->used != 0) {
    printf("test_reset: Arena did not reset used memory\n");
    return;
  }

  printf("test_reset: Passed\n");
}

// Arena memory should be zeroed upon allocation
void test_arena_mem_zeroed() {

  arena_t *arena = arena_init(sizeof(int));

  int *ptr = (int *)arena_alloc(arena, sizeof(int));
  *ptr = 1;

  arena_free(arena);

  int *ptr2 = (int *)arena_alloc(arena, sizeof(int));

  if (*ptr2 == 1) {
    printf("test_arena_mem_zeroed: Arena did not zero previously allocated "
           "memory\n");
    return;
  }
  printf("test_arena_mem_zeroed: Passed\n");
}

int main() {

  test_arena_init();
  test_allocate_and_overflow();
  test_reset();
  test_arena_mem_zeroed();
  return 0;
}
