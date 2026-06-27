#!/usr/bin/env python3
"""
This module automates the Fun4All Condor Procedure
"""
import sys
import os

# Resolve symlinks to find the true path
true_path = os.path.realpath(__file__)
# Since true_path is scripts/utils.py, we go up two levels to get the project root
project_root = os.path.dirname(os.path.dirname(true_path))

if project_root not in sys.path:
    sys.path.insert(0, project_root)

from condor_utils.__main__ import main

if __name__ == "__main__":
    main()
