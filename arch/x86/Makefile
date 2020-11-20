boot_dir := $(srctree)/arch/x86/boot

KBUILD_AFLAGS += $(call cc-option,-m32)
KBUILD_CFLAGS += $(call cc-option,-m32)
LDFLAGS := -m elf_i386

objs-y += arch/x86/src

all: boot vmlanthorn
	$(Q)$(LD) $(LDFLAGS) -T arch/x86/boot/boot.ld arch/x86/boot/built-in.o vmlanthorn -o lanthorn

boot:
	$(Q)$(MAKE) $(build)=$(boot_dir)
