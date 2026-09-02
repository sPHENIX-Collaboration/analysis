#!/usr/bin/env bash
set -euo pipefail

# ANSI color codes for readability
BOLD='\033[1m'
CYAN='\033[0;36m'
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
RESET='\033[0m'

check_updates() {
  local repo_path="$1"
  local base_branch="$2"
  local target_branch="$3"
  local path_filter="$4"

  echo -e "\n${BOLD}${CYAN}=== Checking [${repo_path##*/}] : ${base_branch} -> ${target_branch} (${path_filter}) ===${RESET}"

  # Fetch all remotes quietly
  git -C "$repo_path" fetch --all --quiet

  # Check if there are unapplied commits on target_branch affecting the given path
  local commits
  commits=$(git -C "$repo_path" log --oneline --cherry-pick --right-only "${base_branch}...${target_branch}" -- "$path_filter")

  if [ -z "$commits" ]; then
    echo -e "${GREEN}Up to date. No new commits found.${RESET}"
  else
    echo -e "${YELLOW}Updates available:${RESET}"
    # Log commits with summary stats (files changed, insertions/deletions)
    git -C "$repo_path" log \
      --color=always \
      --stat \
      --cherry-pick \
      --right-only \
      "${base_branch}...${target_branch}" \
      -- "$path_filter"
  fi
}

# --- Repository Definitions ---
MINBIAS_DIR="$HOME/sPHENIX/coresoftware-MinBias"
JETBG_DIR="$HOME/sPHENIX/coresoftware-jetbackground"
CALOTOWERCALIB_DIR="$HOME/sPHENIX/coresoftware-CaloTowerCalib"

# 1. MinBias: trigger package against master
check_updates "$MINBIAS_DIR" "MinBias" "master" ":/offline/packages/trigger"

# 2. Jetbackground: jetbackground package against master
check_updates "$JETBG_DIR" "jetbackground" "master" ":/offline/packages/jetbackground"

# 3. Jetbackground: jetbackground package against ppg14
check_updates "$JETBG_DIR" "jetbackground" "ppg14" ":/offline/packages/jetbackground"

# 4. CaloTowerCalib: CaloReco package against master
check_updates "$CALOTOWERCALIB_DIR" "CaloTowerCalib" "master" ":/offline/packages/CaloReco"

echo -e "\n${BOLD}Done.${RESET}"


