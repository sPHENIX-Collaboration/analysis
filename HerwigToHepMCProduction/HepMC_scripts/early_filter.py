#! /bin/python

import sys
import os
path ="."
filetag="MB"
if len(sys.argv) > 1:
    path=sys.argv[1]
    if len(sys.argv) >2:
        filetag=sys.argv[2]
segment = "000000" 
xs_dir = path+"/cross_section_data/"
xs_file_stem="Cross_Section_"+str(filetag)+"_"
for fname in os.listdir(path):
    val=1.
    power = 1.
    if fname.find(".out") != -1:
        if fname.find("S") != -1: 
            continue;
        seg_ish = str(fname).split("-")[1:][0]
        segment = str(seg_ish).split(".")[:1][0]
        with open(str(path)+"/"+str(fname)) as f:
            for line in f:
                if line.find("generated events") != -1:
                    sublines = line.split(' ')
                    sl=sublines[len(sublines)-1]
                    val=sl.split('(')[:1][0]
                    power = sl.split('e+')[1:][0]
                    val= float(val)*pow(10, float(power))
                    print("segment "+str(segment)+" Read xs "+str(val))
        with open(str(xs_dir)+str(xs_file_stem)+str(segment)+".txt", 'w') as fout:
            fout.write("Generated Events = 1000 \n")
            fout.write("XS/N = "+str(float(float(val)/1000.)))

                    

