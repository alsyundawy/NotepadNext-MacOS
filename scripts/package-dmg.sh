#!/usr/bin/env bash
# ==============================================================================
# NotepadNext-MacOS DMG Drag-and-Drop Packaging Script
# ==============================================================================
# Creates a modern, styled macOS DMG installer with custom background,
# drag-and-drop indicator, volume icon, and Applications symlink.
# ==============================================================================

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "${SCRIPT_DIR}/.." && pwd)"

# Input parameters
APP_DIR="${1:-${REPO_ROOT}/build/install/NotepadNext.app}"
DMG_OUTPUT="${2:-${REPO_ROOT}/build/NotepadNext-v0.15-08-x64.dmg}"
VOL_NAME="${3:-NotepadNext}"

# Ensure absolute paths
APP_PARENT_DIR="$(dirname "${APP_DIR}")"
if [[ -d ${APP_PARENT_DIR} ]]; then
	APP_PARENT_ABS="$(cd "${APP_PARENT_DIR}" && pwd)"
	APP_BASE="$(basename "${APP_DIR}")"
	APP_DIR="${APP_PARENT_ABS}/${APP_BASE}"
fi

DMG_PARENT_DIR="$(dirname "${DMG_OUTPUT}")"
mkdir -p "${DMG_PARENT_DIR}"
DMG_OUTPUT_DIR="$(cd "${DMG_PARENT_DIR}" && pwd)"
DMG_BASE="$(basename "${DMG_OUTPUT}")"
DMG_OUTPUT="${DMG_OUTPUT_DIR}/${DMG_BASE}"

echo "=========================================================="
echo "⚡ NotepadNext DMG Drag-and-Drop Packaging Pipeline"
echo "=========================================================="
echo "Source App Bundle : ${APP_DIR}"
echo "Output Disk Image : ${DMG_OUTPUT}"
echo "Volume Name       : ${VOL_NAME}"
echo "=========================================================="

# Validation
if [[ ! -d ${APP_DIR} ]]; then
	echo "Error: App bundle not found at '${APP_DIR}'" >&2
	exit 1
fi

# Ensure background image exists, regenerate if needed
BG_IMG="${REPO_ROOT}/deploy/macos/dmg-background.png"
if [[ ! -f ${BG_IMG} ]]; then
	echo "[*] Generating DMG background image..."
	if command -v python3 >/dev/null 2>&1; then
		python3 "${SCRIPT_DIR}/generate_dmg_background.py"
	else
		echo "Warning: python3 not available to regenerate DMG background." >&2
	fi
fi

VOL_ICON="${REPO_ROOT}/icon/NotepadNext.icns"

# Ensure create-dmg is installed
if ! command -v create-dmg >/dev/null 2>&1; then
	echo "[*] create-dmg not found in PATH. Checking Homebrew..."
	if command -v brew >/dev/null 2>&1; then
		echo "[*] Installing create-dmg via Homebrew..."
		brew install create-dmg || true
	fi
fi

# Cleanup previous DMG if present
rm -f "${DMG_OUTPUT}"
hdiutil detach "/Volumes/${VOL_NAME}" -force 2>/dev/null || true

# Temporary staging folder containing only NotepadNext.app
STAGE_DIR="$(mktemp -d /tmp/notepadnext-dmg-stage.XXXXXX)"
trap 'rm -rf "${STAGE_DIR}"' EXIT

echo "[*] Staging application bundle..."
cp -R "${APP_DIR}" "${STAGE_DIR}/NotepadNext.app"

# Ensure NotepadNext.icns and documentation files exist in bundle Resources
RESOURCES_DIR="${STAGE_DIR}/NotepadNext.app/Contents/Resources"
mkdir -p "${RESOURCES_DIR}"
if [[ ! -f "${RESOURCES_DIR}/NotepadNext.icns" ]]; then
	if [[ -f ${VOL_ICON} ]]; then
		echo "[*] Injecting application icon into bundle Resources..."
		cp "${VOL_ICON}" "${RESOURCES_DIR}/NotepadNext.icns"
	fi
fi

# Copy DOCNOTE.md and CHANGELOG.md into bundle Resources
if [[ -f "${REPO_ROOT}/DOCNOTE.md" ]]; then
	cp -f "${REPO_ROOT}/DOCNOTE.md" "${RESOURCES_DIR}/"
fi
if [[ -f "${REPO_ROOT}/CHANGELOG.md" ]]; then
	cp -f "${REPO_ROOT}/CHANGELOG.md" "${RESOURCES_DIR}/"
fi

