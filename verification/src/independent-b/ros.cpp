// Independent rosary enumerator (written from scratch, not from the proof package).
// A rosary of degree n, length L: circular word w over {0..n-1} such that every
// permutation p of {0..n-1} is a subsequence of some rotation of w.
//
// Lemma used: if w is a rosary of degree n and s is a most-frequent symbol (freq f),
// deleting all copies of s gives a rosary of degree n-1, length L-f, max freq <= f.
// get(n,L,cap) = all rosaries (up to rotation, reflection, relabeling) of degree n,
// length L, every symbol frequency <= cap.
#include <bits/stdc++.h>
using namespace std;
typedef string W;

static vector<vector<array<uint8_t, 8>>> perms(9);
static vector<vector<int>> ord_(9);
static long long ncand = 0;

static void initPerms(int n) {
    array<uint8_t, 8> a{};
    vector<int> v(n);
    iota(v.begin(), v.end(), 0);
    do {
        for (int i = 0; i < n; i++) a[i] = v[i];
        perms[n].push_back(a);
    } while (next_permutation(v.begin(), v.end()));
    ord_[n].resize(perms[n].size());
    iota(ord_[n].begin(), ord_[n].end(), 0);
}

static bool covers(const W& w, int n) {
    int L = w.size();
    int nxt[2 * 64 + 1][8];
    for (int s = 0; s < n; s++) nxt[2 * L][s] = INT_MAX / 2;
    for (int i = 2 * L - 1; i >= 0; i--) {
        for (int s = 0; s < n; s++) nxt[i][s] = nxt[i + 1][s];
        nxt[i][(int)w[i % L]] = i;
    }
    int occ[8][64], nocc[8] = {0};
    for (int i = 0; i < L; i++) { int c = w[i]; occ[c][nocc[c]++] = i; }
    for (int s = 0; s < n; s++) if (!nocc[s]) return false;
    auto& P = perms[n];
    auto& ord = ord_[n];
    for (size_t k = 0; k < ord.size(); k++) {
        const auto& p = P[ord[k]];
        bool ok = false;
        int c0 = p[0];
        for (int a = 0; a < nocc[c0] && !ok; a++) {
            int j = occ[c0][a], pos = j + 1, lim = j + L;
            bool good = true;
            for (int t = 1; t < n; t++) {
                int q = nxt[pos][p[t]];
                if (q >= lim) { good = false; break; }
                pos = q + 1;
            }
            ok = good;
        }
        if (!ok) { if (k) swap(ord[k], ord[0]); return false; }
    }
    return true;
}

static W orient(const W& w, int dir, int r) {
    int L = w.size();
    W t(L, 0);
    int mp[8];
    fill(mp, mp + 8, -1);
    int nx = 0;
    for (int i = 0; i < L; i++) {
        int idx = dir == 0 ? (r + i) % L : ((r - i) % L + L) % L;
        int c = w[idx];
        if (mp[c] < 0) mp[c] = nx++;
        t[i] = mp[c];
    }
    return t;
}

static W canon(const W& w) {
    int L = w.size();
    W best = orient(w, 0, 0);
    for (int d = 0; d < 2; d++)
        for (int r = 0; r < L; r++) {
            W t = orient(w, d, r);
            if (t < best) best = t;
        }
    return best;
}

// number of distinct linear strings whose circle is in the class of w (n symbols)
static long long orbitStrings(const W& w, int n) {
    set<W> s;
    int L = w.size();
    for (int d = 0; d < 2; d++)
        for (int r = 0; r < L; r++) s.insert(orient(w, d, r));
    long long f = 1;
    for (int i = 2; i <= n; i++) f *= i;
    return (long long)s.size() * f;
}

static map<tuple<int, int, int>, vector<W>> memo;

static const vector<W>& get(int n, int L, int cap) {
    auto key = make_tuple(n, L, cap);
    auto it = memo.find(key);
    if (it != memo.end()) return it->second;
    vector<W> res;
    if (n == 1) {
        if (L >= 1 && L <= cap) res.push_back(W(L, 0));
    } else if (L >= n && (long long)cap * n >= L) {
        unordered_set<W> S;
        int fmin = (L + n - 1) / n, fmax = min(cap, L - (n - 1));
        for (int f = fmin; f <= fmax; f++) {
            const vector<W> B = get(n - 1, L - f, f);  // copy: memo may rehash
            int m = L - f;
            for (const W& b : B) {
                // enumerate nondecreasing gap sequences g_1<=...<=g_f in [0,m)
                vector<int> g(f, 0);
                while (true) {
                    int cnt[64] = {0};
                    for (int x : g) cnt[x]++;
                    W w;
                    w.reserve(L);
                    for (int i = 0; i < m; i++) {
                        for (int c = 0; c < cnt[i]; c++) w.push_back((char)(n - 1));
                        w.push_back(b[i]);
                    }
                    ncand++;
                    if (covers(w, n)) S.insert(canon(w));
                    int k = f - 1;
                    while (k >= 0 && g[k] == m - 1) k--;
                    if (k < 0) break;
                    g[k]++;
                    for (int j = k + 1; j < f; j++) g[j] = g[k];
                }
            }
        }
        res.assign(S.begin(), S.end());
        sort(res.begin(), res.end());
    }
    fprintf(stderr, "get(n=%d,L=%d,cap=%d) -> %zu classes (cands so far %lld)\n", n, L, cap,
            res.size(), ncand);
    return memo[key] = res;
}

