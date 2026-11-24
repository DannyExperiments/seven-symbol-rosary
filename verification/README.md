# Verification

| Directory | Author | Method | Result for n=7, L=23 |
|---|---|---|---|
| `src/original/` | Codex run | Insert symbols in nondecreasing frequency; gap hitting sets; canonical-parent rule; per profile | none (164 profiles, 145472 level-6 classes) |
| `src/second/` | Codex run | Same method, separate code, global canonical set | none |
| `src/independent-a/` | Claude audit | Insert a most frequent symbol; global classes under the full symmetry group; prefix-tree gap computation; brute-force modes | none (145472 level-6 classes); finds rosaries at L=24 |
| `src/independent-b/` | Claude | Delete a most frequent symbol with frequency caps; direct coverage test of every candidate; SAT encoding without any reduction | none (0 classes, 1.5e9 candidates) |

Small cases: `independent-a brute` and `independent-b brute` enumerate every word for $n\le5$ and match the searches exactly (e.g. 29400 rosary strings at $n=5$, $L=12$; none at $L=11$). `independent-b/sat.py 6 16` is UNSAT. Logs: `logs/`.
