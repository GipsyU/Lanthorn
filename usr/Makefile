quiet_cmd_cc_elf_o = CC      $@
      cmd_cc_elf_o = $(CC) -m32 -c $< -o $@

LANTHORNINCLUDE := 

usr-y := init.elf test/

usr-y := $(addprefix $(obj)/, $(usr-y))

usr_lib := $(src)/lib

usr_dir := $(filter %/, $(usr-y))

usr_dir_o := $(patsubst %/, %/built-in.o, $(usr_dir))

usr_dir_elfs := $(patsubst %/, %.elf, $(usr_dir))

usr_sg_elfs := $(filter-out %/, $(usr-y))

LDFLAGS_usr.elf += -r -b binary

KBUILD_CFLAGS += -I$(src)/include -D__USER__

KBUILD_AFLAGS += -D__USER__

quiet_cmd_usr = LD      $@
      cmd_usr = $(LD) $(LDFLAGS) -r -b binary $(usr_sg_elfs) $(usr_dir_elfs) -o $@

$(obj)/usr.elf : $(usr_lib) $(usr_sg_elfs) $(usr_dir_elfs)
	$(call if_changed,usr)

ldflags-y += -T $(srctree)/$(src)/usr.ld

$(usr_dir_elfs) : %.elf : %/built-in.o $(usr_lib)/lib.a
	$(call if_changed,ld)

PHONY += $(usr_dir) $(usr_lib)

$(usr_dir_o): $(usr_dir);

$(usr_lib)/lib.a : $(usr_lib);

$(usr_lib):
	$(Q)$(MAKE) $(build)=$@

$(usr_dir):
	$(Q)$(MAKE) $(build)=$(patsubst %/,%,$@)

$(usr_sg_elfs) : %.elf : %.o $(usr_lib)/lib.a
	$(call if_changed,ld)