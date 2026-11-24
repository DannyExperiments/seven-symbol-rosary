# Claims and evidence

| Claim | Evidence | Load-bearing |
|---|---|---|
| `123456712347654321765432` is a rosary of degree 7 | Direct check of 5040 permutations (`scripts/replay.py`, `original/check_witness.py`, `independent-a` naive mode) | Yes |
| Padding, deletion, cyclic-shift, gap and symmetry lemmas | Hand proofs, paper §2; re-derived in all three audits | Yes |
| No rosary of degree 7 has 23 beads | Exhaustive search, `original/` over 164 profiles (`verification/logs/original_7_23.txt`) | Yes |
| Same | `second/`, `independent-a/`, `independent-b/` (logs in `verification/logs/`) | Corroborating |
| $r(6)=17$ | `original` and `second` (6,16 absent; 6,17 found); SAT `independent-b/sat.py` | Yes for the by-product |
| Search programs are correct on small cases | Brute force over all words, $n\le5$ (`independent-a_brute_*.txt`, `independent-b_small.txt`); audit mutation tests | Corroborating |
| Upper bound previously posted | Ever-Hadani GitHub log dated 2026-08-29 | Priority |
| Lower bound not previously published | Literature audit, three lanes through 2026-11-25 | Priority (qualified) |
