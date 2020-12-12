usr/usr.elf: usr/test1.elf
	ld -b binary usr/test1.elf -o usr/usr.elf

usr/test1.elf: usr/test1.c
	gcc -c usr/test1.c -o usr/test1.elf