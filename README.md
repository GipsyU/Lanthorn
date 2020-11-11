### Lanthorn OS

Lanthorn（阑珊） is an OS Kernel.

There isn't a goal of this kernel, just for fun of creating it.

If you are interested in this project, welcome to contact me by email <su_yuheng@foxmail.com>.

#### Compile
```bash
make virt_defconfig # config in qemu
make # compile
```

### Run in qemu
```bash
make qemu
```

#### Debug in gdb
```bash
make qdb
make gdb # in anothor terminal
```