#include <bits/stdc++.h>
using namespace std;
using ull = unsigned long long;

const int N = 20005, T = 40010, B = 500, WW = 313, LG = 16, TS = 2050;

struct E { int u, v; };
struct O { int t, a, b; long long w; };

int n, m, q, tot, W, stamp, tcnt;
vector<E> e;
vector<int> sp, ord;
int p[T], lc[T], rc[T], val[T], L[T], R[T], pos[N], up[T][LG];
ull bit[T][WW], tmp[WW];
int mk[T], id[T], fa[TS], nd[TS], A[TS], C[TS];

int cn(int x, int w) {
    for (int k = LG - 1; k >= 0; k--) {
        int y = up[x][k];
        if (y != -1 && val[y] <= w) x = y;
    }
    return x;
}

void build(vector<int>& w) {
    vector<int> f(n), sz(n, 1), rt(n);
    iota(f.begin(), f.end(), 0);
    iota(rt.begin(), rt.end(), 0);

    auto find = [&](int x) {
        while (f[x] != x) x = f[x] = f[f[x]];
        return x;
    };

    tot = n;
    fill(p, p + 2 * n, -1);

    for (int i : ord) {
        int a = find(e[i].u), b = find(e[i].v);
        if (a == b) continue;

        int x = rt[a], y = rt[b], z = tot++;
        lc[z] = x; rc[z] = y; val[z] = w[i];
        p[x] = p[y] = z;

        if (sz[a] < sz[b]) swap(a, b);
        f[b] = a;
        sz[a] += sz[b];
        rt[a] = z;
    }

    for (int i = 0; i < tot; i++)
        for (int k = 0; k < LG; k++)
            up[i][k] = -1;

    for (int i = 0; i < tot; i++) up[i][0] = p[i];

    for (int k = 1; k < LG; k++)
        for (int i = 0; i < tot; i++) {
            int x = up[i][k - 1];
            if (x != -1) up[i][k] = up[x][k - 1];
        }

    fill(L, L + tot, 1e9);
    fill(R, R + tot, -1);

    int tim = 0;
    vector<pair<int, int>> st;

    for (int r = 0; r < tot; r++) if (p[r] == -1) {
        st.push_back({r, 0});

        while (!st.empty()) {
            auto [x, s] = st.back();
            st.pop_back();

            if (x < n) {
                pos[x] = tim;
                L[x] = R[x] = tim++;
            } else if (!s) {
                st.push_back({x, 1});
                st.push_back({rc[x], 0});
                st.push_back({lc[x], 0});
            }
        }
    }

    for (int x = n; x < tot; x++) {
        L[x] = min(L[lc[x]], L[rc[x]]);
        R[x] = max(R[lc[x]], R[rc[x]]);
    }

    W = (n + 63) >> 6;
    memset(bit, 0, sizeof(bit));

    for (int i : ord) {
        int u = e[i].u, v = e[i].v;
        bit[u][pos[v] >> 6] |= 1ULL << (pos[v] & 63);
        bit[v][pos[u] >> 6] |= 1ULL << (pos[u] & 63);
    }

    for (int x = n; x < tot; x++)
        for (int j = 0; j < W; j++)
            bit[x][j] = bit[lc[x]][j] | bit[rc[x]][j];
}

int add(int x) {
    if (mk[x] == stamp) return id[x];
    mk[x] = stamp;
    id[x] = tcnt;
    nd[tcnt] = x;
    fa[tcnt] = tcnt;
    return tcnt++;
}

int F(int x) {
    while (fa[x] != x) x = fa[x] = fa[fa[x]];
    return x;
}

void un(int a, int b) {
    a = F(a); b = F(b);
    if (a != b) fa[b] = a;
}

void addseg(int l, int r) {
    int a = l >> 6, b = r >> 6;

    if (a == b) {
        tmp[a] |= (~0ULL << (l & 63)) &
                  (~0ULL >> (63 - (r & 63)));
        return;
    }

    tmp[a] |= ~0ULL << (l & 63);

    for (int i = a + 1; i < b; i++)
        tmp[i] = ~0ULL;

    tmp[b] |= ~0ULL >> (63 - (r & 63));
}

