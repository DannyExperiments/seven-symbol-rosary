# Seven-symbol rosary explorer

An interactive 3D figure for the proof. Download [`seven-symbol-rosary-explorer.zip`](seven-symbol-rosary-explorer.zip), unzip it, and open `index.html` in a browser. It runs offline with no external libraries.

- **Scene 1, read any ordering in one lap.** Type or randomize an ordering of 1–7. The reading path starts at one bead and climbs as it goes clockwise; it never completes a full turn, so no bead is used twice. Click beads to remove them: every one of the 24 beads is needed, since each single deletion makes at least 42 orderings unreadable.
- **Scene 2, adding a symbol is a covering problem.** Delete the three 7s and 21 beads remain. Green dots mark the gaps from which a chosen ordering P of 1–6 can be read in one lap. The circle works for 1–7 exactly when the 7s touch a green gap for all 720 orderings P. Click gaps to move the 7s and watch the count.

The figure illustrates the 24-bead circle and the insertion step. The proof that 23 beads never suffice is the exhaustive search in `verification/`.
