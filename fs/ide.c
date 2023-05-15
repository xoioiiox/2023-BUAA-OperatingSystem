/*
 * operations on IDE disk.
 */

#include "serv.h"
#include <drivers/dev_disk.h>
#include <lib.h>
#include <mmu.h>

int ys[32];
int bitmap1;
int cnt[32];

void ssd_init() {
	for (int i = 0; i < 32; i++) {
		ys[i] = -1;
		cnt[i] = 0;
	}
	bitmap1 = 0xffffffff;
}

int ssd_read(u_int logic_no, void *dst) {
	int n = ys[logic_no];
	if (n == -1) {
		return -1;
	}
	ide_read(0, n, dst, 1);
	return 0;
}
void ssd_write(u_int logic_no, void *src) {
	int n = ys[logic_no];
	int desn;
	if (n != -1) {
		//xiancachu
		ssd_erase(logic_no);
	}
	int min = 1000000;
	int min_num = 0;
	int min1 = 100000;
	int min_num1 = 0;
	for (int i = 0; i < 32; i++) {
		if (((bitmap1 >> i) & 1) == 1) {
			if (cnt[i] < min) {
				min = cnt[i];
				min_num = i;
			}
		}
	}
	desn = min_num;
	if (min >= 5) {
		for (int i = 0; i < 32; i++) {
			if (((bitmap1 >> i) & 1) == 0) {
				if (cnt[i] < min1) {
					min1 = cnt[i];
					min_num1 = i;
				}
			}
		}
		char t[256] = {0};
		ide_read(0, min_num1, &t, 1);
		ide_write(0, min_num, &t, 1);
		//memcpy(0xa0000000 +  DEV_DISK_ADDRESS + BY2SECT * min_num, 0xa0000000 +  DEV_DISK_ADDRESS + BY2SECT * min_num1, BY2SECT);
		bitmap1 -= (1 << min_num);
		for (int i = 0; i < 32; i++) {
			if (ys[i] == min_num1) {
				ssd_erase(i);
				ys[i] = min_num;
				break;
			}
		}
		desn = min_num1;
		//zuowei kexie
	}
	ide_write(0, desn, src, 1);
	bitmap1 -= (1 << desn);
	ys[logic_no] = desn;
}
void ssd_erase(u_int logic_no) {
	if (ys[logic_no] != -1) {
		int n = ys[logic_no];
		cnt[n]++;
		int tmp[16] = {0};
		ide_write(0, n, &tmp, 1);
		bitmap1 |= (1 << n);
		ys[logic_no] = -1;
	}
}


// Overview:
//  read data from IDE disk. First issue a read request through
//  disk register and then copy data from disk buffer
//  (512 bytes, a sector) to destination array.
//
// Parameters:
//  diskno: disk number.
//  secno: start sector number.
//  dst: destination for data read from IDE disk.
//  nsecs: the number of sectors to read.
//
// Post-Condition:
//  Panic if any error occurs. (you may want to use 'panic_on')
//
// Hint: Use syscalls to access device registers and buffers.
// Hint: Use the physical address and offsets defined in 'include/drivers/dev_disk.h':
//  'DEV_DISK_ADDRESS', 'DEV_DISK_ID', 'DEV_DISK_OFFSET', 'DEV_DISK_OPERATION_READ',
//  'DEV_DISK_START_OPERATION', 'DEV_DISK_STATUS', 'DEV_DISK_BUFFER'
void ide_read(u_int diskno, u_int secno, void *dst, u_int nsecs) {
	u_int begin = secno * BY2SECT;
	u_int end = begin + nsecs * BY2SECT;
	u_int rw = 0;
	u_int flag = 0;

	for (u_int off = 0; begin + off < end; off += BY2SECT) {
		uint32_t temp = diskno;
		uint32_t addr = begin + off;
		/* Exercise 5.3: Your code here. (1/2) */
		panic_on(syscall_write_dev(&temp, DEV_DISK_ADDRESS + DEV_DISK_ID, sizeof(uint32_t)));
		panic_on(syscall_write_dev(&addr, DEV_DISK_ADDRESS + DEV_DISK_OFFSET, sizeof(uint32_t)));
		rw = DEV_DISK_OPERATION_READ;
		panic_on(syscall_write_dev(&rw, DEV_DISK_ADDRESS + DEV_DISK_START_OPERATION, sizeof(u_int)));
		flag = 0;
		panic_on(syscall_read_dev(&flag, DEV_DISK_ADDRESS + DEV_DISK_STATUS, sizeof(u_int)));
		if (flag == 0) {
			panic_on("ide_read_fail");
		}
		panic_on(syscall_read_dev(dst + off, DEV_DISK_ADDRESS + DEV_DISK_BUFFER, BY2SECT));
	}
}

// Overview:
//  write data to IDE disk.
//
// Parameters:
//  diskno: disk number.
//  secno: start sector number.
//  src: the source data to write into IDE disk.
//  nsecs: the number of sectors to write.
//
// Post-Condition:
//  Panic if any error occurs.
//
// Hint: Use syscalls to access device registers and buffers.
// Hint: Use the physical address and offsets defined in 'include/drivers/dev_disk.h':
//  'DEV_DISK_ADDRESS', 'DEV_DISK_ID', 'DEV_DISK_OFFSET', 'DEV_DISK_BUFFER',
//  'DEV_DISK_OPERATION_WRITE', 'DEV_DISK_START_OPERATION', 'DEV_DISK_STATUS'
void ide_write(u_int diskno, u_int secno, void *src, u_int nsecs) {
	u_int begin = secno * BY2SECT;
	u_int end = begin + nsecs * BY2SECT;
	u_int rw = 1;
	u_int flag = 0;

	for (u_int off = 0; begin + off < end; off += BY2SECT) {
		uint32_t temp = diskno;
		uint32_t addr = begin + off;
		/* Exercise 5.3: Your code here. (2/2) */
		panic_on(syscall_write_dev(&temp, DEV_DISK_ADDRESS + DEV_DISK_ID, sizeof(uint32_t)));
		panic_on(syscall_write_dev(&addr, DEV_DISK_ADDRESS + DEV_DISK_OFFSET, sizeof(uint32_t)));
		panic_on(syscall_write_dev(src + off, DEV_DISK_ADDRESS + DEV_DISK_BUFFER, BY2SECT));
		rw = DEV_DISK_OPERATION_WRITE;
		panic_on(syscall_write_dev(&rw, DEV_DISK_ADDRESS + DEV_DISK_START_OPERATION, sizeof(u_int)));
		flag = 0;
		panic_on(syscall_read_dev(&flag, DEV_DISK_ADDRESS + DEV_DISK_STATUS, sizeof(u_int)));
		if (flag == 0) {
			panic_on("ide_write_fail");
		}
	}
}
