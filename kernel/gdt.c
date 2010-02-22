#include <mikoOS/kernel.h>
#include "gdt.h"
#include "process.h"

static struct descriptor_table gdtr;
static struct segment_descriptor gdt[GDT_TABLE_NUM];
static u_int32_t gdt_count = 0;

/////////////////////////////////////////////////
// private functions
/////////////////////////////////////////////////
static void setup_gdt_descriptor(void);
static void lgdt(void);

/**
 * setup gdt descriptors.
 */
static void setup_gdt_descriptor(void)
{
	set_gdt_values(0, 0, 0, 0, 0);
	set_gdt_values(1, 0, 0xffffffff, SEG_TYPE_CODE, 0xc0);
	set_gdt_values(2, 0, 0xffffffff, SEG_TYPE_DATA, 0xc0);
	set_gdt_values(3, 0, 0, SEG_TYPE_STACK, 0xc0);
	set_gdt_values(4, 0, 0xffffffff, SEG_TYPE_USER_CODE, 0xc0);
	set_gdt_values(5, 0, 0xffffffff, SEG_TYPE_USER_DATA, 0xc0);
}

/**
 * load gdt table to memory and do segment jump.
 */
static void lgdt(void)
{
	// setup gdtr.
	gdtr.limit = sizeof(gdt) - 1;
	gdtr.base = (u_int32_t) gdt;

	// now, we can load new gdt value.
	__asm__ __volatile__ ("lgdt %0;\n\t" ::"m"(gdtr));

	// after loading gdt, we need to do segment jump.
	__asm__ (
		"ljmp $0x08, $1f;\n\t"
		"1:;\n\t"
		"movl $0x10, %eax;\n\t"
		"movl %eax, %ds;\n\t"
		"movl %eax, %es\n\t;"
		"movl %eax, %fs\n\t"
		"movl %eax, %gs\n\t"
		"movl %eax, %ss\n\t"
		);

}

/////////////////////////////////////////////////
// public functions
/////////////////////////////////////////////////
/**
 * Setup GDTR and segment descriptor for GDT.
 */
void setup_gdt(void)
{
	// setup gdt descriptors.
	setup_gdt_descriptor();

	// load gdtr to memory.
	lgdt();
}

/**
 * set gdt descriptor values.
 * @param index is index of gdt descriptor tabel.
 * @param base bass address.
 * @param limit limit.
 * @param type what type is this descriptor.
 */
void set_gdt_values(u_int32_t index, u_int32_t base, 
		    u_int32_t limit, u_int8_t type,
		    u_int8_t access)
{
	struct segment_descriptor *p = &gdt[index];

	if ((limit > 65536) && (limit & 0xfff) != 0xfff) {
		printk("You can't do that!");
	}

	if (limit > 65536)
		// Adjust granularity if required
		limit = limit >> 12;
	// limit.
	p->byte[0] = limit & 0xff;
	p->byte[1] = (limit >> 8) & 0xff;


	// segment base.
	p->byte[2] = base & 0xff;
	p->byte[3] = (base >> 8) & 0xff;
	p->byte[4] = (base >> 16) & 0xff;
	p->byte[7] = (base >> 24) & 0xff;

	// attribute.
	p->byte[5] = type;
	p->byte[6] = access | (limit >> 16) & 0xf;
	gdt_count++;
}

/**
 * Search unused gdt table.
 * @retun If there is empty table it returns index.
 */
int search_unused_gdt_index(void)
{
	int i, idx;
	for (i = 0, idx = gdt_count; i < GDT_TABLE_NUM; i++, idx++) {
		if (i >= GDT_TABLE_NUM) 
			idx = 1;

		if (gdt[idx].byte[7] == 0)
			return idx;

	}

	return 0;

}

void gdt_types(void)
{
	int i;
	for (i = 1; i < 10; i++) {
		printk("gdt[%d:0x%x]'s type:0x%x attr:0x%x\n", i, i * 8, gdt[i].byte[5], gdt[i].byte[6]);

	}
}

	
