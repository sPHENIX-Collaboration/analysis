import argparse
from condor_utils.cli import get_common_parser
from condor_utils.commands.qa import setup_qa_subparsers
from condor_utils.commands.f4a import setup_f4a_subparsers
from condor_utils.commands.jetAna import setup_jetAna_subparsers
from condor_utils.commands.hadd import setup_hadd_subparsers
from condor_utils.commands.data import setup_data_subparsers

def main():
    parser = argparse.ArgumentParser(description="Fun4All Condor Procedure Automator")
    subparsers = parser.add_subparsers(dest='command', required=True)
    
    common_parser = get_common_parser()
    
    setup_qa_subparsers(subparsers, common_parser)
    setup_f4a_subparsers(subparsers, common_parser)
    setup_jetAna_subparsers(subparsers, common_parser)
    setup_hadd_subparsers(subparsers, common_parser)
    setup_data_subparsers(subparsers, common_parser)
    
    args = parser.parse_args()
    args.func(args)

if __name__ == "__main__":
    main()
