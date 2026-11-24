"""Replay every computation behind r(7) = 24.

Usage: python3 scripts/replay.py [--quick]
  --quick  skips the slowest independent search (independent-b, about 8 minutes).
Requires Python 3 and a C++17 compiler. Exits non-zero on any mismatch.
"""
import os, subprocess, sys, tempfile
from itertools import permutations
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
SRC = ROOT / "verification" / "src"
QUICK = "--quick" in sys.argv

def fail(msg):
    print("FAIL:", msg); sys.exit(1)

def one_lap_rosary(word, alphabet):
    rots = [word[s:] + word[:s] for s in range(len(word))]
    for p in permutations(alphabet):
        if not any(all(c in iter_ for c in p) for iter_ in (iter(r) for r in rots)):
            return False
    return True

def build(src, out):
    subprocess.run(["c++", "-std=c++17", "-O3", str(src), "-o", str(out)], check=True)

# 1. Upper bound: direct check of the witness.
w = (ROOT / "proof" / "witness.txt").read_text().strip()
if len(w) != 24 or set(w) != set("1234567"): fail("witness format")
if not one_lap_rosary(w, "1234567"): fail("witness is not a rosary")
for i in range(24):
    if one_lap_rosary(w[:i] + w[i+1:], "1234567"): fail("a 23-bead deletion is a rosary")
print("upper bound: witness covers all 5040 permutations; every one-bead deletion fails")

with tempfile.TemporaryDirectory() as tmp:
    tmp = Path(tmp)
    # 2. Original program, all 164 profiles.
    build(SRC / "original" / "exhaust.cpp", tmp / "solver")
    env = dict(os.environ, PROFILE_SOLVER=str(tmp / "solver"))
    orig = SRC / "original"
    for args in (["check_witness.py"], ["smallcheck.py"], ["profiles.py", "6", "16", "absent"],
                 ["profiles.py", "6", "17", "exists"], ["profiles.py", "7", "23", "absent"]):
        out = subprocess.run([sys.executable, "-I", str(orig / args[0])] + args[1:], env=env,
                             cwd=orig, capture_output=True, text=True)
        if out.returncode: fail(f"original {' '.join(args)}\n{out.stdout}{out.stderr}")
        print("original", " ".join(args), "->", out.stdout.strip().splitlines()[-1])
    # 3. Second implementation of the same method.
    build(SRC / "second" / "independent_exact.cpp", tmp / "second")
    for n, L, e in ((6, 16, 0), (6, 17, 1), (7, 23, 0)):
        first = subprocess.check_output([str(tmp / "second"), str(n), str(L)], text=True).splitlines()[0]
        if not first.startswith(f"n={n} L={L} exists={e}"): fail(f"second {n} {L}: {first}")
        print("second", first)
    # 4. Independent program A (global symmetry classes, different insertion order).
    build(SRC / "independent-a" / "rosary.cpp", tmp / "ia")
    out = subprocess.check_output([str(tmp / "ia"), "search", "7", "23"], text=True)
    if "exists=NO" not in out or "level 6 : 145472" not in out: fail("independent-a 7 23\n" + out)
    print("independent-a: no 23-bead rosary; level-6 classes 145472")
    out = subprocess.check_output([str(tmp / "ia"), "brute", "5", "12"], text=True)
    print("independent-a brute 5 12 ->", out.strip().splitlines()[-1])
    # 5. Independent program B (deletion of a most frequent symbol, direct coverage test).
    if not QUICK:
        build(SRC / "independent-b" / "ros.cpp", tmp / "ib")
        out = subprocess.check_output([str(tmp / "ib"), "seven", "23"], text=True, stderr=subprocess.DEVNULL)
        if "0 rosary classes found" not in out: fail("independent-b 7 23\n" + out)
        print("independent-b:", [l for l in out.splitlines() if l.startswith("n=7")][0])
print("REPLAY PASS: r(7) = 24")
