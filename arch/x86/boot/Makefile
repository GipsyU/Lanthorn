KBUILD_AFLAGS	:= $(KBUILD_CFLAGS) -D__ASSEMBLY__

obj-y := boot.o __apboot.o

quiet_cmd__apboot = LD      $@
      cmd__apboot = $(LD) $(LDFLAGS) -r -b binary $^ -o $@

quiet_cmd_apboot = LD      $@
      cmd_apboot = $(LD) $(LDFLAGS) -Ttext 0x7000 $^ -o $@; objcopy -S -O binary -j .text $(obj)/_apboot.o $(obj)/_apboot.o;

$(obj)/__apboot.o : $(obj)/_apboot.o
	$(call cmd,_apboot)

$(obj)/_apboot.o : $(obj)/apboot.o
	$(call cmd,apboot)