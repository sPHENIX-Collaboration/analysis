#!/bin/bash

root -l -q -b 'get_luminosity_182630.C("listrunnumber.txt",0,1)' >> 30cmlumi.list
root -l -q -b 'get_luminosity_182630.C("listrunnumber.txt",1,1)' >> 60cmlumi.list
root -l -q -b 'get_luminosity_182630.C("listrunnumber.txt",5,1)' >> allzlumi.list
