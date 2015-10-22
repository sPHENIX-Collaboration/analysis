#!/bin/tcsh -f

set base_dir = '/direct/phenix+sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/';
# set base_dir = '/direct/phenix+sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/embedding/emcstudies/pidstudies/spacal2d/fieldmap/';
#set base_dir = '/direct/phenix+sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/embedding/emcstudies/pidstudies/spacal1d/fieldmap/';

#set kine_config = 'eta0';
set kine_config = 'eta0.90';

# set pids = ('e-' 'pi-');
set pids = ('e-' 'pi-' 'kaon-' 'anti_proton');

set energys = (2GeV 4GeV 8GeV);
# set energys = (2GeV 4GeV );

set q = '"';

foreach energy ($energys)
	foreach pid ($pids)
	
		mkdir ${base_dir}/G4Hits_sPHENIX_${pid}_${kine_config}_${energy}-ALL.root_Ana.root.lst.old/;
	
		mv -fv ${base_dir}/G4Hits_sPHENIX_${pid}_${kine_config}_${energy}-ALL.root_Ana.root.lst* ${base_dir}/G4Hits_sPHENIX_${pid}_${kine_config}_${energy}-ALL.root_Ana.root.lst.old/;
	
		/bin/ls -1 ${base_dir}/G4Hits_sPHENIX_${pid}_${kine_config}_${energy}-*.root_Ana.root   > ${base_dir}/G4Hits_sPHENIX_${pid}_${kine_config}_${energy}-ALL.root_Ana.root.lst;
		wc -l ${base_dir}/G4Hits_sPHENIX_${pid}_${kine_config}_${energy}-ALL.root_Ana.root.lst;

	end
end

	
foreach energy ($energys)
	foreach pid ($pids)
	
		echo root -b -q "Fun4All_EMCalLikelihood.C(0, ${q}${base_dir}${q}, ${q}${pid}${q}, ${q}${kine_config}_${energy}${q})";
		root -b -q "Fun4All_EMCalLikelihood.C(0, ${q}${base_dir}${q}, ${q}${pid}${q}, ${q}${kine_config}_${energy}${q})" & ;

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
	
		echo root -b -q "Fun4All_EMCalLikelihood.C(0, ${q}${base_dir}${q}, ${q}${pid}${q}, ${q}${kine_config}_${energy}${q})";

		root -b -q "Fun4All_EMCalLikelihood.C(0, ${q}${base_dir}${q}, ${q}${pid}${q}, ${q}${kine_config}_${energy}${q})" & ;

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






