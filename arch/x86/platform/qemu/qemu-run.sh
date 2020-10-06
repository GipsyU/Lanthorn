#!/bin/bash
qemu-system-i386    -kernel lanthorn \
                    -m 512 \
                    -nographic \
                    -gdb tcp::5555 \
                    -S