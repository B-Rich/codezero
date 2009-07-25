/*
 * Boot memory allocator
 *
 * Copyright (C) 2009 Bahadir Balban
 */


#include INC_ARCH(linker.h)
#include INC_GLUE(memory.h)
#include <l4/lib/printk.h>

/* All memory allocated here is discarded after boot */

#define BOOTMEM_SIZE		SZ_32K

SECTION(".init.pgd") pgd_table_t init_pgd;
SECTION(".init.bootmem") char bootmem[BOOTMEM_SIZE];

static unsigned long cursor = (unsigned long)&bootmem;

void *alloc_bootmem(int size, int alignment)
{
	void *ptr;

	/* If alignment is required */
	if (alignment) {
		/* And cursor is not aligned */
		if (!is_aligned(cursor, alignment))
			/* Align the cursor to alignment */
			cursor = align_up(cursor, alignment);
	}

	/* Allocate from cursor */
	ptr = (void *)cursor;

	/* Update cursor */
	cursor += size;

	/* Check if cursor is passed bootmem area */
	if (cursor >= (unsigned long)&bootmem[BOOTMEM_SIZE]) {
		printk("Fatal: Insufficient boot memory.\n");
		BUG();
	}

	return ptr;
}

pmd_table_t *alloc_boot_pmd(void)
{
	return alloc_bootmem(sizeof(pmd_table_t), sizeof(pmd_table_t));
}
