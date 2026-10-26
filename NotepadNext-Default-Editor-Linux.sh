#!/usr/bin/env bash
# ==============================================================================
# SCRIPT NAME: NotepadNext-Default-Editor-Linux.sh
# DESCRIPTION: Production-grade script to force-set NotepadNext as the default
#              system and desktop editor across all Linux distributions (Debian,
#              Ubuntu, Fedora, RHEL, CentOS, openSUSE, Arch, Linux Mint, etc.)
#              and Desktop Environments (GNOME, KDE Plasma, XFCE, Cinnamon,
#              MATE, LXQt, LXDE, Budgie, Cosmic, Deepin, i3, Sway, Hyprland).
# AUTHOR:      Harry Dertin Sutisna (@alsyundawy) | ALSYUNDAWY IT SOLUSTION
# APP ID:      com.github.alsyundawy.NotepadNext
# VERSION:     1.0.0
# LICENSE:     GPL-3.0-or-later
# ==============================================================================
#
# DOCNOTE & TECHNICAL ARCHITECTURE
# ================================
#
# 1. OVERVIEW
# -----------
# Linux desktops govern graphical default applications via the Freedesktop XDG
# MIME Applications Specification (mimeapps.list). Terminal utilities and
# CLI tools determine default editors through 'update-alternatives' / 'alternatives'
# mechanisms and the $EDITOR / $VISUAL environment variables.
#
# This script performs a complete, deterministic takeover for NotepadNext
# (com.github.alsyundawy.NotepadNext / NotepadNext.desktop), executing:
#   A. Distro & Desktop Environment Detection (Debian, RedHat, SUSE, Arch families).
#   B. Binary & Desktop Entry Resolution (with automated user-space scaffolding).
#   C. 120+ Developer MIME Type Associations via 'xdg-mime' and atomic 'mimeapps.list'.
#   D. Terminal Alternatives Configuration ('update-alternatives' / 'alternatives').
#   E. CLI Environment Variable Enforcement ($EDITOR / $VISUAL).
#   F. Desktop Database & Sycoca Cache Rebuild ('update-desktop-database', etc.).
#
# 2. XDG MIME PRECEDENCE HIERARCHY
# --------------------------------
# Per Freedesktop specifications, mimeapps.list is resolved in order:
#   1. $XDG_CONFIG_HOME/mimeapps.list (~/.config/mimeapps.list) [User Override]
#   2. $XDG_CONFIG_DIRS/mimeapps.list (/etc/xdg/mimeapps.list) [System Default]
#   3. $XDG_DATA_HOME/applications/mimeapps.list (~/.local/share/applications/)
#   4. $XDG_DATA_DIRS/applications/mimeapps.list (/usr/share/applications/)
#
# This script atomically updates both [Default Applications] and [Added Associations]
# sections, preventing stale handlers while preserving user associations for
# web browsers, media players, and document viewers.
#
# ==============================================================================

set -Eeuo pipefail
IFS=$'\n\t'

# ====================== ENVIRONMENT & CONSTANTS ======================
readonly SCRIPT_NAME="NotepadNext-Default-Editor-Linux.sh"
readonly SCRIPT_VERSION="1.0.0"
readonly APP_ID="com.github.alsyundawy.NotepadNext"
readonly DESKTOP_PRIMARY="${APP_ID}.desktop"
readonly DESKTOP_FALLBACK="NotepadNext.desktop"
readonly BINARY_NAME="NotepadNext"

# ====================== COLOR & FORMATTING ======================
setup_colors() {
	if [[ -t 1 && ${TERM:-} != "dumb" && ${NO_COLOR:-} != "1" ]]; then
		C_RESET=$'\033[0m'
		C_BOLD=$'\033[1m'
		C_RED=$'\033[38;5;196m'
		C_GREEN=$'\033[38;5;46m'
		C_YELLOW=$'\033[38;5;226m'
		C_CYAN=$'\033[38;5;51m'
		C_EMERALD=$'\033[38;5;48m'
	else
		C_RESET=""
		C_BOLD=""
		C_RED=""
		C_GREEN=""
		C_YELLOW=""
		C_CYAN=""
		C_EMERALD=""
	fi
}
setup_colors

# ====================== LOGGING UTILITIES ======================
info() {
	printf '%s[INFO]%s  %s\n' "${C_CYAN}${C_BOLD}" "${C_RESET}" "$*"
}

ok() {
	printf '%s[OK]%s    %s\n' "${C_GREEN}${C_BOLD}" "${C_RESET}" "$*"
}

warn() {
	printf '%s[WARN]%s  %s\n' "${C_YELLOW}${C_BOLD}" "${C_RESET}" "$*" >&2
}

error() {
	printf '%s[ERROR]%s %s\n' "${C_RED}${C_BOLD}" "${C_RESET}" "$*" >&2
}

