# Status

- **Claim:** $r(7)=24$ (OEIS A373728, $n=7$). Lower bound apparently new; upper bound credited to earlier work.
- **State:** private candidate repository. Not yet released, no DOI.
- **Mathematics:** three independent AI audits passed; no invalid step found. Not peer reviewed.
- **Computation:** four separately written programs agree; replay with `python3 scripts/replay.py`.
- **Priority:** three literature lanes through 2026-11-25. Remaining source gaps: Gupta 1981 pp. 274–275, the text of Guy's E22 (3rd ed.), the body of Zmiaikou's 2011 thesis, and OEIS pending edits.
- **Formalization:** not attempted. A Lean check of the upper bound is feasible with `decide`; the lower bound would need a verified checker run with `native_decide`.