# Prune unneeded heavy plugins and unneeded debug symbols from staged bundle
rm -f "${STAGE_DIR}/NotepadNext.app/Contents/PlugIns/imageformats/libqtiff.dylib" \
	"${STAGE_DIR}/NotepadNext.app/Contents/PlugIns/imageformats/libqwebp.dylib" \
	"${STAGE_DIR}/NotepadNext.app/Contents/PlugIns/imageformats/libqmacheif.dylib" \
	"${STAGE_DIR}/NotepadNext.app/Contents/PlugIns/imageformats/libqmacjp2.dylib" \
	"${STAGE_DIR}/NotepadNext.app/Contents/PlugIns/imageformats/libqwbmp.dylib" \
	"${STAGE_DIR}/NotepadNext.app/Contents/PlugIns/imageformats/libqtga.dylib" \
	"${STAGE_DIR}/NotepadNext.app/Contents/PlugIns/tls/libqopensslbackend.dylib" \
	"${STAGE_DIR}/NotepadNext.app/Contents/PlugIns/tls/libqcertonlybackend.dylib" 2>/dev/null || true
strip -x "${STAGE_DIR}/NotepadNext.app/Contents/MacOS/NotepadNext" 2>/dev/null || true

# Touch the app bundle so macOS LaunchServices/Finder refreshes its icon cache
touch "${STAGE_DIR}/NotepadNext.app"

# Attempt DMG creation using create-dmg if available
CREATED=0
if command -v create-dmg >/dev/null 2>&1; then
	echo "[*] Packaging styled DMG using create-dmg..."
	CREATE_DMG_ARGS=(
		--volname "${VOL_NAME}"
		--window-pos 200 120
		--window-size 680 440
		--icon-size 128
		--text-size 12
		--icon "NotepadNext.app" 170 245
		--hide-extension "NotepadNext.app"
		--app-drop-link 510 245
		--format UDZO
		--hdiutil-retries 10
		--applescript-sleep-duration 5
		--overwrite
	)

	if [[ -f ${VOL_ICON} ]]; then
		CREATE_DMG_ARGS+=(--volicon "${VOL_ICON}")
	fi

	if [[ -f ${BG_IMG} ]]; then
		CREATE_DMG_ARGS+=(--background "${BG_IMG}")
	fi

	# Always attempt full styled DMG with AppleScript first
	if create-dmg "${CREATE_DMG_ARGS[@]}" "${DMG_OUTPUT}" "${STAGE_DIR}"; then
		CREATED=1
		echo "[*] Styled DMG successfully created with create-dmg."
	else
		echo "Warning: Standard create-dmg failed (AppleScript unavailable or timed out). Falling back to native hdiutil..." >&2
		CREATED=0
	fi
fi

# Fallback to native hdiutil packaging
if [[ ${CREATED} -eq 0 ]]; then
	echo "[*] Falling back to native hdiutil packaging..."
	ln -s /Applications "${STAGE_DIR}/Applications"

	if [[ -f ${BG_IMG} ]]; then
		mkdir -p "${STAGE_DIR}/.background"
		cp "${BG_IMG}" "${STAGE_DIR}/.background/dmg-background.png"
		chflags hidden "${STAGE_DIR}/.background" 2>/dev/null || true
	fi

	if [[ -f ${VOL_ICON} ]]; then
		cp "${VOL_ICON}" "${STAGE_DIR}/.VolumeIcon.icns"
		chflags hidden "${STAGE_DIR}/.VolumeIcon.icns" 2>/dev/null || true
	fi

	if command -v SetFile >/dev/null 2>&1; then
		SetFile -a V "${STAGE_DIR}/.background" 2>/dev/null || true
		SetFile -a V "${STAGE_DIR}/.VolumeIcon.icns" 2>/dev/null || true
		SetFile -c icnC "${STAGE_DIR}/.VolumeIcon.icns" 2>/dev/null || true
		SetFile -a C "${STAGE_DIR}" 2>/dev/null || true
	fi

	hdiutil create \
		-volname "${VOL_NAME}" \
		-srcfolder "${STAGE_DIR}" \
		-ov \
		-format UDZO \
		-imagekey zlib-level=9 \
		"${DMG_OUTPUT}"
fi

# Verification
if [[ ! -f ${DMG_OUTPUT} ]]; then
	echo "Error: Failed to create disk image '${DMG_OUTPUT}'" >&2
	exit 1
fi

echo "[*] Verifying DMG checksum and structure..."
hdiutil verify "${DMG_OUTPUT}"

DMG_SIZE="$(du -h "${DMG_OUTPUT}" | cut -f1)"
echo "=========================================================="
echo "✔ NotepadNext DMG packaging complete: ${DMG_OUTPUT} (${DMG_SIZE})"
echo "=========================================================="
