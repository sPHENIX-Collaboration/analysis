import datetime
import os
import re
import shutil
import sys
from concurrent.futures import ThreadPoolExecutor
from pathlib import Path

from condor_utils.core.helpers import (
    extract_run_and_seg,
    get_best_submit_node,
    get_line_count,
    resolve_repo_path,
    run_command_and_log,
)
from condor_utils.core.logging import setup_logging


class CondorJobManager:
    def __init__(self, args, job_name="CaloFittingQA"):
        self.args = args
        self.job_name = job_name
        self.output_dir = Path(args.output_dir).resolve()
        self.log_file = self.output_dir / 'log.txt'

        # Create output dir early for logging
        self.output_dir.mkdir(parents=True, exist_ok=True)
        self.logger = setup_logging(self.log_file)

        self.input_list = resolve_repo_path(getattr(args, 'input_list', None))
        self.condor_script = resolve_repo_path(getattr(args, 'condor_script', None))
        self.condor_log_dir = Path(args.condor_log_dir).resolve() if getattr(args, 'condor_log_dir', None) else None
        self.job_output_dir = Path(args.job_output_dir).resolve() if getattr(args, 'job_output_dir', None) else None

        self.files_to_check = []
        self.dirs_to_check = []
        if self.input_list:
            self.files_to_check.append(self.input_list)
        if self.condor_script:
            self.files_to_check.append(self.condor_script)

    def add_file_to_check(self, path):
        if path:
            resolved = resolve_repo_path(path)
            if resolved:
                self.files_to_check.append(resolved)

    def add_dir_to_check(self, path):
        if path:
            resolved = resolve_repo_path(path)
            if resolved:
                self.dirs_to_check.append(resolved)

    def validate_paths(self):
        for f in self.files_to_check:
            if not f.is_file():
                self.logger.critical(f'Required file does not exist: {f}')
                sys.exit(1)
        for d in self.dirs_to_check:
            if not d.is_dir():
                self.logger.critical(f'Required directory does not exist: {d}')
                sys.exit(1)

    def get_best_submit_node(self):
        if not hasattr(self, '_ranked_nodes') or self._ranked_nodes is None:
            manual_node = getattr(self.args, 'node', None)
            user = getattr(self.args, 'user', None) or "anarde"
            detected_nodes, node_status = get_best_submit_node(logger=self.logger, user=user)
            self._ranked_nodes = [manual_node] if manual_node else detected_nodes
            self._node_status = node_status
        return self._ranked_nodes, self._node_status

    def log_initialization(self, extra_logs=None):
        total_files = get_line_count(self.input_list) if self.input_list else 0
        self.logger.info('#' * 60)
        self.logger.info(f'CONDOR SUBMISSION SETUP: {datetime.datetime.now()}')
        self.logger.info(f'Job Name: {self.job_name}')
        if self.input_list:
            self.logger.info(f'Input List: {self.input_list} ({total_files} segments)')
        if hasattr(self.args, 'events'):
            self.logger.info(f'Events per Job: {self.args.events if self.args.events != 0 else "All"}')
        if hasattr(self.args, 'dbtag'):
            self.logger.info(f'CDB Tag: {self.args.dbtag}')
        self.logger.info(f'Output Directory: {self.output_dir}')
        if self.job_output_dir:
            self.logger.info(f'Job Output Directory: {self.job_output_dir}')
        if hasattr(self.args, 'memory'):
            self.logger.info(f'Condor Base Memory: {self.args.memory} GB')
            step = getattr(self.args, 'retry_memory_step', 0.5)
            ceiling = getattr(self.args, 'retry_memory_max', 6.0)
            if step and step > 0:
                self.logger.info(f'Retry Memory Policy: +{step} GB on OOM eviction up to {ceiling} GB')
        if self.condor_script:
            self.logger.info(f'Condor Executable Script: {self.condor_script}')
        if self.condor_log_dir:
            self.logger.info(f'Condor Log Directory: {self.condor_log_dir}')

        ranked_nodes, node_status = self.get_best_submit_node()
        top_node = ranked_nodes[0]
        if node_status:
            user = getattr(self.args, 'user', None) or "anarde"
            status_summary = ", ".join(
                f"{n}: {node_status[n]['user_total']} ({node_status[n]['user_running']}R/{node_status[n]['user_idle']}I)"
                for n in sorted(node_status.keys())
            )
            self.logger.info(f'Submit Nodes Jobs for {user}: {status_summary}')
            self.logger.info(
                f'Top Submit Node: {top_node} '
                f'({node_status[top_node]["user_total"]} jobs for {user} '
                f'[{node_status[top_node]["user_running"]} running, {node_status[top_node]["user_idle"]} idle] | '
                f'node total: {node_status[top_node]["total_running"]} running)'
            )
        else:
            self.logger.info(f'Top Submit Node: {top_node}')

        if extra_logs:
            for k, v in extra_logs.items():
                self.logger.info(f'{k}: {v}')
        self.logger.info('#' * 60)
        return total_files

    def prepare_directories(self):
        if self.condor_log_dir:
            self.condor_log_dir.mkdir(parents=True, exist_ok=True)

        for subdir in ['stdout', 'error', 'failures']:
            (self.output_dir / subdir).mkdir(parents=True, exist_ok=True)

        output_symlink = self.output_dir / 'output'
        if self.job_output_dir:
            self.job_output_dir.mkdir(parents=True, exist_ok=True)
            if output_symlink.is_symlink() or output_symlink.is_file():
                output_symlink.unlink()
            elif output_symlink.is_dir():
                shutil.rmtree(output_symlink)
            output_symlink.symlink_to(self.job_output_dir, target_is_directory=True)
            self.logger.info(f"Created output symlink: {output_symlink} -> {self.job_output_dir}")
        else:
            if output_symlink.is_symlink() or output_symlink.is_file():
                output_symlink.unlink()
            output_symlink.mkdir(parents=True, exist_ok=True)

    def copy_dependencies(self, extra_files=None, extra_dirs=None):
        if self.input_list and self.input_list.is_file():
            shutil.copy(self.input_list, self.output_dir)
        if self.condor_script and self.condor_script.is_file():
            shutil.copy(self.condor_script, self.output_dir)

        if extra_files:
            for f in extra_files:
                if f:
                    p = Path(f).resolve()
                    if p.is_file():
                        shutil.copy(p, self.output_dir)
                    else:
                        self.logger.warning(f"File to copy not found: {f}")

        if extra_dirs:
            for d in extra_dirs:
                if d:
                    src = Path(d).resolve()
                    if src.is_dir():
                        shutil.copytree(src, self.output_dir / src.name, dirs_exist_ok=True)

    def find_build_dir(self, build_tag: str = None) -> Path | None:
        """Finds the release directory under cvmfs for the specified build tag."""
        build = build_tag or getattr(self.args, 'build', None) or (
            Path(os.environ['OFFLINE_MAIN']).name if os.environ.get('OFFLINE_MAIN') else 'new'
        )
        candidates = [
            Path(f'/cvmfs/sphenix.sdcc.bnl.gov/alma9.2-gcc-14.2.0/release/{build}'),
            Path(f'/cvmfs/sphenix.sdcc.bnl.gov/alma9.2-gcc-14.2.0/release/release_new/{build}'),
            Path(f'/cvmfs/sphenix.sdcc.bnl.gov/alma9.2-gcc-14.2.0/release/release_ana/{build}'),
        ]
        if 'OFFLINE_MAIN' in os.environ:
            om = Path(os.environ['OFFLINE_MAIN'])
            candidates.extend([
                om,
                om.parent / build,
                om.parent.parent / build,
            ])
        for c in candidates:
            if c.is_dir() and (c / 'rebuild.info').is_file():
                return c.resolve()
        return None

    def copy_build_info(self, build_tag: str = None):
        """Copies rebuild.log and rebuild.info from the build directory into the job directory."""
        build_dir = self.find_build_dir(build_tag)
        if build_dir:
            self.logger.info(f"Identified sPHENIX build directory: {build_dir}")
            for fname in ['rebuild.info', 'rebuild.log']:
                src = build_dir / fname
                if src.is_file():
                    shutil.copy(src, self.output_dir)
                    self.logger.info(f"Copied {fname} from {build_dir} to {self.output_dir}")
                else:
                    self.logger.warning(f"{fname} not found in {build_dir}")
        else:
            self.logger.warning(f"Could not locate build directory for build '{build_tag}'.")

    def prepare_job_lists(self, jobs_file_name="jobs.list"):
        """
        Parses input_list where each line points to a segment list.
        Validates segment list files, ensures both run number and segment are
        encoded in the output filename, and writes:
        $(input_seg_list),$(output_file) into jobs.list.
        """
        jobs_file = self.output_dir / jobs_file_name
        jobs_file.unlink(missing_ok=True)

        if not self.input_list or not self.input_list.is_file():
            self.logger.error("Input list is not set or does not exist.")
            return []

        raw_lines = [
            line.strip()
            for line in self.input_list.read_text(encoding='utf-8', errors='ignore').splitlines()
            if line.strip() and not line.strip().startswith('#')
        ]

        total_segments = len(raw_lines)
        self.logger.info(f"Parsing and preparing jobs for {total_segments} segment lists...")

        job_entries = []
        missing_count = 0
        invalid_file_count = 0

        for line in raw_lines:
            seg_path = Path(line)
            if not seg_path.is_file():
                # Try relative to input_list parent
                candidate = self.input_list.parent / line
                if candidate.is_file():
                    seg_path = candidate
                else:
                    self.logger.warning(f"Segment list file not found: {line}")
                    missing_count += 1
                    continue

            # Verify that segment list is non-empty
            lines_in_seg = [l.strip() for l in seg_path.read_text(encoding='utf-8', errors='ignore').splitlines() if l.strip() and not l.strip().startswith('#')]
            if len(lines_in_seg) != 19:
                self.logger.debug(f"{seg_path.name} contains {len(lines_in_seg)} files (expected 19).")
                invalid_file_count += 1

            run, seg = extract_run_and_seg(seg_path.name)
            output_file = f"HIST_CaloFittingQA_run{run}_seg{seg}.root"
            job_entries.append(f"{seg_path.resolve()},{output_file}")

        if missing_count > 0:
            self.logger.warning(f"Skipped {missing_count} missing segment lists.")
        if invalid_file_count > 0:
            self.logger.info(f"Note: {invalid_file_count} segment lists did not contain exactly 19 files.")

        if job_entries:
            jobs_file.write_text("\n".join(job_entries) + "\n", encoding='utf-8')

        self.logger.info(f"Total jobs prepared: {len(job_entries)} / {total_segments} written to {jobs_file.name}")
        return job_entries

    def write_submit_file(
        self,
        arguments: str,
        executable: str = None,
        memory=None,
        retry_request_memory=None,
        retry_memory_step=None,
        retry_memory_max=None,
        max_retries=None,
        sub_file_name="genFun4All_CaloFittingQA.sub",
        stdout_dir="stdout",
        error_dir="error",
        log_prefix="job",
    ):
        exec_file = executable or (self.condor_script.name if self.condor_script else "genFun4All_CaloFittingQA.sh")
        mem = memory if memory is not None else getattr(self.args, 'memory', 2.0)

        # Parse base memory
        mem_float = None
        if isinstance(mem, (int, float)):
            mem_float = float(mem)
            mem_str = f"{int(mem_float)}GB" if mem_float.is_integer() else f"{mem_float:g}GB"
        else:
            mem_s = str(mem).strip()
            m = re.match(r"^([\d\.]+)\s*([a-zA-Z]*)$", mem_s)
            if m:
                mem_float = float(m.group(1))
                unit = m.group(2) or "GB"
                mem_str = f"{mem_float:g}{unit}"
            else:
                mem_str = mem_s

        # Determine retry_request_memory stepping
        retry_mem = retry_request_memory or getattr(self.args, 'retry_request_memory', None)
        if retry_mem is None:
            step = retry_memory_step if retry_memory_step is not None else getattr(self.args, 'retry_memory_step', 0.5)
            ceiling = retry_memory_max if retry_memory_max is not None else getattr(self.args, 'retry_memory_max', 6.0)
            if step and step > 0 and mem_float is not None and ceiling and ceiling > mem_float:
                steps = []
                curr = mem_float + float(step)
                while round(curr, 4) <= round(float(ceiling), 4):
                    val = round(curr, 4)
                    steps.append(f"{int(val)}GB" if val.is_integer() else f"{val:g}GB")
                    curr += float(step)
                if steps:
                    retry_mem = ", ".join(steps)

        log_dir = self.condor_log_dir or (self.output_dir / 'logs')
        log_dir.mkdir(parents=True, exist_ok=True)

        num_memory_tiers = len([s for s in retry_mem.split(',') if s.strip()]) if retry_mem else 0
        base_retries = max_retries if max_retries is not None else getattr(self.args, 'max_retries', 3)
        effective_retries = max(int(base_retries), num_memory_tiers)
        if num_memory_tiers > int(base_retries):
            self.logger.info(
                f"Elevating max_retries from {base_retries} to {effective_retries} "
                f"to cover all {num_memory_tiers} memory retry tiers."
            )

        lines = [
            f"executable           = {exec_file}",
            f"arguments            = {arguments}",
            f"log                  = {log_dir}/{log_prefix}-$(ClusterId)-$(Process).log",
            f"output               = {stdout_dir}/job-$(ClusterId)-$(Process).out",
            f"error                = {error_dir}/job-$(ClusterId)-$(Process).err",
            f"request_memory       = {mem_str}",
        ]
        if retry_mem:
            lines.append(f"retry_request_memory = {retry_mem}")
        lines.extend([
            f"max_retries          = {effective_retries}",
            f"stream_output        = True",
            f"stream_error         = True",
            "",
        ])

        sub_file = self.output_dir / sub_file_name
        sub_file.write_text("\n".join(lines))
        self.logger.info(f"Submit file written: {sub_file.name}")
        return sub_file

    def finalize_submission(
        self,
        queue_arg="input_seg_list,output_file from jobs.list",
        sub_file_name="genFun4All_CaloFittingQA.sub",
        limit=15000,
        execute=False,
        clean_log_dir=True,
    ):
        match = re.search(r" from ([\w\.-]+)", queue_arg)
        list_file = match.group(1) if match else "jobs.list"
        list_path = self.output_dir / list_file

        total_lines = get_line_count(list_path)

        if total_lines > limit:
            lines = [l for l in list_path.read_text(encoding='utf-8').splitlines() if l.strip()]
            split_files = []
            for i, chunk_start in enumerate(range(0, len(lines), limit)):
                chunk = lines[chunk_start:chunk_start + limit]
                split_file = self.output_dir / f"jobs-{i}.list"
                split_file.write_text("\n".join(chunk) + "\n", encoding='utf-8')
                split_files.append(split_file)
        else:
            split_files = [list_path]

        ranked_nodes, _ = self.get_best_submit_node()
        log_dir = self.condor_log_dir or (self.output_dir / 'logs')
        prep_cmd = f"rm -rf {log_dir} && mkdir -p {log_dir} && " if clean_log_dir else ""

        for i, sf in enumerate(split_files):
            current_queue_arg = queue_arg.replace(list_file, sf.name)
            base_cmd = f"{prep_cmd}cd {self.output_dir} && condor_submit {sub_file_name} -queue '{current_queue_arg}'"
            target_node = ranked_nodes[i % len(ranked_nodes)]
            ssh_command = f"ssh {target_node} '{base_cmd}'"

            if execute:
                self.logger.info(f"Submitting cluster via: {ssh_command}")
                run_command_and_log(ssh_command, self.logger, self.output_dir)
            else:
                self.logger.info("=" * 60)
                self.logger.info("Submission files generated successfully! To submit jobs to Condor, run:")
                self.logger.info(f"  {ssh_command}")
                self.logger.info("Or locally from the output directory:")
                self.logger.info(f"  {base_cmd}")
                self.logger.info("=" * 60)
