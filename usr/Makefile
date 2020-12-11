quiet_cmd_elfcc = CC      $@
      cmd_elfcc = $(CC) -m32 $< -o $@

elf-y := test.elf test2.elf

USR_ELFS = $(addprefix $(obj)/,$(elf-y))

all:$(obj)/usr.elf

LDFLAGS_usr.elf := -m elf_i386 -r -b binary

$(obj)/usr.elf : $(USR_ELFS)
	$(call if_changed,ld)

$(obj)/%.elf : $(src)/%.c FORCE
	$(call if_changed,elfcc)