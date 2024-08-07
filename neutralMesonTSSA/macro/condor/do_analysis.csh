#!/bin/csh
setenv HOME /sphenix/u/gmattson
setenv PATH $HOME/.local/bin:/usr/bin:/usr/sbin
echo "HOME = $HOME"
echo "PATH = $PATH"
source /etc/csh.login
foreach i (/etc/profile.d/*.csh)
    source $i
end
# source $HOME/.bashrc
echo "Sourcing sPHENIX setup scripts"
source /opt/sphenix/core/bin/sphenix_setup.csh new -n
source $OPT_SPHENIX/bin/setup_local.csh /sphenix/user/gmattson/install

echo "HOME = $HOME"
echo "PATH = $PATH"

@ nProcess = $1
@ nFilesPerProcess = 10

cd /sphenix/user/gmattson/analysis/neutralMesonTSSA/macro/condor
mkdir -p job_filelists
set infile1 = "job_filelists/dst_calo-$nProcess.list"
# if the filelist already exists, assume it's correct and we don't need to remake it
# use with caution!
if (-e $infile1) then
    echo "Filelist $infile1 exists; proceeding with job"
    # rm $infile1
    # touch $infile1
else
    # create the job filelist if it doesn't exist
    @ fileInProcess = 0
    while ( $fileInProcess < $nFilesPerProcess ) 
	@ linenumber1 = 0
	@ correctLineNumber = $nFilesPerProcess * $nProcess + $fileInProcess
	foreach line ( "`cat dst_calo_cluster.list`" )
	    if ( $linenumber1 == $correctLineNumber ) then
		echo $line >> $infile1
	    endif
	    @ linenumber1 = $linenumber1 + 1
	end
	@ fileInProcess = $fileInProcess + 1
    end
endif

echo "DST file is $infile1"
set outfiledir = "/sphenix/tg/tg01/coldqcd/gmattson/neutralMesonTSSA/pass1/hists"
mkdir -p "$outfiledir"
set outfilename = "$outfiledir/NMhists-$nProcess.root"
echo "Output hists will be stored in $outfilename"

# For 2024 RD
time root -b -q ../Fun4All_neutralMesonTSSA.C'(0, "'$infile1'", "", "'$outfilename'")'
