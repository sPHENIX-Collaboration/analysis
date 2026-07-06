import argparse

def get_common_parser():
    """Returns a parent parser with arguments common to most jobs."""
    parser = argparse.ArgumentParser(add_help=False)

    parser.add_argument('-i', '--input-list', type=str, required=True, help='Input DST List.')
    parser.add_argument('-i2', '--dbtag', type=str, default='newcdbtag', help='CDB Tag. Default: newcdbtag')
    parser.add_argument('-o', '--output-dir', type=str, default='scratch/test', help='Project Directory. Default: scratch/test')
    parser.add_argument('-o2', '--job-output-dir', type=str, default=None, help='Alternate Output Directory for job output files. If provided, a symlink will be created in the main output_dir.')
    parser.add_argument('-n', '--events', type=int, default=0, help='Number of events to analyze. Default: All.')
    parser.add_argument('-n2', '--dst-per-job', type=int, default=4, help='Number of DSTs to analyze per job. Default: 4.')
    parser.add_argument('-s', '--memory', type=float, default=1.0, help='Memory (units of GB) to request per condor submission.')
    parser.add_argument('-m', '--max-retries', type=int, default=3, help='Max Condor job retries on failure. Default: 3.')
    parser.add_argument('-l', '--condor-log-dir', type=str, default='/tmp/anarde/dump', help='Condor Log Directory.')

    parser.add_argument('-f2', '--src-dir', type=str, default='src', help='Source Files Directory. Default: src')
    parser.add_argument('-f3', '--condor-script', type=str, help='Condor Script.')
    parser.add_argument('-f4', '--common-errors', type=str, default='files/common-errors.txt', help='Common Errors.')

    return parser