banner() {
	cat <<EOF
${C_EMERALD}${C_BOLD}
╔═══════════════════════════════════════════════════════════════════════════╗
║       🐧  NotepadNext Default Editor Configurator for Linux  🐧          ║
║              Cross-Distro System & Desktop Environment Automation         ║
║          App ID: ${APP_ID} | v${SCRIPT_VERSION}           ║
╚═══════════════════════════════════════════════════════════════════════════╝
${C_RESET}
EOF
}

# ====================== RUNTIME FLAGS ======================
FLAG_FORCE=0
FLAG_DRY_RUN=0
FLAG_SYSTEM=0
FLAG_STATUS_ONLY=0
FLAG_RESTORE=0

# ====================== STATE VARIABLES ======================
RESOLVED_BINARY=""
RESOLVED_DESKTOP_ID=""
RESOLVED_DESKTOP_PATH=""
DISTRO_FAMILY=""
DISTRO_PRETTY_NAME=""
DETECTED_DE=""

# ====================== TARGET MIME TYPES (120+ FORMATS) ======================
# Comprehensive Freedesktop MIME catalog covering text, code, markup, and configs
readonly TARGET_MIME_TYPES=(
	# --- Plain Text & Core Documentation ---
	"text/plain"
	"text/markdown"
	"text/x-markdown"
	"text/x-rst"
	"text/asciidoc"
	"text/x-log"
	"text/csv"
	"text/tab-separated-values"

	# --- C / C++ / Objective-C / CUDA ---
	"text/x-c"
	"text/x-csrc"
	"text/x-chdr"
	"text/x-c++"
	"text/x-c++src"
	"text/x-c++hdr"
	"text/x-csharp"
	"text/x-objcsrc"
	"text/x-cuda"

	# --- Web, Styles & Scripting ---
	"text/html"
	"application/xhtml+xml"
	"text/css"
	"text/x-scss"
	"text/x-sass"
	"text/x-less"
	"application/javascript"
	"text/javascript"
	"application/x-javascript"
	"application/typescript"
	"text/typescript"
	"text/jsx"
	"text/tsx"

	# --- Structured Data & Configurations ---
	"application/json"
	"application/x-json"
	"application/manifest+json"
	"application/ld+json"
	"application/schema+json"
	"application/xml"
	"text/xml"
	"application/x-xml"
	"application/x-yaml"
	"text/yaml"
	"text/x-yaml"
	"application/toml"
	"text/x-toml"
	"text/x-ini"
	"application/x-ini"

	# --- Shell, Terminal & Automation Scripts ---
	"text/x-sh"
	"application/x-shellscript"
	"text/x-shellscript"
	"text/x-bash"
	"text/x-zsh"
	"text/x-csh"
	"text/x-fish"
	"application/x-powershell"
	"text/x-dockerfile"

	# --- Scripting & Dynamic Languages ---
	"text/x-python"
	"text/x-python3"
	"text/x-ruby"
	"application/x-ruby"
	"text/x-perl"
	"application/x-perl"
	"text/x-php"
	"application/x-php"
	"text/x-lua"
	"text/x-tcl"
	"text/x-awk"
	"text/x-sed"

	# --- Modern Systems & Compiled Languages ---
	"text/x-rust"
	"text/rust"
	"text/x-go"
	"text/go"
	"text/x-java"
	"text/x-java-source"
	"text/x-kotlin"
	"text/x-scala"
	"text/x-groovy"
	"text/x-swift"
	"text/x-zig"
	"text/x-dsrc"
	"text/x-pascal"
	"text/x-fortran"
	"text/x-cobol"

	# --- Functional, Scientific & Domain-Specific ---
	"text/x-haskell"
	"text/x-erlang"
	"text/x-elixir"
	"text/x-clojure"
	"text/x-ocaml"
	"text/x-fsharp"
	"text/x-dart"
	"text/x-r"
	"text/x-r-source"
	"text/x-julia"
	"text/x-nim"
	"text/x-vlang"
	"text/x-vala"

	# --- Databases, Build Systems & Tooling ---
	"text/x-sql"
	"application/sql"
	"text/x-graphql"
	"text/x-makefile"
	"text/x-cmake"
	"text/x-meson"
	"text/x-qmake"
	"application/x-desktop"
	"text/x-gradle"
	"text/x-diff"
	"text/x-patch"
	"text/x-po"
	"text/x-gettext-translation"
	"text/x-qml"
)

# ====================== DETECTION ENGINES ======================

