import argparse
from pathlib import Path

def get_parser():
    """Returns the argument parser for Fun4All CaloFittingQA Condor submissions."""
    parser = argparse.ArgumentParser(
        description="Fun4All CaloFittingQA Condor Submission Automator",
        formatter_class=argparse.ArgumentDefaultsHelpFormatter,
    )

    parser.add_argument(
        '-i', '--input-list',
        type=str,
        default='files/dst_triggered.list',
        help='Input list of segment list files (1 job per line).'
    )
    parser.add_argument(
        '-o', '--output-dir',
        type=str,
        default='scratch/calo_fitting_qa',
        help='Submission output directory where job lists, logs, and outputs reside.'
    )
    parser.add_argument(
        '-o2', '--job-output-dir',
        type=str,
        default=None,
        help='Alternate Output Directory for job output files. If provided, a symlink will be created in the main output_dir.'
    )
    parser.add_argument(
        '-n', '--events',
        type=int,
        default=0,
        help='Number of events to analyze per job (0 = all events).'
    )
    parser.add_argument(
        '-i2', '--dbtag',
        type=str,
        default='newcdbtag',
        help='CDB global tag.'
    )
    parser.add_argument(
        '-s', '--memory',
        type=float,
        default=2,
        help='Initial memory (in GB) requested per Condor job.'
    )
    parser.add_argument(
        '--retry-memory-step',
        type=float,
        default=0.5,
        help='Memory (in GB) to step up upon OOM eviction retry.'
    )
    parser.add_argument(
        '--retry-memory-max',
        type=float,
        default=6.0,
        help='Maximum memory (in GB) ceiling for automatic retries.'
    )
    parser.add_argument(
        '--retry-request-memory',
        type=str,
        default=None,
        help='Explicit comma-separated retry memory string (e.g. "2.5GB, 3.0GB, 4.0GB").'
    )
    parser.add_argument(
        '-m', '--max-retries',
        type=int,
        default=3,
        help='Max Condor job retries on failure.'
    )
    parser.add_argument(
        '-l', '--condor-log-dir',
        type=str,
        default='/tmp/anarde/dump',
        help='Condor log directory for $(ClusterId)-$(Process).log files.'
    )
    parser.add_argument(
        '--node', '--submit-node',
        type=str,
        default=None,
        help='Target submission node (e.g. sphnxuser01). Default: auto-detect best node.'
    )
    parser.add_argument(
        '-u', '--user',
        type=str,
        default='anarde',
        help="Target user ID for submit node ranking."
    )
    parser.add_argument(
        '-f', '--f4a-macro',
        type=str,
        default='macros/Fun4All_CaloFittingQA.C',
        help='Fun4All CaloFittingQA ROOT macro.'
    )
    parser.add_argument(
        '-f2', '--calo-fitting-macro',
        type=str,
        default='macros/Calo_Fitting.C',
        help='Calo_Fitting.C macro dependency.'
    )
    parser.add_argument(
        '-f3', '--condor-script',
        type=str,
        default='scripts/genFun4All_CaloFittingQA.sh',
        help='Condor worker execution bash script.'
    )
    parser.add_argument(
        '--build',
        type=str,
        default=None,
        help='sPHENIX build tag (e.g. new, ana470). Default: $OFFLINE_MAIN basename or new.'
    )
    parser.add_argument(
        '--submit',
        action='store_true',
        help='Directly submit Condor cluster via SSH to best node (default: generate submission files only).'
    )

    return parser
