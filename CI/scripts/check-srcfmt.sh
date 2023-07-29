#!/bin/bash
# Copyright 2018-2023 VMware, Inc.
# SPDX-License-Identifier: Apache-2.0
# #############################################################################
# Check source-code formatting, basically per Google style, fine-tuned to our
# code layout preferences as specified in .clang-format rules.
# #############################################################################

set -eu -o pipefail

Me=$(basename "$0")

# Standardize on this version to avoid diffs-in-behaviour across
# versions of clang-format tool.
CLANG_FMT_TOOL="clang-format-11"

# Check if required tool exists
if ! command -v "$CLANG_FMT_TOOL" &> /dev/null; then
   echo "${Me}: Error: missing required tool $CLANG_FMT_TOOL

This tool is typically provided by the clang-format package"
   exit 1
fi

# Establish root-dir for Certifier library.
pushd "$(dirname "$0")" > /dev/null 2>&1

cd ../../

# shellcheck disable=SC2046
CERTIFIER_ROOT="$(pwd)"; export CERTIFIER_ROOT

popd > /dev/null 2>&1

pushd "${CERTIFIER_ROOT}" > /dev/null 2>&1

list_of_files="$(find . \( -name "*.[ch]" -o -name "*.cc" \) -print | grep -v -E 'third_party|protobufs-bin|\.pb.cc|\.pb.h')"

if [ "${1:-""}" = "fixall" ]; then

    nold=$(git status --untracked-files=no | grep -c -E '^nothing to commit|modified:')

    # Fix files in-place that need clang-formatting
    # shellcheck disable=SC2086
    ${CLANG_FMT_TOOL} -i ${list_of_files}

    nnew=$(git status --untracked-files=no | grep -c 'modified:')

    if [ "${nold}" -ne "${nnew}" ]; then
        echo " "
        echo "${Me}: The following files were change to conform to clang-format rules:"
        echo " "
        git status --untracked-files=no | grep 'modified:'
    fi
    exit 0
fi

# shellcheck disable=SC2086
if ${CLANG_FMT_TOOL} --Werror --dry-run --Wclang-format-violations ${list_of_files}; then
    rc=0
else
    echo " "
    echo "${Me}: Following source files must be clang-format'ted."
    echo " "
    ${CLANG_FMT_TOOL} --dry-run --Wclang-format-violations ${list_of_files} \
        2>&1 \
        | grep 'clang-formatted' \
        | cut -f1 -d':' | sort -u
    echo " "
    echo "${Me}: Try running: ${Me} fixall"
    rc=1
fi

popd > /dev/null 2>&1

exit $rc