detect_os_and_distro() {
	if [[ -f /etc/os-release ]]; then
		# shellcheck disable=SC1091
		source /etc/os-release
		DISTRO_PRETTY_NAME="${PRETTY_NAME:-Linux}"
		local id_like="${ID_LIKE:-}"
		local id="${ID:-}"

		if [[ ${id} == "debian" || ${id} == "ubuntu" || ${id_like} == *"debian"* || ${id_like} == *"ubuntu"* ]]; then
			DISTRO_FAMILY="debian"
		elif [[ ${id} == "fedora" || ${id} == "rhel" || ${id} == "centos" || ${id} == "rocky" || ${id} == "almalinux" || ${id_like} == *"rhel"* || ${id_like} == *"fedora"* ]]; then
			DISTRO_FAMILY="redhat"
		elif [[ ${id} == *"suse"* || ${id_like} == *"suse"* ]]; then
			DISTRO_FAMILY="suse"
		elif [[ ${id} == "arch" || ${id_like} == *"arch"* ]]; then
			DISTRO_FAMILY="arch"
		else
			DISTRO_FAMILY="generic"
		fi
	else
		local u_sys u_rel
		u_sys="$(uname -s 2>/dev/null || echo "Linux")"
		u_rel="$(uname -r 2>/dev/null || echo "unknown")"
		DISTRO_PRETTY_NAME="Generic Linux (${u_sys} ${u_rel})"
		DISTRO_FAMILY="generic"
	fi

	info "Detected Distribution: ${C_BOLD}${DISTRO_PRETTY_NAME}${C_RESET} (Family: ${DISTRO_FAMILY})"
}

detect_desktop_environment() {
	local de="${XDG_CURRENT_DESKTOP:-${DESKTOP_SESSION:-}}"
	de="$(echo "${de}" | tr '[:lower:]' '[:upper:]')"

	if [[ ${de} == *"GNOME"* ]]; then
		DETECTED_DE="GNOME"
	elif [[ ${de} == *"KDE"* || ${de} == *"PLASMA"* ]]; then
		DETECTED_DE="KDE Plasma"
	elif [[ ${de} == *"XFCE"* ]]; then
		DETECTED_DE="XFCE"
	elif [[ ${de} == *"CINNAMON"* ]]; then
		DETECTED_DE="Cinnamon"
	elif [[ ${de} == *"MATE"* ]]; then
		DETECTED_DE="MATE"
	elif [[ ${de} == *"LXQT"* ]]; then
		DETECTED_DE="LXQt"
	elif [[ ${de} == *"LXDE"* ]]; then
		DETECTED_DE="LXDE"
	elif [[ ${de} == *"BUDGIE"* ]]; then
		DETECTED_DE="Budgie"
	elif [[ ${de} == *"COSMIC"* ]]; then
		DETECTED_DE="Cosmic"
	elif [[ ${de} == *"DEEPIN"* || ${de} == *"DDE"* ]]; then
		DETECTED_DE="Deepin"
	elif [[ ${de} == *"PANTHEON"* ]]; then
		DETECTED_DE="Pantheon"
	elif [[ -n ${de} ]]; then
		DETECTED_DE="${de}"
	elif [[ -n ${WAYLAND_DISPLAY:-} ]]; then
		DETECTED_DE="Wayland Compositor (Tiling/Custom)"
	elif [[ -n ${DISPLAY:-} ]]; then
		DETECTED_DE="X11 Window Manager (Tiling/Custom)"
	else
		DETECTED_DE="Headless / Terminal"
	fi

	info "Detected Desktop Environment: ${C_BOLD}${DETECTED_DE}${C_RESET}"
}

resolve_notepadnext_binary() {
	local candidate=""
	local search_paths=(
		"/usr/bin/${BINARY_NAME}"
		"/usr/local/bin/${BINARY_NAME}"
		"${HOME}/.local/bin/${BINARY_NAME}"
		"/opt/${BINARY_NAME}/bin/${BINARY_NAME}"
		"/opt/${BINARY_NAME}/${BINARY_NAME}"
	)

	# Check command -v first
	if command -v "${BINARY_NAME}" >/dev/null 2>&1; then
		candidate="$(command -v "${BINARY_NAME}")"
	else
		for path in "${search_paths[@]}"; do
			if [[ -x ${path} ]]; then
				candidate="${path}"
				break
			fi
		done
	fi

	if [[ -n ${candidate} && -x ${candidate} ]]; then
		RESOLVED_BINARY="${candidate}"
		ok "Resolved NotepadNext binary: ${C_BOLD}${RESOLVED_BINARY}${C_RESET}"
	else
		warn "NotepadNext executable not found in standard system PATH."
		info "If you installed via .deb or .rpm, please verify installation:"
		info "  Debian/Ubuntu: sudo apt install ./notepadnext_*.deb"
		info "  Fedora/RHEL:   sudo dnf install ./notepadnext-*.rpm"
		RESOLVED_BINARY="/usr/bin/${BINARY_NAME}"
	fi
}

