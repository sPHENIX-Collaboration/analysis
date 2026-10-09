#!/usr/bin/env python3
"""
Fun4All Condor Procedure Automator for ZSCrossCalibGen.

Usage Examples:
    # 1. Generate submission setup & inspect files (dry run):
    python3 run_condor.py -i files/dst_triggered.list -o scratch/calo_fitting_qa -n 0

    # 2. Directly submit to Condor on the optimal submit node:
    python3 run_condor.py -i files/dst_triggered.list -o scratch/calo_fitting_qa -n 0 --submit
"""
import os
import sys
from pathlib import Path

# Ensure package directory is in sys.path
script_dir = Path(__file__).resolve().parent
if str(script_dir) not in sys.path:
    sys.path.insert(0, str(script_dir))

from condor_utils.cli import get_parser
from condor_utils.commands.calo_fitting_qa import create_calo_fitting_qa_jobs


def main():
    parser = get_parser()
    args = parser.parse_args()

    # Automatically adapt relative paths if run from repository root
    if not Path(args.input_list).is_file():
        alt_input = script_dir / args.input_list
        if alt_input.is_file():
            args.input_list = str(alt_input)

    create_calo_fitting_qa_jobs(args)


if __name__ == "__main__":
    main()
