#include <mikoOS/kernel.h>
#include <mikoOS/printk.h>
#include <mikoOS/pci.h>
#include <mikoOS/block_driver.h>

// driver operations.
static int open_ATA_disk(void);
static int close_ATA_disk(void);

// For find IDE interface.
static struct pci_device_info ata_info[] = {
	{ 0x8086, 0x7010, 1 }, // PIIX3 IDE Interface (Triton II)
};

struct blk_dev_driver_operations ata_dev = {
	.name = "ATA driver",
	.open = &open_ATA_disk,
	.close = &close_ATA_disk,
};

struct pci_device *this_device;

/////////////////////////////////////////////////
// private functions
/////////////////////////////////////////////////
static bool find_ata_device(void);

static int open_ATA_disk(void)
{
	printk("open_ATA_disk\n");

	return 0;
}

static int close_ATA_disk(void)
{
	printk("%s\n", __FUNCTION__);

	remove_blk_driver(&ata_dev);

	return 0;
}

static bool find_ata_device(void)
{
	int i;
	int size = sizeof(ata_info) / sizeof(ata_info[0]);

	for (i = 0; i < size; i++) {
		this_device = get_pci_device(ata_info[i].vender, ata_info[i].devid, ata_info[i].func);
		if (this_device) {
			printk("found ATA device %x:%x:%d\n", ata_info[i].vender, ata_info[i].devid, ata_info[i].func);
			return true;
		}
	}
	return false;

}

/////////////////////////////////////////////////
// public functions
/////////////////////////////////////////////////
bool init_ata_disk_driver(void)
{
	u_int32_t data;

	if (!find_ata_device())
		return false;

	data = pci_data_read(this_device, 0x10);
	printk("data is 0x%x\n", data);

	// register myself.
	register_blk_driver(&ata_dev);

	return true;
}