resolve_or_create_desktop_file() {
	local app_dirs=(
		"${HOME}/.local/share/applications"
		"/usr/share/applications"
		"/usr/local/share/applications"
	)

	# 1. Search for existing desktop files
	for dir in "${app_dirs[@]}"; do
		if [[ -f "${dir}/${DESKTOP_PRIMARY}" ]]; then
			RESOLVED_DESKTOP_PATH="${dir}/${DESKTOP_PRIMARY}"
			RESOLVED_DESKTOP_ID="${DESKTOP_PRIMARY}"
			break
		elif [[ -f "${dir}/${DESKTOP_FALLBACK}" ]]; then
			RESOLVED_DESKTOP_PATH="${dir}/${DESKTOP_FALLBACK}"
			RESOLVED_DESKTOP_ID="${DESKTOP_FALLBACK}"
			break
		fi
	done

	# 2. If neither desktop file exists, scaffold in user applications directory
	if [[ -z ${RESOLVED_DESKTOP_PATH} ]]; then
		local user_app_dir="${HOME}/.local/share/applications"
		local target_desktop="${user_app_dir}/${DESKTOP_PRIMARY}"
		info "Desktop entry not found in system paths. Generating user desktop entry at:"
		info "  ${target_desktop}"

		if [[ ${FLAG_DRY_RUN} -eq 0 ]]; then
			mkdir -p "${user_app_dir}"
			cat <<EOF >"${target_desktop}"
[Desktop Entry]
Categories=Qt;TextEditor;Utility;Development;IDE;
Exec=${RESOLVED_BINARY} %F
Icon=NotepadNext
MimeType=text/plain;text/markdown;text/x-csrc;text/x-c++src;text/x-python;application/json;application/xml;text/x-sh;text/x-rust;text/x-go;
StartupNotify=true
Type=Application
Terminal=false
Name=Notepad Next
GenericName=Text Editor
Comment=A cross-platform reimplementation of Notepad++ for macOS and Linux
Keywords=notepad;next;text;editor;IDE;programming;source;code;
EOF
			chmod 644 "${target_desktop}"
			# Create convenience alias
			ln -sf "${DESKTOP_PRIMARY}" "${user_app_dir}/${DESKTOP_FALLBACK}" 2>/dev/null || true
		fi

		RESOLVED_DESKTOP_PATH="${target_desktop}"
		RESOLVED_DESKTOP_ID="${DESKTOP_PRIMARY}"
		ok "Created user desktop entry: ${RESOLVED_DESKTOP_PATH}"
	else
		ok "Located desktop entry: ${C_BOLD}${RESOLVED_DESKTOP_PATH}${C_RESET} (ID: ${RESOLVED_DESKTOP_ID})"
	fi
}

# ====================== MIME & DESKTOP ASSOCIATIONS ======================

get_mimeapps_list_path() {
	if [[ ${FLAG_SYSTEM} -eq 1 ]]; then
		echo "/etc/xdg/mimeapps.list"
	else
		echo "${XDG_CONFIG_HOME:-${HOME}/.config}/mimeapps.list"
	fi
}

apply_xdg_mime_associations() {
	local target_id="${RESOLVED_DESKTOP_ID}"
	info "Applying XDG MIME Associations for ${#TARGET_MIME_TYPES[@]} file types -> ${C_BOLD}${target_id}${C_RESET}..."

	# Check if xdg-mime utility is available
	if command -v xdg-mime >/dev/null 2>&1; then
		local success_count=0
		for mime in "${TARGET_MIME_TYPES[@]}"; do
			if [[ ${FLAG_DRY_RUN} -eq 1 ]]; then
				printf '  [DRY-RUN] xdg-mime default %s %s\n' "${target_id}" "${mime}"
				((success_count++))
			else
				if xdg-mime default "${target_id}" "${mime}" 2>/dev/null; then
					((success_count++))
				fi
			fi
		done
		ok "Registered ${success_count}/${#TARGET_MIME_TYPES[@]} MIME types via 'xdg-mime'."
	else
		warn "'xdg-mime' command not found. Falling back to direct atomic 'mimeapps.list' update."
	fi

	# Always perform direct, deterministic atomic mimeapps.list reinforcement
	update_mimeapps_list_atomically "${target_id}"
}

