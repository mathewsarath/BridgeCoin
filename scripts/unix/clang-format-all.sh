#!/bin/bash

set -e

REPO_ROOT=$(git rev-parse --show-toplevel)
cd "${REPO_ROOT}"
find ./ -iname '*.hpp' -o -iname '*.cpp' | xargs clang-format -i
