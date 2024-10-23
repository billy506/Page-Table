#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "config.h"
#include "mlpt.h"
#define ZERO_BITS 3 //peer reviewed (received help from James Xu)

// Page table base register definition
size_t ptbr = 0;
int page_size = 1 << POBITS; //refactor ideas from James Xu
#define ALL_ONES ((size_t)-1)

// Helper function to calculate the index for each level
static size_t calculate_index(size_t va, int level) {
    int index_bits = POBITS - ZERO_BITS;
    size_t shift = POBITS + index_bits * (LEVELS - level - 1); 
    size_t mask = (1 << index_bits) - 1;
    return (va >> shift) & mask;
}

// Allocate pages and setup mapping for the given virtual address
void page_allocate(size_t va) {
    if (ptbr == 0) {
        // Allocate the root level page table
        posix_memalign((void **)&ptbr, page_size, page_size);
        memset((void *)ptbr, 0, page_size);
    }

    size_t *current = (size_t *)ptbr;
        
    for (int i = 0; i < LEVELS - 1; ++i) {
        size_t index = calculate_index(va, i);

        if ((current[index] & 1) == 0) {
            // Allocate new table if not valid
            size_t *new_page;
            posix_memalign((void **) &new_page, page_size, page_size);
            memset((void *)new_page, 0, page_size);
            current[index] = (size_t)new_page | 1;  // Valid bit set
        }

        current = (size_t *)(current[index] & ~(page_size - 1));
    }

    // Allocate physical page for the final level
    size_t index = calculate_index(va, LEVELS - 1);
    if ((current[index] & 1) == 0) {
        size_t *new_page;
        posix_memalign((void **) &new_page, page_size, page_size);
        memset((void *)new_page, 0, page_size);
        current[index] = (size_t)new_page | 1;  // Valid bit set
    }
}

// Translate virtual address to physical address
size_t translate(size_t va) {
    if (ptbr == 0) {
        return ALL_ONES;  // No page table allocated
    }

    size_t *current = (size_t *)ptbr;

    for (int i = 0; i < LEVELS; ++i) {
        size_t index = calculate_index(va, i);
        if ((current[index] & 1) == 0) {
            return ALL_ONES;  // Entry is not valid
        }
        current = (size_t *)(current[index] & ~(page_size - 1));
    }

    // Calculate physical address
    size_t page_offset = va & (page_size - 1);
    return ((size_t)current & ~(page_size - 1)) + page_offset; //peer reviewed (modified by peers)
}