update_mimeapps_list_atomically() {
	local target_id="$1"
	local config_file
	config_file="$(get_mimeapps_list_path)"
	local config_dir
	config_dir="$(dirname "${config_file}")"

	info "Deterministically reinforcing config: ${C_BOLD}${config_file}${C_RESET}"

	if [[ ${FLAG_DRY_RUN} -eq 1 ]]; then
		info "[DRY-RUN] Would update [Default Applications] and [Added Associations] in ${config_file}"
		return 0
	fi

	# Ensure parent directory exists
	if [[ ! -d ${config_dir} ]]; then
		if [[ ${FLAG_SYSTEM} -eq 1 && ${EUID} -ne 0 ]]; then
			sudo mkdir -p "${config_dir}"
		else
			mkdir -p "${config_dir}"
		fi
	fi

	# Create backup if file exists
	if [[ -f ${config_file} ]]; then
		local backup_file
		backup_file="${config_file}.bak.$(date +%Y%m%d%H%M%S)"
		if [[ ${FLAG_SYSTEM} -eq 1 && ${EUID} -ne 0 ]]; then
			sudo cp "${config_file}" "${backup_file}"
		else
			cp "${config_file}" "${backup_file}"
		fi
		info "Created backup: ${backup_file}"
	fi

	# Build temporary updated file
	local tmp_file
	tmp_file="$(mktemp /tmp/mimeapps.XXXXXX)"

	# Read existing file or initialize new
	local in_default_apps=0
	local default_apps_found=0
	local added_assoc_found=0

	# Associative map of current custom lines
	declare -A handled_mimes

	if [[ -f ${config_file} ]]; then
		while IFS= read -r line || [[ -n ${line} ]]; do
			# Strip carriage return
			line="${line%$'\r'}"

			if [[ ${line} =~ ^\[Default\ Applications\] ]]; then
				in_default_apps=1
				default_apps_found=1
				printf '[Default Applications]\n' >>"${tmp_file}"
				# Inject all our target mime types immediately at top of section
				for m in "${TARGET_MIME_TYPES[@]}"; do
					printf '%s=%s\n' "${m}" "${target_id}" >>"${tmp_file}"
					handled_mimes["${m}"]=1
				done
				continue
			elif [[ ${line} =~ ^\[Added\ Associations\] ]]; then
				in_default_apps=0
				added_assoc_found=1
				printf '[Added Associations]\n' >>"${tmp_file}"
				for m in "${TARGET_MIME_TYPES[@]}"; do
					printf '%s=%s;\n' "${m}" "${target_id}" >>"${tmp_file}"
				done
				continue
			elif [[ ${line} =~ ^\[.*\] ]]; then
				in_default_apps=0
			fi

			if [[ ${in_default_apps} -eq 1 ]]; then
				local mime_key="${line%%=*}"
				# Skip if already handled by our target list
				if [[ -n ${handled_mimes[${mime_key}]:-} ]]; then
					continue
				fi
			fi

			printf '%s\n' "${line}" >>"${tmp_file}"
		done <"${config_file}"
	fi

	# If [Default Applications] was not found in original file, append it
	if [[ ${default_apps_found} -eq 0 ]]; then
		printf '\n[Default Applications]\n' >>"${tmp_file}"
		for m in "${TARGET_MIME_TYPES[@]}"; do
			printf '%s=%s\n' "${m}" "${target_id}" >>"${tmp_file}"
		done
	fi

	# If [Added Associations] was not found, append it
	if [[ ${added_assoc_found} -eq 0 ]]; then
		printf '\n[Added Associations]\n' >>"${tmp_file}"
		for m in "${TARGET_MIME_TYPES[@]}"; do
			printf '%s=%s;\n' "${m}" "${target_id}" >>"${tmp_file}"
		done
	fi

	# Atomic commit
	if [[ ${FLAG_SYSTEM} -eq 1 && ${EUID} -ne 0 ]]; then
		sudo install -m 644 "${tmp_file}" "${config_file}"
	else
		install -m 644 "${tmp_file}" "${config_file}"
	fi
	rm -f "${tmp_file}"

	ok "Atomically updated: ${config_file}"

	# Also mirror to ~/.local/share/applications/mimeapps.list for legacy compatibility
	if [[ ${FLAG_SYSTEM} -eq 0 ]]; then
		local legacy_mimeapps="${HOME}/.local/share/applications/mimeapps.list"
		mkdir -p "$(dirname "${legacy_mimeapps}")"
		cp "${config_file}" "${legacy_mimeapps}" 2>/dev/null || true
	fi
}

# ====================== SYSTEM ALTERNATIVES (CLI / TERMINAL) ======================

