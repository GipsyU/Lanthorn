### Lanthorn OS

Lanthorn（阑珊） is a OS Kernel in microkernel architecture.

There isn't a goal of this kernel, just for fun of creating it.

If you are interested in this project, welcome to contact me by email <su_yuheng@foxmail.com>.

#### Compile
```bash
cd Lanthorn
mkdir build && cd build
make -C .. O=`pwd` virt_defconfig # config in qemu
make -C .. O=`pwd` # compile
```

### Run in qemu
```bash
make qemu
```

#### Debug in gdb
```bash
gdb
> target remote localhost:5555
```