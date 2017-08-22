#!/bin/sh

# usage: submit make_g4.

# replaces condor_submit 'cause when i use condor_submit i always forget to clear my log files first.
# ...the terminal dot on the argument is so it is easier to use tab completion, with *.job and *.csh files.

rm out/* log/* err/*
condor_submit "${1}job"
