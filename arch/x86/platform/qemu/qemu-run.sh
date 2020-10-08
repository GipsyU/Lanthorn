#!/bin/bash
qemu-system-i386    -kernel lanthorn \
                    -m size=512M \
                    -nographic \
                    -serial mon:stdio \
                    $@