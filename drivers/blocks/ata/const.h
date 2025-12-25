#define ATA_PRIMARY_BASE 0x1F0
#define ATA_SECONDARY_BASE 0x170


#define ATA_PRIMARY_CONTROL 0x3F6
#define ATA_SECONDARY_CONTROL 0x376

#define ATA_SLAVE_DRIVE 1
#define ATA_MASTER_DRIVE 0


/*
 * ATA Status Register
 */
#define ATA_STATUS_BSY 0x80
#define ATA_STATUS_DRDY 0x40
#define ATA_STATUS_DRQ 0x08
#define ATA_STATUS_ERR 0x01

/*
 * ATA PIO Registers offsets from IO base
 */
#define ATA_RW_REG  0x0
#define ATA_ERR_REG 0x1
#define ATA_FEATURE_REG 0x1
#define ATA_SEC_COUNT_REG 0x2
#define ATA_LBA_LOW_REG 0x3
#define ATA_LBA_MID_REG 0x4
#define ATA_LBA_HIGH_REG 0x5
#define ATA_DRIVE_HEAD_REG 0x6
#define ATA_STATUS_REG 0x7
#define ATA_COMMAND_REG 0x7

/*
 * ATA PIO Registers offsets from IO control base
 */
#define ATA_ALT_STATUS_REG 0x0
#define ATA_CONTROL_REG 0x0
#define ATA_DRIVE_ADDRESS_REG 0x1

/*
 * ATA Commands
 */
#define ATA_IDENRIFY_CMD 0xEC
#define ATA_SELECT_SLAVE_DRIVE_CMD 0xB0
#define ATA_SELECT_MASTER_DRIVE_CMD 0xA0
#define ATA_READ_CMD 0x20
#define ATA_WRITE_CMD 0x30
#define ATA_SELECT_LBA48_CMD 0x24

/*
flags
*/
#define ATA_FLAG_PRESENT 0x01
#define ATA_FLAG_LBA48 0x02
#define ATA_FLAG_LBA28 0x04

/*
 * ATA Interrupts
 */
#define ATA_PRIMARY_IRQ 14
#define ATA_SECONDARY_IRQ 15
