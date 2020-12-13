quiet_cmd_elfcc = CC      $@
      cmd_elfcc = $(CC) -m32 -c $< -o $(patsubst %.elf, %.o, $@); \
	  			ld -m elf_i386 -N -e start -Ttext 0 $(patsubst %.elf, %.o, $@) -o $@

elf-y := init.elf

USR_ELFS = $(addprefix $(obj)/,$(elf-y))

LDFLAGS_usr.elf := -m elf_i386 -r -b binary

$(obj)/usr.elf : $(USR_ELFS)
	$(call if_changed,ld)

$(obj)/%.elf : $(src)/%.c
	$(call if_changed,elfcc)

$(obj)/%.elf : $(src)/%.S
	$(call if_changed,elfcc)