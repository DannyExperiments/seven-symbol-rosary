#!/bin/sh
set -eu
cd "$(dirname "$0")"
python3 - <<'PY'
from pathlib import Path
import re
w = "".join(re.search(r"def witness : List Nat :=\s*\[([^\]]*)\]", Path("SevenSymbolRosary.lean").read_text()).group(1).split(", ")).replace("\n", "").replace(" ", "")
if w != Path("../proof/witness.txt").read_text().strip():
    raise SystemExit("Lean witness differs from proof/witness.txt")
for path in [Path("SevenSymbolRosary.lean"), Path("Audit.lean")]:
    source = path.read_text()
    if re.search(r"\b(sorry|admit|unsafe|native_decide|implemented_by|extern)\b|^\s*(axiom|constant)\b", source, re.M):
        raise SystemExit("Prohibited Lean declaration or tactic in " + str(path))
print("Lean witness matches proof/witness.txt; no prohibited declarations.")
PY
build_dir=$(mktemp -d)
trap 'rm -rf "$build_dir"' EXIT
cp SevenSymbolRosary.lean Audit.lean lakefile.toml lake-manifest.json lean-toolchain "$build_dir/"
cd "$build_dir"
lake env lean --version
lake build
lake env leanchecker SevenSymbolRosary
lake env lean Audit.lean > audit.log
cat audit.log
python3 - <<'PY'
from pathlib import Path
import re
decls = re.findall(r"^#print axioms ([\w.]+)$", Path("Audit.lean").read_text(), re.M)
results = {}
for line in Path("audit.log").read_text().splitlines():
    empty = re.fullmatch(r"'([\w.]+)' does not depend on any axioms", line)
    some = re.fullmatch(r"'([\w.]+)' depends on axioms: \[(.*)\]", line)
    if empty:
        results[empty[1]] = set()
    elif some:
        results[some[1]] = set(some[2].split(", "))
    else:
        raise SystemExit("Unexpected audit output: " + line)
if set(results) != set(decls):
    raise SystemExit("An audited declaration was missing")
allowed = {"propext", "Quot.sound", "Classical.choice"}
for name, axioms in results.items():
    if not axioms <= allowed:
        raise SystemExit("Unexpected axioms for " + name + ": " + str(axioms))
print(f"Checked axiom dependencies for all {len(results)} declarations: only standard axioms.")
PY
