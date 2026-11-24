// Independent rosary checker / exhaustive search (lane 2 audit). Written from scratch.
//
// Modes:
//   rosary naive  WORD            naive check (all rotations x all perms, plain subsequence scan)
//   rosary fast   WORD            trie-based check (same criterion, different algorithm)
//   rosary brute  n L             enumerate ALL n^L strings, count rosaries (both checkers on hits)
//   rosary search n N [full]      level-set insertion search with GLOBAL dedupe under the full
//                                 group (rotation x reflection x all relabelings)
//                                 existence mode: final level only asks if a hitting set exists
//                                 full mode: enumerate all final words, report labeled count
//   rosary randx  n L trials      cross-validate naive vs fast on random words
//
// Letters are 0..n-1 internally; WORD may be given with digits 1..9 (or 0..8; auto-shifted).
#include <bits/stdc++.h>
using namespace std;
typedef uint32_t Mask;
typedef unsigned long long ull;

// ---------- naive checker ----------
static bool is_subseq(const vector<int>& p, const vector<int>& s) {
    size_t i = 0;
    for (int c : s) { if (i < p.size() && p[i] == c) i++; }
    return i == p.size();
}
// returns number of permutations of 0..n-1 realized
static long naive_count(const vector<int>& w, int n) {
    int L = w.size();
    vector<int> p(n); iota(p.begin(), p.end(), 0);
    long cnt = 0;
    do {
        bool ok = false;
        for (int j = 0; j < L && !ok; j++) {
            vector<int> r(w.begin() + j, w.end()); r.insert(r.end(), w.begin(), w.begin() + j);
            if (is_subseq(p, r)) ok = true;
        }
        if (ok) cnt++;
    } while (next_permutation(p.begin(), p.end()));
    return cnt;
}

// ---------- trie-based machinery ----------
// For a cyclic word w of length L (L <= 31) over k letters, and every permutation P of the
// k letters, compute the mask of starts g with P a subsequence of w[g:]w[:g].
struct Trie {
    int L, k;
    int nxt[64][8];          // nxt[i][c] = smallest j >= i with dbl[j]==c, else 99
    vector<Mask> leaf;       // masks at leaves (one per permutation)
    bool stop_on_empty;
    bool failed;
    void init(const vector<int>& w, int k_) {
        L = w.size(); k = k_;
        for (int c = 0; c < 8; c++) nxt[2 * L][c] = 99;
        for (int i = 2 * L - 1; i >= 0; i--) {
            for (int c = 0; c < 8; c++) nxt[i][c] = nxt[i + 1][c];
            nxt[i][w[i % L]] = i;
        }
    }
    // pos[g] = index just after last matched letter for start g (only meaningful if bit g set)
    void dfs(int depth, int used, Mask m, const int* pos) {
        if (failed) return;
        if (depth == k) { leaf.push_back(m); return; }
        for (int c = 0; c < k; c++) if (!(used >> c & 1)) {
            int np[32]; Mask nm = 0;
            for (Mask t = m; t; t &= t - 1) {
                int g = __builtin_ctz(t);
                int q = (pos[g] < 2 * L) ? nxt[pos[g]][c] : 99;
                if (q < g + L) { np[g] = q + 1; nm |= 1u << g; }
            }
            if (nm == 0 && stop_on_empty) { failed = true; return; }
            dfs(depth + 1, used | 1 << c, nm, np);
            if (failed) return;
        }
    }
    // returns false if some permutation is not realized (when stop_on_empty) ; fills leaf
    bool run(bool stop) {
        stop_on_empty = stop; failed = false; leaf.clear();
        int pos[32]; for (int g = 0; g < L; g++) pos[g] = g;
        Mask all = (L == 32) ? 0xffffffffu : ((1u << L) - 1);
        if (k == 0) { leaf.push_back(all); return true; }
        dfs(0, 0, all, pos);
        if (failed) return false;
        for (Mask x : leaf) if (!x) return false;
        return true;
    }
};

static bool fast_is_rosary(const vector<int>& w, int n) {
    for (int c = 0; c < n; c++) if (find(w.begin(), w.end(), c) == w.end()) return false;
    Trie t; t.init(w, n); return t.run(true);
}

