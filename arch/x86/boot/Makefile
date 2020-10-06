boot-y := boot.o main.o

BOOT_OBJS = $(addprefix $(obj)/,$(boot-y))

KBUILD_AFLAGS	:= $(KBUILD_CFLAGS) -D__ASSEMBLY__

LDFLAGS_boot.elf := -T
$(obj)/boot.elf: $(src)/boot.ld $(BOOT_OBJS) FORCE
	$(call if_changed,ld)
