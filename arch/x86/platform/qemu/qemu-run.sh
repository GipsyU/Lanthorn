#!/bin/bash
qemu-system-i386    -kernel $1 \
                    -m size=512M \
                    -smp 4 \
                    -nographic \
                    -serial mon:stdio \
                    -no-reboot \
                    $2 $3