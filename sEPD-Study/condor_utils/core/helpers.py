import subprocess
from pathlib import Path

def run_command_and_log(command, logger, current_dir = '.', do_logging = True, description="Executing command"):
    """
    Runs an external command using subprocess and logs its stdout, stderr, and return code.
    """
    if do_logging:
        logger.info(f"{description}: '{command}'")

    try:
        # subprocess.run is the recommended high-level API
        # capture_output=True: captures stdout and stderr
        # text=True: decodes output as text (usually UTF-8)
        # check=False: do NOT raise an exception for non-zero exit codes immediately.
        #              We want to log stderr even on failure before deciding to raise.
        result = subprocess.run(['bash','-c',command], cwd=current_dir, capture_output=True, text=True, check=False)

        # Log stdout if any
        if result.stdout and do_logging:
            # Using logger.debug allows capturing even verbose outputs
            logger.debug(f"  STDOUT from '{command}':\n{result.stdout.strip()}")

        # Log stderr if any
        if result.stderr:
            # Using logger.error for stderr, as it often indicates problems
            logger.error(f"  STDERR from '{command}':\n{result.stderr.strip()}")

        if do_logging:
            # Log the return code
            logger.info(f"  Command exited with code: {result.returncode}")

        # You can choose to raise an exception here if the command failed
        if result.returncode != 0:
            logger.error(f"Command failed: '{command}' exited with non-zero code {result.returncode}")
            # Optionally, raise an error to stop execution
            # raise subprocess.CalledProcessError(result.returncode, command, output=result.stdout, stderr=result.stderr)
            return False
        return True

    except FileNotFoundError:
        logger.critical(f"Error: Command '{command}' not found. Is it in your PATH?")
        return False
    except Exception as e:
        logger.critical(f"An unexpected error occurred while running '{command}': {e}")
        return False

# Helper to chunk list into smaller lists
def chunk_list(input_list, size):
    """Yield successive size-sized chunks from input_list."""
    for i in range(0, len(input_list), size):
        yield input_list[i:i + size]

def get_line_count(file_path: Path) -> int:
    """
    Checks if a file exists at the given path and, if so, returns the number of lines.

    Args:
        file_path: A path to the file.

    Returns:
        The number of lines in the file if it exists.
        Returns 0 if the file does not exist.
    """
    if not file_path.is_file():
        return 0

    return len(file_path.read_text(encoding='utf-8').splitlines())

def count_total_lines(directory_path: Path) -> int:
    """Calculates the sum of lines in all files within a directory."""

    # Use a generator expression to sum the results of count_lines for each file
    total = sum(count_lines(f) for f in directory_path.iterdir() if f.is_file())

    return total

def count_lines(file_path: Path) -> int:
    """Counts lines by iterating through the file."""
    try:
        with file_path.open('rb') as f:
            return sum(1 for _ in f)
    except (PermissionError, OSError):
        return 0

def sort_files_by_lines(directory_path: Path, output_file: Path) -> None:
    file_data = []

    # 1. Create a list of (path, count) tuples using a list comprehension
    file_data = [
        (f.resolve(), count_lines(f))
        for f in directory_path.iterdir() if f.is_file()
    ]

    # 2. Sort descending by line count (index 1)
    file_data.sort(key=lambda x: x[1], reverse=True)

    # 3. Join the paths into one big string separated by newlines
    output_content = "\n".join(str(path) for path, count in file_data) + "\n"

    # 4. Use write_text to save everything at once
    output_file.write_text(output_content)
