"""SAT check for rosaries with no deletion lemma.
Question: is there a circular word of length L over n symbols such that every
permutation of the n symbols can be read clockwise within one lap?
Encoding: x[i][s] one-hot letters; for each permutation p, order-encoded positions
c_t (o[t][i] <=> c_t >= i), letter match, and the cyclic sequence c_1..c_n must have
exactly one cyclic descent with all steps strict (=> distinct, cyclically ordered).
Symmetry breaking: relabel by first occurrence from position 0 (valid for any word).
"""
import sys, time, itertools, threading
from pysat.solvers import Solver
from pysat.formula import IDPool

def build(n, L):
    pool = IDPool()
    cl = []
    X = lambda i, s: pool.id(('x', i, s))
    for i in range(L):
        cl.append([X(i, s) for s in range(n)])
        for a in range(n):
            for b in range(a + 1, n):
                cl.append([-X(i, a), -X(i, b)])
    # first-occurrence symmetry breaking: S(i,s) = s occurs in positions 0..i
    S = lambda i, s: pool.id(('seen', i, s))
    for i in range(L):
        for s in range(n):
            # S(i,s) <-> S(i-1,s) or X(i,s)
            prev = [S(i - 1, s)] if i > 0 else []
            cl.append([-S(i, s), X(i, s)] + prev)
            cl.append([S(i, s), -X(i, s)])
            if i > 0: cl.append([S(i, s), -S(i - 1, s)])
            if s > 0:
                if i == 0: cl.append([-X(0, s)])
                else: cl.append([-X(i, s), S(i - 1, s - 1)])
    TRUE = pool.id('T'); cl.append([TRUE])
    for pi, p in enumerate(itertools.permutations(range(n))):
        def O(t, i):
            if i <= 0: return TRUE
            if i >= L: return -TRUE
            return pool.id(('o', pi, t, i))
        D = [pool.id(('d', pi, t)) for t in range(n)]
        for t in range(n):
            for i in range(1, L - 1):
                cl.append([-O(t, i + 1), O(t, i)])
            for i in range(L):  # c_t = i  ->  x[i][p_t]
                cl.append([-O(t, i), O(t, i + 1), X(i, p[t])])
            nx = (t + 1) % n
            for i in range(L):
                cl.append([D[t], -O(t, i), O(nx, i + 1)])   # no descent: c_nx > c_t
                cl.append([-D[t], -O(nx, i), O(t, i + 1)])  # descent: c_nx < c_t
        for a in range(n):
            for b in range(a + 1, n):
                cl.append([-D[a], -D[b]])
    return cl, pool

def decode(model, pool, n, L):
    ms = set(v for v in model if v > 0)
    return "".join(str(s + 1) for i in range(L) for s in range(n) if pool.id(('x', i, s)) in ms)

if __name__ == "__main__":
    n, L = int(sys.argv[1]), int(sys.argv[2])
    limit = float(sys.argv[3]) if len(sys.argv) > 3 else 1e9
    solver_name = sys.argv[4] if len(sys.argv) > 4 else 'cadical195'
    t0 = time.time()
    cl, pool = build(n, L)
    print(f"n={n} L={L}: {pool.top} vars, {len(cl)} clauses, built {time.time()-t0:.1f}s", flush=True)
    with Solver(name=solver_name, bootstrap_with=cl) as s:
        timer = threading.Timer(limit, s.interrupt); timer.start()
        r = s.solve_limited(expect_interrupt=True)
        timer.cancel()
        dt = time.time() - t0
        if r is True: print(f"SAT ({dt:.1f}s): {decode(s.get_model(), pool, n, L)}")
        elif r is False: print(f"UNSAT ({dt:.1f}s)")
        else: print(f"UNKNOWN / interrupted ({dt:.1f}s)")
