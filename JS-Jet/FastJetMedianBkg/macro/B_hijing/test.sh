 #! /bin/bash
 out_name=out.test
 nevnt=20
 l_bbc=dst_bbc_g4hit_1.list
 l_calo_cluster=dst_calo_cluster_1.list

 root.exe -q -l ../Fun4All_RhoMedianFluct.C\(\"$out_name\",$nevnt,\"$l_calo_cluster\",\"$l_bbc\",0\) 

 echo all done