// ---------- canonical form under rotation x reflection x all relabelings ----------
static string canon(const vector<int>& w) {
    int L = w.size();
    string best; bool have = false;
    string cur(L, 0);
    for (int dir = 0; dir < 2; dir++) for (int s = 0; s < L; s++) {
        int lab[8]; for (int c = 0; c < 8; c++) lab[c] = -1;
        int nl = 0, cmp = have ? 0 : -1; int t = 0;
        for (; t < L; t++) {
            int idx = dir == 0 ? (s + t) % L : ((s - t) % L + L) % L;
            int x = w[idx];
            if (lab[x] < 0) lab[x] = nl++;
            cur[t] = (char)lab[x];
            if (cmp == 0) { if (cur[t] > best[t]) break; if (cur[t] < best[t]) cmp = -1; }
        }
        if (t == L && cmp < 0) { best = cur; have = true; }
    }
    return best;
}
static vector<int> tovec(const string& s) { vector<int> v(s.size()); for (size_t i = 0; i < s.size(); i++) v[i] = (unsigned char)s[i]; return v; }
static string show(const string& s) { string r; for (char c : s) r += char('1' + c); return r; }
static string showv(const vector<int>& s) { string r; for (int c : s) r += char('1' + c); return r; }

// first-occurrence pattern of a linear string
static string fo(const vector<int>& v) {
    int lab[8]; for (int c = 0; c < 8; c++) lab[c] = -1; int nl = 0; string r(v.size(), 0);
    for (size_t i = 0; i < v.size(); i++) { if (lab[v[i]] < 0) lab[v[i]] = nl++; r[i] = lab[v[i]]; }
    return r;
}
static long double factl(int n) { long double f = 1; for (int i = 2; i <= n; i++) f *= i; return f; }
// number of distinct strings in the orbit of w under rotation x reflection x S_n
static ull orbit_size(const vector<int>& w, int n) {
    int L = w.size(); string f0 = fo(w); int stab = 0;
    for (int dir = 0; dir < 2; dir++) for (int s = 0; s < L; s++) {
        vector<int> v(L);
        for (int t = 0; t < L; t++) v[t] = w[dir == 0 ? (s + t) % L : ((s - t) % L + L) % L];
        if (fo(v) == f0) stab++;
    }
    long double g = 2.0L * L * factl(n);
    return (ull)llroundl(g / stab);
}

// ---------- search ----------
int n_target, N_target; bool full_mode;
long long final_hits = 0; vector<string> final_words; unordered_set<string> final_set;

static void reduce_masks(vector<Mask>& ms, Mask all) {
    vector<Mask> e; for (Mask x : ms) if (x != all) e.push_back(x);
    sort(e.begin(), e.end(), [](Mask a, Mask b) { int pa = __builtin_popcount(a), pb = __builtin_popcount(b); return pa != pb ? pa < pb : a < b; });
    e.erase(unique(e.begin(), e.end()), e.end());
    vector<Mask> mn;
    for (Mask x : e) { bool red = false; for (Mask y : mn) if ((x & y) == y) { red = true; break; } if (!red) mn.push_back(x); }
    ms.swap(mn);
}
// existence of hitting set of size <= budget (branch on smallest unhit constraint)
static bool hit_exists(const vector<Mask>& cs, Mask chosen, int budget) {
    Mask best = 0; int bw = 99;
    for (Mask e : cs) if (!(e & chosen)) { int p = __builtin_popcount(e); if (p < bw) { bw = p; best = e; } }
    if (!best) return true;
    if (budget == 0) return false;
    for (Mask t = best; t; t &= t - 1) if (hit_exists(cs, chosen | (t & -t), budget - 1)) return true;
    return false;
}
static Mask hit_witness;
static bool hit_find(const vector<Mask>& cs, Mask chosen, int budget) {
    Mask best = 0; int bw = 99;
    for (Mask e : cs) if (!(e & chosen)) { int p = __builtin_popcount(e); if (p < bw) { bw = p; best = e; } }
    if (!best) { hit_witness = chosen; return true; }
    if (budget == 0) return false;
    for (Mask t = best; t; t &= t - 1) if (hit_find(cs, chosen | (t & -t), budget - 1)) return true;
    return false;
}

