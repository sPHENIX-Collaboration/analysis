while IFS= read -r line; 
    do 
        echo "$line"; 
        
        root -l -b -q hot_chan_finder.C\(\"${line}\"\)

        sleep 10;

    done < /sphenix/user/ChengWei/INTT/INTT_commissioning/ZeroField/run_list.txt
