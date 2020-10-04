boot_dir := $(srctree)/arch/x86/boot

KBUILD_AFLAGS += $(call cc-option,-m32)
KBUILD_CFLAGS += $(call cc-option,-m32)

KBUILD_CFLAGS += -msoft-float -mregparm=3 -freg-struct-return

# prevent gcc from keeping the stack 16 byte aligned
KBUILD_CFLAGS += $(call cc-option,-mpreferred-stack-boundary=2)

# Disable unit-at-a-time mode on pre-gcc-4.0 compilers, it makes gcc use
# a lot more stack due to the lack of sharing of stacklots:
KBUILD_CFLAGS += $(call cc-ifversion, -lt, 0400, \
		$(call cc-option,-fno-unit-at-a-time))

KBUILD_CFLAGS += -march=i386

# temporary until string.h is fixed
KBUILD_CFLAGS += -ffreestanding

LDFLAGS := -m elf_i386

all: vmlanthorn boot.bin
	cat $(boot_dir)/boot.bin vmlanthorn > lanthorn

boot.bin:
	$(Q)$(MAKE) $(build)=$(boot_dir) $(boot_dir)/$@