struct Gen {
    const vector<int>* W; int S, k, m; const vector<Mask>* cs;
    unordered_set<string>* out; long long raw = 0;
    int mult[32];
    vector<int> sup;
    void emit() {
        vector<int> c; c.reserve(S + m);
        for (int g = 0; g < S; g++) { for (int r = 0; r < mult[g]; r++) c.push_back(k); c.push_back((*W)[g]); }
        raw++;
        out->insert(canon(c));
    }
    void distribute(int i, int left) {   // extra copies over support positions sup[i..]
        if (i == (int)sup.size() - 1) { mult[sup[i]] += left; emit(); mult[sup[i]] -= left; return; }
        for (int a = 0; a <= left; a++) { mult[sup[i]] += a; distribute(i + 1, left - a); mult[sup[i]] -= a; }
    }
    // choose support gaps in increasing order
    void choose(int from, Mask chosen, int size) {
        bool allhit = true;
        for (Mask e : *cs) if (!(e & chosen)) {
            allhit = false;
            if ((e >> from) == 0) return;    // can no longer be hit
        }
        if (allhit && size > 0) {
            sup.clear(); for (Mask t = chosen; t; t &= t - 1) sup.push_back(__builtin_ctz(t));
            for (int g : sup) mult[g] = 1;
            distribute(0, m - size);
            for (int g : sup) mult[g] = 0;
        }
        if (size == m) return;
        for (int g = from; g < S; g++) choose(g + 1, chosen | 1u << g, size + 1);
    }
};

static void search() {
    vector<string> level;
    // level 1: f copies of letter 0, need n*f <= N
    for (int f = 1; n_target * f <= N_target; f++) level.push_back(string(f, 0));
    printf("level 1 : %zu words\n", level.size()); fflush(stdout);
    for (int k = 1; k < n_target; k++) {
        bool last = (k + 1 == n_target);
        unordered_set<string> next;
        long long raw = 0, parents = 0;
        for (const string& ws : level) {
            vector<int> W = tovec(ws); int S = W.size();
            int cnt[8] = {0}; for (int c : W) cnt[c]++;
            int M = *max_element(cnt, cnt + k);
            Trie t; t.init(W, k);
            if (!t.run(true)) { fprintf(stderr, "BUG: level word not a rosary %s\n", show(ws).c_str()); exit(3); }
            vector<Mask> cs = t.leaf; Mask all = (1u << S) - 1; reduce_masks(cs, all);
            parents++;
            for (int m = max(M, 1); (n_target - k) * m <= N_target - S; m++) {
                if (last && m != N_target - S) continue;
                if (last && !full_mode) {
                    if (hit_find(cs, 0, m)) {
                        final_hits++;
                        // build explicit witness and verify with BOTH checkers
                        vector<int> c;
                        Mask T = hit_witness; if (!T) T = 1; int extra = m - __builtin_popcount(T);
                        int g0 = __builtin_ctz(T);
                        for (int g = 0; g < S; g++) {
                            if (T >> g & 1) { int r = 1 + (g == g0 ? extra : 0); while (r--) c.push_back(k); }
                            c.push_back(W[g]);
                        }
                        long nc = naive_count(c, n_target); bool fr = fast_is_rosary(c, n_target);
                        if (final_hits <= 5) printf("FOUND %s naive=%ld fast=%d\n", showv(c).c_str(), nc, (int)fr);
                    }
                    continue;
                }
                Gen G; G.W = &W; G.S = S; G.k = k; G.m = m; G.cs = &cs; G.out = &next;
                memset(G.mult, 0, sizeof G.mult);
                G.choose(0, 0, 0);
                raw += G.raw;
            }
        }
        if (last && !full_mode) {
            printf("level %d : parents %lld, final existence hits %lld\n", k + 1, parents, final_hits);
            return;
        }
        level.assign(next.begin(), next.end()); sort(level.begin(), level.end());
        printf("level %d : %zu canonical words (raw children %lld, parents %lld)\n", k + 1, level.size(), raw, parents); fflush(stdout);
    }
    // full mode: final level words
    ull labeled = 0; long bad = 0;
    for (const string& ws : level) {
        vector<int> w = tovec(ws);
        if ((int)w.size() != N_target) { fprintf(stderr, "BUG length\n"); exit(3); }
        if (!fast_is_rosary(w, n_target)) bad++;
        labeled += orbit_size(w, n_target);
    }
    final_words = level;
    printf("FULL n=%d N=%d orbits=%zu labeled_strings=%llu nonrosary_in_output=%ld\n", n_target, N_target, level.size(), labeled, bad);
    for (size_t i = 0; i < level.size() && i < 3; i++) printf("  e.g. %s\n", show(level[i]).c_str());
}

static vector<int> parse(const string& s) {
    vector<int> v; int mn = 9; for (char c : s) mn = min(mn, c - '0');
    for (char c : s) v.push_back(c - '0' - (mn >= 1 ? 1 : 0));
    return v;
}

