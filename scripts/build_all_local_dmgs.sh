#!/usr/bin/env bash
set -Eeuo pipefail

# Scripts directory and repo root
SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd -P)"
REPO_ROOT="$(cd -- "${SCRIPT_DIR}/.." && pwd -P)"
cd "${REPO_ROOT}"

echo "=== Starting Automated Local DMG Build & Push Pipeline (0.15-03 to 0.15-07) ==="

QT_DIR="/Users/alsyundawy/Qt/6.12.0/macos"
BRANCHES=("0.15-03" "0.15-04" "0.15-05" "0.15-06" "0.15-07")

for BRANCH in "${BRANCHES[@]}"; do
    echo "=================================================="
    echo "Processing Branch: ${BRANCH}"
    echo "=================================================="
    
    # 1. Switch to branch
    git switch "${BRANCH}"
    
    # 2. Check and apply OptionSet.h fix if cstdlib is missing
    if ! grep -q "cstdlib" thirdparty/lexilla/lexlib/OptionSet.h; then
        echo "[*] Adding missing <cstdlib> include to OptionSet.h"
        python3 -c '
path = "thirdparty/lexilla/lexlib/OptionSet.h"
with open(path, "r", encoding="utf-8") as f:
    content = f.read()
target = "#ifndef OPTIONSET_H\n#define OPTIONSET_H\n"
replacement = "#ifndef OPTIONSET_H\n#define OPTIONSET_H\n\n#include <cstdlib>\n#include <string>\n"
if target in content and "#include <cstdlib>" not in content:
    content = content.replace(target, replacement, 1)
    with open(path, "w", encoding="utf-8") as f:
        f.write(content)
    print("Successfully patched OptionSet.h")
'
    fi

    # 3. Check and modernize PackagingMac.cmake if outdated
    if grep -q "COMMAND \${MACDEPLOYQT_EXECUTABLE}.*-dmg" cmake/PackagingMac.cmake; then
        echo "[*] Modernizing cmake/PackagingMac.cmake"
        git checkout master -- cmake/PackagingMac.cmake
    fi

    # 4. Configure & Build DMG
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
    if [[ ! -f "${DMG_PATH}" ]]; then
        # Try generic name if versioned suffix differs
        DMG_PATH="build/NotepadNext-v0.15-x64.dmg"
    fi

    echo "[*] Verifying DMG: ${DMG_PATH}"
    hdiutil verify "${DMG_PATH}"

    echo "[*] Copying verified DMG to build-dmg/NotepadNext-v${BRANCH}-x64.dmg"
    mkdir -p build-dmg
    cp -f "${DMG_PATH}" "build-dmg/NotepadNext-v${BRANCH}-x64.dmg"

    # 5. Commit and push fixes if there are modifications
    if [[ -n "$(git status --porcelain)" ]]; then
        echo "[*] Staging and committing changes on branch ${BRANCH}"
        git add -u
        git commit -m "fix(build): add cstdlib include in OptionSet.h and modernize macOS DMG packaging" || true
        echo "[*] Pushing branch ${BRANCH} to origin"
        git push origin "refs/heads/${BRANCH}:refs/heads/${BRANCH}"
    else
        echo "[*] No file changes needed on branch ${BRANCH}"
    fi

    echo "=== Branch ${BRANCH} Completed Successfully! ==="
done

# Switch to master
echo "=================================================="
echo "Returning to master"
echo "=================================================="
git switch master

# Update SHA256SUMS
echo "[*] Regenerating SHA256SUMS for all DMGs"
cd build-dmg
shasum -a 256 NotepadNext-v0.15-01-x64.dmg \
           NotepadNext-v0.15-02-x64.dmg \
           NotepadNext-v0.15-03-x64.dmg \
           NotepadNext-v0.15-04-x64.dmg \
           NotepadNext-v0.15-05-x64.dmg \
           NotepadNext-v0.15-06-x64.dmg \
           NotepadNext-v0.15-07-x64.dmg > SHA256SUMS
cd ..

echo "=== ALL LOCAL DMGS BUILT, VERIFIED, AND SYNCED SUCCESSFULLY! ==="
