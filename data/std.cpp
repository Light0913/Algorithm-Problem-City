// 娜娜莉的巡视 —— 时间线段树 + LCT
// 编译: g++ -O2 -std=c++17 std.cpp -o std
#include <bits/stdc++.h>
using namespace std;

// ==================== 数据 ====================
struct EdgeVer { int u, v, w, l, r; };
struct Query   { int t, u, v, ans; };

int n, m, q;
vector<EdgeVer> ver;
vector<Query>   ask;
vector<vector<int>> seg;
vector<vector<int>> at;

// ==================== LCT ====================
int MAXV;
vector<int> ch0, ch1, fa, val, mx;
vector<char> rev;
vector<int> stk;

inline bool isroot(int x) {
    int f = fa[x];
    return ch0[f] != x && ch1[f] != x;
}
inline void pushup(int x) {
    mx[x] = x;
    int l = ch0[x], r = ch1[x];
    if (l && val[mx[l]] > val[mx[x]]) mx[x] = mx[l];
    if (r && val[mx[r]] > val[mx[x]]) mx[x] = mx[r];
}
inline void pushdown(int x) {
    if (rev[x]) {
        swap(ch0[x], ch1[x]);
        if (ch0[x]) rev[ch0[x]] ^= 1;
        if (ch1[x]) rev[ch1[x]] ^= 1;
        rev[x] = 0;
    }
}
inline void rotate(int x) {
    int y = fa[x], z = fa[y];
    int k = (ch1[y] == x);
    if (!isroot(y)) {
        if (ch0[z] == y) ch0[z] = x;
        else             ch1[z] = x;
    }
    fa[x] = z;
    if (k == 0) {
        ch0[y] = ch1[x];
        if (ch1[x]) fa[ch1[x]] = y;
        ch1[x] = y;
    } else {
        ch1[y] = ch0[x];
        if (ch0[x]) fa[ch0[x]] = y;
        ch0[x] = y;
    }
    fa[y] = x;
    pushup(y);
    pushup(x);
}
void splay(int x) {
    int top = 0;
    int y = x;
    stk[++top] = y;
    while (!isroot(y)) { y = fa[y]; stk[++top] = y; }
    while (top) pushdown(stk[top--]);

    while (!isroot(x)) {
        int y = fa[x], z = fa[y];
        if (!isroot(y)) {
            if ((ch0[y] == x) ^ (ch0[z] == y)) rotate(x);
            else rotate(y);
        }
        rotate(x);
    }
}
// ★ 关键修复
void access(int x) {
    int z = x;
    int y = 0;
    while (x) {
        splay(x);
        ch1[x] = y;
        pushup(x);
        y = x;
        x = fa[x];
    }
    splay(z);
}
void makeroot(int x) {
    access(x);
    splay(x);
    rev[x] ^= 1;
}
int findroot(int x) {
    access(x);
    splay(x);
    while (ch0[x]) {
        pushdown(x);
        x = ch0[x];
    }
    splay(x);
    return x;
}
inline void link(int x, int y) {
    makeroot(x);
    fa[x] = y;
}
void cut(int x, int y) {
    makeroot(x);
    access(y);
    splay(y);
    if (ch0[y] == x && ch1[x] == 0) {
        ch0[y] = 0;
        fa[x] = 0;
        pushup(y);
    }
}
int query_max(int u, int v) {
    makeroot(u);
    access(v);
    splay(v);
    return mx[v];
}

// ==================== 操作历史 ====================
struct LCTOp { int type, u, e, v; };
vector<LCTOp> hist;

inline void link_edge(int u, int e, int v) {
    link(u, e);
    link(e, v);
}
inline void cut_edge(int u, int e, int v) {
    cut(u, e);
    cut(e, v);
}

// ==================== 时间线段树 ====================
void seg_add(int node, int l, int r, int ql, int qr, int eid) {
    if (qr < l || r < ql) return;
    if (ql <= l && r <= qr) {
        seg[node].push_back(eid);
        return;
    }
    int mid = (l + r) >> 1;
    seg_add(node * 2, l, mid, ql, qr, eid);
    seg_add(node * 2 + 1, mid + 1, r, ql, qr, eid);
}

