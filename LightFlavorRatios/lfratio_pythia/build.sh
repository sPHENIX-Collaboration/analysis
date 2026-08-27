#!/bin/bash

g++ -O2 -std=c++17 lfratio_pythia.cc -o lfratio_pythia $(root-config --cflags --libs) $(HepMC3-config --cflags --libs)