#!/usr/bin/env bash

# Usage: ./builds.sh <project_name> [-f] [build_dir]
# Examples:
#   ./builds.sh jetbackground
#   ./builds.sh jetbackground -f
#   ./builds.sh jetbackground -f custom_builds
#   ./builds.sh jetbackground custom_builds -f

SRC_INPUT=$1
FAST_MODE=""
BUILD_BASE="${BUILD_BASE_DIR:-builds}"

# Check if project name/path was provided
if [ -z "$SRC_INPUT" ]; then
    echo "Error: Please provide a project name or path."
    echo "Usage: $0 <source_path_or_name> [-f] [build_dir]"
    exit 1
fi

# Parse optional arguments for fast mode (-f) and build directory
if [ "$2" == "-f" ]; then
    FAST_MODE="-f"
    if [ -n "$3" ]; then
        BUILD_BASE="$3"
    fi
elif [ -n "$2" ]; then
    BUILD_BASE="$2"
    if [ "$3" == "-f" ]; then
        FAST_MODE="-f"
    fi
fi

PROJ_NAME="$(basename "$SRC_INPUT")"

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
ROOT_DIR="$( cd "$SCRIPT_DIR/.." && pwd )"

# Resolve Source Directory (SRC_DIR)
if [ -n "$SRC_DIR" ]; then
    : # Use explicit environment variable if set
elif [ -d "$SRC_INPUT" ]; then
    SRC_DIR="$( cd "$SRC_INPUT" && pwd )"
elif [ -d "$ROOT_DIR/$SRC_INPUT" ]; then
    SRC_DIR="$( cd "$ROOT_DIR/$SRC_INPUT" && pwd )"
else
    SRC_DIR="$ROOT_DIR/$SRC_INPUT"
fi

BUILD_DIR="$BUILD_BASE/$PROJ_NAME"

# Ensure BUILD_DIR is resolved relative to ROOT_DIR if relative
if [[ "$BUILD_DIR" != /* ]]; then
    BUILD_DIR="$ROOT_DIR/$BUILD_DIR"
fi

# --- Logic Branching ---

if [ "$FAST_MODE" == "-f" ]; then
    echo ">>> Fast Mode: Running 'make install' for $PROJ_NAME in $BUILD_DIR..."
    make install -j8 --directory "$BUILD_DIR"
else
    echo ">>> Full Build: Refreshing and configuring $PROJ_NAME..."
    echo "    Source: $SRC_DIR"
    echo "    Build:  $BUILD_DIR"
    
    # 1. Clean and Create Build Dir
    rm -rf "$BUILD_DIR" && mkdir -p "$BUILD_DIR" || exit
    
    # 2. Configure (autogen)
    cd "$BUILD_DIR" && "$SRC_DIR"/autogen.sh --prefix="$MYINSTALL" || exit
    
    # 3. Build and Install
    cd "$ROOT_DIR" && make install -j8 --directory "$BUILD_DIR"
fi