bool check(int u, int v, int direct, int x, const int* sw) {
    if (u == v) return 1;

    ++stamp;
    tcnt = 0;

    int ru = add(cn(u, x));
    int rv = add(cn(v, x));
    int s = sp.size();

    for (int i = 0; i < s; i++) {
        int z = sp[i];
        A[i] = add(cn(e[z].u, x));
        C[i] = add(cn(e[z].v, x));
    }

    for (int i = 0; i < s; i++)
        if (sp[i] != direct && sw[i] <= x)
            un(A[i], C[i]);

    ru = F(ru);
    rv = F(rv);

    if (ru == rv) return 1;

    for (int i = 0; i < s; i++)
        if (sp[i] != direct && sw[i] > x) {
            int a = F(A[i]), b = F(C[i]);
            if ((a == ru && b == rv) || (a == rv && b == ru))
                return 1;
        }

    static int ga[TS], gb[TS];
    int ca = 0, cb = 0;

    for (int i = 0; i < tcnt; i++) {
        int z = F(i);
        if (z == ru) ga[ca++] = nd[i];
        if (z == rv) gb[cb++] = nd[i];
    }

    if (!ca || !cb) return 0;

    if (ca > cb) {
        for (int i = 0; i < cb; i++)
            swap(ga[i], gb[i]);
        for (int i = cb; i < ca; i++)
            gb[i] = ga[i];
        swap(ca, cb);
    }

    memset(tmp, 0, W * sizeof(ull));

    for (int i = 0; i < cb; i++)
        addseg(L[gb[i]], R[gb[i]]);

    for (int i = 0; i < ca; i++)
        for (int j = 0; j < W; j++)
            if (bit[ga[i]][j] & tmp[j])
                return 1;

    return 0;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    cin >> n >> m >> q;

    e.resize(m);
    vector<int> cur(m), vs(1, 0);

    for (int i = 0; i < m; i++) {
        long long w;
        cin >> e[i].u >> e[i].v >> w;
        --e[i].u; --e[i].v;
        cur[i] = abs(w);
        vs.push_back(cur[i]);
    }

    vector<O> op(q);

    for (auto& o : op) {
        cin >> o.t >> o.a >> o.b;

        if (o.t == 1) {
            --o.a;
            o.w = o.b;
            vs.push_back(abs(o.w));
        } else {
            --o.a;
            --o.b;
        }
    }

    sort(vs.begin(), vs.end());
    vs.erase(unique(vs.begin(), vs.end()), vs.end());

    unordered_map<unsigned long long, int> mp;
    mp.reserve(m * 2 + 10);
    mp.max_load_factor(.7);

    auto key = [](int u, int v) {
        if (u > v) swap(u, v);
        return (unsigned long long)(unsigned)u << 32 |
               (unsigned)v;
    };

    for (int i = 0; i < m; i++)
        mp[key(e[i].u, e[i].v)] = i;

    vector<int> ans(q, -1);

    for (int l = 0; l < q; l += B) {
        int r = min(q, l + B), len = r - l;

        vector<char> special(m);
        vector<int> direct(len, -1);

        for (int i = l; i < r; i++) {
            if (op[i].t == 1) {
                special[op[i].a] = 1;
            } else if (op[i].a != op[i].b) {
                auto it = mp.find(key(op[i].a, op[i].b));

                if (it != mp.end()) {
                    special[it->second] = 1;
                    direct[i - l] = it->second;
                }
            }
        }

        sp.clear();

        for (int i = 0; i < m; i++)
            if (special[i])
                sp.push_back(i);

        int s = sp.size();

        vector<int> snap((size_t)len * max(1, s));
        vector<int> work = cur, qs;

        int row = 0;

        for (int i = l; i < r; i++) {
            if (op[i].t == 1) {
                work[op[i].a] = abs(op[i].w);
            } else {
                qs.push_back(i);

                for (int j = 0; j < s; j++)
                    snap[(size_t)row * s + j] = work[sp[j]];

                row++;
            }
        }

        cur.swap(work);

        ord.clear();

        for (int i = 0; i < m; i++)
            if (!special[i])
                ord.push_back(i);

        sort(ord.begin(), ord.end(),
            [&](int a, int b) {
                return cur[a] < cur[b];
            });

        build(cur);

        row = 0;

        for (int qi : qs) {
            int u = op[qi].a, v = op[qi].b;

            if (u == v) {
                ans[qi] = 0;
                row++;
                continue;
            }

            const int* sw = s ?
                snap.data() + (size_t)row * s : nullptr;

            auto good = [&](int k) {
                return check(
                    u, v, direct[qi - l],
                    vs[k], sw
                );
            };

            int hi = vs.size() - 1;

            if (!good(hi)) {
                row++;
                continue;
            }

            int lo = 0;

            while (lo < hi) {
                int mid = (lo + hi) >> 1;

                if (good(mid))
                    hi = mid;
                else
                    lo = mid + 1;
            }

            ans[qi] = vs[lo];
            row++;
        }
    }

    for (int i = 0; i < q; i++)
        if (op[i].t == 2)
            cout << ans[i] << '\n';
}