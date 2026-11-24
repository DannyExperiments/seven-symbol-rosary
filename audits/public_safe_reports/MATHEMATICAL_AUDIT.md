# Mathematical audit (24–25 November 2026)

Three independent Claude instances audited the frozen proof. Each saw only the frozen proof and code, not the other audits or the director's conclusions, and worked without internet access.

| Lane | Verdict | Confidence | Main checks |
|---|---|---|---|
| 1. Hostile line-by-line | PASS, no invalid step | 0.97 | Own solver from scratch matches the original's class counts at every level on all 164 profiles; brute force on 209 small profile tests and 3118 sampled gap placements, 0 mismatches; three planted pruning bugs detected |
| 2. Independent reconstruction | CONFIRMED | 0.99 | Derived the required lemmas before reading the proof; own exhaustive checker (`verification/src/independent-a/`) finds no 23-bead rosary and finds rosaries at 24; matches brute force up to $5^{13}$ words |
| 3. Scope, computation, formalization | PASS, statement exact | 0.97 | Byte-identical replay; line-by-line code review (one-lap test, masks, bounds, canonical form, unique parent, pruning); 13 harmless mutants unchanged and 5 broken mutants caught; a third checker with no pruning agrees |

Editorial repairs adopted in the paper: the completeness of the canonical-parent rule is proved explicitly; the original small-case test (`smallcheck.py`, $n\le4$) is supplemented by the brute-force comparisons; the two original programs are described as two implementations of one method, with independent methods listed separately.

Remaining risk named by all lanes: a shared error in the hand-proved reduction. All lanes re-derived it, and it agrees with reduction-free brute force for $n\le6$.

These are AI audits, not peer review.
