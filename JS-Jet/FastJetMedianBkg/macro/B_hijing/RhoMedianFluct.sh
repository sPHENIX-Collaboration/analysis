out_name=RhoMedianFluct.root
nevnt=$1

if [ -z $nevnt ]; then
  nevnt=4
fi


l_calo_cluster=dst_calo_cluster.list  
l_bbc=dst_bbc_g4hit.list  

root -l Fun4All_RhoMedianFluct.C\(\"$out_name\",$nevnt,\"$l_calo_cluster\",\"$l_bbc\",0\) 
