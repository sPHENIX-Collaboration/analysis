#!/bin/bash                                                                                                                                                                                  

N_file=1323;

submit_condor_job () {

    local i=$1

    echo condor submission $i
    #cp run_condor_job_mother.job run_condor_job_sub.job
    cp run_root_mother.sh run_root_sub_evtZ_${i}.sh
    sed -i "s/the_file_number/${i}/g" run_root_sub_evtZ_${i}.sh
    chmod +777 run_root_sub_evtZ_${i}.sh
    
    cp run_condor_job_mother.job run_condor_job_sub.job
    sed -i "s/the_file_number/${i}/g" run_condor_job_sub.job

    condor_submit run_condor_job_sub.job
    sleep 1
    

    rm run_condor_job_sub.job
    # rm run_root_sub_${i}.sh
    sleep 1
}

# for ((i=0; i<=500; i+=1));
# do
#     submit_condor_job $N_file
#     N_file=$(( N_file + 1 ))
#     echo $N_file
#     sleep 1
# done


while [ $N_file -lt 40000 ]; 
do
    current_condor_job=$(( `condor_q | grep run_root_sub_evtZ | wc -l` - 0 ))
    echo current running jobs: $current_condor_job
    if [ $current_condor_job -lt 500 ]; then
        N_short=$(( 500 - $current_condor_job ))
        for ((i=0; i<$N_short; i+=1));
        do
            file_id=$(printf "%06d" $N_file)
            waiting_loop=0
            while [ ! -f /sphenix/user/ChengWei/sPH_dNdeta/Sim_Ntuple_HIJING_new_20240424_HR_test/Geo_test/avg_vtxXY/complete_file/geo_scan_avgXY_${file_id}.root ]; do
                echo the file geo_scan_avgXY_${file_id}.root does not exist waiting!
                echo the waiting_loop $waiting_loop
                waiting_loop=$(( waiting_loop + 1 ))
                sleep 5
            done

            # if [ ! -f /sphenix/user/ChengWei/sPH_dNdeta/Sim_Ntuple_HIJING_new_20240424_HR_test/Geo_test/avg_vtxXY/complete_file/geo_scan_avgXY_${file_id}.root ]; then
            #     echo the file geo_scan_avgXY_${file_id}.root does not exist
            #     exit 1
            # fi
            submit_condor_job $N_file
            N_file=$(( N_file + 1 ))
            echo '---- job submitted -----' $N_file
            sleep 1
        done
    fi

    sleep 3
done
