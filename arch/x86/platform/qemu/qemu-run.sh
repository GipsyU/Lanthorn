#!/bin/bash
qemu-system-i386    -kernel lanthorn \
                    -m size=512M \
                    -smp 4 \
                    -nographic \
                    -serial mon:stdio \
                    -no-reboot \
                    $@