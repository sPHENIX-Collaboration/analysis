import sys
import shutil
import datetime
import textwrap
from pathlib import Path
import logging

from condor_utils.core.logging import setup_logging
from condor_utils.core.helpers import run_command_and_log, get_line_count

class CondorJobManager:
    def __init__(self, args, job_name="Job"):
        self.args = args
        self.job_name = job_name
        self.output_dir = Path(args.output_dir).resolve()
        self.log_file = self.output_dir / 'log.txt'
        
        # Create output dir early so we can log
        self.output_dir.mkdir(parents=True, exist_ok=True)
        self.logger = setup_logging(self.log_file, logging.DEBUG)
        
        # Commonly resolved paths
        self.input_list = Path(args.input_list).resolve() if hasattr(args, 'input_list') and args.input_list else None
        self.condor_script = Path(args.condor_script).resolve() if hasattr(args, 'condor_script') and args.condor_script else None
        self.condor_log_dir = Path(args.condor_log_dir).resolve() if hasattr(args, 'condor_log_dir') and args.condor_log_dir else None
        self.common_errors = Path(args.common_errors).resolve() if hasattr(args, 'common_errors') and args.common_errors else None
        
        self.files_to_check = []
        self.dirs_to_check = []
        if self.input_list:
            self.files_to_check.append(self.input_list)
        if self.condor_script:
            self.files_to_check.append(self.condor_script)
        if self.common_errors:
            self.files_to_check.append(self.common_errors)

    def add_file_to_check(self, path):
        if path:
            self.files_to_check.append(Path(path).resolve())
            
    def add_dir_to_check(self, path):
        if path:
            self.dirs_to_check.append(Path(path).resolve())

    def validate_paths(self):
        for f in self.files_to_check:
            if not f.is_file():
                self.logger.critical(f'File: {f} does not exist!')
                sys.exit(1)
        for d in self.dirs_to_check:
            if not d.is_dir():
                self.logger.critical(f'Directory: {d} does not exist!')
                sys.exit(1)

    def log_initialization(self, extra_logs=None):
        total_files = get_line_count(self.input_list) if self.input_list else 0
        self.logger.info('#'*40)
        self.logger.info(f'LOGGING: {datetime.datetime.now()}')
        self.logger.info(f'Job Name: {self.job_name}')
        if self.input_list:
            self.logger.info(f'Input DST List: {self.input_list}')
        self.logger.info(f'Total DSTs: {total_files}')
        if hasattr(self.args, 'dst_per_job'):
            self.logger.info(f'DST Per Job: {self.args.dst_per_job}')
        if hasattr(self.args, 'events'):
            self.logger.info(f'Events to process: {self.args.events if self.args.events != 0 else "All"}')
        if hasattr(self.args, 'dbtag'):
            self.logger.info(f'DB Tag: {self.args.dbtag}')
        self.logger.info(f'Output Directory: {self.output_dir}')
        self.logger.info(f'Log File: {self.log_file}')
        if hasattr(self.args, 'memory'):
            self.logger.info(f'Condor Memory: {self.args.memory} GB')
        if self.condor_script:
            self.logger.info(f'Condor Script: {self.condor_script}')
        if self.condor_log_dir:
            self.logger.info(f'Condor Log Directory: {self.condor_log_dir}')
        if self.common_errors:
            self.logger.info(f'Common Errors File: {self.common_errors}')
        
        if extra_logs:
            for k, v in extra_logs.items():
                self.logger.info(f'{k}: {v}')
                
        return total_files

    def prepare_directories(self):
        if self.condor_log_dir:
            shutil.rmtree(self.condor_log_dir, ignore_errors=True)
            self.condor_log_dir.mkdir(parents=True, exist_ok=True)
            
        for subdir in ['stdout', 'error', 'output']:
            shutil.rmtree(self.output_dir / subdir, ignore_errors=True)
            (self.output_dir / subdir).mkdir(parents=True, exist_ok=True)
            
        files_dir = self.output_dir / 'files'
        files_dir.mkdir(parents=True, exist_ok=True)
        return files_dir

    def copy_dependencies(self, extra_files=None, extra_dirs=None):
        if self.input_list:
            shutil.copy(self.input_list, self.output_dir)
        if self.condor_script:
            shutil.copy(self.condor_script, self.output_dir)
        if self.common_errors:
            shutil.copy(self.common_errors, self.output_dir)
        
        if extra_files:
            for f in extra_files:
                if f:
                    shutil.copy(Path(f).resolve(), self.output_dir)
                
        if extra_dirs:
            for d in extra_dirs:
                if d:
                    src = Path(d).resolve()
                    shutil.copytree(src, self.output_dir / src.name, dirs_exist_ok=True)

    def write_submit_file(self, arguments, executable=None, memory=None):
        exec_file = executable or (self.condor_script.name if self.condor_script else "script.sh")
        mem = memory or getattr(self.args, 'memory', 1)
        
        log_dir = self.condor_log_dir or (self.output_dir / 'logs')
        
        submit_content = textwrap.dedent(f"""\
            executable     = {exec_file}
            arguments      = {arguments}
            log            = {log_dir}/job-$(ClusterId)-$(Process).log
            output         = stdout/job-$(ClusterId)-$(Process).out
            error          = error/job-$(ClusterId)-$(Process).err
            request_memory = {mem}GB
        """)
        sub_file = self.output_dir / 'genFun4All.sub'
        sub_file.write_text(submit_content)
        return sub_file

    def finalize_submission(self, queue_arg="input_dst from jobs.list", sub_file_name="genFun4All.sub"):
        command = f'cd {self.output_dir} && condor_submit {sub_file_name} -queue "{queue_arg}"'
        self.logger.info(command)
