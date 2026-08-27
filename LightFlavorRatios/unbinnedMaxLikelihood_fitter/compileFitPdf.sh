#! /bin/bash

if [ "$#" -ne 1 ]; then
    echo "Usage: $0 <figpdfdir>"
    exit 1
fi
# the directory of the fit pdfs to be compiled
figpdfdir=$1

shopt -s nullglob
fitpdfs=("${figpdfdir}"/*/RDataframeToRoofit_*_mass.pdf)

if ((${#fitpdfs[@]} == 0)); then
    echo "No RDataframeToRoofit_*_mass.pdf files found in ${figpdfdir}"
    exit 1
fi

echo "Compiling ${#fitpdfs[@]} fit pdfs into ${figpdfdir}/allfit.pdf"
gs -dNOPAUSE -sDEVICE=pdfwrite -sOUTPUTFILE=${figpdfdir}/allfit.pdf -dBATCH "${fitpdfs[@]}"