#!/bin/bash

root -l -q -b 'get_luminosity_182630.C("listrunnumber.txt",0,1)' > 30cmlumi_allxing.list
root -l -q -b 'get_luminosity_182630.C("listrunnumber.txt",1,1)' > 60cmlumi_allxing.list
root -l -q -b 'get_luminosity_182630.C("listrunnumber.txt",5,1)' > allzlumi_allxing.list

root -l -q -b 'get_luminosity_182630.C("listrunnumber.txt",0,1,1)' > 30cmlumi_0mrad.list
root -l -q -b 'get_luminosity_182630.C("listrunnumber.txt",1,1,1)' > 60cmlumi_0mrad.list
root -l -q -b 'get_luminosity_182630.C("listrunnumber.txt",5,1,1)' > allzlumi_0mrad.list

root -l -q -b 'get_luminosity_182630.C("listrunnumber.txt",0,1,2)' > 30cmlumi_15mrad.list
root -l -q -b 'get_luminosity_182630.C("listrunnumber.txt",1,1,2)' > 60cmlumi_15mrad.list
root -l -q -b 'get_luminosity_182630.C("listrunnumber.txt",5,1,2)' > allzlumi_15mrad.list
