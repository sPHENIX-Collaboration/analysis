# run_Nevent=(35 25 50 50 45 45 30 46 56)
run_Nevent=(30 25 30 30 30 30 30 30 30)
run_index=0

while IFS= read -r line; 
    do 
        echo "$line"; 
        
        nohup root -l -b -q INTT_zvtx.C\(\"${line}\",\"${run_Nevent[$run_index]}\"\)&>log_${line}.log&
        # echo INTT_zvtx.C\(\"${line}\",\"${run_Nevent[$run_index]}\"\)

        ((run_index++))
        sleep 3;
        
        if [ "$run_index" == 4 ]; then
            sleep 1600
        fi


    done < /sphenix/user/ChengWei/INTT/INTT_commissioning/ZeroField/run_list.txt
