#!/bin/bash
# Script to clean the project directory of all generated files for publishing.

echo "Cleaning generated files..."

# Remove top-level generated files
rm -f aclocal.m4 compile config.log config.status configure depcomp install-sh Makefile Makefile.in missing stamp-h1 config.h.in

# Remove cache directory
rm -rf autom4te.cache

# Clean src directory
make -C src clean 2>/dev/null || true
rm -f src/Makefile.in src/Makefile

# Clean data directories
rm -f data/Makefile.in data/Makefile
rm -f data/icons/Makefile.in data/icons/Makefile
rm -f data/icons/16x16/Makefile.in data/icons/16x16/Makefile
rm -f data/icons/22x22/Makefile.in data/icons/22x22/Makefile
rm -f data/icons/24x24/Makefile.in data/icons/24x24/Makefile
rm -f data/icons/32x32/Makefile.in data/icons/32x32/Makefile
rm -f data/icons/scalable/Makefile.in data/icons/scalable/Makefile


# Remove backup files
find . -name '*~' -exec rm -f {} \;
find . -name '*.bak' -exec rm -f {} \;

echo "Project cleaned for publishing."
