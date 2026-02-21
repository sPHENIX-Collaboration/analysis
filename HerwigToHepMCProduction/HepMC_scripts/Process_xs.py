#! /bin/python

import sys
import os

filetag="jet30"
path="."
if len(sys.argv) > 1:
    filetag=sys.argv[1]
    if len(sys.argv) > 2:
        path=sys.argv[2]
avg_val = 0
n = 0
for fname in os.listdir(path):
    if fname.find(filetag) != -1:
        with open(str(path)+"/"+str(fname)) as f:
            mtpx = 1.
            for line in f:
                if line.find("Generated") != -1:
                    mtpx = line.split("=")[1:][0]
                if line.find("XS/N") != -1:
                    val= line.split("=")[1:][0]
                    val=float(val)*float(mtpx)
                    n += 1
                    avg_val += val

avg_val = avg_val /float(n)
print("Average cross section for " +str(filetag)+" is "+str(avg_val)+" nb")
                
