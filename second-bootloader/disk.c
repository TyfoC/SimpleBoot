#include "disk.h"

static uint8_t disk_index_ = 0;
static uint8_t last_head_index_ = 0;
static uint8_t last_sector_number_ = 0;
static uint16_t last_cylinder_index_ = 0;
static uint8_t sector_buffer_[SECTOR_SIZE];

uint8_t disk_select(uint8_t diskIndex) {
	uint16_t tmpLastHead;
	ASM(
		"pushl %%es;"
		"int $0x13;"
		"jc err%=;"
		"test %%ax, %%ax;"
		"jz done%=;"
		"err%=:;"
		"mov $0, %%dx;"
		"done%=:;"
		"popl %%es"
		:"=c"(last_cylinder_index_), "=d"(tmpLastHead)
		:"a"(0x0800), "d"(diskIndex));

	if (!tmpLastHead) return FALSE;

	disk_index_ = diskIndex;
	last_head_index_ = tmpLastHead >> 8;
	last_sector_number_ = last_cylinder_index_ & MAX_SECTOR_NUMBER;
	last_cylinder_index_ = (last_cylinder_index_ >> 8) | (((last_cylinder_index_ & 0xC0) >> 6) << 8);

	return TRUE;
}

uint8_t disk_get_index(void) {
	return disk_index_;
}

uint8_t disk_get_sectors_per_track(void) {
	return last_sector_number_;
}

uint16_t disk_get_heads_per_cylinder(void) {
	return last_head_index_ + 1;
}

uint16_t disk_get_number_of_cylinders(void) {
	return last_cylinder_index_ + 1;
}

uint8_t disk_get_last_head_index(void) {
	return last_head_index_;
}

uint16_t disk_get_last_cylinder_index(void) {
	return last_cylinder_index_;
}

void disk_lba_to_chs(uint32_t lba, uint8_t* sector, uint8_t* head, uint16_t* cylinder) {
	uint32_t tmp = lba / last_sector_number_;
	*sector = (lba % last_sector_number_) + 1;
	*head = tmp % (last_head_index_ + 1);
	*cylinder = tmp / (last_head_index_ + 1);
}

uint32_t disk_chs_to_lba(uint8_t sector, uint8_t head, uint16_t cylinder) {
	return (cylinder * (last_head_index_ + 1) + head) * last_sector_number_ + sector - 1;
}

/*
	dstAddr - buffer physical address, must not exceed 0xFFFFF
*/
uint16_t disk_read_sector(uint8_t sector, uint8_t head, uint8_t cylinder, uint16_t count, uint32_t dstAddr) {
	uint16_t readCount = 0, curReadCnt = 0;

	for (size_t i = 0; i < count; i++) {
		ASM(
			"pushl %%es;"
			"mov %%si, %%es;"
			"int $0x13;"
			"jc err%=;"
			"test %%ah, %%ah;"
			"jz done%=;"
			"err%=:;"
			"mov $0, %%ax;"
			"done%=:;"
			"popl %%es;"
			:"=a"(curReadCnt)
			:"a"(0x0201),
			"c"(CHS_PACK_CYL_SEC(cylinder, sector)),
			"d"(CHS_PACK_HEAD_DISK(head, disk_index_)),
			"b"(dstAddr & 0xFFFF),
			"S"((dstAddr >> 4) & 0xF000)
		);

		curReadCnt &= 0xFF;
		if (curReadCnt != 1) return readCount + curReadCnt;

		dstAddr += SECTOR_SIZE;

		if (sector < last_sector_number_) ++sector;
		else {
			if (head < last_head_index_) ++head;
			else {
				if (cylinder >= last_cylinder_index_) return readCount + curReadCnt;
				++cylinder;
				head = 0;
			}

			sector = 1;
		}

		readCount += curReadCnt;
	}

	return readCount;
}

