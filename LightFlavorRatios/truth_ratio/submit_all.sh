#!/bin/bash

condor_submit submit_truth_ratio.job
condor_submit submit_truth_ratio_pos.job
condor_submit submit_truth_ratio_neg.job
