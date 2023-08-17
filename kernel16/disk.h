#pragma once
#ifndef DISK_H
#define DISK_H

/*
	References:
		https://en.wikipedia.org/wiki/INT_13H
		https://wiki.osdev.org/Disk_access_using_the_BIOS_(INT_13h)
*/

#include "bios.h"
#include "mem.h"

#define MAX_SECTOR_NUMBER					0x3F
#define CHS_PACK_CYL_SEC($cyl, $sec)		((($cyl & 0xFF) << 8) | ((($cyl >> 8) & 0xFF) << 6) | ($sec & MAX_SECTOR_NUMBER))
#define CHS_PACK_HEAD_DISK($head, $disk)	(($head << 8) | ($disk & 0xFF))

uint8_t disk_select(uint8_t diskIndex);

uint8_t disk_get_index(void);
uint8_t disk_get_sectors_per_track(void);
uint16_t disk_get_heads_per_cylinder(void);
uint16_t disk_get_number_of_cylinders(void);
uint8_t disk_get_last_head_index(void);
uint16_t disk_get_last_cylinder_index(void);

void disk_lba_to_chs(uint32_t lba, uint8_t* sector, uint8_t* head, uint16_t* cylinder);
uint32_t disk_chs_to_lba(uint8_t sector, uint8_t head, uint16_t cylinder);

uint16_t disk_read_sector(uint8_t sector, uint8_t head, uint8_t cylinder, uint16_t count, uint32_t dstAddr);
uint16_t disk_write_sector(uint8_t sector, uint8_t head, uint8_t cylinder, uint16_t count, uint32_t srcAddr);

uint16_t disk_read_sector_lba(uint32_t lba, uint16_t count, uint32_t dstAddr);
uint16_t disk_write_sector_lba(uint32_t lba, uint16_t count, uint32_t srcAddr);

uint32_t disk_read_sector_ext(uint32_t lba, uint32_t count, uint32_t dstAddr);
uint32_t disk_write_sector_ext(uint32_t lba, uint32_t count, uint32_t srcAddr);

#endif