quiet_cmd_mkfs = MKFS	  $(obj)/fs.o
      cmd_mkfs = $(CONFIG_SHELL) $(srctree)/$(src)/mkfs.sh

$(obj)/fs.o:
	$(call cmd,mkfs)