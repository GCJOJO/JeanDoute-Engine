#/bin/bash

SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"

pushd "$SCRIPT_DIR/../" > dev/null
./vendor/premake/bin/premake5 ninja
popd > dev/null

read -rp "Press any key to continue..." -n1 -s