#!/bin/tcsh -f


set base_dirs = ( \
	/phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/ \
	/phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal1d/fieldmap/ \
	/phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/embedding/emcstudies/pidstudies/spacal2d/fieldmap/ \
	/phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/embedding/emcstudies/pidstudies/spacal1d/fieldmap/ \
);

set kine_configs = (eta0 eta0.90);



foreach base_dir ($base_dirs)
	foreach kine_config ($kine_configs)
	
		echo "======================================================";
		echo Likelihood.sh $kine_config $base_dir;
		echo $base_dir/Likelihood_$kine_config.log;
		echo "======================================================";

		Likelihood.sh $kine_config $base_dir | & tee $base_dir/Likelihood_$kine_config.log; 
	end
end