void solve_dfs(int node, int l, int r) {
    int snap = (int)hist.size();

    for (int eid : seg[node]) {
        auto& e = ver[eid];
        int u = e.u, v = e.v;
        int ev = n + 1 + eid;

        if (findroot(u) != findroot(v)) {
            link_edge(u, ev, v);
            hist.push_back({0, u, ev, v});
        } else {
            int me = query_max(u, v);
            if (me > n && val[me] > e.w) {
                int old_eid = me - n - 1;
                int ou = ver[old_eid].u;
                int ov = ver[old_eid].v;
                cut_edge(ou, me, ov);
                hist.push_back({1, ou, me, ov});
                link_edge(u, ev, v);
                hist.push_back({0, u, ev, v});
            }
        }
    }

    if (l == r) {
        for (int qid : at[l]) {
            int u = ask[qid].u, v = ask[qid].v;
            if (findroot(u) != findroot(v)) {
                ask[qid].ans = -1;
            } else {
                int me = query_max(u, v);
                ask[qid].ans = (me > n) ? val[me] : 0;
            }
        }
    } else {
        int mid = (l + r) >> 1;
        solve_dfs(node * 2, l, mid);
        solve_dfs(node * 2 + 1, mid + 1, r);
    }

    while ((int)hist.size() > snap) {
        auto op = hist.back(); hist.pop_back();
        if (op.type == 0) cut_edge(op.u, op.e, op.v);
        else              link_edge(op.u, op.e, op.v);
    }
}

// ==================== main ====================
int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    cin >> n >> m >> q;

    vector<int> eu(m + 1), ev(m + 1), cw(m + 1), last(m + 1, 1);
    for (int i = 1; i <= m; ++i) {
        cin >> eu[i] >> ev[i] >> cw[i];
        cw[i] = abs(cw[i]);
    }

    for (int t = 1; t <= q; ++t) {
        int op, x, y;
        cin >> op >> x >> y;
        if (op == 1) {
            int nw = abs(y);
            if (nw == cw[x]) continue;
            ver.push_back({eu[x], ev[x], cw[x], last[x], t - 1});
            cw[x] = nw;
            last[x] = t;
        } else {
            ask.push_back({t, x, y, x == y ? 0 : -1});
        }
    }
    for (int i = 1; i <= m; ++i)
        ver.push_back({eu[i], ev[i], cw[i], last[i], q});

    int K = (int)ver.size();
    MAXV = n + K + 2;

    ch0.assign(MAXV, 0);
    ch1.assign(MAXV, 0);
    fa.assign(MAXV, 0);
    val.assign(MAXV, 0);
    mx.assign(MAXV, 0);
    rev.assign(MAXV, 0);
    stk.assign(MAXV + 5, 0);

    for (int i = 1; i <= n; ++i) { val[i] = -1; mx[i] = i; }
    for (int i = 0; i < K; ++i) {
        int v = n + 1 + i;
        val[v] = ver[i].w;
        mx[v] = v;
    }

    seg.assign(4 * (q + 1) + 5, {});
    if (q > 0) {
        for (int i = 0; i < K; ++i) {
            auto& e = ver[i];
            if (e.l <= e.r)
                seg_add(1, 1, q, e.l, e.r, i);
        }
    }

    at.assign(q + 1, {});
    for (int i = 0; i < (int)ask.size(); ++i)
        if (ask[i].u != ask[i].v)
            at[ask[i].t].push_back(i);

    hist.reserve((size_t)K * 20);

    if (q > 0 && K > 0)
        solve_dfs(1, 1, q);

    string out;
    out.reserve(ask.size() * 12);
    for (auto& x : ask) {
        char buf[16];
        int len = sprintf(buf, "%d\n", x.ans);
        out.append(buf, len);
    }
    fwrite(out.data(), 1, out.size(), stdout);

    return 0;
}