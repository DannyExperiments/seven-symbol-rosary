# Problem and proof

## Problem

For a word $w=w_0\cdots w_{L-1}$ let $w^{(s)}=w_s\cdots w_{L-1}w_0\cdots w_{s-1}$. A word over $[n]=\{1,\dots,n\}$ is a *rosary of degree n* if every permutation of $[n]$ is a subsequence of some $w^{(s)}$. $r(n)$ is the least length of a rosary of degree $n$ (Gupta 1981; Lecouturier–Zmiaikou 2012; OEIS A373728).

## Theorem

$r(7)=24$.

## Upper bound

`123456712347654321765432` ([witness.txt](witness.txt)) is a rosary; `scripts/replay.py` checks all 5040 permutations. Credit: Lecouturier–Zmiaikou construction with one bead removed; posted earlier by A. Ever-Hadani (2026).

## Lower bound

1. **Padding.** Inserting a bead preserves the rosary property, so it suffices to exclude length 23.
2. **Deletion.** Deleting every copy of a symbol from a rosary leaves a rosary on the remaining symbols.
3. **Cyclic shifts.** If a word of distinct letters can be read in one lap, so can each of its cyclic shifts.
4. **Gap criterion.** Insert $c\ge1$ beads of a new symbol $x$ into a cyclic word $v$, occupying gap set $S$. The result is a rosary iff $S$ meets $G_v(P)=\{g: P \text{ is a subsequence of } v^{(g)}\}$ for every permutation $P$ of the old symbols.
5. **Symmetries.** Rotation, reversal and relabelling preserve rosaries.
6. **Search.** For each of the 164 partitions $c_1\le\cdots\le c_7$ of 23, build all rosaries level by level, inserting $c_{k+1}$ beads of symbol $k+1$ in every placement allowed by step 4, keeping canonical forms with a canonical-parent rule (complete by induction; see paper §3). Level 7 is empty for every profile.

Full proofs: [paper](../paper/manuscript.pdf). Computation: [verification](../verification/README.md).
