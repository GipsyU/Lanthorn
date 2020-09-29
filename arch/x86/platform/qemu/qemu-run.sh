#!/bin/bash
qemu-system-i386    -cpu qemu32 \
                    -m size=4096M \
                    -nographic \
                    -gdb tcp::5555 \
                    -fda lanthorn -boot a \
                    -S