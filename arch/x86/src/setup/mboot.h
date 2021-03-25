#ifndef _X86_BOOT_MBOOT_H_
#define _X86_BOOT_MBOOT_H_
#include <arch/basic.h>

struct multiboot_t {
	u32_t flags;
	/** 
	 * 从 BIOS 获知的可用内存
	 *
	 * mem_lower和mem_upper分别指出了低端和高端内存的大小，单位是K。
	 * 低端内存的首地址是0，高端内存的首地址是1M。
	 * 低端内存的最大可能值是640K。
	 * 高端内存的最大可能值是最大值减去1M。但并不保证是这个值。
	 */
	u32_t mem_lower;
	u32_t mem_upper;

	u32_t boot_device;		// 指出引导程序从哪个BIOS磁盘设备载入的OS映像
	u32_t cmdline;		// 内核命令行
	u32_t mods_count;		// boot 模块列表
	u32_t mods_addr;
	
	/**
	 * ELF 格式内核映像的section头表。
	 * 包括每项的大小、一共有几项以及作为名字索引的字符串表。
	 */
	u32_t num;
	u32_t size;
	u32_t addr;
	u32_t shndx;

	/**
	 * 以下两项指出保存由BIOS提供的内存分布的缓冲区的地址和长度
	 * mmap_addr是缓冲区的地址，mmap_length是缓冲区的总大小
	 * 缓冲区由一个或者多个下面的大小/结构对 mmap_entry_t 组成
	 */
	u32_t mmap_length;		
	u32_t mmap_addr;
	
	u32_t drives_length; 	// 指出第一个驱动器结构的物理地址	
	u32_t drives_addr; 		// 指出第一个驱动器这个结构的大小
	u32_t config_table; 		// ROM 配置表
	u32_t boot_loader_name; 	// boot loader 的名字
	u32_t apm_table; 	    	// APM 表
	u32_t vbe_control_info;
	u32_t vbe_mode_info;
	u32_t vbe_mode;
	u32_t vbe_interface_seg;
	u32_t vbe_interface_off;
	u32_t vbe_interface_len;
} *mboot_ptr;

/**
 * size是相关结构的大小，单位是字节，它可能大于最小值20
 * base_addr_low是启动地址的低32位，base_addr_high是高32位，启动地址总共有64位
 * length_low是内存区域大小的低32位，length_high是内存区域大小的高32位，总共是64位
 * type是相应地址区间的类型，1代表可用RAM，所有其它的值代表保留区域
 */

struct mmap_entry_t {
	u32_t size; 		// 留意 size 是不含 size 自身变量的大小
	u32_t base_addr_low;
	u32_t base_addr_high;
	u32_t length_low;
	u32_t length_high;
	u32_t type;
};

#endif