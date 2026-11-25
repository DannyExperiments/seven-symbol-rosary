# Badge activation record

The repository-verification, exhaustive-search replay, PDF reconstruction and partial Lean workflows passed on public `main` at commit `0f5f603`. Workflow runs: `45516858022` (verify), `45516858054` (replay), `45516857845` (PDF), `45516857904` (Lean).

On 2026-11-25 a browser not signed in to GitHub loaded the public README. All four badge images loaded and reported "passing", each badge target returned HTTP 200, and the explorer GIF rendered.

The Lean badge is labelled "Partial Lean verification" because only the upper bound $r(7)\le24$ is kernel checked. The DOI badge is added only after the Zenodo DOI resolves and the archived files match the release assets.