static W fromDigits(const string& s) {  // "1234" -> 0-based
    W w;
    for (char c : s) w.push_back(c - '1');
    return w;
}

int main(int argc, char** argv) {
    for (int n = 1; n <= 7; n++) initPerms(n);
    string mode = argc > 1 ? argv[1] : "small";
    // sanity: known rosaries
    printf("sanity n=6 OEIS example covers: %d\n", covers(fromDigits("12345612156431265"), 6));
    printf("sanity n=7 Danny 24 covers: %d\n", covers(fromDigits("123456712347654321765432"), 7));
    printf("sanity n=7 Danny 24 minus last bead covers (expect 0): %d\n",
           covers(fromDigits("12345671234765432176543"), 7));
    if (mode == "small") {
        int maxn = argc > 2 ? atoi(argv[2]) : 6;
        for (int n = 1; n <= maxn; n++) {
            for (int L = 1;; L++) {
                auto t0 = chrono::steady_clock::now();
                const auto& R = get(n, L, L);
                double sec = chrono::duration<double>(chrono::steady_clock::now() - t0).count();
                if (!R.empty()) {
                    long long tot = 0;
                    for (auto& w : R) tot += orbitStrings(w, n);
                    printf("n=%d: r(n)=%d  classes=%zu  strings=%lld  (L=%d-1 empty)  %.1fs\n", n, L,
                           R.size(), tot, L, sec);
                    if (n == 6) {
                        printf("  n=6 optimal classes:\n");
                        for (auto& w : R) {
                            string s;
                            for (char c : w) s.push_back('1' + c);
                            printf("  %s\n", s.c_str());
                        }
                    }
                    fflush(stdout);
                    break;
                }
            }
        }
    } else if (mode == "brute") {  // all n^L strings, no deletion lemma
        int n = atoi(argv[2]), L = atoi(argv[3]);
        long long total = 1, cnt = 0;
        for (int i = 0; i < L; i++) total *= n;
        W w(L, 0);
        for (long long x = 0; x < total; x++) {
            long long y = x;
            for (int i = 0; i < L; i++) { w[i] = y % n; y /= n; }
            if (covers(w, n)) cnt++;
        }
        printf("brute n=%d L=%d: %lld rosary strings out of %lld\n", n, L, cnt, total);
    } else if (mode == "control") {
        // delete a max-frequency symbol from the known 24-bead rosary, re-insert all ways,
        // confirm the known class is recovered by the same insertion+check code.
        W d = fromDigits("123456712347654321765432");
        int L = d.size();
        int del = 1;  // symbol '2' (freq 4)
        W b;
        for (char c : d) if (c != del) b.push_back(c > del ? c - 1 : c);
        // relabel so deleted symbol becomes 6 (new symbol) -- b is over 0..5
        int f = L - b.size(), m = b.size();
        printf("base length %d, f=%d, base covers S6: %d\n", m, f, covers(b, 6));
        W target = canon(d);
        long long found = 0, rosaries = 0;
        vector<int> g(f, 0);
        while (true) {
            int cnt[64] = {0};
            for (int x : g) cnt[x]++;
            W w;
            for (int i = 0; i < m; i++) {
                for (int c = 0; c < cnt[i]; c++) w.push_back(6);
                w.push_back(b[i]);
            }
            if (covers(w, 7)) { rosaries++; if (canon(w) == target) found++; }
            int k = f - 1;
            while (k >= 0 && g[k] == m - 1) k--;
            if (k < 0) break;
            g[k]++;
            for (int j = k + 1; j < f; j++) g[j] = g[k];
        }
        printf("insertions giving S7 rosaries: %lld; matching Danny's class: %lld\n", rosaries, found);
    } else if (mode == "seven") {
        int L = argc > 2 ? atoi(argv[2]) : 23;
        auto t0 = chrono::steady_clock::now();
        const auto& R = get(7, L, L);
        double sec = chrono::duration<double>(chrono::steady_clock::now() - t0).count();
        printf("n=7 L=%d: %zu rosary classes found; candidates checked %lld; %.1fs\n", L, R.size(),
               ncand, sec);
        for (auto& w : R) {
            string s;
            for (char c : w) s.push_back('1' + c);
            printf("  %s\n", s.c_str());
        }
    }
}
