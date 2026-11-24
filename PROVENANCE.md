# Provenance

1. **Discovery (23 November 2026).** An automated Codex cloud research run selected the problem from a public open-problem list, proved the reduction, wrote `verification/src/original/` and `verification/src/second/`, and ran them. Raw run records are kept in a private evidence repository.
2. **Freeze (24 November 2026).** The run's proof text, programs and logs were frozen with SHA-256 hashes before review.
3. **Independent computation.** `verification/src/independent-b/` (Claude, 23 November) and `verification/src/independent-a/` (Claude audit, 25 November) were written without using the original code.
4. **Audits (24–25 November 2026).** Three mathematical audit lanes and three literature lanes ran as separate Claude instances that did not see each other's conclusions. Summaries: `audits/public_safe_reports/`.
5. **Manuscript and repository (25 November 2026).** Prepared by Claude from the frozen proof and the audit repairs.

Files under `verification/src/original/` and `verification/src/second/` are byte-identical to the frozen run except `original/word.txt`, a copy of `proof/witness.txt` placed where `check_witness.py` expects it.
