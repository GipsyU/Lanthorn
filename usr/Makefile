quiet_cmd_cc_elf_o = CC      $@
      cmd_cc_elf_o = $(CC) -m32 -c $< -o $@

LANTHORNINCLUDE :=

usr-y := init.elf devsrv/ helloworld.elf poweroff.elf cat.elf test/ shell/ tmp.elf ls.elf logctrl.elf touch.elf rm.elf

usr-y := $(addprefix $(src)/, $(usr-y))

usr_lib := $(src)/lib

usr_dir := $(filter %/, $(usr-y))

usr_dir_o := $(patsubst %/, %/built-in.o, $(usr_dir))

usr_dir_elfs := $(patsubst %/, %.elf, $(usr_dir))

usr_sg_elfs := $(filter-out %/, $(usr-y))

KBUILD_CFLAGS += -I$(src)/include -I$(src)/arch/$(SRCARCH)/include -D__USER__

KBUILD_AFLAGS += -D__USER__

PHONY += $(usr_dir) $(usr_lib) fspre

quiet_cmd_usr = LD      $@
      cmd_usr = $(LD) $(LDFLAGS) -r -b binary $^ -o $@

quiet_cmd_cp = CP      $@
      cmd_cp = cp $@ $(srctree)/$(src)/fssrv/mkfs/root/bin

$(obj)/usr.elf : $(usr_sg_elfs) $(usr_dir_elfs) $(obj)/fssrv.elf
	$(call cmd,usr)

ldflags-y += -T $(srctree)/$(src)/usr.ld

$(obj)/fssrv.elf : $(obj)/fssrv/built-in.o $(usr_lib)/lib.a
	$(call cmd,ld)

$(obj)/fssrv/built-in.o: $(usr_sg_elfs) $(usr_dir_elfs)
	$(Q)$(MAKE) $(build)=$(src)/fssrv

$(usr_sg_elfs) : %.elf : %.o $(usr_lib)/lib.a
	$(call cmd,ld)
	$(call cmd,cp)

$(usr_dir_elfs) : %.elf : %/built-in.o $(usr_lib)/lib.a
	$(call cmd,ld)
	$(call cmd,cp)

$(usr_dir_o): $(usr_dir);

$(usr_lib)/lib.a : $(usr_lib);

$(usr_lib):
	$(Q)$(MAKE) $(build)=$@

$(usr_dir):
	$(Q)$(MAKE) $(build)=$(patsubst %/,%,$@)