#!/usr/bin/env bash

SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"

pushd "$SCRIPT_DIR/../" > /dev/null
./vendor/premake/bin/premake5 gmake
popd > dev/null

read -rp "Press any key to continue..."