int main(int argc, char** argv) {
    if (argc < 2) return 2;
    string mode = argv[1];
    if (mode == "naive" || mode == "fast") {
        vector<int> w = parse(argv[2]); int n = *max_element(w.begin(), w.end()) + 1;
        if (argc > 3) n = atoi(argv[3]);
        if (mode == "naive") { long c = naive_count(w, n); long f = 1; for (int i = 2; i <= n; i++) f *= i; printf("naive n=%d L=%zu covered %ld / %ld -> %s\n", n, w.size(), c, f, c == f ? "ROSARY" : "NOT"); }
        else printf("fast n=%d L=%zu -> %s\n", n, w.size(), fast_is_rosary(w, n) ? "ROSARY" : "NOT");
        return 0;
    }
    if (mode == "brute") {
        int n = atoi(argv[2]), L = atoi(argv[3]);
        vector<int> w(L, 0); ull total = 0, hits = 0, mism = 0; string firsthit;
        while (true) {
            total++;
            if (fast_is_rosary(w, n)) {
                hits++;
                if (hits % 997 == 1) { long f = 1; for (int i = 2; i <= n; i++) f *= i; if (naive_count(w, n) != f) mism++; }
                if (firsthit.empty()) firsthit = showv(w);
            }
            int i = L - 1; while (i >= 0 && w[i] == n - 1) { w[i] = 0; i--; }
            if (i < 0) break; w[i]++;
        }
        printf("BRUTE n=%d L=%d strings=%llu rosaries=%llu naive_mismatch_on_sampled_hits=%llu first=%s\n", n, L, total, hits, mism, firsthit.c_str());
        return 0;
    }
    if (mode == "randx") {
        int n = atoi(argv[2]), L = atoi(argv[3]); long T = atol(argv[4]); mt19937 rng(12345); long mism = 0, pos = 0;
        long f = 1; for (int i = 2; i <= n; i++) f *= i;
        for (long t = 0; t < T; t++) {
            vector<int> w(L); for (int& x : w) x = rng() % n;
            bool a = naive_count(w, n) == f, b = fast_is_rosary(w, n); if (a != b) mism++; if (a) pos++;
        }
        printf("RANDX n=%d L=%d trials=%ld rosaries=%ld mismatches=%ld\n", n, L, T, pos, mism);
        return 0;
    }
    if (mode == "member") {
        n_target = atoi(argv[2]); N_target = atoi(argv[3]); full_mode = true; long T = atol(argv[4]);
        search();
        unordered_set<string> fs(final_words.begin(), final_words.end());
        mt19937 rng(777); long found = 0, inset = 0; unordered_set<string> distinct;
        for (long t = 0; t < T; t++) {
            vector<int> w(N_target); for (int& x : w) x = rng() % n_target;
            auto score = [&](const vector<int>& v) { Trie tr; tr.init(v, n_target); tr.run(false); long s = 0; for (Mask x : tr.leaf) if (x) s++; return s; };
            long sc = score(w); long full = 1; for (int i = 2; i <= n_target; i++) full *= i;
            for (int it = 0; it < 4000 && sc < full; it++) {
                int p = rng() % N_target, old = w[p]; w[p] = rng() % n_target;
                long s2 = score(w); if (s2 >= sc) sc = s2; else w[p] = old;
            }
            if (sc == full) { found++; string c = canon(w); distinct.insert(c); if (fs.count(c)) inset++; else printf("MISSING %s\n", showv(w).c_str()); }
        }
        printf("MEMBER n=%d N=%d restarts=%ld rosaries_found=%ld in_search_output=%ld distinct_orbits_hit=%zu of %zu\n", n_target, N_target, T, found, inset, distinct.size(), fs.size());
        return 0;
    }
    if (mode == "search") {
        n_target = atoi(argv[2]); N_target = atoi(argv[3]); full_mode = (argc > 4 && string(argv[4]) == "full");
        if (N_target > 31) return 2;
        auto t0 = chrono::steady_clock::now();
        search();
        double sec = chrono::duration<double>(chrono::steady_clock::now() - t0).count();
        printf("RESULT n=%d N=%d mode=%s exists=%s time=%.1fs\n", n_target, N_target, full_mode ? "full" : "exist",
               full_mode ? "(see FULL line)" : (final_hits ? "YES" : "NO"), sec);
        return 0;
    }
    return 2;
}
