# Badge activation record

The repository-verification, exhaustive-search replay, PDF reconstruction and partial Lean workflows passed on public `main` at commit `0f5f603`. Workflow runs: `45516858022` (verify), `45516858054` (replay), `45516857845` (PDF), `45516857904` (Lean).

On 2026-11-25 a browser not signed in to GitHub loaded the public README. All four badge images loaded and reported "passing", each badge target returned HTTP 200, and the explorer GIF rendered.

The Lean badge is labelled "Partial Lean verification" because only the upper bound $r(7)\le24$ is kernel checked. The Version 1.0.0 DOI badge was activated only after the version and concept DOIs resolved, all six archived files matched the immutable GitHub release assets, and the badge image loaded anonymously (HTTP 200). It uses the version DOI because it cites the exact Version 1.0.0 artifact; the concept DOI is linked in the README citation section.
