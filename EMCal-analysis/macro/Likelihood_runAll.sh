#!/bin/tcsh -f


#set base_dirs = ( \
	#	/gpfs/mnt/gpfs02/sphenix/sim/sim01/phnxreco/users/jinhuang/sPHENIX_work//production_analysis_cemc2x2//emcstudies/pidstudies/spacal2d/fieldmap/ \
#);

set base_dirs = ( \
	/gpfs/mnt/gpfs02/sphenix/sim/sim01/phnxreco/users/jinhuang/sPHENIX_work//production_analysis_cemc2x2//emcstudies/pidstudies/spacal2d/fieldmap/ \
	/gpfs/mnt/gpfs02/sphenix/sim/sim01/phnxreco/users/jinhuang/sPHENIX_work//production_analysis_cemc2x2//emcstudies/pidstudies/spacal1d/fieldmap/ \
	/gpfs/mnt/gpfs02/sphenix/sim/sim01/phnxreco/users/jinhuang/sPHENIX_work//production_analysis_cemc2x2//embedding/emcstudies/pidstudies/spacal2d/fieldmap/ \
	/gpfs/mnt/gpfs02/sphenix/sim/sim01/phnxreco/users/jinhuang/sPHENIX_work//production_analysis_cemc2x2//embedding/emcstudies/pidstudies/spacal1d/fieldmap/ \
);

#set base_dirs = ( \
#	/gpfs/mnt/gpfs02/sphenix/sim/sim01/phnxreco/users/jinhuang/sPHENIX_work//production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/ \
#	/gpfs/mnt/gpfs02/sphenix/sim/sim01/phnxreco/users/jinhuang/sPHENIX_work//production_analysis/emcstudies/pidstudies/spacal1d/fieldmap/ \
#	/gpfs/mnt/gpfs02/sphenix/sim/sim01/phnxreco/users/jinhuang/sPHENIX_work//production_analysis/embedding/emcstudies/pidstudies/spacal2d/fieldmap/ \
#	/gpfs/mnt/gpfs02/sphenix/sim/sim01/phnxreco/users/jinhuang/sPHENIX_work//production_analysis/embedding/emcstudies/pidstudies/spacal1d/fieldmap/ \
#);

set kine_configs = (eta0 eta0.90);



foreach base_dir ($base_dirs)
	foreach kine_config ($kine_configs)
	
		echo "======================================================";
		echo Likelihood.sh $kine_config $base_dir;
		echo $base_dir/Likelihood_$kine_config.log;
		echo "======================================================";

		nice Likelihood.sh $kine_config $base_dir | & tee $base_dir/Likelihood_$kine_config.log; 
	end
end
