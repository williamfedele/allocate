#include <stdalign.h>
#include <stdio.h>

#include "../src/arena_allocator.h"

// Ensures arenas can be created correctly
void test_arena_init() {

  printf("test_arena_init: ");

  arena_t *arena = arena_init(2 * sizeof(int));

  if (arena == NULL) {
    printf("error: failed to initialize arena\n");
    return;
  }

  if (arena->size != 2 * sizeof(int)) {
    printf("error: arena did not allocate enough memory\n");
    return;
  }

  if (arena->offset != 0) {
    printf("error: new arena should not have used memory\n");
    return;
  }

  printf("passed\n");
}

// Should be able to allocate up to the maximum allocated memory
// Further allocations should fail
void test_allocate_and_overflow() {

  printf("test_allocate_and_overflow: ");

  arena_t *arena = arena_init(2 * sizeof(int));

  void *alloc1 = arena_alloc(arena, sizeof(int), alignof(int));

  if (arena->offset != sizeof(int)) {
    printf("error: arena failed the first allocation\n");
    return;
  }

  void *alloc2 = arena_alloc(arena, sizeof(int), alignof(int));

  if (arena->offset != 2 * sizeof(int)) {
    printf("error: arena failed the second allocation\n");
    return;
  }

  // Overflow
  void *alloc3 = arena_alloc(arena, sizeof(char), alignof(char));

  if (alloc3 != NULL) {
    printf("error: arena allocation overflow\n");
    return;
  }

  printf("passed\n");
}

// Freeing the arena sets the used field to 0
void test_free() {

  printf("test_free: ");

  arena_t *arena = arena_init(sizeof(int));

  void *ptr = arena_alloc(arena, sizeof(int), alignof(int));

  if (arena->offset != sizeof(int)) {
    printf("error: arena allocation failed\n");
    return;
  }

  arena_free(arena);

  if (arena->offset != 0) {
    printf("error: arena did not reset used memory\n");
    return;
  }

  printf("passed\n");
}

// Arena memory should be zeroed upon allocation
void test_arena_mem_zeroed() {

  printf("test_arena_mem_zeroed: ");
  arena_t *arena = arena_init(sizeof(int));

  int *ptr = (int *)arena_alloc(arena, sizeof(int), alignof(int));
  *ptr = 1;

  arena_free(arena);

  int *ptr2 = (int *)arena_alloc(arena, sizeof(int), alignof(int));

  if (*ptr2 == 1) {
    printf("error: arena did not zero previously allocated memory\n");
    return;
  }
  printf("passed\n");
}

// Memory should be aligned to word boundaries
// A char uses 1 byte, 1 byte alignment
// An int used 4 bytes, 4 byte alignment
// Memory alignment should look like the following:
//  0 1 2 3 4 5 6 7
// |c| | | |i|i|i|i|
// Further allocations should fail due to exhausting memory
void test_arena_alignment() {

  printf("test_arena_alignment: ");

  arena_t *arena = arena_init(8);

  void *ptr1 = arena_alloc(arena, sizeof(char), alignof(char));

  if (arena->offset != sizeof(char)) {
    printf("error: arena failed the first allocation\n");
    return;
  }

  void *ptr2 = arena_alloc(arena, sizeof(int), alignof(int));

  // Arena used space should be the whole 8 byte mapping due to alignment
  if (arena->offset != 2 * sizeof(int)) {
    printf("error: arena did not align correctly\n");
    return;
  }

  // This allocation should fail. Arena is full with aligned int
  void *ptr3 = arena_alloc(arena, sizeof(char), alignof(char));

  if (ptr3 != NULL) {
    printf("error: arena should not have allocated past mapping\n");
    return;
  }

  printf("passed\n");
}

// Alignment of data types should all be powers of 2, nonzero
void test_arena_alignment_not_pow_two() {

  printf("test_arena_alignment_not_pow_two: ");

  arena_t *arena = arena_init(4);

  void *ptr = arena_alloc(arena, sizeof(int), 3);

  if (ptr != NULL) {
    printf("error: non power of two alignment should have failed\n");
    return;
  }

  printf("passed\n");
}

int main() {

  test_arena_init();
  test_allocate_and_overflow();
  test_free();
  test_arena_mem_zeroed();
  test_arena_alignment();
  test_arena_alignment_not_pow_two();
  return 0;
}
