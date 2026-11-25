# Lean verification (upper bound only)

The project pins Lean 4.30.0 and uses its bundled standard library, with no external dependencies. With that toolchain and Python 3 installed, run `./verify.sh` from this directory.

The script checks that the Lean witness equals `../proof/witness.txt`, rejects `sorry`, `admit`, `native_decide`, `unsafe` and new axioms, builds a fresh copy, runs the Lean kernel checker, and audits the axioms of the named theorems.

## Formal statements

- `SevenSymbolRosary.witness_is_rosary`: every permutation `p` of `[1,2,3,4,5,6,7]` is a subsequence of `witness.rotateLeft i` for some `i < 24`. This is the upper bound $r(7)\le24$.
- `SevenSymbolRosary.all_orderings_readable`: the finite check over all 5040 orderings, proved by kernel evaluation (`decide +kernel`, not `native_decide`).
- `SevenSymbolRosary.orderings_complete`: every permutation of a list appears in the enumeration used by the finite check.

## Scope

Only the upper bound is formalized. The lower bound $r(7)\ge24$, which is the new part of the result, rests on the exhaustive computation in `../verification/` and the reduction proved in the paper; it is not kernel checked.