uint16_t disk_write_sector(uint8_t sector, uint8_t head, uint8_t cylinder, uint16_t count, uint32_t srcAddr) {
	uint16_t writeCount = 0, curWriteCount = 0;

	for (size_t i = 0; i < count; i++) {
		ASM(
			"pushl %%es;"
			"mov %%si, %%es;"
			"int $0x13;"
			"jc err%=;"
			"test %%ah, %%ah;"
			"jz done%=;"
			"err%=:;"
			"mov $0, %%ax;"
			"done%=:;"
			"popl %%es;"
			:"=a"(curWriteCount)
			:"a"(0x0301),
			"c"(CHS_PACK_CYL_SEC(cylinder, sector)),
			"d"(CHS_PACK_HEAD_DISK(head, disk_index_)),
			"b"(srcAddr & 0xFFFF),
			"S"((srcAddr >> 4) & 0xF000)
		);

		curWriteCount &= 0xFF;
		if (curWriteCount != 1) return writeCount + curWriteCount;

		srcAddr += SECTOR_SIZE;

		if (sector < last_sector_number_) ++sector;
		else {
			if (head < last_head_index_) ++head;
			else {
				if (cylinder >= last_cylinder_index_) return writeCount + curWriteCount;
				++cylinder;
				head = 0;
			}

			sector = 1;
		}

		writeCount += curWriteCount;
	}

	return writeCount;
}

uint16_t disk_read_sector_lba(uint32_t lba, uint16_t count, uint32_t dstAddr) {
	uint8_t sector, head;
	uint16_t cylinder;
	disk_lba_to_chs(lba, &sector, &head, &cylinder);
	return disk_read_sector(sector, head, cylinder, count, dstAddr);
}

uint16_t disk_write_sector_lba(uint32_t lba, uint16_t count, uint32_t srcAddr) {
	uint8_t sector, head;
	uint16_t cylinder;
	disk_lba_to_chs(lba, &sector, &head, &cylinder);
	return disk_write_sector(sector, head, cylinder, count, srcAddr);
}

uint32_t disk_read_sector_ext(uint32_t lba, uint32_t count, uint32_t dstAddr) {
	uint32_t readCount = 0, curReadCnt = 0;

	uint8_t sector, head;
	uint16_t cylinder;
	disk_lba_to_chs(lba, &sector, &head, &cylinder);

	const size_t sectorLengthInLongs = SECTOR_SIZE >> 2;

	for (size_t i = 0; i < count; i++) {
		curReadCnt = disk_read_sector(sector, head, cylinder, 1, (uint32_t)&sector_buffer_[0]);
		if (curReadCnt != 1) return readCount;

		mem_copy_longs_from_pm((uint32_t)&sector_buffer_[0], dstAddr, sectorLengthInLongs);

		dstAddr += SECTOR_SIZE;

		if (sector < last_sector_number_) ++sector;
		else {
			if (head < last_head_index_) ++head;
			else {
				if (cylinder >= last_cylinder_index_) return readCount + curReadCnt;
				++cylinder;
				head = 0;
			}

			sector = 1;
		}

		readCount += curReadCnt;
	}

	return readCount;
}

uint32_t disk_write_sector_ext(uint32_t lba, uint32_t count, uint32_t srcAddr) {
	uint32_t writeCount = 0, curWriteCnt = 0;

	uint8_t sector, head;
	uint16_t cylinder;
	disk_lba_to_chs(lba, &sector, &head, &cylinder);

	const size_t sectorLengthInLongs = SECTOR_SIZE >> 2;

	for (size_t i = 0; i < count; i++) {
		mem_copy_longs_from_pm(srcAddr, (uint32_t)&sector_buffer_[0], sectorLengthInLongs);
		curWriteCnt = disk_write_sector(sector, head, cylinder, 1, (uint32_t)&sector_buffer_[0]);
		if (curWriteCnt != 1) return writeCount;

		srcAddr += SECTOR_SIZE;

		if (sector < last_sector_number_) ++sector;
		else {
			if (head < last_head_index_) ++head;
			else {
				if (cylinder >= last_cylinder_index_) return writeCount + curWriteCnt;
				++cylinder;
				head = 0;
			}

			sector = 1;
		}

		writeCount += curWriteCnt;
	}

	return writeCount;
}