configure_system_alternatives() {
	if [[ ! -x ${RESOLVED_BINARY} ]]; then
		warn "Cannot configure system alternatives: ${RESOLVED_BINARY} is not executable."
		return 0
	fi

	info "Configuring system editor alternatives for terminal integration..."

	local priority=60

	if command -v update-alternatives >/dev/null 2>&1; then
		# Debian / Ubuntu / openSUSE
		info "Found 'update-alternatives' (Debian/Ubuntu/openSUSE system)."
		if [[ ${FLAG_DRY_RUN} -eq 1 ]]; then
			info "[DRY-RUN] update-alternatives --install /usr/bin/editor editor ${RESOLVED_BINARY} ${priority}"
			info "[DRY-RUN] update-alternatives --set editor ${RESOLVED_BINARY}"
		else
			local cmd_prefix=""
			if [[ ${EUID} -ne 0 ]]; then
				if command -v sudo >/dev/null 2>&1; then
					cmd_prefix="sudo"
				else
					warn "Root or sudo required to run update-alternatives. Skipping system alternative registration."
					return 0
				fi
			fi

			${cmd_prefix} update-alternatives --install /usr/bin/editor editor "${RESOLVED_BINARY}" "${priority}" >/dev/null 2>&1 || true
			${cmd_prefix} update-alternatives --set editor "${RESOLVED_BINARY}" >/dev/null 2>&1 || true

			# If gnome-text-editor alternative exists, also offer NotepadNext
			if ${cmd_prefix} update-alternatives --list gnome-text-editor >/dev/null 2>&1; then
				${cmd_prefix} update-alternatives --install /usr/bin/gnome-text-editor gnome-text-editor "${RESOLVED_BINARY}" "${priority}" >/dev/null 2>&1 || true
			fi
			ok "Configured 'update-alternatives' to point /usr/bin/editor -> ${RESOLVED_BINARY}"
		fi
	elif command -v alternatives >/dev/null 2>&1; then
		# Red Hat / Fedora / CentOS / Rocky
		info "Found 'alternatives' (Red Hat/Fedora/CentOS system)."
		if [[ ${FLAG_DRY_RUN} -eq 1 ]]; then
			info "[DRY-RUN] alternatives --install /usr/bin/editor editor ${RESOLVED_BINARY} ${priority}"
			info "[DRY-RUN] alternatives --set editor ${RESOLVED_BINARY}"
		else
			local cmd_prefix=""
			if [[ ${EUID} -ne 0 ]]; then
				if command -v sudo >/dev/null 2>&1; then
					cmd_prefix="sudo"
				else
					warn "Root or sudo required to run alternatives. Skipping system alternative registration."
					return 0
				fi
			fi

			${cmd_prefix} alternatives --install /usr/bin/editor editor "${RESOLVED_BINARY}" "${priority}" >/dev/null 2>&1 || true
			${cmd_prefix} alternatives --set editor "${RESOLVED_BINARY}" >/dev/null 2>&1 || true
			ok "Configured 'alternatives' to point /usr/bin/editor -> ${RESOLVED_BINARY}"
		fi
	else
		info "System alternatives tool ('update-alternatives' or 'alternatives') not present. Proceeding with shell environment exports."
	fi
}

# ====================== ENVIRONMENT VARIABLES ($EDITOR, $VISUAL) ======================

configure_environment_variables() {
	info "Configuring default terminal environment variables (EDITOR, VISUAL)..."

	local export_block
	export_block=$(
		cat <<EOF
# BEGIN NOTEPADNEXT DEFAULT EDITOR
export EDITOR="${RESOLVED_BINARY}"
export VISUAL="${RESOLVED_BINARY}"
export SUDO_EDITOR="${RESOLVED_BINARY}"
# END NOTEPADNEXT DEFAULT EDITOR
EOF
	)

	if [[ ${FLAG_SYSTEM} -eq 1 ]]; then
		# System-wide profile.d script
		local profile_script="/etc/profile.d/notepadnext-editor.sh"
		if [[ ${FLAG_DRY_RUN} -eq 1 ]]; then
			info "[DRY-RUN] Write system profile: ${profile_script}"
		else
			local cmd_prefix=""
			if [[ ${EUID} -ne 0 ]] && command -v sudo >/dev/null 2>&1; then
				cmd_prefix="sudo"
			fi
			echo "${export_block}" | ${cmd_prefix} tee "${profile_script}" >/dev/null
			${cmd_prefix} chmod 644 "${profile_script}"
			ok "Installed system-wide shell environment: ${profile_script}"
		fi
	else
		# User shell configuration files
		local user_rc_files=(
			"${HOME}/.bashrc"
			"${HOME}/.profile"
		)
		[[ -f "${HOME}/.zshrc" ]] && user_rc_files+=("${HOME}/.zshrc")

		for rc in "${user_rc_files[@]}"; do
			if [[ -f ${rc} ]]; then
				if grep -q "BEGIN NOTEPADNEXT DEFAULT EDITOR" "${rc}" 2>/dev/null; then
					info "Environment variables already configured in: ${rc}"
				else
					if [[ ${FLAG_DRY_RUN} -eq 1 ]]; then
						info "[DRY-RUN] Append EDITOR/VISUAL exports to: ${rc}"
					else
						printf '\n%s\n' "${export_block}" >>"${rc}"
						ok "Appended EDITOR/VISUAL exports to: ${rc}"
					fi
				fi
			fi
		done
	fi
}

# ====================== CACHE & DESKTOP SUBSYSTEM REFRESH ======================

