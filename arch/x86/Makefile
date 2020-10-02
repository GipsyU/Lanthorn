boot_dir := $(srctree)/arch/x86/boot

all: vmlanthorn boot.bin
	cat $(boot_dir)/boot.bin vmlanthorn > lanthorn

boot.bin:
	$(Q)$(MAKE) $(build)=$(boot_dir) $(boot_dir)/$@
