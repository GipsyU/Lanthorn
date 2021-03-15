#!/bin/bash
find arch include kernel lib test usr  "(" -name "*c" -or -name "*.h" -or -name "*.S" ")" -print | xargs wc -l