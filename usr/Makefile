quiet_cmd_cc_elf_o = CC      $@
      cmd_cc_elf_o = $(CC) -m32 -c $< -o $@

usr-y := init.elf main.elf

usr-y := $(addprefix $(obj)/, $(usr-y))

usr_dir := $(filter %/, $(usr-y))

usr_dir_o := $(patsubst %/, %/built-in.o, $(usr_dir))

usr_dir_elfs := $(patsubst %/, %.elf, $(usr_dir))

usr_sg_elfs := $(filter-out %/, $(usr-y))

LDFLAGS_usr.elf += -r -b binary

quiet_cmd_usr = LD      $@
      cmd_usr = $(LD) $(LDFLAGS) -r -b binary $^ -o $@

$(obj)/usr.elf : $(usr_sg_elfs) $(usr_dir_elfs)
	$(call if_changed,usr)

ldflags-y += -T $(src)/usr.ld

$(usr_dir_elfs) : %.elf : %/built-in.o
	$(call if_changed,ld)

PHONY += $(usr_dir)

$(usr_dir_o): $(usr_dir)

$(usr_dir):
	$(Q)$(MAKE) $(build)=$@

$(usr_sg_elfs) : %.elf : %.o
	$(call if_changed,ld)