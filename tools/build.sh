#!/bin/bash
make -C .. O=`pwd` clean --no-print-directory
rm -rf arch kernel lib test usr
make -C .. O=`pwd` --no-print-directory -j8