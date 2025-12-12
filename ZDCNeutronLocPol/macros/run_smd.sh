#!/bin/bash
#
# condor executes runana.cmd 0, runana.cmd 1, etc.
# where $1 = 0, 1, etc...
#

export USER="$(id -u -n)"
export LOGNAME=${USER}
export HOME=/sphenix/u/${LOGNAME}

source ${HOME}/.bashrc

condor=true
condorrun=0
condorjobs=500
fullrun=true
fname="DST_TRIGGERED_EVENT_seb20_run2pp_ana502_nocdbtag_v001-00047972-%05d.root"
ofname1="47972/0/zdcneutronlocpol_47972_%05d.root"
ofname2="47972/0/zdcneutronlocpol_47972_%04d_%05d.root"

if [ "$condor" = false ]; then    
    #=================== Run standalone =========================#
    startevent=100000
    index=0
    filename=$(printf "$fname" $index)

    if [ "$fullrun" = true ]; then	
	outname=$(printf "$ofname1" $index)
	root -l -q "Fun4All_ZDCNeutronLocPol.C(\"$filename\",\"$outname\")"
    else
	outname=$(printf "$ofname2" $index $startevent)
	root -l -q "Fun4All_ZDCNeutronLocPol.C(\"$filename\",\"$outname\", $startevent)"
    fi
    #=========================================================#

else
    #=================== Run with condor =========================#
    startevent=$(( ${1}*100000))
    filename=$(printf "$fname" $(( ${1} + condorrun * condorjobs )))

    if [ "$fullrun" = true ]; then
	outname=$(printf "$ofname1" $(( ${1} + condorrun * condorjobs )))
	root -l -q "Fun4All_ZDCNeutronLocPol.C(\"$filename\",\"$outname\")"
    else
	outname=$(printf "$ofname2" ${1} ${1})
	root -l -q "Fun4All_ZDCNeutronLocPol.C(\"$filename\",\"$outname\", $startevent)"
    fi
    #=========================================================#
fi
