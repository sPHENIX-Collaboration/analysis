# Source this file (do not execute it):  source setup.sh

if [[ "${BASH_SOURCE[0]}" == "${0}" ]]; then
    echo "Run as: source ${0}"
    exit 1
fi

NNSCALEMAP_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# The sPHENIX environment points PYTHONPATH and LD_LIBRARY_PATH at the cvmfs Python 3.13 site-packages and libtorch_python.so, which break this venv, so both are cleared in the current shell only
unset PYTHONPATH LD_LIBRARY_PATH

# requirements.txt needs Python >= 3.10 (filelock==3.20.3); system python3.12 satisfies it
if [[ ! -d "${NNSCALEMAP_DIR}/.venv" ]]; then
    /usr/bin/python3.12 -m venv "${NNSCALEMAP_DIR}/.venv"
    "${NNSCALEMAP_DIR}/.venv/bin/python" -m pip install --upgrade pip
    "${NNSCALEMAP_DIR}/.venv/bin/pip" install --no-cache-dir -r "${NNSCALEMAP_DIR}/requirements.txt"
fi

source "${NNSCALEMAP_DIR}/.venv/bin/activate"