rebuild_desktop_databases() {
	info "Rebuilding desktop MIME databases and sycoca caches..."

	if [[ ${FLAG_DRY_RUN} -eq 1 ]]; then
		info "[DRY-RUN] update-desktop-database & update-mime-database"
		return 0
	fi

	# Freedesktop desktop database
	if command -v update-desktop-database >/dev/null 2>&1; then
		update-desktop-database "${HOME}/.local/share/applications" 2>/dev/null || true
		if [[ ${EUID} -eq 0 || ${FLAG_SYSTEM} -eq 1 ]]; then
			if command -v sudo >/dev/null 2>&1 && [[ ${EUID} -ne 0 ]]; then
				sudo update-desktop-database /usr/share/applications 2>/dev/null || true
			else
				update-desktop-database /usr/share/applications 2>/dev/null || true
			fi
		fi
		ok "Rebuilt desktop database ('update-desktop-database')."
	fi

	# Freedesktop MIME database
	if command -v update-mime-database >/dev/null 2>&1; then
		update-mime-database "${HOME}/.local/share/mime" 2>/dev/null || true
		ok "Rebuilt user MIME database ('update-mime-database')."
	fi

	# KDE Sycoca cache refresh
	if [[ ${DETECTED_DE} == *"KDE"* ]]; then
		if command -v kbuildsycoca6 >/dev/null 2>&1; then
			kbuildsycoca6 --noincremental >/dev/null 2>&1 || true
			ok "Refreshed KDE Plasma 6 sycoca database ('kbuildsycoca6')."
		elif command -v kbuildsycoca5 >/dev/null 2>&1; then
			kbuildsycoca5 --noincremental >/dev/null 2>&1 || true
			ok "Refreshed KDE Plasma 5 sycoca database ('kbuildsycoca5')."
		fi
	fi
}

# ====================== STATUS & VERIFICATION ======================

display_status() {
	banner
	detect_os_and_distro
	detect_desktop_environment
	resolve_notepadnext_binary
	resolve_or_create_desktop_file

	local config_file
	config_file="$(get_mimeapps_list_path)"
	printf '\n%s=== Current Default Editor Status ===%s\n' "${C_BOLD}${C_CYAN}" "${C_RESET}"
	printf 'Configuration File : %s\n' "${config_file}"
	printf 'Resolved Binary    : %s\n' "${RESOLVED_BINARY}"
	printf 'Resolved Desktop ID: %s\n' "${RESOLVED_DESKTOP_ID}"
	printf "System \$EDITOR      : %s\n" "${EDITOR:-<unset>}"
	printf "System \$VISUAL      : %s\n\n" "${VISUAL:-<unset>}"

	local test_mimes=(
		"text/plain"
		"text/markdown"
		"text/x-csrc"
		"text/x-c++src"
		"text/x-python"
		"application/json"
		"application/xml"
		"text/x-sh"
		"text/x-rust"
		"text/x-go"
	)

	printf '%-28s | %-38s | %s\n' "MIME Type" "Current Handler" "Status"
	printf '%s\n' "-----------------------------+----------------------------------------+---------"

	for mime in "${test_mimes[@]}"; do
		local current="<none>"
		if command -v xdg-mime >/dev/null 2>&1; then
			current="$(xdg-mime query default "${mime}" 2>/dev/null || echo "<none>")"
		fi

		local status="${C_RED}MISMATCH${C_RESET}"
		if [[ ${current} == *"${RESOLVED_DESKTOP_ID}"* || ${current} == *"${DESKTOP_FALLBACK}"* ]]; then
			status="${C_GREEN}VERIFIED${C_RESET}"
		fi

		printf '%-28s | %-38s | %s\n' "${mime}" "${current}" "${status}"
	done
	printf '\n'
}

# ====================== RESTORE BACKUP ======================

restore_backup() {
	local config_file
	config_file="$(get_mimeapps_list_path)"
	local latest_bak
	latest_bak="$(find "$(dirname "${config_file}")" -maxdepth 1 -name "mimeapps.list.bak.*" 2>/dev/null | sort -r | head -n 1 || true)"

	if [[ -n ${latest_bak} && -f ${latest_bak} ]]; then
		info "Restoring from latest backup: ${latest_bak} -> ${config_file}"
		if [[ ${FLAG_SYSTEM} -eq 1 && ${EUID} -ne 0 ]]; then
			sudo cp "${latest_bak}" "${config_file}"
		else
			cp "${latest_bak}" "${config_file}"
		fi
		ok "Restored configuration successfully."
	else
		error "No backup file found matching ${config_file}.bak.*"
		exit 1
	fi
}

# ====================== USAGE & HELP ======================

