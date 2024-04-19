run_index=0

while IFS= read -r line; 
    do 
        echo "$line"; 
        
        nohup root -l -b -q gen_INTT_cluster_BCO.C\(\"ZeroField/${line}\",\"beam_inttall-000${line}-0000_event_base_ana\",0,20000,1,30000,3\)&>log_${line}.log&

        ((run_index++))
        sleep 3;
        
        if [ "$run_index" == 4 ]; then
            sleep 1200
        fi

    done < /sphenix/user/ChengWei/INTT/INTT_commissioning/ZeroField/run_list.txt
