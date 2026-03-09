#!/bin/bash

condor_submit submit_genKshort.job
condor_submit submit_genphi.job
condor_submit submit_genlambda.job
condor_submit submit_genminbias.job
