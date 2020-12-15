KBUILD_AFLAGS += $(call cc-option,-m32)
KBUILD_CFLAGS += $(call cc-option,-m32)
LDFLAGS := -m elf_i386

objs-y += arch/x86/src

ldflags-y += -T $(srctree)/arch/x86/boot/boot.ld

PHONY += boot

quiet_cmd_lanthorn = LD      $@
      cmd_lanthorn = $(LD) $(LDFLAGS) $(ldflags-y) $^ -o $@

all:lanthorn

lanthorn : arch/x86/boot/built-in.o vmlanthorn
	$(call if_changed,lanthorn)

arch/x86/boot/built-in.o : boot

boot:
	$(Q)$(MAKE) $(build)=arch/x86/boot