#! /bin/bash

xcheckdir=cross-checks-20250313

centralities_phobos=(0 3 6 10 15 20 25 30 35 40 45 50 55 60 65 70) # PHOBOS 2011 binnings
length_phobos=${#centralities_phobos[@]}

for ((i=0; i<$length_phobos-1; i++))
do
    python systematics.py --desc Centrality${centralities_phobos[i]}to${centralities_phobos[i+1]}_Zvtxm10p0to10p0_noasel
    python closure.py --datahistdir ./systematics --simhistdir ./corrections/HIJING_closure_dRcut0p5_NominalVtxZ_RandomClusSet0_clusAdcCutSet0_clusPhiSizeCutSet0 --filedesc Centrality${centralities_phobos[i]}to${centralities_phobos[i+1]}_Zvtxm10p0to10p0_noasel --docompare --plotdir ${xcheckdir}
done

python systematics.py --desc Centrality0to70_Zvtxm10p0to10p0_noasel 
python closure.py --datahistdir ./systematics --simhistdir ./corrections/HIJING_closure_dRcut0p5_NominalVtxZ_RandomClusSet0_clusAdcCutSet0_clusPhiSizeCutSet0 --filedesc Centrality0to70_Zvtxm10p0to10p0_noasel --docompare --plotdir ${xcheckdir}

# current directory
THISDIR=$(pwd)

# Create the PDF summary
cd ./corrections/${xcheckdir}
gs -dNOPAUSE -sDEVICE=pdfwrite -sOUTPUTFILE=dNdeta_crosscheck_merged.pdf -dBATCH dNdeta_Centrality*_Zvtxm10p0to10p0_noasel_crosscheck.pdf

# print the systematics summary
cd ${THISDIR}
root -l -b -q print_SystTable.C