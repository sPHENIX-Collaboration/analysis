#!/bin/tcsh -f

if ($#argv < 2) then
	echo "Missing parameters. Example usages:"
	echo "$0 eta0.90 /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/ "
	echo "$0 eta0 /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/ "
		
	exit;
endif

echo "$0 $*"

set base_dir =  $argv[2]
# set base_dir = '/phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/';
# set base_dir = '/phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/embedding/emcstudies/pidstudies/spacal2d/fieldmap/';
#set base_dir = '/phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/embedding/emcstudies/pidstudies/spacal1d/fieldmap/';

#set kine_config = 'eta0';
#set kine_config = 'eta0.90';
set kine_config = $argv[1];

# set pids = ('e-' 'pi-');
#set pids = ('e-' 'pi-' 'kaon-' 'anti_proton');
set pids = ('e-' 'pi-' 'kaon-' 'anti_proton' 'e+' 'pi+' 'kaon+' 'proton');

set energys = (2GeV 4GeV 8GeV);
# set energys = (2GeV 4GeV );
# set energys = (8GeV );

set q = '"';


foreach energy ($energys)
	foreach pid ($pids)
	
		mkdir ${base_dir}/G4Hits_sPHENIX_${pid}_${kine_config}_${energy}-ALL.root_Ana.root.lst.old/;
	
		mv -fv ${base_dir}/G4Hits_sPHENIX_${pid}_${kine_config}_${energy}-ALL.root_Ana.root.lst* ${base_dir}/G4Hits_sPHENIX_${pid}_${kine_config}_${energy}-ALL.root_Ana.root.lst.old/;
	
		/bin/ls -1 ${base_dir}/G4Hits_sPHENIX_${pid}_${kine_config}_${energy}-[0-9][0-9][0-9][0-9].root_Ana.root   > ${base_dir}/G4Hits_sPHENIX_${pid}_${kine_config}_${energy}-ALL.root_Ana.root.lst;
		wc -l ${base_dir}/G4Hits_sPHENIX_${pid}_${kine_config}_${energy}-ALL.root_Ana.root.lst;

	end
end

# exit
	
foreach energy ($energys)
	foreach pid ($pids)
	
		echo root -b -q "Fun4All_EMCalLikelihood.C(1000, ${q}${base_dir}${q}, ${q}${pid}${q}, ${q}${kine_config}_${energy}${q})";
		root -b -q "Fun4All_EMCalLikelihood.C(1000, ${q}${base_dir}${q}, ${q}${pid}${q}, ${q}${kine_config}_${energy}${q})" & ;
# echo root -b -q "Fun4All_EMCalLikelihood.C(10000000, ${q}${base_dir}${q}, ${q}${pid}${q}, ${q}${kine_config}_${energy}${q})";
#root -b -q "Fun4All_EMCalLikelihood.C(10000000, ${q}${base_dir}${q}, ${q}${pid}${q}, ${q}${kine_config}_${energy}${q})" & ;
	end
end
		wait;
	
	
foreach energy ($energys)
	foreach pid ($pids)
	
		echo root -b -q "DrawEcal_Likelihood.C(${q}${base_dir}${q}, ${q}${pid}${q}, ${q}${kine_config}_${energy}${q},0 )";
		root -b -q "DrawEcal_Likelihood.C(${q}${base_dir}${q}, ${q}${pid}${q}, ${q}${kine_config}_${energy}${q},0 )" & ;

	end
end
		wait;
		
		
foreach energy ($energys)
	foreach pid ($pids)
	
		echo root -b -q "Fun4All_EMCalLikelihood.C(1000, ${q}${base_dir}${q}, ${q}${pid}${q}, ${q}${kine_config}_${energy}${q})";

		root -b -q "Fun4All_EMCalLikelihood.C(1000, ${q}${base_dir}${q}, ${q}${pid}${q}, ${q}${kine_config}_${energy}${q})" & ;

	end
end
		wait;
	
foreach energy ($energys)
	foreach pid ($pids)
	
		echo root -b -q "DrawEcal_Likelihood.C(${q}${base_dir}${q}, ${q}${pid}${q}, ${q}${kine_config}_${energy}${q},1 )"  ;
		root -b -q "DrawEcal_Likelihood.C(${q}${base_dir}${q}, ${q}${pid}${q}, ${q}${kine_config}_${energy}${q},1 )" & ;

	end
end
		
		wait;
		
echo done;
date;

exit;





