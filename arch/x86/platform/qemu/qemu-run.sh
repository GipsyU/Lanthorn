#!/bin/bash
qemu-system-i386    -fda lanthorn -boot a \
                    -m 512 \
                    -nographic \
                    -gdb tcp::5555 \
                    -S