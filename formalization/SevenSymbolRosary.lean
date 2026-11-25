/-!
# The 24-bead rosary on seven symbols

Upper bound of `r(7) = 24`: the cyclic word `123456712347654321765432` is a
rosary of degree 7, i.e. every permutation of `[1,2,3,4,5,6,7]` is a
subsequence of some rotation of the word (one clockwise lap).

The lower bound `r(7) ≥ 24` is not formalized here.
-/

namespace SevenSymbolRosary

/-- The 24-bead witness. -/
def witness : List Nat :=
  [1, 2, 3, 4, 5, 6, 7, 1, 2, 3, 4, 7, 6, 5, 4, 3, 2, 1, 7, 6, 5, 4, 3, 2]

/-- The seven symbols. -/
def symbols : List Nat := [1, 2, 3, 4, 5, 6, 7]

/-- `p` can be read from the cyclic word `w` in one lap: it is a subsequence
of the rotation of `w` starting at some position. -/
abbrev OneLap (w p : List Nat) : Prop :=
  ∃ i ∈ List.range w.length, p.Sublist (w.rotateLeft i)

/-- All orderings of `n` distinct entries taken from `l`. -/
def orderings : Nat → List Nat → List (List Nat)
  | 0, _ => [[]]
  | n + 1, l => l.flatMap (fun x => (orderings n (l.erase x)).map (x :: ·))

/-- Every permutation of `l` of length `n` appears in `orderings n l`. -/
theorem orderings_complete :
    ∀ (n : Nat) (l p : List Nat), p.Perm l → p.length = n → p ∈ orderings n l := by
  intro n
  induction n with
  | zero =>
    intro l p _ hlen
    have hp : p = [] := List.eq_nil_of_length_eq_zero hlen
    subst hp
    simp [orderings]
  | succ n ih =>
    intro l p hperm hlen
    cases p with
    | nil => simp at hlen
    | cons x rest =>
      have hx : x ∈ l := hperm.mem_iff.mp (List.Mem.head rest)
      have hrest : rest.Perm (l.erase x) :=
        (hperm.trans (List.perm_cons_erase hx)).cons_inv
      have hlen' : rest.length = n := by simpa using hlen
      exact List.mem_flatMap.mpr ⟨x, hx, List.mem_map.mpr ⟨rest, ih _ _ hrest hlen', rfl⟩⟩

/-- Kernel-checked finite verification over the 5040 orderings. -/
theorem all_orderings_readable :
    ∀ p ∈ orderings 7 symbols, OneLap witness p := by
  decide +kernel

/-- The witness has 24 beads. -/
theorem witness_length : witness.length = 24 := rfl

/-- **Upper bound.** Every permutation of `1..7` can be read from the 24-bead
witness in one clockwise lap. -/
theorem witness_is_rosary (p : List Nat) (hp : p.Perm symbols) : OneLap witness p :=
  all_orderings_readable p (orderings_complete 7 symbols p hp hp.length_eq)

end SevenSymbolRosary
