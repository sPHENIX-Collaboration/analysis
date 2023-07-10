 #! /bin/bash
 out_name=out.root
 nevnt=250
 l_bbc=dst_bbc_g4hit_0.list
 l_calo_cluster=dst_calo_cluster_0.list

 root.exe -q -l ../Fun4All_RhoMedianFluct.C\(\"$out_name\",$nevnt,\"$l_calo_cluster\",\"$l_bbc\",0\) 

 echo all done
