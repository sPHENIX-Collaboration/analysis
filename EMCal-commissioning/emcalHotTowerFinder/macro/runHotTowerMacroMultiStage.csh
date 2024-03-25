#!/bin/tcsh

setenv HOME /phenix/u/$LOGNAME
source /etc/csh.login
foreach i(/etc/profile.d/*.csh)
    source $i
end

#set run list variable
set fileList=$1

# echo Submitting Pass 1 Condor Jobs

set v=`cat $1`
set nLines=`wc -l $fileList`
set i=1
set sigma=4.5

while ( $i <= $#v )
    if ( -f  submission/submitFor_Batch$i.sub ) then
	rm submission/submitFor_Batch$i.sub
    endif
    set runnumber=`echo $v[$i] | awk '{print substr($0,113,5)}'`
    if( ! -d '/tmp/ahodges/logs/hotTower/log_'$runnumber) then 
	mkdir '/tmp/ahodges/logs/hotTower/log_'$runnumber
    endif 
    if( ! -d 'out/'$runnumber) then 
	mkdir 'out/'$runnumber
    endif
    if( ! -d 'err/'$runnumber) then 
	mkdir 'err/'$runnumber
    endif

    touch submission/submitFor_Batch$i.sub
    echo 'executable = condorsubmitHotMapMakerPass0.csh' >> submission/submitFor_Batch$i.sub
    echo 'arguments = $(filename) ' $sigma >> submission/submitFor_Batch$i.sub
    echo 'log  = /tmp/ahodges/logs/hotTower/log_'$runnumber'/job-$(Process).log' >> submission/submitFor_Batch$i.sub
    echo 'output  = out/'$runnumber'/job-$(Process).out'  >> submission/submitFor_Batch$i.sub 
    echo 'error = err/'$runnumber'/job-$(Process).err' >> submission/submitFor_Batch$i.sub
    echo 'request_memory = 4GB' >> submission/submitFor_Batch$i.sub
    echo 'should_transfer_files  = NO' >> submission/submitFor_Batch$i.sub
    echo ""  >> submission/submitFor_Batch$i.sub
    echo 'queue filename from '${v[$i]} >> submission/submitFor_Batch$i.sub
    condor_submit submission/submitFor_Batch$i.sub
   @ i++
 end

echo Jobs submitted, waiting...
set nLines=10
while ($nLines > 9)
   sleep 1m
   @ nLines=`condor_q | wc -l`
   set remaining=`expr $nLines - 9` 
   echo There are $remaining jobs remaining 
end

#hadd files together
echo Submitting HADD jobs
set i=1
while ( $i <= $#v )
    set runnumber=`echo $v[$i] | awk '{print substr($0,113,5)}'`
    set nFiles=`wc -l $v[$i]`
    run_hadd_condor.csh $runnumber 100 $nFiles
    @ i++
end
   
echo Jobs submitted, waiting...
set nLines=10
while ($nLines > 9)
   sleep 1m
   @ nLines=`condor_q | wc -l`
   set remaining=`expr $nLines - 9` 
   echo There are $remaining jobs remaining 
end

    
#hadd big files togehter
echo Combining big files
set i=1
while ( $i <= $#v )
    set runnumber=`echo $v[$i] | awk '{print substr($0,113,5)}'`
    #echo 'hadd -f -k output/'$runnumber'/DST_CALOR-000'${runnumber}'_badTowerMapTree.root DST_CALOR-000'${runnumber}'-RANGE*.root'
    hadd -f -k output/$runnumber/DST_CALOR-000${runnumber}_badTowerMapTree.root output/$runnumber/DST_CALOR-000${runnumber}-RANGE*.root
    @ i++
end

condor_submit hotMapMakerPass1.sub 

echo Jobs submitted, waiting...
set nLines=10
while ($nLines > 9)
   sleep 1m
   @ nLines=`condor_q | wc -l`
   set remaining=`expr $nLines - 9` 
   echo There are $remaining jobs remaining 
end

echo Finished!


