# Literature and priority audit (through 25 November 2026)

Three independent search lanes: (1) official status and priority, (2) identical or stronger results, (3) proof ingredients and informal or unindexed reports.

## Findings

- **Original problem.** Gupta (1981, p. 273, read directly) conjectures that the shortest one-direction rosary has exactly $\lfloor n^2/2\rfloor$ beads, verified for $2\le n\le6$; his $n=6$ example has 18 beads.
- **Known results.** Lecouturier–Zmiaikou (2012) prove $r(n)\le n^2/2$ for even $n$ and $r(n)<n^2/2+n/4-1$ for odd $n$ ($r(7)\le25$), and give rosaries of length 17 for $n=6$ and 31 for $n=8$. OEIS A373728 (revision of 29 June 2024) lists $n\le6$ only. No published lower bound beyond an asymptotic one was found.
- **Prior posting of the upper bound.** A. Ever-Hadani's public GitHub repository `adamhadani/superpatterns`, file `experiments/w4-alphabet-rosary/log.md` (commit dated 29 August 2026), states $r(7)\le24$ with the same word and other 24-bead rosaries found by annealing. Its SAT run for length 23 was stopped, and no lower bound is claimed. The upper bound is credited to this posting and to the Lecouturier–Zmiaikou construction.
- **Lower bound.** No source states or proves $r(7)\ge24$. Classification: **apparently new after a documented search through 25 November 2026**, confidence about 0.8.
- **$r(6)\ge17$.** No published proof was found; OEIS lists 17 without a lower-bound proof.
- **Proof ingredients.** Deletion of a symbol: Newey (1973), linear setting. Gap criterion: closest earlier analogue is Radomirović (2012, Theorem 5), linear; the circular statement appears to be new as a stated lemma. Canonical-parent rule: McKay (1998).

## Source gaps

Gupta (1981) pp. 274–275; the text of Guy's E22 (3rd edition); the body of Zmiaikou's 2011 thesis; OEIS pending edits; full-text arXiv search; Reddit and some Q&A sites.
