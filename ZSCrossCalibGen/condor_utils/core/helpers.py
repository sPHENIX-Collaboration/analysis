import re
import shutil
import subprocess
from pathlib import Path

SUBMISSION_NODES = [f"sphnxuser{i:02d}" for i in range(1, 9)]

def resolve_repo_path(raw_path) -> Path | None:
    """Resolves a file or directory path relative to cwd, ZSCrossCalibGen package dir, or analysis repo."""
    if not raw_path:
        return None
    p = Path(raw_path)
    if p.is_file() or p.is_dir():
        return p.resolve()

    pkg_dir = Path(__file__).resolve().parent.parent.parent
    candidate1 = pkg_dir / raw_path
    if candidate1.is_file() or candidate1.is_dir():
        return candidate1.resolve()

    candidate2 = pkg_dir.parent / raw_path
    if candidate2.is_file() or candidate2.is_dir():
        return candidate2.resolve()

    # If raw_path has a prefix like ZSCrossCalibGen/... and we are in ZSCrossCalibGen
    parts = p.parts
    if len(parts) > 1 and parts[0] == "ZSCrossCalibGen":
        sub_path = Path(*parts[1:])
        candidate3 = pkg_dir / sub_path
        if candidate3.is_file() or candidate3.is_dir():
            return candidate3.resolve()

    return p.resolve()

def run_command_and_log(command: str, logger=None, current_dir='.', do_logging=True, description="Executing command") -> bool:
    """
    Runs an external shell command using subprocess and logs its stdout, stderr, and return code.
    """
    if do_logging and logger:
        logger.info(f"{description}: '{command}'")

    try:
        result = subprocess.run(['bash', '-c', command], cwd=current_dir, capture_output=True, text=True, check=False)

        if result.stdout and do_logging and logger:
            logger.debug(f"  STDOUT:\n{result.stdout.strip()}")

        if result.stderr and logger:
            logger.error(f"  STDERR:\n{result.stderr.strip()}")

        if do_logging and logger:
            logger.info(f"  Exited with code: {result.returncode}")

        return result.returncode == 0

    except FileNotFoundError:
        if logger:
            logger.critical(f"Error: Command '{command}' not found.")
        return False
    except Exception as e:
        if logger:
            logger.critical(f"An unexpected error occurred while running '{command}': {e}")
        return False

def get_line_count(file_path: Path) -> int:
    """Returns the line count of an existing text file."""
    path = Path(file_path)
    if not path.is_file():
        return 0
    try:
        return len(path.read_text(encoding='utf-8', errors='ignore').splitlines())
    except Exception:
        return 0

def extract_run_and_seg(path_or_str: str) -> tuple[str, str]:
    """
    Extracts run and segment strings from a segment list filename or path.
    Examples:
        dst_triggered_run67633_seg00025.list -> ("67633", "00025")
        DST_TRIGGERED_EVENT_...-00067633-00025.root -> ("67633", "00025")
    """
    stem = Path(path_or_str).stem

    # Pattern: run<RUN>_seg<SEG>
    m = re.search(r'run(\d+)_seg(\d+)', stem, re.IGNORECASE)
    if m:
        return m.group(1), m.group(2)

    # Pattern: -<RUN>-<SEG>
    m = re.search(r'-(\d+)-(\d+)', stem)
    if m:
        run_num = m.group(1).lstrip('0') or '0'
        return run_num, m.group(2)

    # Fallbacks: separate run and seg matches
    run_m = re.search(r'run(\d+)', stem, re.IGNORECASE) or re.search(r'\b(\d{5,})\b', stem)
    seg_m = re.search(r'seg(\d+)', stem, re.IGNORECASE) or re.search(r'_(\d{4,5})\b', stem)

    run = run_m.group(1).lstrip('0') if run_m else "run"
    seg = seg_m.group(1) if seg_m else "00000"
    return run, seg

def parse_submitters_output(output_text: str, user: str = "anarde") -> dict[str, dict[str, int]]:
    """
    Parses output of 'condor_status -submitters'.
    Aggregates user-specific and total RunningJobs and IdleJobs for sphnxuser01-08 nodes.
    """
    nodes = {
        node: {
            "user_running": 0,
            "user_idle": 0,
            "user_total": 0,
            "total_running": 0,
            "total_idle": 0,
        }
        for node in SUBMISSION_NODES
    }

    user_str = (user or "anarde").split("@")[0].lower()
    user_lower = user_str.split(".")[-1]

    for line in output_text.splitlines():
        line = line.strip()
        if not line:
            continue
        parts = line.split()
        if len(parts) >= 4:
            m = re.search(r"(sphnxuser0[1-8])", parts[1], re.IGNORECASE)
            if m:
                node = m.group(1).lower()
                raw_user = parts[0].split("@")[0].lower()
                submitter_user = raw_user.split(".")[-1]
                try:
                    r_jobs = int(parts[2])
                    i_jobs = int(parts[3])
                except ValueError:
                    continue

                nodes[node]["total_running"] += r_jobs
                nodes[node]["total_idle"] += i_jobs

                if submitter_user == user_lower or raw_user == user_str:
                    nodes[node]["user_running"] += r_jobs
                    nodes[node]["user_idle"] += i_jobs
                    nodes[node]["user_total"] += (r_jobs + i_jobs)
    return nodes

def get_best_submit_node(logger=None, user: str = "anarde") -> tuple[list[str], dict[str, dict[str, int]]]:
    """
    Ranks submission nodes (sphnxuser01-08) by running 'condor_status -submitters'.
    Returns:
        (ranked_nodes_list, nodes_dict)
    """
    nodes = {
        node: {
            "user_running": 0,
            "user_idle": 0,
            "user_total": 0,
            "total_running": 0,
            "total_idle": 0,
        }
        for node in SUBMISSION_NODES
    }
    try:
        res = subprocess.run(
            ["condor_status", "-submitters"],
            capture_output=True,
            text=True,
            timeout=15,
            check=False,
        )
        if res.returncode == 0 and res.stdout:
            nodes = parse_submitters_output(res.stdout, user=user)
        elif logger:
            logger.warning(
                f"'condor_status -submitters' returned non-zero code {res.returncode}: {res.stderr.strip()}"
            )
    except Exception as e:
        if logger:
            logger.warning(f"Failed to execute 'condor_status -submitters': {e}")

    ranked_nodes = sorted(
        SUBMISSION_NODES,
        key=lambda n: (
            nodes[n]["total_running"] + nodes[n]["user_idle"],
            nodes[n]["user_total"],
            nodes[n]["total_idle"],
            SUBMISSION_NODES.index(n),
        ),
    )
    return ranked_nodes, nodes
