#!/bin/bash
# -----------------------------------------------------------------------------
# 'wipe-source.sh'
# Derek Anderson
# 12.06.2022
#
# Use this wipe your source directory!
# -----------------------------------------------------------------------------

# move key files out of directory
mv ./src/*.{cc,h} ./
mv ./src/autogen.sh ./
mv ./src/Makefile.am ./
mv ./src/configure.ac ./
mv ./src/sphx-build ./

# remove source
rm -r ./src/

# recreate directory and move key files back
mkdir ./src/
mv ./*.{cc,h} ./src/
mv ./autogen.sh ./src/
mv ./Makefile.am ./src/
mv ./configure.ac ./src/
mv ./sphx-build ./src/

# end -------------------------------------------------------------------------
