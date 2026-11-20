#!/usr/bin/env bash
# Usage: ./format.sh [--check]
#   --check   Don't modify files; exit non-zero if any file needs formatting.

set -euo pipefail

cd "$(dirname "${BASH_SOURCE[0]}")"

mode=(-i)
if [[ "${1:-}" == "--check" ]]; then
    mode=(--dry-run --Werror)
elif [[ $# -gt 0 ]]; then
    echo "usage: $0 [--check]" >&2
    exit 2
fi

find src include -type f \( -name '*.cpp' -o -name '*.hpp' -o -name '*.c' -o -name '*.h' \) -print0 |
    xargs -0 clang-format --style=file "${mode[@]}"
