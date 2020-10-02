boot-y := boot.o

BOOT_OBJS = $(addprefix $(obj)/,$(boot-y))

OBJCOPYFLAGS_boot.bin := -O binary
$(obj)/boot.bin: $(obj)/boot.elf FORCE
	$(call if_changed,objcopy)

LDFLAGS_boot.elf := -T
$(obj)/boot.elf: $(src)/boot.ld $(BOOT_OBJS) FORCE
	$(call if_changed,ld)
