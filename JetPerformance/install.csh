source /opt/sphenix/core/bin/sphenix_setup.csh -a       # sPHENIX environment, if you have not yet run it.
setenv MYINSTALL /direct/phenix+u/kraghav/WORK/analysis/JetPerformance
setenv LD_LIBRARY_PATH $MYINSTALL/lib:$LD_LIBRARY_PATH  # do this step after source /opt/sphenix/core/bin/sphenix_setup.csh -n, which overwrites $LD_LIBRARY_PATH
set path = ( $MYINSTALL/bin $path )                     # optional

autogen.sh --prefix=$MYINSTALL
make -j 4
make install

echo "Done compiling and installing JEWEL analysis library"
