#!/bin/bash

if [ $# -lt 2 ]; then
    echo "Need argument for your build and install directory (in that order). Use the FULL path. Exiting."
    exit 1
fi

BUILDDIR=$1
INSTALLDIR=$2

echo '#!/bin/bash' > earlydata.sh

echo "source /opt/sphenix/core/bin/setup_local.sh ${INSTALLDIR}" >> earlydata.sh
cat earlydata.config >> earlydata.sh
chmod +x earlydata.sh
echo '#!/bin/bash' > addcommand.sh
echo "source /opt/sphenix/core/bin/setup_local.sh ${INSTALLDIR}" >> addcommand.sh
cat addcommand.config >> addcommand.sh
chmod +x addcommand.sh
cd ..
THISREPODIR=`pwd`

mkdir -p $BUILDDIR/triggercountmodule
cd $BUILDDIR/triggercountmodule
echo "Entered "`pwd`
$THISREPODIR/src/autogen.sh --prefix=$BUILDDIR/triggercountmodule
make clean
make install

cd $THISREPODIR/run
echo "Entered "`pwd`
mkdir -p output/err
mkdir -p outout/out
mkdir -p output/added
mkdir -p subs
mkdir -p lists
mkdir -p /sphenix/tg/tg01/jets/$USER/trigcount

echo "Done setting up!"