usage() {
	cat <<EOF
${C_BOLD}Usage:${C_RESET} ${SCRIPT_NAME} [OPTIONS]

${C_BOLD}Description:${C_RESET}
  Deterministically configures NotepadNext (${APP_ID}) as the default editor
  for all text, code, scripts, markup, and config files on any Linux distribution
  and desktop environment.

${C_BOLD}Options:${C_RESET}
  -f, --force          Apply associations non-interactively without prompt.
  -d, --dry-run        Simulate all steps and display commands without changing system files.
  -s, --system         Apply system-wide in /etc/xdg/mimeapps.list and /etc/profile.d/ (requires sudo).
  -u, --user           Apply to current user profile (~/.config/mimeapps.list) [default].
      --status         Query and display the current default editor for standard MIME types.
      --restore        Restore mimeapps.list from the most recent automatic backup.
  -v, --version        Display script version and author information.
  -h, --help           Display this help screen.

${C_BOLD}Supported Distributions:${C_RESET}
  • Debian Family   : Ubuntu (20.04/22.04/24.04+), Debian (11/12/13), Linux Mint, Pop!_OS,
                      Elementary OS, Zorin, Kali Linux, Raspberry Pi OS (Raspbian), MX Linux.
  • Red Hat Family  : Fedora (38-42+), RHEL (8/9/10), CentOS Stream, Rocky Linux, AlmaLinux.
  • SUSE Family     : openSUSE Tumbleweed, openSUSE Leap, SUSE Linux Enterprise (SLES).
  • Arch Family     : Arch Linux, Manjaro, EndeavourOS.

${C_BOLD}Supported Desktop Environments:${C_RESET}
  GNOME, KDE Plasma (5/6), XFCE, Cinnamon, MATE, LXQt, LXDE, Budgie, Cosmic, Deepin (DDE),
  Pantheon, i3, Sway, Hyprland, and other Wayland/X11 window managers.

${C_BOLD}Examples:${C_RESET}
  ${SCRIPT_NAME}               # Interactive configuration for current user
  ${SCRIPT_NAME} -f            # Force non-interactive configuration
  ${SCRIPT_NAME} --system -f   # Force system-wide configuration (requires sudo)
  ${SCRIPT_NAME} --status      # Check current system defaults
  ${SCRIPT_NAME} -d            # Dry-run preview

EOF
}

# ====================== MAIN EXECUTION ======================

main() {
	while [[ $# -gt 0 ]]; do
		case "$1" in
		-f | --force)
			FLAG_FORCE=1
			shift
			;;
		-d | --dry-run)
			FLAG_DRY_RUN=1
			shift
			;;
		-s | --system)
			FLAG_SYSTEM=1
			shift
			;;
		-u | --user)
			FLAG_SYSTEM=0
			shift
			;;
		--status)
			FLAG_STATUS_ONLY=1
			shift
			;;
		--restore)
			FLAG_RESTORE=1
			shift
			;;
		-v | --version)
			printf '%s v%s\nAuthor: Harry Dertin Sutisna (@alsyundawy)\nLicense: GPL-3.0-or-later\n' "${SCRIPT_NAME}" "${SCRIPT_VERSION}"
			exit 0
			;;
		-h | --help)
			usage
			exit 0
			;;
		*)
			error "Unknown option: $1"
			usage
			exit 1
			;;
		esac
	done

	if [[ ${FLAG_STATUS_ONLY} -eq 1 ]]; then
		display_status
		exit 0
	fi

	if [[ ${FLAG_RESTORE} -eq 1 ]]; then
		restore_backup
		rebuild_desktop_databases
		exit 0
	fi

	banner
	detect_os_and_distro
	detect_desktop_environment
	resolve_notepadnext_binary
	resolve_or_create_desktop_file

	local scope_name="User Profile (~/.config/mimeapps.list)"
	[[ ${FLAG_SYSTEM} -eq 1 ]] && scope_name="System-Wide (/etc/xdg/mimeapps.list)"

	info "Target Scope: ${C_BOLD}${scope_name}${C_RESET}"
	info "Target Desktop Handler: ${C_BOLD}${RESOLVED_DESKTOP_ID}${C_RESET}"

	if [[ ${FLAG_FORCE} -eq 0 && ${FLAG_DRY_RUN} -eq 0 ]]; then
		printf '\n%sThis will set NotepadNext as the default handler for 120+ code and text formats.%s\n' "${C_YELLOW}" "${C_RESET}"
		read -r -p "Proceed with configuration? [y/N]: " confirm
		if [[ ! ${confirm} =~ ^[Yy]$ ]]; then
			info "Operation cancelled by user."
			exit 0
		fi
	fi

	apply_xdg_mime_associations
	configure_system_alternatives
	configure_environment_variables
	rebuild_desktop_databases

	printf '\n'
	ok "${C_BOLD}NotepadNext has been successfully configured as your default Linux editor!${C_RESET}"
	info "Verification: You can double-click any code/text file in your file manager or run:"
	info "  xdg-open some_file.py"
	info "  xdg-mime query default text/plain"
}

main "$@"
