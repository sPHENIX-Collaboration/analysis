#!/bin/bash                                                                                                                                                                                  

for ((i=0; i<=999; i+=1));
do
    echo condor submission $i
    #cp run_condor_job_mother.job run_condor_job_sub.job
    cp run_root_mother.sh run_root_sub_${i}.sh
    sed -i "s/the_file_number/${i}/g" run_root_sub_${i}.sh
    chmod +777 run_root_sub_${i}.sh
    
    cp run_condor_job_mother.job run_condor_job_sub.job
    sed -i "s/the_file_number/${i}/g" run_condor_job_sub.job

    condor_submit run_condor_job_sub.job
    sleep 1
    

    rm run_condor_job_sub.job
    # rm run_root_sub_${i}.sh
    sleep 0.5
done
