import sys
from pathlib import Path

# Ensure Scintilla scripts directory is in python search path
_REPO_ROOT = Path(__file__).resolve().parent.parent
_SCINTILLA_SCRIPTS_DIR = _REPO_ROOT / "thirdparty" / "scintilla" / "scripts"
if str(_SCINTILLA_SCRIPTS_DIR) not in sys.path:
    sys.path.insert(0, str(_SCINTILLA_SCRIPTS_DIR))

import Face  # type: ignore # noqa: E402
import FileGenerator  # type: ignore # noqa: E402
import HFacer  # type: ignore # noqa: E402

DEAD_VALUES = {
    "INDIC_CONTAINER",
    "INDIC_IME",
    "INDIC_IME_MAX",
    "INDIC_MAX",
}


def h_messages(f):
    out = ["enum class Message {"]
    for name in f.order:
        v = f.features[name]
        if v["Category"] != "Deprecated" and v["FeatureType"] in ("fun", "get", "set"):
            out.append(f"    {name} = {v['Value']},")
    out.append("};")
    out.append("Q_ENUM(Message);")
    return out


def _resolve_enum_value_name(f, value_name, prefix_matched):
    if value_name in f.aliases:
        return f.aliases[value_name]

    name_without_prefix = value_name[len(prefix_matched) :]
    if not name_without_prefix:
        name_without_prefix = value_name
    if name_without_prefix.startswith("SC_"):
        name_without_prefix = name_without_prefix[len("SC_") :]
    return name_without_prefix


def _generate_single_enum(f, name, value_prefixes):
    prefixes = value_prefixes.split()
    lines = ["", f"enum class {name} {{"]

    for value_name in f.order:
        prefix_matched = ""
        for p in prefixes:
            if value_name.startswith(p) and value_name not in DEAD_VALUES:
                prefix_matched = p
                break

        if prefix_matched:
            v_enum = f.features[value_name]
            name_no_prefix = _resolve_enum_value_name(f, value_name, prefix_matched)
            pascal_name = Face.PascalCase(name_no_prefix)
            lines.append(f"    {pascal_name} = {v_enum['Value']},")

    lines.append("};")
    lines.append(f"Q_ENUM({name});")
    return lines


def h_enumerations(f):
    out = []
    for name in f.order:
        v = f.features[name]
        if (
            v["Category"] != "Deprecated"
            and v["FeatureType"] == "enu"
            and name != "Lexer"
        ):
            out.extend(_generate_single_enum(f, name, v["Value"]))

    out.append("\t")
    out.append("\tenum class Notification {")
    for name in f.order:
        v = f.features[name]
        if v["Category"] != "Deprecated" and v["FeatureType"] == "evt":
            out.append(f"\t\t{name} = {v['Value']},")
    out.append("\t};")
    out.append("\tQ_ENUM(Notification);")
    return out


def regenerate_all(root):
    scintilla_dir = root / "thirdparty" / "scintilla"
    HFacer.RegenerateAll(scintilla_dir, False)

    f = Face.Face()
    f.ReadFromFile(scintilla_dir / "include" / "Scintilla.iface")
    output_header = root / "src" / "ScintillaEnums.h"
    FileGenerator.Regenerate(output_header, "/* ", h_messages(f) + h_enumerations(f))


# Backwards compatibility aliases
HMessages = h_messages
HEnumerations = h_enumerations
RegenerateAll = regenerate_all


if __name__ == "__main__":
    repo_root = Path(__file__).resolve().parent.parent
    print(f"Regenerating Scintilla enums in: {repo_root}")
    regenerate_all(repo_root)
