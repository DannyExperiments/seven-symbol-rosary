# Reproducibility

Requirements: Python 3.9+ and a C++17 compiler (`c++`). No other dependencies, except the optional SAT check, which needs `python-sat`.

```sh
python3 scripts/replay.py           # everything, about 12 minutes on one core
python3 scripts/replay.py --quick   # skips independent-b, about 3 minutes
python3 scripts/verify_repository.py
```

`replay.py` checks the witness and every one-bead deletion, runs the original program on all 164 profiles for (7,23) and the calibrations (6,16) and (6,17), runs the second implementation, runs independent program A (`search 7 23` and `brute 5 12`), and runs independent program B (`seven 23`). It exits non-zero on any mismatch. Saved outputs are in `verification/logs/`.

Optional: `pip install python-sat` then `python3 verification/src/independent-b/sat.py 6 16` (UNSAT in a few minutes).
