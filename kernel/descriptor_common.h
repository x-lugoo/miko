#ifndef __DESCRIPTOR_COMMON_H
#define __DESCRIPTOR_COMMON_H 1

#include <mikoOS/kernel.h>

// selectors.
#define KERN_CS 0x08
#define KERN_DS 0x10
#define KERN_SS 0x18

// gate types.
#define GATE_TYPE_CALL_GATE 0x84
#define GATE_TYPE_INTR_GATE 0x8e
#define GATE_TYPE_TRAP_GATE 0x8f
#define GATE_TYPE_TASK_GATE 0x85

/**
 * this structure represent of segment descriptor
 */
struct segment_descriptor {
	u_int16_t limit0;
	u_int8_t base0;
	u_int8_t base1;
	u_int8_t base2;
	u_int8_t attr1;
	u_int8_t attr2;
	u_int8_t base3;
} __attribute__ ((packed));

/**
 * Gate descriptor table.
 */
struct gate_descriptor {
	u_int16_t offsetL;
	u_int16_t selector;
	u_int8_t count;
	u_int8_t type;
	u_int16_t offsetH;
} __attribute__((packed));

/** 
 * This structure uses for descriptor table
 * e.g. gdt, interruput and etc.
 * This structure shouldn't have any padding space.
 */
struct descriptor_table {
	u_int16_t limit;
	u_int32_t base;
} __attribute__ ((packed));


#endif // __DESCRIPTOR_COMMON_H
