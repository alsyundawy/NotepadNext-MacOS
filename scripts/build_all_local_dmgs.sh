#!/usr/bin/env bash
set -Eeuo pipefail

# Repo root detection
if git rev-parse --show-toplevel &>/dev/null; then
	REPO_ROOT="$(git rev-parse --show-toplevel)"
else
	SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd -P)"
	REPO_ROOT="$(cd -- "${SCRIPT_DIR}/.." && pwd -P)"
fi
cd "${REPO_ROOT}"

echo "=== Starting Automated Local DMG Build Pipeline (NotepadNext-MacOS) ==="

QT_DIR="/Users/alsyundawy/Qt/6.12.0/macos"

if [[ $# -gt 0 ]]; then
	BRANCHES=("$@")
else
	BRANCHES=(
		"0.15-01"
		"0.15-02"
		"0.15-03"
		"0.15-04"
		"0.15-05"
		"0.15-06"
		"0.15-07"
		"0.15-08"
	)
fi

mkdir -p build-dmg

for BRANCH in "${BRANCHES[@]}"; do
	echo "=================================================="
	echo "Processing Branch: ${BRANCH}"
	echo "=================================================="

	# 1. Switch to target branch
	git switch "${BRANCH}"

	# 2. Clean previous build directory, configure & build DMG
	echo "[*] Cleaning build directory..."
	rm -rf build

	echo "[*] Configuring and building DMG target for ${BRANCH}..."
	cmake -S . -B build -G Ninja \
		-DCMAKE_BUILD_TYPE=Release \
		-DAPP_DISTRIBUTION=dmg \
		-DCMAKE_OSX_DEPLOYMENT_TARGET=15.0 \
		-DCMAKE_OSX_ARCHITECTURES=x86_64 \
		-DMAC_ARCH_SUFFIX=x64 \
		-DCMAKE_PREFIX_PATH="${QT_DIR}" \
		-DCMAKE_C_COMPILER=/usr/bin/clang \
		-DCMAKE_CXX_COMPILER=/usr/bin/clang++

	cmake --build build --target dmg

	DMG_PATH="build/NotepadNext-v${BRANCH}-x64.dmg"
	if [[ ! -f ${DMG_PATH} ]]; then
		FOUND_DMG=$(find build -maxdepth 1 -name "NotepadNext*.dmg" | head -n 1)
		if [[ -n ${FOUND_DMG} ]]; then
			DMG_PATH="${FOUND_DMG}"
		fi
	fi

	echo "[*] Verifying DMG: ${DMG_PATH}"
	hdiutil verify "${DMG_PATH}"

	echo "[*] Copying verified DMG to build-dmg/NotepadNext-v${BRANCH}-x64.dmg"
	cp -f "${DMG_PATH}" "build-dmg/NotepadNext-v${BRANCH}-x64.dmg"

	echo "=== Branch ${BRANCH} Completed Successfully! ==="
done

# Switch back to master
echo "=================================================="
echo "Returning to master"
echo "=================================================="
git switch master

# Update SHA256SUMS
echo "[*] Regenerating SHA256SUMS for all DMGs in build-dmg/"
cd build-dmg
shasum -a 256 NotepadNext*.dmg >SHA256SUMS
cat SHA256SUMS
cd ..

echo "=== ALL LOCAL DMGS BUILT AND VERIFIED SUCCESSFULLY! ==="
