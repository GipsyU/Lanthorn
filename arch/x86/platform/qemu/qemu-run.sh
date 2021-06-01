#!/bin/bash
qemu-system-x86_64  -kernel $1 \
                    -m size=512M \
                    -smp 1 \
                    -nographic \
                    -serial mon:stdio \
                    -no-reboot \
                    $2 $3
